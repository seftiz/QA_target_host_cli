#ifndef __ATE_V2X_CLI_ECC_H__
#define __ATE_V2X_CLI_ECC_H__


#include <libcli.h>



/* Format string for ECC scalar */
#define ECC_SCALAR_FMT \
  "0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx"

/* Format argument list for ecc_scalar_t */
#define ECC_SCALAR_FMT_ARGS(x)                    \
  x.value[0], x.value[1], x.value[2], x.value[3], \
  x.value[4], x.value[5], x.value[6], x.value[7]

 
#define ACTION_VERIFY       "verify"
#define ACTION_SIGN         "sign"
#define ACTION_ALL          "all"

#define IS_VERIFY   (strcmp(action, ACTION_VERIFY)== 0)
#define IS_SIGN     (strcmp(action, ACTION_SIGN)== 0)
#define IS_ALL      (strcmp(action, ACTION_ALL)== 0)
 
#define PRIVATE_KEY_LEN  8
#define SIGNATURE_KEY_LEN  8
#define HASH_DATA_LEN    8

int cli_v2x_ecc_service_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_service_delete( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) );
int cli_v2x_ecc_curve_type_get( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_curve_type_set( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_socket_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_socket_delete( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );

int cli_v2x_ecc_sign_private_key( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_sign_request_send( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_sign_response_receive( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_verification_set_public_key( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_verification_request_send( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_ecc_verification_set_signature( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ); 

#endif