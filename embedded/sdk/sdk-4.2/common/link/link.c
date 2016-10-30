
#include <stdio.h>
#include "../v2x_cli/v2x_cli.h"
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "link.h"

v2x_service_t 		*v2x_service;

unsigned int 	m_link_rx_packets = 0,
							m_link_tx_packets = 0;
				
	
void print_hexdump(const void *buf, size_t len, int ascii);


	
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

int cli_v2x_link_service_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t             rc    = ATLK_OK;
  char                  str_data[256] = "";
 
  IS_HELP_ARG("link service create -type hw|remote [-server_addr ip_address]")
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_STRING("-type", str_data, 0, "Specify service type, local or remote");
	
  if ( strcmp( (char*) str_data,  "hw") == 0 ) {

    rc = v2x_hw_service_get(&v2x_service);
    if (atlk_error(rc)) {
      cli_print ( cli, "ERROR : v2x_hw_service_get: %s\n", atlk_rc_to_str(rc) );
      goto error;
    }	
  } 
  else if ( strcmp( (char*) str_data, "remote") == 0 ) {
    cli_print( cli, "ERROR : Remote can is not implemented");
  } 
  else {
    cli_print( cli, "ERROR : unknown mode of can api");
  }
	
error:
  return atlk_error(rc);
}

