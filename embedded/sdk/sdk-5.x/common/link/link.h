#ifndef __ATE_V2X_CLI_LINK_H__
#define __ATE_V2X_CLI_LINK_H__


#include "../../linux/libcli/libcli.h"

//#include ".././src/include/dbg.h"
#include <atlk/wdm.h>

int cli_v2x_cmd_sdk_version( struct cli_def *cli, const char *command, char *argv[], int argc );

int cli_v2x_link_socket_create( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_socket_delete( struct cli_def *cli, const char *command, char *argv[], int argc );


int cli_v2x_link_service_create( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_service_delete( struct cli_def *cli, const char *command, char *argv[], int argc );

int cli_v2x_link_tx( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_rx( struct cli_def *cli, const char *command, char *argv[], int argc );

int cli_v2x_link_reset_cntrs( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_print_cntrs( struct cli_def *cli, const char *command, char *argv[], int argc );

int cli_v2x_get_link_socket_addr( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_set_link_socket_addr( struct cli_def *cli, const char *command, char *argv[], int argc );
int cli_v2x_link_rxtx_start(struct cli_def *cli, const char *command, char *argv[], int argc);
int cli_v2x_link_rxtx_stop(struct cli_def *cli, const char *command, char *argv[], int argc);
int cli_v2x_dot4_channel_start_req(struct cli_def *cli, const char *command, char *argv[], int argc);
int cli_v2x_dot4_channel_end_req(struct cli_def *cli, const char *command, char *argv[], int argc);
int cli_v2x_wave6_rx(struct cli_def *cli, const char *command, char *argv[], int argc);
int cli_v2x_wave6_init(struct cli_def *cli, const char *command, char *argv[], int argc);
int cli_v2x_wave6_tx(struct cli_def *cli, const char *command, char *argv[], int argc);

atlk_rc_t diag_wlan_stat(wdm_service_t *wdm_service, int32_t wlan_if_index, wdm_stats_t *wlan_stat);
//int cli_v2x_netif_profile_set( struct cli_def *cli, const char *command, char *argv[], int argc ); //chrub

int cli_v2x_cmd_sdk_version(struct cli_def *cli, const char *command, char *argv[], int argc);

int cli_v2x_link_rx_thread_stop(struct cli_def *cli, const char *command, char *argv[], int argc);
int cli_v2x_link_tx_thread_stop(struct cli_def *cli, const char *command, char *argv[], int argc);

int v2x_set_freq(struct cli_def *cli, const char *command, char *argv[], int argc);


#endif /* __ATE_V2X_CLI_LINK_H__ */
