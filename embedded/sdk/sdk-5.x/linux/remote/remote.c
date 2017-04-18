
#include <stdio.h>
#include "../../common/general/general.h"
#include "../../common/v2x_cli/v2x_cli.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <atlk/ddm_service.h>
#include <atlk/wdm.h>
#include <atlk/dsm.h>
//#include <dsm_internal.h>
#include <atlk/wdm_service.h>

//#include <atlk/eui48_utils.h>
//#include <v2x_internal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netinet/in.h>
#include <atlk/v2x_service.h>
#include "remote.h"
#define NETWORK_INTERFACE_NAME "eth1"
#define SECTON_DEVICE_NAME "Secton"
static dsm_device_config_t dsm_device;
static dsm_service_config_t dsm_service;

static int context;
#define CODE_PATH "./dsp_sw_code.bin"
#define DATA_PATH "./dsp_sw_data.bin"
#define CACHE_PATH "./dsp_sw_code_cache.bin"


/* Device descriptor */
struct ll_dev {
  /* Socket file descriptor */
  int socket_fd;
  /* Interface name */
  char *dev_name;
};

/* Length of Ethernet device interface string */
#define ETH_DEV_NAME_LEN IF_NAMESIZE
/* Ethernet interface name */
static char eth_dev_name[ETH_DEV_NAME_LEN];

