
#ifndef __ATE_V2X_CLI_MNG_API_H__
#define __ATE_V2X_CLI_MNG_API_H__

#include "../../linux/libcli/libcli.h"
#include "../v2x_cli/v2x_cli.h"

#include <atlk/mib_service.h>
//#include <atlk/remote.h>
#include <atlk/mibs/wlan-mib.h>
#include <atlk/mibs/snmpv2-mib.h>
#include "../../linux/remote/remote.h"

int cli_v2x_managment_init( void );
int cli_v2x_managment_service_create( struct cli_def *cli, const char *command, char *argv[], int argc  );
int cli_v2x_managment_service_delete( struct cli_def *cli, const char *command, char *argv[], int argc  );

mib_service_t 	*cli_v2x_managment_service_get( void );

int cli_v2x_mibApi_testGet( struct cli_def *cli, UNUSED(const char *command), char *argv[], UNUSED(int argc) );
int cli_v2x_mibApi_testSet( struct cli_def *cli, UNUSED(const char *command), char *argv[], UNUSED(int argc) );

int cli_v2x_set_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_set_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_set_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanFrameRxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ); 
int cli_v2x_get_wlanFrameTxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanMacAddress( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_set_wlanMacAddress( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );

#endif

