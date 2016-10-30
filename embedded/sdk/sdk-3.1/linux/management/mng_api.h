
#ifndef __ATE_V2X_CLI_MNG_API_H__
#define __ATE_V2X_CLI_MNG_API_H__

#include <libcli.h>
#include "../v2x_cli/v2x_cli.h"
#include <atlk/mibs/wlan-mib.h>


typedef atlk_rc_t (*mib_get_func)( mib_mgr_t *, int32_t, int32_t *) ;
typedef atlk_rc_t (*mib_set_func)( mib_mgr_t *, int32_t, int32_t) ;
typedef atlk_rc_t (*mib)( void ) ;


typedef enum {

  MIB_SET = 1,
  MIB_GET
  
} mib_func_type_et;


typedef struct {
  
  mib_func_type_et    mib_type;
  char                *mib_name;
  mib                 mib_func;
  int                 num_param;

} mib_info_st;

#define MIB_GET_TYPE (mib) (mib_get_func)

int cli_v2x_set_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_set_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_set_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int cli_v2x_get_wlanFrameRxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ); 
int cli_v2x_get_wlanFrameTxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );

#endif

