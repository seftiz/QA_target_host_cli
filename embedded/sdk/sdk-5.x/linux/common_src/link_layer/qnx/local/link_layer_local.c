#include <string.h>
#include <stdio.h>
// #include <net/ethernet.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
// #include <linux/if_packet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <atlk/sdk.h>

#include <common/eui48.h>

#include <link_layer.h>

static struct sockaddr_un local_addr;
static struct sockaddr_un server_address;

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
  struct sockaddr_un local_server_address;

  if(!device || !pdu) {
    (void)fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  memset(&local_server_address, 0, sizeof(struct sockaddr_un));
  local_server_address.sun_family = AF_UNIX;
  strcpy(local_server_address.sun_path, "./UDSDGSRV");

while (1) {
  len = sendto(dev->socket_fd,
               pdu,
               pdu_size,
               0,
               (struct sockaddr *)&local_server_address,
               sizeof(local_server_address));
  if(len < 0) {
    if (errno == ENOBUFS) {
      continue;
    }
    perror("sendto");
    return ATLK_E_UNSPECIFIED;
  }
  break;
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
  socklen_t address_length = sizeof(struct sockaddr_un);

  if(!device || !pdu || !pdu_size_ptr) {
    (void)fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  len = recvfrom(dev->socket_fd,
                 pdu,
                 *pdu_size_ptr,
                 0,
                 (struct sockaddr *)&server_address,
                 &address_length);
  if(len < 0) {
    perror("recvfrom");
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
//   struct ll_dev *dev = (struct ll_dev *)device;
//   /* Interface request*/
//   struct ifreq ifr;
// 
//   strcpy(ifr.ifr_ifrn.ifrn_name, eth_dev_name);
//   ioctl(dev->socket_fd, SIOCGIFHWADDR, &ifr);
//   memcpy(address->octets, ifr.ifr_hwaddr.sa_data, 6);
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
ll_get(const char *dev_name)
{
  (void)dev_name;
  return &ll_remote_device;
}

const char *SOCKNAME = "/tmp/mysock";

int
ll_init(const char *dev_name)
{
//   struct sockaddr_ll local_addr;
  int socket_fd;
  int rv;
  pid_t pid;

  memset(&local_addr, 0, sizeof(local_addr)); /* Clear structure */
  local_addr.sun_family = AF_UNIX;
  strncpy(local_addr.sun_path, SOCKNAME, sizeof(local_addr.sun_path) - 1);
  unlink(SOCKNAME);
//   local_addr.sll_family = PF_PACKET;
//   local_addr.sll_protocol = htons(ETH_P_ALL);
//   local_addr.sll_ifindex = ifindex;
//   local_addr.sll_halen = 6;
//   memcpy(&local_addr.sll_addr[0], "92:11:22:33:44:55", 6);

  socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
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

  memset(&server_address, 0, sizeof(struct sockaddr_un));
  server_address.sun_family = AF_UNIX;
  strcpy(server_address.sun_path, "./UDSDGSRV");

  /* Register */
  pid = getpid();
  ll_send(ll_remote_device.link_context_ptr, &pid, sizeof(pid));

  return 0;
}
