
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

#include <atlk/wdm_service.h>


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
#include <netinet/ether.h>
#include <ref_sys.h>


#define NETWORK_INTERFACE_NAME "eth1"
#define SECTON_DEVICE_NAME "Secton"
#define CRATON2_DEVICE_NAME "Craton2"
#if 0

static dsm_device_config_t dsm_device;
static dsm_service_config_t dsm_service;

/* Length of Ethernet device interface string */
#define ETH_DEV_NAME_LEN IF_NAMESIZE
/* Ethernet interface name */
//static char eth_dev_name[ETH_DEV_NAME_LEN];

//static int context;
#define CODE_PATH "./dsp_sw_code.bin"
#define DATA_PATH "./dsp_sw_data.bin"
#define CACHE_PATH "./dsp_sw_code_cache.bin"
#define SW_PATH "./dsp_sw.bin"

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

  rc = read_file_contents(SW_PATH,
		                  &config->binary_buffer_ptr,
		                  &config->binary_size);
   if (atlk_error(rc)) {
     return rc;
   }
  return ATLK_OK;
}

#endif



//SECTON new remote registration method...

int
cli_device_register(struct cli_def *cli, const char *command, char *argv[], int argc)
{

  /* get user context */
  atlk_rc_t rc = ATLK_OK;
  char  hw_addr[256] = {'\0'};
  int32_t device_type = 0;
  (void) command;
  char if_name[16] = {'\0'};
  int argc_param;
  
  IS_HELP_ARG("register to remote device -hw_addr -device_type -if");
  strcpy((char *) if_name, "eth1");
  CHECK_NUM_ARGS /* make sure all parameter are there */
   cli_print ( cli,"cli_device_register");	
  GET_STRING("-hw_addr", hw_addr, 0, "Get MAC address  xx:yy:ee:ff:gg:cc (MAC address)");
  GET_INT("-device_type", device_type, 2, "Set device type [DSM_DEVICE_TYPE_CRATON2 = 0, DSM_DEVICE_TYPE_SECTON = 1]");
  
  GET_STRING("-if", if_name, 4, "Get net interface name (for example eth1)");
  
  char *argv_param[2];
  cli_print(cli,"if - %s",if_name);
  cli_print(cli,"%d",__LINE__);
  //dsm_device.device_type = device_type;
  cli_print(cli, "device type %d", device_type);
  
  argc_param = 2;
  
  argv_param[0] = (char *)malloc(20);
  argv_param[1] = (char *)malloc(20);
  memset((void *)argv_param[0],0,20);
  memset((void *)argv_param[1],0,20);
  strcpy((char *) argv_param[1],(const char *)if_name);
  strcpy((char *) argv_param[0],(const char *)if_name);
  cli_print ( cli,"cli_device_register strcp");
  /** Reference system initialization */
  cli_print ( cli,"cli_device_register ref_sys_init, argv[1] - %s", (char *)argv_param[1]);
  rc = ref_sys_init(argc_param, argv_param);
  if (atlk_error(rc)) {
    (void)fprintf(stderr, "ref-sys init failed: %d\n", rc);
    return EXIT_FAILURE;
  }
  return (int)ATLK_OK;
}


int
cli_service_register(struct cli_def *cli, const char *command, char *argv[], int argc)
{

  /* get user context */
  // user_context *myctx = (user_context *) cli_get_context(cli);

  //atlk_rc_t rc = ATLK_OK;

  (void) command;
  (void) argc;
  int service_type = 0;
  IS_HELP_ARG("register to remote service -service_name -service_type -device_name");
  char  service_name[256] = "", device_name[256] = SECTON_DEVICE_NAME;
  GET_STRING("-service_name", service_name, 0, "Set service name ");
  GET_INT("-service_type", service_type, 2, "Set service type [DSM_SERVICE_TYPE_V2X = 0, DSM_SERVICE_TYPE_ECC, DSM_SERVICE_TYPE_HSM, DSM_SERVICE_TYPE_WDM, DSM_SERVICE_TYPE_DEV]");
  GET_STRING("-device_name", device_name, 4, "Set device name ");
  printf("%d",service_type);
#if 0
  dsm_service.service_type = service_type;

  dsm_service.service_name = (char *)&(service_name[0]);
  dsm_service.device_name = (char *)&(device_name[0]);
  /* The name bundles the service to the specific device */

  cli_print ( cli,"device name  - %s", (char *)&(dsm_service.device_name[0]));
  cli_print ( cli,"service name - %s", (char *)&(dsm_service.service_name[0]));
  cli_print ( cli,"service type - %d", dsm_service.service_type);

  rc = dsm_service_register(&dsm_service, 1);
  if (atlk_error(rc)) {
	  cli_print ( cli, "ERROR : Failed to register the service : %s", atlk_rc_to_str(rc));	  
	  return atlk_error(rc);  
  }


  rc = dsm_service_init(dsm_service.service_name, ATLK_FOREVER);
    if (atlk_error(rc)) {
  	  cli_print ( cli, "ERROR : Failed to register the service : %s", atlk_rc_to_str(rc));
  	  return atlk_error(rc);
   }

  cli_print ( cli,"Device and service are registered");
#endif

  return (int)ATLK_OK;
}


