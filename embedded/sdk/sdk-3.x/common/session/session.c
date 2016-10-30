


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef __LINUX__
#include <arpa/inet.h>
#endif
#include "../v2x_cli/v2x_cli.h"


    
//static v2x_se_t *v2x_se = NULL;   /* V2X session */

int cli_v2x_session_open( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{

  atlk_rc_t 		rc 					= ATLK_OK;
	v2x_se_attr_t v2x_se_attr = V2X_SE_ATTR_INIT;
	char        	str_data[256] = "";	
	int						i;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
	IS_HELP_ARG("session open -type local|remote [-server_addr ip_address]")
  CHECK_NUM_ARGS
	
 
	v2x_se_attr.se_type = NAV_SE_LOCAL;

	i = 0;
  GET_STRING("-type", str_data, i, "Specify session type, local or remote");
  
  if ( strcmp( (char*) str_data,  "local") == 0 ) {
    v2x_se_attr.se_type = NAV_SE_LOCAL;
  } 
  else if ( strcmp( (char*) str_data, "remote") == 0 ) {
    v2x_se_attr.se_type = V2X_SE_VTP_UDP_IP4;
  } 

	if ( v2x_se_attr.se_type == V2X_SE_VTP_UDP_IP4 ) {
		i = 2;
		GET_STRING("-server_addr", str_data, i, "Specify session type, local or remote");
#ifdef __LINUX__
		in_addr_t 		server_ip4_addr;
		server_ip4_addr = inet_addr(str_data);
		
		cli_print( cli, "Connecting to remote unit @ %s, raw : %s\n", inet_ntoa( *(struct in_addr *) &server_ip4_addr ), str_data );
		if ( server_ip4_addr == INADDR_NONE) {
			cli_print( cli, "Invalid ip address format %s\n", str_data );
			goto exit;
		}

		v2x_se_attr.se_type = V2X_SE_VTP_UDP_IP4;
		v2x_se_attr.server_ip4_addr = server_ip4_addr;
			
#elif defined (__THREADX__)
		cli_print( cli, "ERROR : emmbedded mode not support remote !!\n");
		goto exit;
	
#endif
	}
	
  /* Open V2X session */
  rc = v2x_se_open(&myctx->v2x_se, &v2x_se_attr);
  if (atlk_error(rc)) {
    cli_print(cli, "v2x_se_open: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }


exit:
  return atlk_error(rc);
}

int cli_v2x_session_close( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

  v2x_se_close(&myctx->v2x_se);
	myctx->v2x_se = NULL;
  
  return CLI_OK;
}


int cli_v2x_get_session( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  cli_print( cli, "Session : %x", (unsigned int) myctx->v2x_se);
  return CLI_OK;
}

int cli_v2x_set_session( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  unsigned int    addr    = 0;
  int							i       = 0;
  
	
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("session set -addr 0xNNNNN");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
	   GET_TYPE_INT("-addr", addr, unsigned int, i, "Address of current session ", "%x"); 
  } 
  
  if ( addr == 0 ) {
    cli_print( cli, "ERROR : address is incorrect %x\n", (int) addr );
    return CLI_ERROR;
  }
  else {
		v2x_se_t        *v2x_se		= NULL;

		v2x_se = (v2x_se_t*) addr;
		cli_print( cli, "Copy Session : %x", (unsigned int) v2x_se);
		myctx->v2x_se = v2x_se;
		
  }
  
  return CLI_OK;
}

