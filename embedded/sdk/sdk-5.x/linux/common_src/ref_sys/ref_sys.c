#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/stat.h>

#include <atlk/ddm_service.h>

#include <link_layer.h>
#include <target_type.h>
#include <ref_sys.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DSP_PATH "./dsp_sw.bin"
#define PARAMS_PATH "./module_params.conf"
#define CALIB_PATH "./calib_file.bin"

/**
   Currently SECTON MAC address is constant
*/
static const eui48_t ref_sys_secton_mac_address = {
  .octets = {0x00, 0x02, 0xcc, 0xf0, 0x00, 0x07},
};

static atlk_rc_t
ref_sys_read_file_content(const char *path, char **buf_ptr, size_t *size)
{
  int f;
  int rv;
  struct stat file_stat;
  ssize_t read_size;
  atlk_rc_t rc;

  rv = stat(path, &file_stat);

  if (rv == -1) {
    fprintf(stderr, "%s: ", path);
    perror("Unable to stat");
    errno = 0;
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
    rc = ATLK_E_UNSPECIFIED;
    goto err;
  }

  read_size = read(f, *buf_ptr, *size);
  if (read_size < 0) {
    perror("read failed");
    rc = ATLK_E_UNSPECIFIED;
    goto err_with_fclose;
  }

  if ((size_t)read_size != *size) {
    printf("Supposed to read %zu bytes, instead read %zu bytes from %s\n",
           *size, read_size, path);
    rc = ATLK_E_UNSPECIFIED;
    goto err_with_fclose;
  }

  close(f);

  return ATLK_OK;

err_with_fclose:
  close(f);

err:
  free(buf_ptr);
  *buf_ptr = NULL;
  *size = 0;

  return rc;
}

static atlk_rc_t
ddm_config_init(const char *ceva_path,
                const char *module_params_path,
                const char *calib_path,
                ddm_configure_t *config)
{
  atlk_rc_t rc;

  if (config == NULL) {
    fprintf(stderr, "config parameter is NULL\n");
    return ATLK_E_INVALID_ARG;
  }

  rc = ref_sys_read_file_content(ceva_path,
                                 &config->binary_buffer_ptr,
                                 &config->binary_size);
  if (atlk_error(rc)) {
    return rc;
  }

  ref_sys_read_file_content(module_params_path,
                            &config->module_params_ptr,
                            &config->module_params_size);

  rc = ref_sys_read_file_content(calib_path,
                                 &config->calib_buffer_ptr,
                                 &config->calib_size);
  if (atlk_error(rc)) {
	 (void)fprintf(stderr, "%s file is missing\n", calib_path);
  }

  return ATLK_OK;
}

/* Convert ddm_status_t to string */
static inline const char *
ref_sys_ddm_state_to_str(ddm_status_t state)
{
  static const char *ddm_state_str[] = {
    [DDM_STATUS_DISCONNECTED] = "DDM_STATUS_DISCONNECTED",
    [DDM_STATUS_UNINITIALIZED] = "DDM_STATUS_UNINITIALIZED",
    [DDM_STATUS_READY] = "DDM_STATUS_READY",
  };

  /* Check if rc is out of bounds */
  if (atlk_unlikely(state >= ARRAY_SIZE(ddm_state_str))) {
    return "Unknown DDM State";
  }

  return ddm_state_str[state];
}

static void
status_change_handler(ddm_service_t *service_ptr, ddm_status_t status)
{
  (void)service_ptr;

  if (status != DDM_STATUS_READY) {
    (void)fprintf(stderr, "DDM status changed to %s\n",
                  ref_sys_ddm_state_to_str(status));
  }
}

static atlk_rc_t
ref_sys_ddm_init(ddm_service_t* ddm_service_ptr,
                 const char *ceva_path,
                 const char *module_params_path,
                 const char *calib_path)
{
  int retries = 3;
  ddm_configure_t ddm_config = DDM_CONFIGURE_INIT;
  atlk_wait_t wait;
  atlk_rc_t rc;
  ddm_status_t status;

  /* Set up device configuration */
  rc = ddm_config_init(ceva_path,
                       module_params_path,
                       calib_path,
                       &ddm_config);
  if (atlk_error(rc)) {
    fprintf(stderr,
            "\n** Device config files missing, "\
            "not initializing device ** \n\n");
  }

  while(retries > 0) {
    --retries;

    rc = ddm_status_get(ddm_service_ptr, &status, &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr,"Invalid state\n");
      return rc;
    }

    if (status == DDM_STATUS_DISCONNECTED) {
      usleep(1000000);
      continue;
    }
  }

  if (status == DDM_STATUS_DISCONNECTED) {
    return ATLK_E_INVALID_STATE;
  }

  wait.wait_type = ATLK_WAIT_INTERVAL;
  wait.wait_usec = 1000000;

  rc = ddm_configuration_set(ddm_service_ptr,
                             &ddm_config,
                             &wait);
  if (atlk_error(rc)) {
    switch (rc) {
    case ATLK_E_EXISTS:
      (void)fprintf(stderr, "Configuration already loaded\n");
      rc = ATLK_OK;
      break;

    case ATLK_E_INVALID_STATE:
      (void)fprintf(stderr,
                    "ddm_configuration_set invalid state, retrying...\n");
      break;

    case ATLK_E_TIMEOUT:
      (void)fprintf(stderr,
                    "ddm_configuration_set timedout, retrying...\n");
      break;

    default:
      (void)fprintf(stderr, "ddm_configuration_set failed, rc = %d\n", rc);
      return EXIT_FAILURE;
    }
  }

  rc = ddm_status_change_notify_register(ddm_service_ptr,
                                         status_change_handler);
  if (atlk_error(rc)) {
    (void)fprintf(stderr, "ddm_status_change_notify_register failed: %s\n",
                  atlk_rc_to_str(rc));
    return EXIT_FAILURE;
  }

  return ATLK_OK;
}

