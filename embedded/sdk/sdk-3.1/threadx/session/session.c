


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../v2x_cli/v2x_cli.h"


    
//static v2x_se_t *v2x_se = NULL;   /* V2X session */

int cli_v2x_session_open( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  /* V2X API return code */
  atlk_rc_t rc = ATLK_OK;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  if ( myctx == NULL ) {
    cli_print( cli, "ERROR, user context retreive failed");
    return CLI_ERROR;
  }
  else {
     cli_print( cli, "user context test %x", myctx->test); 
  }
  
  /* Open V2X session */
  rc = v2x_se_open(&myctx->v2x_se, NULL);
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

