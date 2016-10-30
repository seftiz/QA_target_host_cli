#ifndef __ATE_V2X_CLI_LINK_H__
#define __ATE_V2X_CLI_LINK_H__


#include <libcli.h>


int cli_v2x_link_sk_open( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_link_sk_close( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_link_tx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_link_rx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );

int cli_v2x_link_reset_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)  ) ;
int cli_v2x_link_print_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) );



#endif /* __ATE_V2X_CLI_LINK_H__ */