/* Linux platform Link Layer send handler */
static atlk_rc_t
v2x_ll_send(void *device, const void *pdu, size_t pdu_size)
{
  /* LL device */
  struct ll_dev *dev = (struct ll_dev *)device;
  /* Actually send length */
  ssize_t len;

  if(!device || !pdu) {
    fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  len = send(dev->socket_fd, pdu, pdu_size, 0);
  if(len < 0) {
    fprintf(stderr, "Failed to send, pdu_size %zu  errno %d\n", pdu_size,
    errno);
    return ATLK_E_UNSPECIFIED;
  }

  if((size_t)len != pdu_size) {
    fprintf(stderr, "Partial send len = %zd\n", len);
    return ATLK_E_UNSPECIFIED;
  }

  return ATLK_OK;
}

/* Linux platform Link Layer receive handler */
static atlk_rc_t
v2x_ll_receive(void *device, void *pdu, size_t *pdu_size_ptr)
{
  /* LL device */
  struct ll_dev *dev = (struct ll_dev *)device;
  /* Actually send length */
  ssize_t len;

  if(!device || !pdu || !pdu_size_ptr) {
    fprintf(stderr, "Mandatory argument is missing\n");
    return ATLK_E_INVALID_ARG;
  }

  len = recv(dev->socket_fd, pdu, *pdu_size_ptr, 0);
  if(len < 0) {
    fprintf(stderr, "Failed to send\n");
    return ATLK_E_UNSPECIFIED;
  }

  *pdu_size_ptr = len;

  return ATLK_OK;
}

/* Linux platform Link Layer local-address-get handler */
static atlk_rc_t
v2x_ll_address_get(void *device, eui48_t *address)
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
v2x_ll_acquire(void *device)
{
  if(!device)
    return ATLK_E_INVALID_ARG;
  return 0;
}

static atlk_rc_t
v2x_ll_release(void *device)
{
  if(!device)
    return ATLK_E_INVALID_ARG;
  return 0;
}

static rdev_link_ops_t linkOps = {
  .acquire = v2x_ll_acquire, .release = v2x_ll_release,
  .send = v2x_ll_send,
  .receive = v2x_ll_receive,
  .host_address_get = v2x_ll_address_get
};


static rdev_desc_t
sectonRemoteDevice = {
  .rdev_address.octets = { 0x00, 0x02, 0xcc, 0xf0, 0x00, 0x07},
  .rdev_interface_name = NULL,
  .link_ops_ptr = &linkOps,
  .link_context_ptr = &context,
  .hw_ops_ptr = NULL,
  .hw_context_ptr = NULL,
};

  /**
     This part should be done by the driver context.
     In the future, we should assume that this is already done and
  **/
static atlk_rc_t
read_file_contents(const char *path, char **buf_ptr, size_t *size)
{
  int f;
  int rv;
  struct stat file_stat;
  ssize_t read_size;

  rv = stat(path, &file_stat);

  if (rv == -1) {
    printf("%s:\n", path);
    perror("Unable to stat");
    return ATLK_E_UNSPECIFIED;
  }

  *size = file_stat.st_size;
  if (*size == 0) {
    *buf_ptr = NULL;
    return ATLK_OK;
  }

  *buf_ptr = (void *)malloc(*size);
  if (!(*buf_ptr)) {
    return ATLK_E_OUT_OF_MEMORY;
  }

  f = open(path, O_RDONLY);
  if (f == -1) {
    printf("%s:\n", path);
    perror("Unable to open");
    return ATLK_E_UNSPECIFIED;
  }

  read_size = read(f, *buf_ptr, *size);
  if (read_size < 0) {
    perror("read failed");
    return ATLK_E_UNSPECIFIED;
  }

  if ((size_t)read_size != *size) {
    printf("Supposed to read %zu bytes, instead read %zu bytes from %s\n",
           *size, read_size, path);
    return ATLK_E_UNSPECIFIED;
  }

  close(f);

  return ATLK_OK;
}


static void
status_change_handler(ddm_service_t *service_ptr, ddm_status_t status)
{
	printf("%p: new state %d\n", service_ptr, status);
}


static atlk_rc_t
ddm_config_init(ddm_configure_t *config)
{
  atlk_rc_t rc;

  if (config == NULL) {
    return ATLK_E_INVALID_ARG;
  }

  rc = read_file_contents(CODE_PATH,
                          &config->binary_buffer_ptr,
                          &config->binary_size);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = read_file_contents(DATA_PATH,
                          &config->binary_buffer_ptr,
                          &config->binary_size);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = read_file_contents(CACHE_PATH,
                          &config->module_params_ptr,
                          &config->module_params_size);
  if (atlk_error(rc)) {
    return rc;
  }

  return ATLK_OK;
}



//SECTON new remote registration method...

int
cli_device_register(struct cli_def *cli, const char *command, char *argv[], int argc)
{

  /* get user context */
  // user_context *myctx = (user_context *) cli_get_context(cli);
  atlk_rc_t rc = ATLK_OK;
  char  hw_addr[256] = {'\0'};
  ddm_service_t *ddm_service_ptr = NULL;

  struct sockaddr_ll local_addr;
  int socket_fd;
  int rv;
  int retries;
  int ifindex;
  int fd;
  struct ifreq ifr;
  int32_t device_type = 0;
  unsigned char *host_hw_addr = NULL;
  char host_hw_addr_string[80] = {'\0'};
  (void) command;
  ddm_status_t ddm_status;
  ddm_configure_t ddm_config = DDM_CONFIGURE_INIT;
  atlk_wait_t wait;
  char if_name[16] = {"eth1"};
  IS_HELP_ARG("register to remote device -hw_addr -device_type -if");

  CHECK_NUM_ARGS /* make sure all parameter are there */
  	
  GET_STRING("-hw_addr", hw_addr, 0, "Get MAC address  xx:yy:ee:ff:gg:cc (MAC address)");
  GET_INT("-device_type", device_type, 2, "Set device type [DSM_DEVICE_TYPE_CRATON2 = 0, DSM_DEVICE_TYPE_SECTON = 1]");
  GET_STRING("-if", if_name, 4, "Get net interface name (for example eth1)");

  if (hw_addr[0] != '\0' && sscanf(hw_addr, "%x:%x:%x:%x:%x:%x",
           (unsigned int *)&(sectonRemoteDevice.rdev_address.octets[0]),
           (unsigned int *)&(sectonRemoteDevice.rdev_address.octets[1]),
           (unsigned int *)&(sectonRemoteDevice.rdev_address.octets[2]),
           (unsigned int *)&(sectonRemoteDevice.rdev_address.octets[3]),
           (unsigned int *)&(sectonRemoteDevice.rdev_address.octets[4]),
           (unsigned int *)&(sectonRemoteDevice.rdev_address.octets[5]) ) < 6)
  {
     cli_print ( cli, "ERROR : Failed to parse %s",(char *)&(sectonRemoteDevice.rdev_address.octets[0]) );
	 return ATLK_E_INVALID_ARG;
  }

  cli_print(cli, "hw_address %02x:%02x:%02x:%02x:%02x:%02x", sectonRemoteDevice.rdev_address.octets[0],
		                                                    sectonRemoteDevice.rdev_address.octets[1],
		                                                    sectonRemoteDevice.rdev_address.octets[2],
		                                                    sectonRemoteDevice.rdev_address.octets[3],
		                                                    sectonRemoteDevice.rdev_address.octets[4],
		                                                    sectonRemoteDevice.rdev_address.octets[5]);

  dsm_device.device_type = device_type;
  cli_print(cli, "device type %d", device_type);
  cli_print(cli, "device type %d", dsm_device.device_type);
  memset(&ifr, 0, sizeof(ifr));

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  cli_print(cli, "[%d] socket fd %d", __LINE__, fd);
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name , NETWORK_INTERFACE_NAME , ETH_DEV_NAME_LEN);

  if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
	  host_hw_addr = (unsigned char *)ifr.ifr_hwaddr.sa_data;
  }
  sprintf(host_hw_addr_string, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", host_hw_addr[0], host_hw_addr[1], host_hw_addr[2], host_hw_addr[3], host_hw_addr[4], host_hw_addr[5]);
  cli_print(cli, "[%d] host hw addr %.2X:%.2X:%.2X:%.2X:%.2X:%.2X", __LINE__, host_hw_addr[0], host_hw_addr[1], host_hw_addr[2], host_hw_addr[3], host_hw_addr[4], host_hw_addr[5]);

  ifindex = if_nametoindex(if_name);

  if (ifindex == 0) {
    perror("if_nametoindex");
    return 1;
  }
  rv = close(fd);

  cli_print(cli, "[%d] close returned %d, socket fd %d", __LINE__, rv, fd);
  fd = 0;
  cli_print(cli, "[%d] socket fd %d", __LINE__, fd);
  local_addr.sll_family = PF_PACKET;
  local_addr.sll_protocol = htons(ETH_P_ALL);
  local_addr.sll_ifindex = ifindex;
  local_addr.sll_halen = 6;

  memcpy(&local_addr.sll_addr[0], &host_hw_addr[0], 6);

  socket_fd = socket(PF_PACKET, SOCK_RAW, htons(/*ETH_P_ALL*/ETH_P_IP));

  cli_print(cli, "[%d] socket socket_fd %d", __LINE__, socket_fd);
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

/*
  rc = dsm_module_init();
  if (atlk_error(rc)) {
		  cli_print ( cli, "ERROR : Failed to initilize dsm : %s", atlk_rc_to_str(rc));	  
		  return atlk_error(rc);  
  }
*/
  dsm_device.device_name = SECTON_DEVICE_NAME;
  //dsm_device.device_type = device_type;
  dsm_device.rdev_ptr = &sectonRemoteDevice;

  rc = dsm_device_register(&dsm_device, 1);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR : Failed to register the device : %s", atlk_rc_to_str(rc));	  
	  return atlk_error(rc);  
  }

  rc = dsm_device_init(dsm_device.device_name, ATLK_FOREVER);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR : Failed to init the device : %s", atlk_rc_to_str(rc));	  
	  return atlk_error(rc);  
  }

  dsm_service.service_name = "DDM";
  dsm_service.service_type = DSM_SERVICE_TYPE_DDM;
  /* The name bundles the service to the specific device */
  dsm_service.device_name = SECTON_DEVICE_NAME;
  rc = dsm_service_register(&dsm_service, (size_t)1);
  if (atlk_error(rc)) {
  	  cli_print ( cli, "ERROR : Failed to register ddm service : %s", atlk_rc_to_str(rc));
  	  return atlk_error(rc);
    }
  cli_print(cli, "[%d] ddm service registration rc = %d", __LINE__, (int)rc);
  rc = dsm_service_init(dsm_service.service_name, ATLK_FOREVER);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR : Failed to init ddm service : %s", atlk_rc_to_str(rc));
	  return atlk_error(rc);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////// handle ddm - start //////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  rc = ddm_service_get(NULL, &ddm_service_ptr);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR :ddm_service_get failed: %s",  atlk_rc_to_str(rc));
    return EXIT_FAILURE;
  }

  rc = ddm_status_change_notify_register(ddm_service_ptr,
                                         status_change_handler);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR :ddm_status_change_notify_register failed: %s",
                  atlk_rc_to_str(rc));
    return EXIT_FAILURE;
  }

  rc = ddm_status_get(ddm_service_ptr, &ddm_status, &atlk_wait_forever);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR :ddm_status_get failed: %s", atlk_rc_to_str(rc));
    return EXIT_FAILURE;
  }

  cli_print ( cli, "ddm_status: %d", ddm_status);

  rc = ddm_config_init(&ddm_config);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR :** Device config files missing, "\
            "not initializing device ** %s", atlk_rc_to_str(rc));
  }
  else {
    retries = 3;
    wait.wait_type = ATLK_WAIT_INTERVAL;
    wait.wait_usec = 1000000;
    do {
      rc = ddm_configuration_set(ddm_service_ptr,
                                &ddm_config,
                                &wait);
      if (atlk_error(rc)) {
        switch (rc) {
          case ATLK_E_EXISTS:
        	  cli_print ( cli, "ERROR :Configuration already loaded");
            rc = ATLK_OK;
            break;

          case ATLK_E_INVALID_STATE:
            cli_print ( cli, "ERROR :ddm_configuration_set invalid state, retrying...");
            break;

          case ATLK_E_TIMEOUT:
        	  cli_print ( cli, "ERROR :ddm_configuration_set timedout, retrying...");
            break;

          default:
        	  cli_print ( cli, "ERROR :ddm_configuration_set failed, %s",
                          atlk_rc_to_str(rc));
            return EXIT_FAILURE;
        }
        retries--;
        if (retries <= 0) {
        	cli_print ( cli, "ERROR :ddm_configuration_set exceeded max retries");
          return EXIT_FAILURE;
        }
        if (rc == ATLK_E_INVALID_STATE) {
          usleep(1000000);
        }
      }
    } while(atlk_error(rc));
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////// handle ddm - end   //////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////



  return ATLK_OK;
}


