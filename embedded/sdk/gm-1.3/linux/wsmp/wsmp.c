#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../libcli/libcli.h"
#include <atlk/v2x/wave.h>
#include "../v2x_cli/v2x_cli.h"
#include "../session/session.h"
#include "wsmp.h"


#define DEFAULT_PSID  0xc01234
    
/* Globals */

unsigned int 	m_link_rx_packets = 0,
							m_link_tx_packets = 0;
				
	
	
int cli_v2x_link_reset_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)  ) 
{
	/* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

	myctx->cntrs.link_rx_packets = 0;
	myctx->cntrs.link_tx_packets = 0;
	m_link_rx_packets = 0;
	m_link_tx_packets = 0;
	
	return ATLK_OK;
}


int cli_v2x_link_print_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
 /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
	cli_print(cli, "TX : module = %u, session = %u", m_link_tx_packets, myctx->cntrs.link_tx_packets ); 
	cli_print(cli, "RX : module = %u, session = %u", m_link_rx_packets, myctx->cntrs.link_rx_packets ); 

	return ATLK_OK;
}



int cli_v2x_wsmp_sk_open( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  v2x_wsmp_sk_attr_t    wsmp_sk_attr = V2X_WSMP_SK_ATTR_INIT;
  int32_t               i     = 0;
  v2x_rc_t              rc    = V2X_OK;

  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("wsmp open [-psid 0xe0010101] [-disable_rx 1|0]")
  CHECK_NUM_ARGS /* make sure all parameter are there */
  
	 wsmp_sk_attr.psid = 0xe0010101;
  for ( i = 0 ; i < argc; i += 2 ) {
    
    GET_TYPE_INT("-psid", wsmp_sk_attr.psid, i, "Specify the socket psid", "%x");
    GET_INT("-disable_rx", wsmp_sk_attr.sk_attr.rx_disable, i, "Specify if socket is for sending only");
  }
	
  rc = v2x_wsmp_open( &myctx->wsmp_sk, &myctx->v2x_se , &wsmp_sk_attr);
  if (v2x_failed(rc)) {
    cli_print(cli, "v2x_wsmp_open: %s\n", v2x_rc_to_str(rc));
    goto exit;
  }
  
exit:
  return v2x_failed(rc);
}

int cli_v2x_wsmp_sk_close( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  user_context *myctx = (user_context *) cli_get_context(cli);

  
  v2x_sk_close( &myctx->wsmp_sk);
  myctx->wsmp_sk = NULL;
  cli_print(cli, "Socket closed\n" );
  
  return CLI_OK;
}



int cli_v2x_wsmp_tx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
  int32_t       num_frames = 1,
								rate_hz = 1,    
								payload_len = 0,
								msg_size		= 0;

  v2x_rc_t      rc = V2X_OK;
  v2x_wsmp_tx_t wsmp_tx = V2X_WSMP_TX_INIT;
	v2x_tx_buf_t  tx_buf = V2X_TX_BUF_INIT;
  v2x_mac_tx_t  mac_tx = V2X_MAC_TX_INIT;
	char          str_data[50] 					= "",
								tx_data[MAX_TX_MSG_LEN] 	= "";

  
  int i;
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  
  IS_HELP_ARG("send_frame wsmp [-frames 1- ...] [-rate_hz 1 - ...] [-data_rate 54,54,45] [-interface_idx 0|1] [power_dbm8 -20-20] [-psid 0xc01234] \n \
                                            [elem_id 1-23] [-tx_power_used -100-80] [-element_data_rate 1]" );
                                            
  CHECK_NUM_ARGS /* make sure all parameter are there */
  
  
  for ( i = 0 ; i < argc; i += 2 ) {

		GET_INT("-frames", num_frames, i, "Specify the number of frames to send");
    GET_INT("-rate_hz", rate_hz, i, "Specify the rate of frames to send");
		GET_INT("-payload_len", payload_len, i, "Sets the payload length insted of tx data parameter");
    GET_INT("-data_rate", mac_tx.datarate, i, "Specify the frame user priority, range 0:7");
    GET_INT("-power_dbm8", mac_tx.power_dbm8, i, "Sets the mac interface to transmit from");
    GET_INT("-if_idx", mac_tx.if_index, i, "Sets mac interface idx to transmit from, 0-1");
    /* wsmp tx parameters */
    GET_INT("-psid", wsmp_tx.psid, i, "set wsmp psid value");
    GET_INT("-set_tx_power_used", wsmp_tx.ext_elem.set_tx_power_used, i, "set tx_power_used field in wsmp message");
    GET_INT("-element_data_rate", wsmp_tx.ext_elem.set_datarate, i, "set tx_power_used field in wsmp message");
		
		GET_STRING("-dest_addr", str_data, i, "Set destination mac address"); 
  } 
  
  if ( strlen(str_data) ) {
      sscanf(str_data,"%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
           &wsmp_tx.dest_addr.octets[0], &wsmp_tx.dest_addr.octets[1],
           &wsmp_tx.dest_addr.octets[2], &wsmp_tx.dest_addr.octets[3],
           &wsmp_tx.dest_addr.octets[4], &wsmp_tx.dest_addr.octets[5]);
  }

	GET_STRING_VALUE("-tx_data", tx_data,"Define data to send over the link layer");
  if ( !strlen(tx_data) ) {

		if (payload_len > 0 ) {
		
			if ( payload_len > MAX_TX_MSG_LEN ) {
				payload_len = MAX_TX_MSG_LEN;
			}
			memset( tx_data, 65, payload_len); /* chr a */
		}
		else {
			sprintf(tx_data,  "Autotalks - Confidence of Knowing Ahead\n");
		}
  }
  msg_size = strlen(tx_data) + 1;
	
	/* Set payload pointer & size */
	v2x_tx_buf_set(&tx_buf, tx_data, msg_size);

	for (i = 0; i < num_frames; ++i) {


    /* Transmit WSMP */
    rc = v2x_wsmp_tx( &myctx->wsmp_sk, &tx_buf, 0, &wsmp_tx, &mac_tx );
    if (v2x_failed(rc)) {
				cli_print(cli, "ERROR : v2x_wsmp_tx@frame %d out of %d failed due to %s\n", (int) i, (int) num_frames, v2x_rc_to_str(rc));
		}
		else {
			myctx->cntrs.link_tx_packets ++;
			m_link_tx_packets ++;
 
			/* Sleep 100 ms between transmissions */
			if ( (num_frames >= 1) && (rate_hz >= 1) ){
				int sleep_time_uSec = (1e6 / rate_hz );
				usleep( sleep_time_uSec );
			}
		}
		
  }

  return CLI_OK;
}


