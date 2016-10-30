

#include "../v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
//#include "../session/session.h"
#include "wsmp.h"


#define DEFAULT_PSID  0xc01234
    
/* Globals */

/* Shared WSMP socket */
//static v2x_wsmp_sk_t *toysa_wsmp_sk = NULL;

/* ToySA message format string: QA #<seq_num> <latitude> <longitude> */
static const char qa_msg_fmt[] = "QA #%lu %.7lf %.7lf";

/* ToySA message string maximum length */
static const size_t qa_msg_size_max = sizeof(qa_msg_fmt) + 30;


int cli_v2x_wsmp_sk_open( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  v2x_wsmp_sk_attr_t    wsmp_sk_attr = V2X_WSMP_SK_ATTR_INIT; /* WSMP socket attributes */
  int32_t               i     = 0;
  atlk_rc_t             rc    = ATLK_OK;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

 
  wsmp_sk_attr.psid = DEFAULT_PSID;
  wsmp_sk_attr.sk_attr.rx_disable = 1;
  
  IS_HELP_ARG("wsmp open [-psid 1234] [disable-rx true|false]")

  CHECK_NUM_ARGS /* make sure all parameter are there */
  
  /*
  if (mode == SEND) {
    wsmp_sk_attr.sk_attr.disable_rx = 1;
  }
  */
  for ( i = 0 ; i < argc; i += 2 ) {
    
    GET_INT("-psid", wsmp_sk_attr.psid, i, "Specify the socket psid");
    GET_INT("-disable-rx", wsmp_sk_attr.sk_attr.rx_disable, i, "Specify if socket is for sending only");
    
  }
  
  rc = v2x_wsmp_sk_open(&myctx->wsmp_sk, myctx->v2x_se , &wsmp_sk_attr);
  if (atlk_error(rc)) {
    cli_print(cli, "v2x_wsmp_sk_open: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }
  
exit:
  return atlk_error(rc);
}

int cli_v2x_wsmp_sk_close( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

  
  v2x_wsmp_sk_close(&myctx->wsmp_sk);
  myctx->wsmp_sk = NULL;
  cli_print(cli, "Socket closed\n" );
 
  return CLI_OK;
  
}

int cli_v2x_wsmp_tx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
  int32_t       num_frames = 1;     /* total num frames to send */
  int32_t       rate_hz = 1;        /* rate of sending frames   */

  /* V2X API return code */
  atlk_rc_t      rc = ATLK_OK;
  
  /* WSMP TX descriptor */
  v2x_wsmp_sk_tx_t wsmp_sk_tx = V2X_WSMP_SK_TX_INIT;
  
  char          str_data[50] = "",
                tx_data[256] = "";
  int i, msg_size;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  
  IS_HELP_ARG("send_frame wsmp [-frames 1- ...] [-rate_hz 1 - ...] [-tx_data 'ddddd'] [-data_rate 54,54,45] [-interface_idx 0|1] [power_dbm8 -20-20] [-psid 0xc01234] \n \
                                            [elem_id 1-23] [-tx_power_used -100-80] [-element_data_rate 1]" );
                                            
  CHECK_NUM_ARGS /* make sure all parameter are there */
  
  GET_STRING_VALUE("-tx_data", tx_data,"Define data to send over the wsmp layer");

  for ( i = 0 ; i < argc; i += 2 ) {
    
    GET_INT("-frames", num_frames, i, "Specify the number of frames to send");
    GET_INT("-rate_hz", rate_hz, i, "Specify the rate of frames to send");
    /* mac information override defaults */
    GET_INT("-user_priority", wsmp_sk_tx.user_prio, i, "Specify the frame user priority, range 0-7");
    GET_INT("-data_rate", wsmp_sk_tx.datarate, i, "Specify the frame user priority, range 0:7");
    GET_INT("-if_idx", wsmp_sk_tx.if_index, i, "Sets mac interface idx to transmit from, 0-1");
    GET_INT("-power_dbm8", wsmp_sk_tx.power_dbm8, i, "Sets the mac interface to transmit from");
    /* wsmp tx parameters */
    GET_INT("-psid", wsmp_sk_tx.psid, i, "set wsmp psid value");
    GET_INT("-set_tx_power_used", wsmp_sk_tx.ext_elem.set_tx_power_used, i, "set tx_power_used field in wsmp message");
    GET_INT("-element_data_rate", wsmp_sk_tx.ext_elem.set_datarate, i, "set tx_power_used field in wsmp message");
    GET_STRING("-dest_addr", str_data, i, "Set destination mac address"); 
  } 
  if ( strlen(str_data) ) {
      sscanf(str_data,"%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
           &wsmp_sk_tx.dest_addr.octets[0], &wsmp_sk_tx.dest_addr.octets[1],
           &wsmp_sk_tx.dest_addr.octets[2], &wsmp_sk_tx.dest_addr.octets[3],
           &wsmp_sk_tx.dest_addr.octets[4], &wsmp_sk_tx.dest_addr.octets[5]);
  }
 
  GET_STRING_VALUE("-tx_data", tx_data,"Define data to send over the link layer");
  if ( !strlen(tx_data) ) {
    sprintf(tx_data, "qa testing string default");
  }
  msg_size = strlen(tx_data) + 1;

 
  for (i = 0; i < num_frames; ++i) {
    /* TX buffer descriptor */
    v2x_buf_t tx_buf = V2X_BUF_INIT;

    /* Allocate TX buffer */
    rc = v2x_wsmp_sk_buf_alloc(myctx->wsmp_sk, &tx_buf, msg_size, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_wsmp_sk_buf_alloc: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
    
    size_t size = snprintf(v2x_buf_ptr(&tx_buf), v2x_buf_size(&tx_buf), "%s ", tx_data);
    rc = v2x_buf_size_set(&tx_buf, size);
  
    /* Transmit WSMP PDU (buffer is freed regardless of operation success) */
    rc = v2x_wsmp_sk_tx(myctx->wsmp_sk, &tx_buf, &wsmp_sk_tx, NULL);
    if (atlk_error(rc)) {
      cli_print(cli, "ERROR : v2x_wsmp_sk_tx: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
 
    /* Sleep 100 ms between transmissions */
    if ( (num_frames >= 1) && (rate_hz >= 1) ){
      int sleep_time_uSec = (1e6 / rate_hz );
      usleep( sleep_time_uSec );
    }
  }

error:
  return rc;

}

int cli_v2x_wsmp_rx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
 
  /* Received LINK descriptor */
  atlk_rc_t      rc = ATLK_OK;

  /* Received WSMP descriptor */
  v2x_wsmp_sk_rx_t wsmp_sk_rx = V2X_WSMP_SK_RX_INIT;
 
  int32_t         frames   = 1,
                  i        = 0,
                  timeout  = 20;
                  
  
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("wsmp rx [-frames 1-N -time_out sec]");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", frames, i, "Specify the number of frames to receive");
    GET_INT("-timeout", timeout, i, "Define timeout of last frame");
  } 
  i = 0;
  cli_print( cli, "Note : System will wait for %d frames or timeout %d", (int) frames, (int) timeout );
  
  
  while (frames--) {
     
    /* RX buffer descriptor */
    v2x_buf_t buf = V2X_BUF_INIT;
    
    rc = v2x_wsmp_sk_rx(myctx->wsmp_sk, &buf, &wsmp_sk_rx, &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_wsmp_sk_rx: %s\n", atlk_rc_to_str(rc));
      goto error;
    }


    /* Obtain data as zero-terminated string */
    const char *msg = v2x_buf_const_ptr(&buf);
    if (msg[v2x_buf_size(&buf) - 1] != '\0') {
      printf("* Bad message (not zero-terminated)\n");
      //goto error;
    }

    cli_print( cli,   "Frame: %d, %02x:%02x:%02x:%02x:%02x:%02x, %s, %.2f",
       (int) ++i,
       wsmp_sk_rx.src_addr.octets[0], wsmp_sk_rx.src_addr.octets[1],
       wsmp_sk_rx.src_addr.octets[2], wsmp_sk_rx.src_addr.octets[3],
       wsmp_sk_rx.src_addr.octets[4], wsmp_sk_rx.src_addr.octets[5],
       msg,
       v2x_power_dbm8_to_dbm(wsmp_sk_rx.power_dbm8));

    /* Free RX buffer memory */
    v2x_buf_free(&buf);
  }
  
error:
  return atlk_error(rc);
}
  