int cli_v2x_link_service_delete( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{

	IS_HELP_ARG("link service delete")
  CHECK_NUM_ARGS /* make sure all parameter are there */

	v2x_service_delete(v2x_service);

  return CLI_OK;
}

int cli_v2x_link_socket_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
	v2x_socket_config_t	link_sk_params = V2X_SOCKET_CONFIG_INIT;

  int32_t               i     = 0;
  atlk_rc_t             rc    = ATLK_OK;
  char                  str_data[256] = "";
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("link socket create -if_idx 1|2 [-frame_type data|vsa] [-protocol_id 0xXXXX|0xXXXXXXXXXX]")

  CHECK_NUM_ARGS /* make sure all parameter are there */
  
  GET_INT("-if_idx", link_sk_params.if_index, i, "Specify interface index");
  if ( link_sk_params.if_index < 1 || link_sk_params.if_index > 2) {
    cli_print(cli, "ERROR : if_index is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  for ( i = 2 ; i < argc; i += 2 ) {
  
    GET_STRING("-frame_type", str_data, i, "Specify frame type");
    if ( strlen(str_data) ) {
      if ( strcmp( (char*) str_data,  "data") == 0 ) {
        link_sk_params.protocol.frame_type = V2X_FRAME_TYPE_DATA;
      } 
      else if ( strcmp( (char*) str_data, "vsa") == 0 ) {
        link_sk_params.protocol.frame_type = V2X_FRAME_TYPE_VSA;
      } 
      else {
        link_sk_params.protocol.frame_type = V2X_FRAME_TYPE_DATA;
      } 
    }
    
		if ( (argv[i] != NULL) && (strcmp(argv[i], "-protocol_id") == 0) ) {           
			unsigned int value = 0;                           
      int rc = 0;                               
      int par_idx = (i+1);                          
      if (!argv[par_idx] && !&argv[par_idx]) {          
          cli_print(cli, "ERROR : Specify the socket protocol id");                   
          return CLI_OK;                                
      }                                                 
      rc = sscanf(argv[par_idx], "%x", &value);              
      if ( rc > 0 ) { \
        link_sk_params.protocol.protocol_id = value;                                   
        /*cli_print( cli, "DEBUG : Processed parameter %s, value %d" , "-protocol_id" , value );*/
      } 
      else { 
        cli_print( cli, "ERROR : Processed parameter %s, value %x failed" , "-protocol_id" , value );
      }
    }                                                   \
  }

	rc = v2x_socket_create(v2x_service, &myctx->v2x_socket, &link_sk_params);
  if (atlk_error(rc)) {
    cli_print(cli, "ERROR : v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

error:
  return atlk_error(rc);
}

int cli_v2x_link_socket_delete( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
	v2x_socket_delete(myctx->v2x_socket);

	myctx->v2x_socket = NULL;
 
  return CLI_OK;
  
}

int cli_v2x_link_tx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
  int32_t       num_frames = 1;     /* total num frames to send */
  int32_t       rate_hz = 1;        /* rate of sending frames   */
  int32_t       payload_len		 	= -1;
								
	size_t				msg_size = 0;
  char          str_data[200] = "",
                tx_data[MAX_TX_MSG_LEN] = "",
								hex_arr[MAX_TX_MSG_LEN / 2] = "";
                
  atlk_rc_t      rc = ATLK_OK;
  
	v2x_send_params_t link_sk_tx_param = V2X_SEND_PARAMS_INIT;

  int i = 0;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("link tx [-frames 1- ...] [-rate_hz 1 - ...] [-tx_data 'ddddd' | -payload_len 100] [-use_nav 0|1][-dest_address XX:XX:XX:XX:XX:XX] [-data_rate 3|4.5|6|9|12|18|24|27|36|48|108 MBPS] [-user_prio 0-7] [-power_dbm8 -20-20]");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", num_frames, i, "Specify the number of frames to send");
    GET_INT("-rate_hz", rate_hz, i, "Specify the rate of frames to send");
		GET_INT("-payload_len", payload_len, i, "Sets the payload length insted of tx data parameter");
    GET_INT("-user_priority", link_sk_tx_param.user_priority, i, "Specify the frame user priority, range 0-7");
    GET_INT("-data_rate", link_sk_tx_param.datarate, i, "Specify the frame user priority, range 0:7");
    GET_INT("-power_dbm8", link_sk_tx_param.power_dbm8, i, "Sets the mac interface to transmit from");

    GET_STRING("-dest_addr", str_data, i, "Set destination mac address"); 
  } 
  
  if ( strlen(str_data) ) {
      sscanf(str_data,"%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
           &link_sk_tx_param.dest_address.octets[0], &link_sk_tx_param.dest_address.octets[1],
           &link_sk_tx_param.dest_address.octets[2], &link_sk_tx_param.dest_address.octets[3],
           &link_sk_tx_param.dest_address.octets[4], &link_sk_tx_param.dest_address.octets[5]);
  }

  GET_STRING_VALUE("-tx_data", tx_data,"Define data to send over the link layer");
  if ( strlen(tx_data) == 0 ) {

		if (payload_len == -1 ) {
			payload_len = 100;
		}
		if ( payload_len % 2 != 0 ) {
			payload_len++;
		}
		
		if ( payload_len > MAX_TX_MSG_LEN ) {
			payload_len = MAX_TX_MSG_LEN;
		}
		memset( tx_data, 70, payload_len); /* FF */
  }
	
	msg_size = (size_t) (strlen(tx_data) / 2);
	hexstr_to_buffer(tx_data, strlen(tx_data), (uint8_t*) hex_arr, &msg_size);

  for (i = 0; i < num_frames; ++i) {
		rc = v2x_send(myctx->v2x_socket, hex_arr, msg_size, &link_sk_tx_param, NULL);
		if (atlk_error(rc)) {
			cli_print(cli, "ERROR : v2x_link_sk_tx: %s\n", atlk_rc_to_str(rc));
			goto error;
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
error:
  return rc;
}

#define MAX_WAVE_FRAME_SIZE 3000
int cli_v2x_link_rx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
 
  atlk_rc_t      rc = ATLK_OK;

  /* Received LINK descriptor */
	v2x_receive_params_t 	link_sk_rx = V2X_RECEIVE_PARAMS_INIT;
  int32_t         			frames   		= 1,
												i        		= 0,
												timeout  		= 5000,
												print_frms	=	1, 
												rx_timeout	=	0;
	char 									buf[MAX_WAVE_FRAME_SIZE] = {0};
									
	struct timeval start, current;

  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  IS_HELP_ARG("link rx [-frames 1- ... [-timeout_ms (0-1e6) -print (0|1)]");
  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", frames, i, "Specify the number of frames to receive");
    GET_INT("-timeout_ms", timeout, i, "Define timeout of last frame");
    GET_INT("-print", print_frms, i, "Sets frames printing");
  } 
  i = 0;
  cli_print( cli, "Note : System will wait for %d frames or timeout %d", (int) frames, (int) timeout );
  
	myctx->cntrs.link_rx_packets = 0;
	
  while ( frames-- ) {

		size_t size = sizeof(buf);

		memset( buf, 0, sizeof(buf) );
		/* f_timeout is only for first frame timeout, */
		if ( myctx->cntrs.link_rx_packets ) {
			timeout = 5000;
		}
		
		rx_timeout = 0;
		gettimeofday (&start, NULL);	
		
		do {
			
			rc = v2x_receive(myctx->v2x_socket, buf, &size, &link_sk_rx, NULL);
			if (atlk_error(rc)) {
				gettimeofday (&current, NULL);	
				double elapsedTime = (current.tv_sec - start.tv_sec) * 1000.0;
				if ( elapsedTime > timeout ) {
					rx_timeout = 1;
				}
				else {
					usleep( 1000 );
				}
			}
			else {
				rx_timeout = 2;
			}
			
		} while ( !rx_timeout );
		
		if ( rx_timeout == 1 ) {
				cli_print(cli, "ERROR : rx time out : %s\n", atlk_rc_to_str(rc));
				goto error;
		}

  	myctx->cntrs.link_rx_packets ++;
		m_link_rx_packets ++;
		
		if ( print_frms ) { 
		
			int j = 0;
			const char *msg = buf;
			char* buf_str = (char*) malloc (2 * size + 1);
			char* buf_ptr = buf_str;
			
			for (j = 0; j < (int) size; j++) {
				buf_ptr += sprintf(buf_ptr, "%02X", msg[j]);
			}
			
			*(buf_ptr + 1) = '\0';
			
			cli_print( cli,   "Frame: %d, SA : %02x:%02x:%02x:%02x:%02x:%02x, DA : %02x:%02x:%02x:%02x:%02x:%02x\r\nData %s\r\n Power : %.2f\r\n",
				 (int) ++i,
				 /* SA */
				 link_sk_rx.source_address.octets[0], link_sk_rx.source_address.octets[1],
				 link_sk_rx.source_address.octets[2], link_sk_rx.source_address.octets[3],
				 link_sk_rx.source_address.octets[4], link_sk_rx.source_address.octets[5],
				 /* DA */
				 link_sk_rx.dest_address.octets[0], link_sk_rx.dest_address.octets[1],
				 link_sk_rx.dest_address.octets[2], link_sk_rx.dest_address.octets[3],
				 link_sk_rx.dest_address.octets[4], link_sk_rx.dest_address.octets[5],
				 buf_str,
				 link_sk_rx.power_dbm8 == V2X_POWER_DBM8_NA ? NAN : (double)link_sk_rx.power_dbm8 / 8.0 );
				 
			free(buf_str);
		}
  }
  
error:
  return atlk_error(rc);
}

int cli_v2x_get_link_socket_addr( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  cli_print( cli, "Link : %x", (unsigned int) myctx->v2x_socket);
  return CLI_OK;
}

int cli_v2x_set_link_socket_addr( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  unsigned int    addr    = 0;
  int							i       = 0;
  
	
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("link set -addr 0xNNNNN");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
	   GET_TYPE_INT("-addr", addr, unsigned int, i, "Address of current session ", "%x"); 
  } 
  
  if ( addr == 0 ) {
    cli_print( cli, "ERROR : address is incorrect %x\n", (int) addr );
    return CLI_ERROR;
  }
  else {
		v2x_socket_t   *v2x_socket		= NULL;
		
		v2x_socket = (v2x_socket_t*) addr;
		cli_print( cli, "Copy Link : %x", (unsigned int) v2x_socket);
		myctx->v2x_socket = v2x_socket;
		
  }
  
  return CLI_OK;
}
