

#include "../v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mng_api.h"

/* MIB service */
mib_service_t 	*mib_service;


int cli_v2x_managment_init( void )
{
	atlk_rc_t 	rc        = ATLK_OK;

/* Get default MIB service */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    return (-1);
  }
	return 0;
}
mib_service_t 	*cli_v2x_managment_service_get( void )
{
	return mib_service;
}


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
  
  rc = mib_set_wlanDefaultTxDataRate(mib_service, if_idx, mib_data); 
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
  
  rc = mib_get_wlanDefaultTxDataRate(mib_service, if_idx, (int32_t*) &mib_data); 
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
  
  rc =  mib_set_wlanDefaultTxPower (mib_service, if_idx, mib_data); 
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
  
  rc =  mib_get_wlanDefaultTxPower(mib_service, if_idx, (int32_t*) &mib_data); 
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
  
  rc =  mib_set_wlanFrequency (mib_service, if_idx, mib_data); 
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
  
  rc =  mib_get_wlanFrequency(mib_service, if_idx, (int32_t*) &mib_data); 
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
  
  rc =  mib_get_wlanFrameRxCnt(mib_service, if_idx, (uint32_t*) &mib_data); 
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
  
  rc =  mib_get_wlanFrameTxCnt(mib_service, if_idx, (uint32_t*) &mib_data); 
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


int cli_v2x_get_wlanMacAddress( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t     rc        = ATLK_OK;
  
  int           if_idx    = ERR_VALUE;
  
	eui48_t				mac_addr = EUI48_ZERO_INIT;
             
  
  IS_HELP_ARG("get wlanMacAddress -if_idx 1|2");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanMacAddress(mib_service, if_idx, &mac_addr); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_get_wlanMacAddress failed for if %d,  %s\n", if_idx, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
		char      str_data[200] = "";

		sprintf(str_data,"%02x:%02x:%02x:%02x:%02x:%02x", 
					 mac_addr.octets[0], mac_addr.octets[1],
           mac_addr.octets[2], mac_addr.octets[3],
           mac_addr.octets[4], mac_addr.octets[5]);
    cli_print( cli, "MIB_VALUE : %s", str_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_set_wlanMacAddress( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
 atlk_rc_t rc        = ATLK_OK;
  
  int       if_idx    		= ERR_VALUE;
	char      str_data[200] = "";
	
	eui48_t		mac_addr;

  IS_HELP_ARG("set wlanMacAddress -if_idx 1|2 -addr xx:xx:xx:xx:xx:xx");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
	
	GET_STRING_VALUE("-addr", str_data,"WLAN Mac address");
	if ( strlen(str_data) ) {
      sscanf(str_data,"%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
           &mac_addr.octets[0], &mac_addr.octets[1],
           &mac_addr.octets[2], &mac_addr.octets[3],
           &mac_addr.octets[4], &mac_addr.octets[5]);
  }

  rc =  mib_set_wlanMacAddress (mib_service, if_idx, mac_addr); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR :  wlanFrequency  failed for if %d, %s\n", if_idx, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}
