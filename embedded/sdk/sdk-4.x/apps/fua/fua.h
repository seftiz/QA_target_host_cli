#ifndef __ATE_V2X_CLI_FUA_H__
#define __ATE_V2X_CLI_FUA_H__

#include <libcli.h>

#define DEFAULT_FUA_IF_INDEX        0


void cli_v2x_fua_init(struct cli_def *cli, const char *command, char *argv[], int argc);


#endif
