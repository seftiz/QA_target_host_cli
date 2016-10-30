

#include "../v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mng_api.h"

/*
cli_register_command(*cli, c, "wlanDefaultTxDataRate", cli_v2x_set_wlanDefaultTxDataRate, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set default transmission data rate.");
cli_register_command(*cli, c, "wlanDefaultTxPower", cli_v2x_set_wlanDefaultTxDataRate, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set default transmission power.");
cli_register_command(*cli, c, "wlanFrequency", cli_v2x_set_wlanFrequency, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set current RX and TX frequency");
 
cli_register_command(*cli, c, "wlanDefaultTxDataRate", cli_v2x_get_wlanDefaultTxDataRate, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get default transmission data rate.");
cli_register_command(*cli, c, "wlanDefaultTxPower", cli_v2x_get_wlanDefaultTxPower, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get default transmission power.");
cli_register_command(*cli, c, "wlanFrameRxCnt", cli_v2x_get_wlanFrameRxCnt, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get received frame counter");
cli_register_command(*cli, c, "wlanFrameTxCnt", cli_v2x_get_wlanFrameTxCnt, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get transmitted frame counter");
cli_register_command(*cli, c, "wlanFrequency", cli_v2x_get_wlanDefaultTxDataRate, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get current RX and TX frequency");
  
 */

mib_info_st mng_mib_table[] = { 
                          {MIB_GET, "wlanDefaultTxDataRate",  MIB_GET_TYPE mib_get_wlanDefaultTxDataRate,2},
                          {MIB_GET, "wlanDefaultTxPower", MIB_GET_TYPE mib_get_wlanDefaultTxPower,2},
                          {-1, NULL, NULL,-1}
};

int cli_v2x_set_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int       if_idx    = ERR_VALUE, 
            mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("set wlanDefaultTxDataRate -if_idx 1|2 -value 0-100");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }

  GET_INT("-value", mib_data, 2, "Tx datarate");
  if ( mib_data == ERR_VALUE ) {
    cli_print(cli, "ERROR : tx_rate is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc = mib_set_wlanDefaultTxDataRate(NULL, if_idx, mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int     if_idx    = ERR_VALUE, 
          mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("get wlanDefaultTxDataRate -if_idx 1|2");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc = mib_get_wlanDefaultTxDataRate(NULL, if_idx, (int32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %d", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

  
  
  
int cli_v2x_set_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int       if_idx    = ERR_VALUE, 
            mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("set wlanDefaultTxPower -if_idx 1|2 -value -100 - 20");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }

  GET_INT("-value", mib_data, 2, "Tx power");
  if ( mib_data == ERR_VALUE ) {
    cli_print(cli, "ERROR : tx_rate is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_set_wlanDefaultTxPower (NULL, if_idx, mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR :  mib_set_wlanDefaultTxPower  failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int     if_idx    = ERR_VALUE, 
          mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("get wlanDefaultTxPower -if_idx 1|2");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanDefaultTxPower(NULL, if_idx, (int32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %d", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_set_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
 atlk_rc_t rc        = ATLK_OK;
  
  int       if_idx    = ERR_VALUE, 
            mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("set wlanFrequency -if_idx 1|2 -value 5800 - 6000");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }

  GET_INT("-value", mib_data, 2, "RX and TX frequency in MHz");
  if ( mib_data == ERR_VALUE ) {
    cli_print(cli, "ERROR : freq_mhz is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_set_wlanFrequency (NULL, if_idx, mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR :  wlanFrequency  failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
 atlk_rc_t rc        = ATLK_OK;
  
  int     if_idx    = ERR_VALUE, 
          mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("get wlanFrequency -if_idx 1|2");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanFrequency(NULL, if_idx, (int32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %d", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanFrameRxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
 atlk_rc_t      rc        = ATLK_OK;
  
  int           if_idx    = ERR_VALUE;
  
  unsigned int  mib_data  = ERR_VALUE;
            
  
  IS_HELP_ARG("get wlanFrameRxCnt -if_idx 1|2");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanFrameRxCnt(NULL, if_idx, (uint32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : wlanFrameRxCnt failed for if %d and value %u: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %u", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanFrameTxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t     rc        = ATLK_OK;
  
  int           if_idx    = ERR_VALUE;
  
  unsigned int  mib_data  = ERR_VALUE;
             
  
  IS_HELP_ARG("get wlanFrameTxCnt -if_idx 1|2");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanFrameTxCnt(NULL, if_idx, (uint32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : wlanFrameTxCnt failed for if %d and value %u: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %u", mib_data );
  }
   
exit:
  return atlk_error(rc);
}
