#ifndef __ATE_V2X_CLI_CAN_H__
#define __ATE_V2X_CLI_CAN_H__

#include <libcli.h>

#define DEFAULT_CAN_IF_INDEX        1


int cli_v2x_can_service_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_can_service_delete( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) );
int cli_v2x_can_socket_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_can_socket_delete( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) );
int cli_v2x_can_rx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_can_tx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );


int cli_v2x_can_reset_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)  );
int cli_v2x_can_print_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) );


#endif
