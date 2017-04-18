#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/netlink.h>

#include <atlk/sdk.h>

#include <common/eui48.h>

#include "link_layer.h"

#define NETLINK_USER 31

/* Length of Ethernet device interface string */
#define LL_DEV_NAME_LEN IF_NAMESIZE
/* Ethernet interface name */
static char eth_dev_name[LL_DEV_NAME_LEN];

/* Device descriptor */
struct ll_dev {
  /* Socket file descriptor */
  int socket_fd;
  /* Interface name */
  char *dev_name;
};

/* Linux platform Link Layer send handler */
static atlk_rc_t
ll_send(void *device, const void *pdu, size_t pdu_size)
{
  atlk_rc_t rc;
  /* Send message header */
  struct msghdr send_msg;
  /* NetLink message header */
  struct nlmsghdr *send_nlh = NULL;
  /* I/O vector */
  struct iovec send_iov;
  /* Destination address for NetLink socket */
  struct sockaddr_nl dest_addr;

  /* LL device */
  struct ll_dev *dev = (struct ll_dev *)device;
  /* Actually send length */
  ssize_t len;

  if(!device || !pdu) {
    (void)fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  /* Clear send message fields */
  memset(&send_msg, 0x00, sizeof(send_msg));

  memset(&dest_addr, 0x00, sizeof(dest_addr));
  dest_addr.nl_family = AF_NETLINK;
  /* PID for Linux Kernel */
  dest_addr.nl_pid = 0;
  /* Unicast */
  dest_addr.nl_groups = 0;

  send_nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(pdu_size));
  if (send_nlh == NULL) {
    return ATLK_E_OUT_OF_MEMORY;
  }

  /* Set initial value for rc */
  rc = ATLK_E_UNSPECIFIED;

  memset(send_nlh, 0, NLMSG_SPACE(pdu_size));
  send_nlh->nlmsg_len = NLMSG_SPACE(pdu_size);
  send_nlh->nlmsg_pid = getpid();
  send_nlh->nlmsg_flags = 0;
  /* Holds the actual data size */
  send_nlh->nlmsg_type = pdu_size;

  send_iov.iov_base = (void *)send_nlh;
  send_iov.iov_len = send_nlh->nlmsg_len;
  send_msg.msg_name = (void *)&dest_addr;
  send_msg.msg_namelen = sizeof(dest_addr);
  send_msg.msg_iov = &send_iov;
  send_msg.msg_iovlen = 1;

  (void)memcpy(NLMSG_DATA(send_nlh),
               pdu,
               pdu_size);

  len = sendmsg(dev->socket_fd, &send_msg, 0);
  if(len < 0) {
    (void)fprintf(stderr, "Failed to send, pdu_size %zu  errno %d\n",
                  pdu_size,
                  errno);
    rc = ATLK_E_UNSPECIFIED;
    goto exit;
  }

  if((size_t)len != NLMSG_SPACE(pdu_size)) {
    (void)fprintf(stderr, "Partial send len = %zd\n", len);
    rc = ATLK_E_UNSPECIFIED;
    goto exit;
  }
  /* Everything went good */
  rc = ATLK_OK;

exit:
  free(send_nlh);

  return rc;
}

/* Linux platform Link Layer receive handler */
static atlk_rc_t
ll_receive(void *device, void *pdu, size_t *pdu_size_ptr)
{
  atlk_rc_t rc;
  /* LL device */
  struct ll_dev *dev = (struct ll_dev *)device;
  /* Actually send length */
  ssize_t len;
  /* Receive message header */
  struct msghdr recv_msg;
  /* I/O vector of received message */
  struct iovec recv_iov;
  /* NetLink receive message header */
  struct nlmsghdr *recv_nlh = NULL;
  /* Destination address for NetLink socket */
  struct sockaddr_nl dest_addr;

  if(device == NULL || pdu == NULL || pdu_size_ptr == NULL) {
    (void)fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  rc = ATLK_E_UNSPECIFIED;

  recv_nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(*pdu_size_ptr));
  if (recv_nlh == NULL) {
    return ATLK_E_INVALID_ARG;
  }

  memset(recv_nlh, 0, NLMSG_SPACE(*pdu_size_ptr));
  recv_nlh->nlmsg_len = NLMSG_SPACE(*pdu_size_ptr);
  recv_nlh->nlmsg_pid = getpid();
  recv_nlh->nlmsg_flags = 0;
  recv_nlh->nlmsg_type = 0;

  recv_iov.iov_base = (void *)recv_nlh;
  recv_iov.iov_len = recv_nlh->nlmsg_len;
  recv_msg.msg_name = (void *)&dest_addr;
  recv_msg.msg_namelen = sizeof(dest_addr);
  recv_msg.msg_iov = &recv_iov;
  recv_msg.msg_iovlen = 1;

  len = recvmsg(dev->socket_fd, &recv_msg, 0);

  if(len < 0) {
    (void)fprintf(stderr, "Failed to send\n");
    rc = ATLK_E_UNSPECIFIED;
    goto exit;
  }

  len = recv_nlh->nlmsg_type;

  memcpy(pdu, NLMSG_DATA(recv_nlh), len);

  *pdu_size_ptr = len;

  /* All went good */
  rc = ATLK_OK;
exit:
  free(recv_nlh);

  return rc;
}

/* Linux platform Link Layer local-address-get handler */
static atlk_rc_t
ll_address_get(void *device, eui48_t *address)
{
  /* LL device */
  struct ll_dev *dev = (struct ll_dev *)device;
  /* Interface request*/
  struct ifreq ifr;

  strcpy(ifr.ifr_ifrn.ifrn_name, eth_dev_name);
  ioctl(dev->socket_fd, SIOCGIFHWADDR, &ifr);
  memcpy(address->octets, ifr.ifr_hwaddr.sa_data, 6);
  return ATLK_OK;
}

static atlk_rc_t
ll_acquire(void *device)
{
  if(!device)
    return ATLK_E_INVALID_ARG;
  return 0;
}

static atlk_rc_t
ll_release(void *device)
{
  if(!device)
    return ATLK_E_INVALID_ARG;
  return 0;
}

static rdev_link_ops_t ll_ops = {
  .acquire = ll_acquire, .release = ll_release,
  .send = ll_send,
  .receive = ll_receive,
  .host_address_get = ll_address_get
};

static int context;

static rdev_desc_t
ll_remote_device = {
  .rdev_address.octets = { 0x00, 0x02, 0xcc, 0xf0, 0x00, 0x07},
  .rdev_interface_name = NULL,
  .link_ops_ptr = &ll_ops,
  .link_context_ptr = &context,
  .hw_ops_ptr = NULL,
  .hw_context_ptr = NULL,
};

rdev_desc_t *
ll_get(void)
{
  return &ll_remote_device;
}

int
ll_init(const char *dev_name)
{
  struct sockaddr_nl src_addr;
  int socket_fd;
  int rv;

  (void)dev_name;

  socket_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
  if(socket_fd < 0) {
    perror("Could not open socket");
    return 1;
  }

  memset(&src_addr, 0, sizeof(src_addr));
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid = getpid(); /* self pid */
  src_addr.nl_groups = 1;

  /* Bind socket */
  rv = bind(socket_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
  if (rv == -1) {
    perror("bind");
    return 1;
  }

  context = socket_fd;

  return 0;
}
