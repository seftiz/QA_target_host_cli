

#include "../v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "nav_api.h"


/* Nav thread */


/* declare float convert to str */
char *ftoa(char *outbuf, float f);

/* Navigation data session */
int cli_v2x_nav_init( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc = ATLK_OK;
  
  /* Navigation data session attributes */
  nav_se_attr_t se_attr     = NAV_SE_ATTR_INIT;
  char        str_data[256] = "";
  int32_t     i             = 0;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

  
  IS_HELP_ARG("nav init -type local|remote [-server_addr ip_address]")
  CHECK_NUM_ARGS
 
  i = 0;
  GET_STRING("-type", str_data, i, "Specify session type, local or remote");
  
  if ( strcmp( (char*) str_data,  "local") == 0 ) {
    se_attr.se_type = NAV_SE_LOCAL;
  } 
  else if ( strcmp( (char*) str_data, "remote") == 0 ) {
    //in_addr_t addr;
     
    i+=2;
    se_attr.se_type = NAV_SE_CSV_UDP_IP4;
  } 
  else {
    se_attr.se_type = NAV_SE_LOCAL;
  }

  
  /* Open Nav session for TX thread */
  rc = nav_se_open( &(myctx->nav_data.nav_se), &se_attr);
  if (atlk_error(rc)) {
    cli_print(cli, "nav_se_open failed (%s)\n", atlk_rc_to_str(rc) );
    goto exit;
  }
  
  /* save user context */
  //cli_set_context(cli, (void*)&myctx);
 
exit:
  return atlk_error(rc);
}

#define NAV_THREAD_PRIO 10
#define NAV_THREAD_TIME_SLICE 50

int cli_v2x_nav_start( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  //ULONG         trv;
  atlk_rc_t rc = ATLK_OK;
  nav_fix_t nav_fix = NAV_FIX_INIT;

  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  myctx->nav_data.loop_flag = 1;
   
  while (myctx->nav_data.loop_flag) {
    
    // Receive new navigation fix, if any
    rc = nav_se_fix_rx( myctx->nav_data.nav_se, &nav_fix, NULL);
    if (atlk_error(rc) && (rc != ATLK_E_NOT_READY)) {
      cli_print(cli,"nav_se_fix_rx: %s\n", atlk_rc_to_str(rc));
    }
    else if ( rc == ATLK_E_NOT_READY ) {
      usleep(100000); /* Sleep 100 mSec */
    }
    else {
      print_nav_fix_data( cli, &nav_fix );
    }
    
    usleep(10000); /* Sleep 10 mSec */
    
  }
  /*
  trv = tx_thread_create(&(myctx->nav_data.nav_thread), "nav rx thread",
                         nav_rx_loop, (ULONG) cli,
                         myctx->nav_data.nav_thread_stack, sizeof(myctx->nav_data.nav_thread_stack),
                         NAV_THREAD_PRIO, NAV_THREAD_PRIO,
                         NAV_THREAD_TIME_SLICE, TX_AUTO_START);
  if (trv != TX_SUCCESS) {
    cli_print(cli, "tx_thread_create failed, trv=0x%lx\n", trv);
    return CLI_ERROR;
  }
  */
 
  return CLI_OK;

}

int cli_v2x_nav_stop( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

  myctx->nav_data.loop_flag = 1;

  return CLI_OK;
}

void print_nav_fix_data( void *context, const nav_fix_t *fix)
{
  
  struct cli_def *cli     = (struct cli_def *) context;
  char  line[400]         = "";
  double posix_time       = 0.0;
  
  posix_time = nav_time_to_posix_time (&fix->time);
  /* Use navigation fix */
  
  sprintf(line , "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", posix_time, 
                fix->position_latitude_deg,
                fix->position_longitude_deg,
                fix->position_altitude_m,
                fix->movement_horizontal_direction_deg,
                fix->movement_horizontal_speed_mps,
                fix->movement_vertical_speed_mps,
                fix->error_time_s,
                fix->error_position_horizontal_major_axis_direction_deg,
                fix->error_position_horizontal_semi_major_axis_length_m,
                fix->error_position_horizontal_semi_minor_axis_length_m,
                fix->error_position_altitude_m,
                fix->error_movement_horizontal_direction_deg,
                fix->error_movement_horizontal_speed_mps,
                fix->error_movement_vertical_speed_mps);
  
  cli_print(cli, "%s", line);
}