static atlk_rc_t
ref_sys_device_register(const char *ref_device_name,
                        const char *interface_name)
{
#define SECTON_DEVICE_NAME "Secton"
#define CRATON2_DEVICE_NAME "Craton2"
#define SERVICES_COUNT 4
  atlk_rc_t rc;
  dsm_device_config_t dsm_device;
  dsm_service_config_t dsm_service[SERVICES_COUNT] = {{0}};
  target_type_t target;
  const char *device_name;
  size_t i;

  dsm_device.rdev_ptr = ll_get(interface_name);
  if (dsm_device.rdev_ptr == NULL) {
    fprintf(stderr, "remote device pointer is NULL\n");
    return ATLK_E_NOT_FOUND;
  }

  target = target_type_get();

  switch (target) {
    case TARGET_TYPE_SECTON:
      device_name = SECTON_DEVICE_NAME;
      dsm_device.device_type = DSM_DEVICE_TYPE_SECTON;
      break;
    case TARGET_TYPE_CRATON2:
      device_name = CRATON2_DEVICE_NAME;
      dsm_device.device_type = DSM_DEVICE_TYPE_CRATON2;
      break;
    default:
      return ATLK_E_INVALID_ARG;
  }

  if (ref_device_name != NULL) {
    /* Override with the name came from ref sys */
    device_name = ref_device_name;
  }

  memcpy(&dsm_device.rdev_ptr->rdev_address,
         &ref_sys_secton_mac_address,
         EUI48_LEN);

  dsm_device.device_name = device_name;

  rc = dsm_device_register(&dsm_device, 1);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = dsm_device_init(dsm_device.device_name, ATLK_FOREVER);
  if (atlk_error(rc)) {
    return rc;
  }

  dsm_service[0].service_name = "V2X";
  dsm_service[0].service_type = DSM_SERVICE_TYPE_V2X;
  /* The name bundles the service to the specific device */
  dsm_service[0].device_name = device_name;

  dsm_service[1].service_name = "DEV";
  dsm_service[1].service_type = DSM_SERVICE_TYPE_DDM;
  /* The name bundles the service to the specific device */
  dsm_service[1].device_name = device_name;

  dsm_service[2].service_name = "WDM";
  dsm_service[2].service_type = DSM_SERVICE_TYPE_WDM;
  /* The name bundles the service to the specific device */
  dsm_service[2].device_name = device_name;

  dsm_service[3].service_name = "ECC";
  dsm_service[3].service_type = DSM_SERVICE_TYPE_ECC;
  /* The name bundles the service to the specific device */
  dsm_service[3].device_name = device_name;
#if 0
  dsm_service[4].service_name = "IVN";
  dsm_service[4].service_type = DSM_SERVICE_TYPE_IVN;
  /* The name bundles the service to the specific device */
  dsm_service[4].device_name = device_name;
#endif
  rc = dsm_service_register(dsm_service, ARRAY_SIZE(dsm_service));
  if (atlk_error(rc)) {
    return rc;
  }

  for (i = 0; i < ARRAY_SIZE(dsm_service); i++) {
    rc = dsm_service_init(dsm_service[i].service_name, ATLK_FOREVER);
    if (atlk_error(rc)) {
      return rc;
    }
  }

  printf("Device and service are registered and initialized\n");

  return ATLK_OK;
}

atlk_rc_t
ref_sys_init(int argc, char *argv[])
{
  int rv;
  atlk_rc_t rc;
  target_type_t target_type;
  ddm_service_t *ddm_service_ptr;
  char *remote_interface_name = NULL;

  target_type = target_type_get();

  if (target_type == TARGET_TYPE_SECTON) {
    if (argc < 2) {
      fprintf(stderr, "Missing remote interface name\n");
      return ATLK_E_INVALID_ARG;
    }
    else {
      remote_interface_name = argv[1];
    }
  }

  rv = ll_init(remote_interface_name);
  if (rv < 0) {
    fprintf(stderr, "Link layer initialization filed, err=%d\n", rv);
    return ATLK_E_UNSPECIFIED;
  }

  rc = ref_sys_device_register(NULL, remote_interface_name);
  if (atlk_error(rc)) {
  	fprintf(stderr, "ref_sys_device_register filed, err=%d\n", rv);
    return rc;
  }

  rc = ddm_service_get(NULL,
                       &ddm_service_ptr);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = ref_sys_ddm_init(ddm_service_ptr,
                        DSP_PATH,
                        PARAMS_PATH,
                        CALIB_PATH);
  if (atlk_error(rc)) {
    return rc;
  }

  return ATLK_OK;
}