int cli_v2x_wsmp_rx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
  
	/* Socket RX timeout: 30 sec */
  static struct timeval timeout = { .tv_sec = 30, .tv_usec = 0 };
  v2x_rc_t 				rc 				 	= V2X_OK;
  int32_t         frames   		= 1,
									timeout_ms	=	0,
                  i        		= 0,
									print_frms	=	1;

	v2x_wsmp_rx_t 	wsmp_rx 		= V2X_WSMP_RX_INIT;
	v2x_mac_rx_t 		mac_rx 			= V2X_MAC_RX_INIT;
									

  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("link rx [-frames 1- ... [-timeout_ms (0-1e6) -print (0|1)]");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", frames, i, "Specify the number of frames to receive");
    GET_INT("-timeout_ms", timeout_ms , i, "Define timeout of last frame");
    GET_INT("-print", print_frms, i, "Sets frames printing");
		GET_INT("-if_idx", mac_rx.if_index, i, "Sets mac interface idx to transmit from, 0-1");

  } 
  i = 0;
	
	timeout.tv_sec = (timeout_ms / 1000);
	timeout.tv_usec = ((timeout_ms % 1000) * 1000);

  cli_print( cli, "Note : System will wait for %d frames or timeout %d.%d", (int) frames, (int) timeout.tv_sec, (int) timeout.tv_usec );
  
	myctx->cntrs.link_rx_packets = 0;

  /* Set socket RX timeout */
  rc = v2x_sk_rx_timeout_set(&myctx->wsmp_sk, &timeout);
  if (v2x_failed(rc)) {
    fprintf(stderr, "v2x_sk_rx_timeout_set: %s\n", v2x_rc_to_str(rc));
    return rc;
  }
	
  while ( frames-- ) {
    /* RX buffer descriptor */
    v2x_rx_buf_t rx_buf = V2X_RX_BUF_INIT;
    /* Received WSM descriptor */

    /* Receive WSM */
    rc = v2x_wsmp_rx(&myctx->wsmp_sk, &rx_buf, 0, &wsmp_rx, &mac_rx);
    if (v2x_failed(rc)) {
      fprintf(stderr, "v2x_wsmp_rx: %s\n", v2x_rc_to_str(rc));
      return rc;
    }
		
		myctx->cntrs.link_rx_packets ++;
		m_link_rx_packets ++;
	
	
    /* Print received payload */
if ( print_frms == 1 ) { 
		
			cli_print( cli,   "Frame: %d, %02x:%02x:%02x:%02x:%02x:%02x, %s, %.2f",
				 (int) ++i,
				 wsmp_rx.src_addr.octets[0], wsmp_rx.src_addr.octets[1],
				 wsmp_rx.src_addr.octets[2], wsmp_rx.src_addr.octets[3],
				 wsmp_rx.src_addr.octets[4], wsmp_rx.src_addr.octets[5],
				 (char *)v2x_rx_buf_ptr(&rx_buf),
				 v2x_power_dbm8_to_dbm(mac_rx.power_dbm8) );
		}
		
    /* Free RX buffer memory */
    v2x_rx_buf_close(&rx_buf);

  }

  return V2X_OK;
}
