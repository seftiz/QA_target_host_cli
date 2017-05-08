#include <string.h>
#include <stdio.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <atlk/sdk.h>

#include <common/eui48.h>

#include "link_layer.h"

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
  /* LL device */
  struct ll_dev *dev = (struct ll_dev *)device;
  /* Actually send length */
  ssize_t len;

  if(!device || !pdu) {
    (void)fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  len = send(dev->socket_fd, pdu, pdu_size, 0);
  if(len < 0) {
    (void)fprintf(stderr, "Failed to send, pdu_size %zu  errno %d\n", pdu_size,
    errno);
    return ATLK_E_UNSPECIFIED;
  }

  if((size_t)len != pdu_size) {
    (void)fprintf(stderr, "Partial send len = %zd\n", len);
    return ATLK_E_UNSPECIFIED;
  }

  return ATLK_OK;
}

/* Linux platform Link Layer receive handler */
static atlk_rc_t
ll_receive(void *device, void *pdu, size_t *pdu_size_ptr)
{
  /* LL device */
  struct ll_dev *dev = (struct ll_dev *)device;
  /* Actually send length */
  ssize_t len;

  if(!device || !pdu || !pdu_size_ptr) {
    (void)fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  len = recv(dev->socket_fd, pdu, *pdu_size_ptr, 0);
  if(len < 0) {
    (void)fprintf(stderr, "Failed to send\n");
    return ATLK_E_UNSPECIFIED;
  }

  *pdu_size_ptr = len;

  return ATLK_OK;
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
  struct sockaddr_ll local_addr;
  int socket_fd;
  int rv;
  int ifindex;

  if (!dev_name) {
    printf("eth_dev_name is NULL");
    return 1;
  }

  memcpy(eth_dev_name, dev_name, LL_DEV_NAME_LEN);

  ifindex = if_nametoindex(dev_name);
  if (ifindex == 0) {
    perror("if_nametoindex");
    return 1;
  }

  local_addr.sll_family = PF_PACKET;
  local_addr.sll_protocol = htons(ETH_P_ALL);
  local_addr.sll_ifindex = ifindex;
  local_addr.sll_halen = 6;
  memcpy(&local_addr.sll_addr[0], "92:11:22:33:44:55", 6);

  socket_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (socket_fd == -1) {
    perror("socket");
    return 1;
  }

  /* Bind socket */
  rv = bind(socket_fd,
            (struct sockaddr *)&local_addr,
            sizeof(local_addr));
  if (rv == -1) {
    perror("bind");
    return 1;
  }

  context = socket_fd;

  return 0;
}