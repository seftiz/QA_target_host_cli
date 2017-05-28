
#include <atlk/common.h>
#include "../../common/general/general.h"
#include "../../common/v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <atlk/can.h>
#include "can.h"

#include <craton/can_fua.h>


/* CAN FUA configuration parameters */
#define CANFUA_THREAD_PRIO 20
#define CANFUA_THREAD_TIME_SLICE 0
#define CANFUA_DEVICE_ID 0
#define CANFUA_COMMAND_ID 12
#define CANFUA_RESPONSE_ID 34
#define CANFUA_BUFFER_SIZE 0x200000
static uint8_t canfuabuf[CANFUA_BUFFER_SIZE];

/* CAN FUA configuration structure values */
#define CAN_FUA_CONFIG {{CANFUA_THREAD_PRIO, CANFUA_THREAD_TIME_SLICE},\
 CANFUA_DEVICE_ID, CANFUA_COMMAND_ID, CANFUA_RESPONSE_ID,\
 canfuabuf, CANFUA_BUFFER_SIZE}


void cli_v2x_fua_init(struct cli_def *cli, const char *command, char *argv[], int argc)
{

	atlk_rc_t 		rc = ATLK_OK;
	char          str_data[256] = "hw";
	int32_t       i = 0;
	can_fua_config_t can_fua_config = CAN_FUA_CONFIG;
	
	/* get user context */
	user_context *myctx = (user_context *)cli_get_context(cli);
	(void)myctx; /* not used  */
	(void)command;

	IS_HELP_ARG("FW update agent initialization");
	
	rc = can_fua_init(&can_fua_config);
	if (atlk_error(rc)) {
		cli_print(cli, "fua init failed (%s)", atlk_rc_to_str(rc));
		goto error;
	}

	return CLI_OK;

error:
	return atlk_error(rc);
}
