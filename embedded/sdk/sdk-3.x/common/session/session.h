
#ifndef __ATE_V2X_CLI_SESSION_H__
#define __ATE_V2X_CLI_SESSION_H__

#include "../v2x_cli/v2x_cli.h"


int cli_v2x_session_open( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_session_close( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ); 

int cli_v2x_get_session( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) );
int cli_v2x_set_session( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );


#endif


