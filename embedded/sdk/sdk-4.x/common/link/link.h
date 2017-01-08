#ifndef __ATE_V2X_CLI_LINK_H__
#define __ATE_V2X_CLI_LINK_H__


#include "../../linux/libcli/libcli.h"


int cli_v2x_link_socket_create( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_socket_delete( struct cli_def *cli, const char *command, char *argv[], int argc );


int cli_v2x_link_service_create( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_service_delete( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_test_v2x_link_service_get (struct cli_def *cli, const char *command, char *argv[], int argc); // chrub

int cli_v2x_link_tx( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_rx( struct cli_def *cli, const char *command, char *argv[], int argc );

int cli_v2x_link_reset_cntrs( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_print_cntrs( struct cli_def *cli, const char *command, char *argv[], int argc );

int cli_v2x_get_link_socket_addr( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_set_link_socket_addr( struct cli_def *cli, const char *command, char *argv[], int argc );

int cli_test_v2x_link_dot4_channel_start( struct cli_def *cli, const char *command, char *argv[], int argc );   // chrub
int cli_test_v2x_link_dot4_channel_end( struct cli_def *cli, const char *command, char *argv[], int argc );   // chrub
int cli_test_v2x_link_dot4_channel_end_receive( struct cli_def *cli, const char *command, char *argv[], int argc );   // chrub

//int cli_test_v2x_link_netif_profile_set( struct cli_def *cli, const char *command, char *argv[], int argc );   // chrub - not checked (Zohar's requirement)
int cli_test_v2x_link_send( struct cli_def *cli, const char *command, char *argv[], int argc );   // chrub

int cli_test_v2x_link_receive( struct cli_def *cli, const char *command, char *argv[], int argc ); //chrub
int cli_test_v2x_link_socket_create( struct cli_def *cli, const char *command, char *argv[], int argc );

//int cli_test_v2x_link_sample_subscriber_create( struct cli_def *cli, const char *command, char *argv[], int argc ); //chrub - not checked (Zohar's requirement)

//int cli_test_v2x_link_sample_subscriber_delete( struct cli_def *cli, const char *command, char *argv[], int argc );//chrub - not checked (Zohar's requirement)

//int cli_test_v2x_link_sample_int32_receive( struct cli_def *cli, const char *command, char *argv[], int argc );//chrub - not checked (Zohar's requirement)
#endif /* __ATE_V2X_CLI_LINK_H__ */