int
cli_service_register(struct cli_def *cli, const char *command, char *argv[], int argc)
{

  /* get user context */
  // user_context *myctx = (user_context *) cli_get_context(cli);

  atlk_rc_t rc = ATLK_OK;

  (void) command;
  (void) argc;

  IS_HELP_ARG("register to remote service -service_name -service_type ");
  char  service_name[256] = "";
  GET_STRING("-service_name", service_name, 0, "Set service name ");
  GET_INT("-service_type", dsm_service.service_type, 2, "Set service type [DSM_SERVICE_TYPE_V2X = 0, DSM_SERVICE_TYPE_ECC, DSM_SERVICE_TYPE_HSM, DSM_SERVICE_TYPE_WDM, DSM_SERVICE_TYPE_DEV]");

  dsm_service.service_name = (char *)&(service_name[0]);

  //dsm_service.service_type = service_type;
  /* The name bundles the service to the specific device */
  dsm_service.device_name = SECTON_DEVICE_NAME;
  cli_print ( cli,"service name - %s", (char *)&(service_name[0]));
  cli_print ( cli,"service name - %s", (char *)&(dsm_service.service_name[0]));
  cli_print ( cli,"service type - %d", dsm_service.service_type);

  rc = dsm_service_register(&dsm_service, 1);
  cli_print ( cli,"dsm_service_register rc = %d",rc); //chrub
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR : Failed to register the service : %s", atlk_rc_to_str(rc));	  
	  return atlk_error(rc);  
  }


  rc = dsm_service_init(dsm_service.service_name, ATLK_FOREVER);
  cli_print ( cli,"dsm_service_init rc = %d",rc); //chrub
    if (atlk_error(rc)) {
  	  cli_print ( cli, "ERROR : Failed to register the service : %s", atlk_rc_to_str(rc));
  	  return atlk_error(rc);
   }

  cli_print ( cli,"Device and service are registered");

  return ATLK_OK;
}


