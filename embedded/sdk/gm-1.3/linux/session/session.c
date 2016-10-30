#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../libcli/libcli.h"
#include <atlk/v2x/wave.h>
#include "../v2x_cli/v2x_cli.h"

int cli_v2x_session_open( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  int       i   = 0;
  atlk_rc_t rc  = ATLK_OK;

  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

	IS_HELP_ARG("session open [-addr 0xNNNNN]");
  
  if ( myctx == NULL ) {
    cli_print( cli, "ERROR, user context retreive failed");
    return CLI_ERROR;
  }
 
  
  if ( argc > 1 ) {
    int           addr    = 0;

    /* get user context */
    user_context *myctx = (user_context *) cli_get_context(cli);

    CHECK_NUM_ARGS /* make sure all parameter are there */

    for ( i = 0 ; i < argc; i += 2 ) {
			GET_TYPE_INT("-addr", addr, i, "Address of current session ", "%x"); 
    } 

    if ( addr <= 0 ) {
      cli_print( cli, "ERROR : address is incorrect %x\n", (int) addr );
    return CLI_ERROR;
    }
    else {
			v2x_se_t        *v2x_se		= NULL;
			
			v2x_se = (v2x_se_t*) addr;
      memcpy( &myctx->v2x_se,  v2x_se, sizeof( v2x_se_t ) );
    }
  }
  else {
    /* Open V2X session */
		myctx->v2x_se = V2X_SK_INIT;
    rc = v2x_se_open(&myctx->v2x_se, NULL);
    if (atlk_error(rc)) {
      cli_print(cli, "v2x_se_open: %s\n", atlk_rc_to_str(rc));
      goto exit;
    }
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
  
  cli_print( cli, "Session : %x", (unsigned int) &myctx->v2x_se);
  return CLI_OK;
}

int cli_v2x_set_session( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  int       addr    = 0,
            i       = 0;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("session set -addr 0xNNNNN");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
	   GET_TYPE_INT("-addr", addr, i, "Address of current session ", "%x"); 
  } 
  
  if ( addr <= 0 ) {
    cli_print( cli, "ERROR : address is incorrect %x\n", (int) addr );
    return CLI_ERROR;
  }
  else {
		v2x_se_t        *v2x_se		= NULL;

		v2x_se = (v2x_se_t*) addr;
		memcpy( &myctx->v2x_se,  v2x_se, sizeof( v2x_se_t ) );
  }
  
  return CLI_OK;
}

