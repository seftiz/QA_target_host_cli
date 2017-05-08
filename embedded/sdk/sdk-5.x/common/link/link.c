

#include <stdio.h>
#include "../general/general.h"
#include "../v2x_cli/v2x_cli.h"
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
//#include </media/sf_ATE_proj/dbg-secton-sdk-5.2.0-beta3/include/atlk/v2x_service.h>
#include <atlk/v2x_service.h>
#include <atlk/v2x.h>
#include <atlk/ddm_service.h>
#include <atlk/wdm_service.h>
#include <atlk/version_string.h>
#ifdef __THREADX__
#include <tx_api.h>
#include <nx_api.h>
#include <craton/net.h>
#include <craton/wave_ipv6.h>
#endif


#include "link.h"
#include "../../linux/remote/remote.h"

#include "/media/sf_fw_release/dbg-secton-sdk-5.3.0-alpha2/include/atlk/wdm.h"
#include "/media/sf_fw_release/dbg-secton-sdk-5.3.0-alpha2/include/atlk/wdm_service.h"


//trying
typedef struct {
  ddm_service_t *ddm_service_ptr;
  wdm_service_t *wdm_service_ptr;
  v2x_service_t *v2x_service_ptr;
  int32_t ddm_init;
} diag_cli_services_t;
//till here

static v2x_service_t 						*v2x_service = NULL;
static wdm_service_t                                            *wdm_service_ptr = NULL;
// /* End indication thread */
// /* thread priority */
// #ifdef __THREADX__
// #define TX_THREAD_PRIORITY 40
// static TX_THREAD end_indication_thread;
// static void end_indication_thread_entry(ULONG input);
// static uint8_t end_indication_thread_stack[0x1000];
// static void end_indication_thread_entry(ULONG input);

// #elif defined (__linux__)
// pthread_t end_indication_thread;
// void *end_indication_thread_entry( void *thread_desc );
// #endif

// static bool dot4_ch_end_ind_thx_is_up = false;
// /* Sync loss indication */
//static int is_sync_loss = 0;
/* channel access configuration */
/*
#define V2X_DOT4_CHANNEL_START_REQUEST_INIT {  \
 .if_index = V2X_IF_INDEX_NA,                  \
 .channel_id = V2X_CHANNEL_ID_INIT,            \
 .time_slot = V2X_TIME_SLOT_NA,                \
 .immediate_access = 0                         \
}
*/
#ifdef __THREADX__
///////////////////                   /////////////////////////////////////////
///////////////////   WAVE  IPv6      /////////////////////////////////////////
///////////////////   start           /////////////////////////////////////////
/* Pointer to `untrusted` network IP instance */
static NX_IP *untrusted_instance = NULL;
/* wave6 message format string: Wave IP6 msg <seq_num> */
static const char wave6_msg_fmt[] = "Wave IP6 msg %" PRIu32;

/* wave6 message string maximum length */
static const size_t wave6_msg_size_max = MAX_TX_MSG_LEN;//sizeof(wave6_msg_fmt) + 10;
/* UDP server and client ports */
#define WAVE6_SERVER_PORT 6666
#define WAVE6_CLIENT_PORT 6667

///////////////////                   /////////////////////////////////////////
///////////////////   WAVE  IPv6      /////////////////////////////////////////
///////////////////   end              /////////////////////////////////////////
#endif

unsigned int 	m_link_rx_packets = 0,
							m_link_tx_packets = 0;
				
void print_hexdump(const void *buf, size_t len, int ascii);

int cli_v2x_link_reset_cntrs( struct cli_def *cli, const char *command, char *argv[], int argc  ) 
{
	/* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  (void) command;
  (void) argv;
  (void) argc;

	myctx->cntrs.link_rx_packets = 0;
	myctx->cntrs.link_tx_packets = 0;
	m_link_rx_packets = 0;
	m_link_tx_packets = 0;
	
	return ATLK_OK;
}

int cli_v2x_link_print_cntrs( struct cli_def *cli, const char *command, char *argv[], int argc  ) 
{
 /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  (void) command;
  (void) argv;
  (void) argc;

	cli_print(cli, "TX : module = %u, session = %u", m_link_tx_packets, myctx->cntrs.link_tx_packets ); 
	cli_print(cli, "RX : module = %u, session = %u", m_link_rx_packets, myctx->cntrs.link_rx_packets ); 

	return ATLK_OK;
}

int cli_v2x_link_service_create( struct cli_def *cli, const char *command, char *argv[], int argc  ) 
{
  char                  str_data[256] = "";
/* ThreadX return value */

  (void) command;
  
  IS_HELP_ARG("link service create -type hw|remote")
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_STRING("-type", str_data, 0, "Specify service type, local or remote");
	
  if ( strcmp( (char*) str_data,  "hw") == 0 ) {
		
#if defined(__CRATON_NO_ARC) || defined(__CRATON_ARC1)
    if ( v2x_service != NULL ) {
      cli_print ( cli, "NOTE : Local service is active" );
    }
    else {
      atlk_rc_t rc = v2x_default_service_get(&v2x_service);
		  if (atlk_error(rc)) {
	      cli_print ( cli, "ERROR : v2x_hw_service_get: %s\n", atlk_rc_to_str(rc) );
    	  return atlk_error(rc);
  	  }	
    }
#else
    cli_print ( cli, "ERROR : v2x Link service is not supported\n" );
#endif
  
  } 
  else if ( strcmp( (char*) str_data, "remote") == 0 ) {
#ifdef __linux__
		
		/* Create the remote V2X service */
		atlk_rc_t rc = v2x_service_get(NULL, &v2x_service);
		if (atlk_error(rc)) {
			cli_print( cli, "Remote V2X service create: %s\n", atlk_rc_to_str(rc));
			return atlk_error(rc);
		}
#else 
		cli_print( cli, "Remote V2X service is not avaliable" );
#endif		
  } 
  else {
    cli_print( cli, "ERROR : unknown mode of link api");
  }
	syslog( LOG_DEBUG, "v2x_service pointer is %p", (void*) v2x_service );
 	
	  // /* Create End Indication thread */
  // if (!dot4_ch_end_ind_thx_is_up) {
// #ifdef __linux__
		// /*
		// pthread_attr_init(&new_thread->attr);
    // pthread_attr_setstacksize(&new_thread->attr, CLI_THREAD_STACKSIZE);
		// */
		// int err ;

		// err = pthread_create( &end_indication_thread , NULL, end_indication_thread_entry , NULL);
    // if ( err ) {
      // printf("could not create thread %d\n", err );
      // return CLI_ERROR;
		// }
// #elif defined(__THREADX__)

  	// tx_thread_create(&end_indication_thread, "end_indication_thread",
                       // end_indication_thread_entry, 0,
                       // &end_indication_thread_stack,
                       // sizeof(end_indication_thread_stack),
                       // TX_THREAD_PRIORITY,
                       // TX_THREAD_PRIORITY,
                       // TX_NO_TIME_SLICE, TX_AUTO_START);
// #endif
	// dot4_ch_end_ind_thx_is_up = true;
  // }


  return CLI_OK; 
}

int cli_v2x_link_service_delete( struct cli_def *cli, const char *command, char *argv[], int argc ) 
{ 
  atlk_rc_t             rc    = ATLK_OK;





  (void) command;
  
	IS_HELP_ARG("link service delete")
  CHECK_NUM_ARGS /* make sure all parameter are there */

  if ( v2x_service == NULL ) {
    cli_print ( cli, "ERROR : Service not created, please create first\n" );
    return CLI_ERROR;
  }
	
	syslog( LOG_DEBUG, "v2x_service_delete pointer is %p", (void*) v2x_service );
	//disable till support will be available from SDK
#if 0
	rc = v2x_service_delete(v2x_service);
#endif
  if ( atlk_error(rc) ) {
    cli_print ( cli, "ERROR : v2x_service_delete: %d, %s\n", rc, atlk_rc_to_str(rc) );
    goto error;
  }	

error:
  v2x_service = NULL;

  return atlk_error(rc);
  
}


int cli_v2x_link_socket_create( struct cli_def *cli, const char *command, char *argv[], int argc ) 
{
	v2x_socket_config_t	link_sk_params = V2X_SOCKET_CONFIG_INIT;




  int32_t               i     = 0;
  atlk_rc_t             rc    = ATLK_OK;
  char                  str_data[256] = "";
  size_t index;
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  (void) command;
  
  IS_HELP_ARG("link socket create -if_idx 1|2 [-frame_type data|vsa] [-protocol_id 0xXXXX|0xXXXXXXXXXX]")

 

  CHECK_NUM_ARGS /* make sure all parameter are there */
  
rc = wdm_service_get(NULL, &wdm_service_ptr);
    if (atlk_error(rc)) {
      cli_print( cli, "ERROR :wdm_service_get fiailed: %s\n", atlk_rc_to_str(rc));
      return EXIT_FAILURE;
    }
   cli_print( cli, "wdm_service: %p",wdm_service_ptr);
 /* Enable WDM interfaces */
  for (index = 0; index < 2; index++) {
    rc = wdm_interface_state_set(wdm_service_ptr,
                                 index,
                                 WDM_INTERFACE_STATE_ENABLED);
    if(atlk_error(rc)) {
      printf("Could not enable interface %zu\n", index);
    }
    else {
      printf("Interface %zu enabled\n", index);
    }
  }

 // get v2x service
  
    rc = v2x_service_get(NULL, &v2x_service);
    if (atlk_error(rc)) {
      cli_print( cli, "ERROR :v2x_service_get fiailed: %s\n", atlk_rc_to_str(rc));
      return EXIT_FAILURE;
    }
  GET_INT("-if_idx", link_sk_params.if_index, i, "Specify interface index");
  if ( link_sk_params.if_index < 1 || link_sk_params.if_index > 4) {
    cli_print(cli, "ERROR : if_index is not optional and must be in range of 1-4");
    return CLI_ERROR;
  }
  
  for ( i = 2 ; i < argc; i += 2 ) {
  
    GET_STRING("-frame_type", str_data, i, "Specify frame type");
    if ( strlen(str_data) ) {
      if ( strcmp( (char*) str_data,  "data") == 0 ) {

        link_sk_params.protocol.frame_type = V2X_FRAME_TYPE_DATA_LPD_ETHERTYPE;
      } 
      else if ( strcmp( (char*) str_data, "vsa") == 0 ) {

        link_sk_params.protocol.frame_type = V2X_FRAME_TYPE_DATA_LPD_ETHERTYPE;
      } 
      else if ( strcmp( (char*) str_data, "err") == 0 ) {
        link_sk_params.protocol.frame_type = 231;
      }
      else {

        link_sk_params.protocol.frame_type = V2X_FRAME_TYPE_DATA_LPD_ETHERTYPE;
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
      if ( rc > 0 ) {
        link_sk_params.protocol.protocol_id = value;                                   
      } 
      else { 
        cli_print( cli, "ERROR : Processed parameter %s, value %x failed" , "-protocol_id" , value );
      }
    }                                                   
  }
  if (v2x_service == NULL) {
		rc = v2x_service_get(NULL, &v2x_service);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_service_get failed: %d\n", rc);
      return EXIT_FAILURE;
    }
  }

  rc = v2x_socket_create(v2x_service, &myctx->v2x_socket, &link_sk_params);
  if (atlk_error(rc)) {
    cli_print(cli, "ERROR : v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
 }


  
   
  cli_print(cli, "INFO : Create socket pointer %p\n", (void*) myctx->v2x_socket);
  myctx->if_idx = link_sk_params.if_index;

error:
  return atlk_error(rc);
}

int cli_v2x_link_socket_delete( struct cli_def *cli, const char *command, char *argv[], int argc ) 
{
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  (void) command;
  (void) argv;
  (void) argc;

  atlk_rc_t             rc    = ATLK_OK;
  (void) command;
  
	IS_HELP_ARG("link socket delete")
  CHECK_NUM_ARGS /* make sure all parameter are there */

  if ( myctx->v2x_socket == NULL ) {
    cli_print ( cli, "INFO : Socket not created\n" );
    return CLI_ERROR;
  }

  cli_print(cli, "INFO : Delete socket pointer %p\n", (void*) myctx->v2x_socket);

	rc = v2x_socket_delete(myctx->v2x_socket);
  if ( atlk_error(rc) ) {
    cli_print ( cli, "ERROR : v2x_socket_delete: %d, %s\n", rc, atlk_rc_to_str(rc) );
    goto error;
  }	

 
error:
	myctx->v2x_socket = NULL;
  return atlk_error(rc);
}

int cli_v2x_link_tx( struct cli_def *cli, const char *command, char *argv[], int argc )
{
  int32_t       num_frames = 1;     /* total num frames to send */
  int32_t       rate_hz = 1;        /* rate of sending frames   */
  int32_t       payload_len		 	= -1;
								
	size_t				msg_size = 0;
  char          str_data[200] = "",
                tx_data[MAX_TX_MSG_LEN] = "";
  uint8_t       hex_arr[MAX_TX_MSG_LEN / 2] = "";
                
  atlk_rc_t      rc = ATLK_OK;
  char          *pos = NULL;
  
	v2x_send_params_t link_sk_tx_param = V2X_SEND_PARAMS_INIT;
  (void) command;

  int i = 0;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("link socket tx [-frames 1- ...] [-rate_hz 1 - ...] [-tx_data 'ddddd' | -payload_len 100] [-use_nav 0|1][-dest_address XX:XX:XX:XX:XX:XX] [-data_rate 3|4.5|6|9|12|18|24|27|36|48|108 MBPS] [-user_prio 0-7] [-power_dbm8 -20-20] [op_class 0-4] [ch_idx (IEEE1609.4)] ");

  CHECK_NUM_ARGS /* make sure all parameter are there */
  /*
  link_sk_tx_param.datarate = 0;
  // link_sk_tx_param.power_dbm8 = -80;
  */
  for (i = 0; i < argc; i += 2) {
    GET_INT("-frames", num_frames, i, "Specify the number of frames to send");
    GET_INT("-rate_hz", rate_hz, i, "Specify the rate of frames to send");
	GET_INT("-payload_len", payload_len, i, "Sets the payload length insted of tx data parameter");
    GET_INT("-user_priority", link_sk_tx_param.user_priority, i, "Specify the frame user priority, range 0-7");
    GET_INT("-data_rate", link_sk_tx_param.datarate, i, "Specify the frame user priority, range 0:7");
    GET_INT("-power_dbm8", link_sk_tx_param.power_dbm8, i, "Sets the mac interface to transmit from");
    GET_STRING("-dest_addr", str_data, i, "Set destination mac address"); 
    GET_INT("-op_class", link_sk_tx_param.channel_id.op_class, i, "Specify operational class");
    GET_INT("-time_slot", link_sk_tx_param.channel_id.time_slot, i, "which alternating access is requested");
    GET_INT("-ch_idx", link_sk_tx_param.channel_id.channel_num, i, "Sets the channel number (band) to be used");
  } 
  
  // Convert mac address xx:xx:xx:xx:xx:xx to array of bytes
  if ( strlen(str_data) ) {
    pos = str_data;
    for (i = 0; i < 6; i++){
      link_sk_tx_param.dest_address.octets[i] = hex_to_byte(pos);
      pos += 3;
    }
    /* For debug:
    printf("cli_v2x_link_tx : mac address = 0x");
    for (i = 0; i < 6; i++)
      printf("%02x", link_sk_tx_param.dest_address.octets[i]);
    printf("\n");
    */
  }

  GET_STRING_VALUE("-tx_data", tx_data, "Define data to send over the link layer");
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
	/*handle special case to support CHS TCs*/
  if ((strcmp(tx_data, "CHS") == 0) && (payload_len > 0)){
	  memset((char *)tx_data, 0, sizeof(tx_data));
	  bulid_hex_str((char *)&(tx_data[0]), payload_len);
  }

	msg_size = (size_t) (strlen(tx_data) / 2);
	cli_print(cli, "cli_v2x_link_tx - convert hexstr to buffer, msg_size = %d, strlen(tx_data) = %d\n", (int) msg_size, (int)strlen(tx_data) );
  rc = hexstr_to_bytes_arr(tx_data, strlen(tx_data), hex_arr, &msg_size);
	//cli_print(cli, " rc = %d, data : %s, msg_size : %d, hex_arr : 0x%02x, hex_arr : 0x%02x, hex_arr : 0x%02x ,hex_arr : 0x%02x  ",rc, tx_data,(int)msg_size,hex_arr[0],hex_arr[1],hex_arr[2],hex_arr[3]);
  if (atlk_error(rc)) {
    cli_print(cli, "ERROR : cli_v2x_link_tx - cannot convert hexstr to buffer, error= %s\n", atlk_rc_to_str(rc));
    goto error;
  }
  
  for (i = 0; i < num_frames; ++i) {
	  	hex_arr[1] = i;
	  	hex_arr[0] = (i & 0xff00)>>8;
		rc = v2x_send(myctx->v2x_socket, hex_arr, msg_size, &link_sk_tx_param, NULL);
		
		//int j = msg_size;
		
		//cli_print(cli, "data :");

		//while (j){
		//j--;
		
		//cli_print(cli, "%02x\n", hex_arr[j] );
		//}
		
		if ( atlk_error(rc) ) {
			cli_print(cli, "ERROR : v2x_send: %s\n", atlk_rc_to_str(rc));
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
int cli_v2x_link_rx( struct cli_def *cli, const char *command, char *argv[], int argc )
{
 
  atlk_rc_t      rc = ATLK_OK;

  /* Received LINK descriptor */
	v2x_receive_params_t 	link_sk_rx;// = V2X_RECEIVE_PARAMS_INIT;
  int32_t         			frames   		= 1,
												i        		= 0,
												cycle_timeout  		= 5000,
												timeout  		= 50000,
												print_frms	=	1, 
												rx_timeout	=	0;
	char 									buf[MAX_WAVE_FRAME_SIZE] = {0};
									
	struct timeval start, current, session_start, current_cycle;
  (void) command;
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  IS_HELP_ARG("link socket rx [-frames 1- ...] [-timeout_ms (0-1e6) -print (0|1)] [op_class 0-4] [ch_idx (IEEE1609.4)] ");
  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", frames, i, "Specify the number of frames to receive");
    GET_INT("-timeout_ms", timeout, i, "Define timeout of last frame");
    GET_INT("-print", print_frms, i, "Sets frames printing");
    GET_INT("-op_class", link_sk_rx.channel_id.op_class, i, "Specify operational class");
    GET_INT("-ch_idx", link_sk_rx.channel_id.channel_num, i, "Sets the channel number (band) to be used");

  } 
  i = 0;
  cli_print( cli, "Note : System will wait for %d frames or timeout %d", (int) frames, (int) timeout );
  
	myctx->cntrs.link_rx_packets = 0;
	
  gettimeofday (&session_start, NULL);
  while ( frames-- ) {

		size_t size = sizeof(buf);

		memset( buf, 0, sizeof(buf) );
		/* f_timeout is only for first frame timeout, */
		if ( myctx->cntrs.link_rx_packets ) {
			cycle_timeout = 5000;
		}
		
		rx_timeout = 0;
		gettimeofday (&start, NULL);	
		
		do {
			gettimeofday (&current_cycle, NULL);
		    double elapsed_from_session_start = (current_cycle.tv_sec - session_start.tv_sec) * 1000.0;
		    if (elapsed_from_session_start > timeout) {
				cli_print(cli, "ERROR : rx session timed out:\n");
				goto error;
		    }
			//cli_print(cli, "v2x_socket : %p\n",myctx->v2x_socket );

			rc = v2x_receive(myctx->v2x_socket, buf, &size, &link_sk_rx, NULL);
			if ( (rc == ATLK_E_TIMEOUT) || (rc == ATLK_E_NOT_READY)||(rc == ATLK_E_EMPTY)) {
				gettimeofday (&current, NULL);	
				double elapsedTime = (current.tv_sec - start.tv_sec) * 1000.0;
				if ( (elapsedTime > cycle_timeout) && (rc != ATLK_E_NOT_READY)) {
					rx_timeout = 1;
				}
				else {
					usleep( 1000 );
				}
			}
			else if ((rc == ATLK_OK)) {
				rx_timeout = 2;
			}
			else if (rc != ATLK_OK && rc != ATLK_E_NOT_READY) {
				rx_timeout = 10 + rc;
			}
      
		} while ( !rx_timeout );
		
		if ( rx_timeout == 1 ) {
			cli_print(cli, "ERROR : rx time out or not ready : %s\n", atlk_rc_to_str(rc));
			goto error;
		}
    else if (rx_timeout > 10) {
      cli_print(cli, "ERROR : %d, %s\n", rc, atlk_rc_to_str(rc) );
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
			//added for chs tests
				if (link_sk_rx.channel_id.channel_num != 0){
				cli_print( cli,   "Frame: %d, ch_num: %d, time_slot: %d  timestamp: %" PRIu64 "\n",
				 (int) ++i,link_sk_rx.channel_id.channel_num ,link_sk_rx.channel_id.time_slot,link_sk_rx.receive_time_us);
			
				}
			//till here

		cli_print( cli,   "Frame: %d, SA : %02x:%02x:%02x:%02x:%02x:%02x, DA : %02x:%02x:%02x:%02x:%02x:%02x\r\nData %s\r\n" /*Power : %.2f\r\n"*/,
				 (int) ++i,
				 /* SA */
				 link_sk_rx.source_address.octets[0], link_sk_rx.source_address.octets[1],
				 link_sk_rx.source_address.octets[2], link_sk_rx.source_address.octets[3],
				 link_sk_rx.source_address.octets[4], link_sk_rx.source_address.octets[5],
				 /* DA */
				 link_sk_rx.dest_address.octets[0], link_sk_rx.dest_address.octets[1],
				 link_sk_rx.dest_address.octets[2], link_sk_rx.dest_address.octets[3],
				 link_sk_rx.dest_address.octets[4], link_sk_rx.dest_address.octets[5],
				 buf_str/*,
				 link_sk_rx.power_dbm8 == V2X_POWER_DBM8_NA ? NAN : (double)link_sk_rx.power_dbm8 / 8.0 */);
				 
			free(buf_str);
		}
  }
  
error:
  return atlk_error(rc);
}

int cli_v2x_get_link_socket_addr( struct cli_def *cli, const char *command, char *argv[], int argc ) 
{ 
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  (void) command;
  (void) argv;
  (void) argc;
  
  cli_print( cli, "Link : %p", (void*) myctx->v2x_socket);
  return CLI_OK;
}

int cli_v2x_set_link_socket_addr( struct cli_def *cli, const char *command, char *argv[], int argc ) 
{
  unsigned int    addr    = 0;
  int							i       = 0;
  
	
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  (void) command;
  
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
		
		v2x_socket = (void*) (long) addr;
		cli_print( cli, "Copy Link : %p", (void*) v2x_socket);
		myctx->v2x_socket = v2x_socket;
  }
  
  return CLI_OK;
}


/*
int cli_v2x_netif_profile_set( struct cli_def *cli, const char *command, char *argv[], int argc ) //chrub
{
	atlk_rc_t      rc = ATLK_OK;
	
  	user_context *myctx = (user_context *) cli_get_context(cli);
	(void) command;
	(void) argv;
  	(void) argc;

  v2x_netif_profile_t netif_profile = {
    .if_index = myctx->if_idx,
    .channel_id = V2X_CHANNEL_ID_INIT,
    .datarate = V2X_DATARATE_6MBPS,
    .power_dbm8 = -20
  };

  IS_HELP_ARG("link netif_profile set -datarate[] -power_dbm8[-20-20]");

  CHECK_NUM_ARGS // make sure all parameter are there


    for (int i = 0; i < argc; i += 2) {
      GET_INT("-data_rate", netif_profile.datarate, i, "Specify the frame user priority, range 0:7");
      GET_INT("-power_dbm8", netif_profile.power_dbm8 , i, "power dBm");
    }

  rc = v2x_netif_profile_set(v2x_service, 0, &netif_profile);
  if (atlk_error(rc)) {
    cli_print(cli, "v2x_netif_profile_set: %s\n", atlk_rc_to_str(rc));
  }
	return rc;

} 
*/



//disable till support will be available from SDK


int cli_v2x_dot4_channel_start_req(struct cli_def *cli, const char *command, char *argv[], int argc)
{

  v2x_dot4_channel_start_request_t request;
 // atlk_wait_t wait;
  int i = 0;
  atlk_rc_t      rc = ATLK_OK;
  int32_t       if_index = 1;
  int32_t       op_class = 1;
  int32_t       chan_id  = 0;
  int32_t		  slot_id  = 0;
  int32_t		  imm_acc  = 0;
wdm_service_t *wdm_service_ptr = NULL;

  (void) command;

 // get v2x service
  
    rc = v2x_service_get(NULL, &v2x_service);
    if (atlk_error(rc)) {
      cli_print( cli, "ERROR :v2x_service_get failed: %s\n", atlk_rc_to_str(rc));
      return EXIT_FAILURE;
    }

  /* get user context */
  //user_context *myctx = (user_context *) cli_get_context(cli);

  IS_HELP_ARG("link dot4 ch_start [-if_index 1 2] [-op_class 0-4] [-chan_id see 1609.4] [-slot_id 1 2] [-imm_acc 0-255] ");

  CHECK_NUM_ARGS /* make sure all parameter are there */



	  for (i = 0; i < argc; i += 2) {
	      GET_INT("-if_index", if_index, i, "RF index");
	      GET_INT("-op_class", op_class, i, "operation class");
	  	  GET_INT("-ch_id", chan_id, i, "Sets the channel number (band) to be used");
	  	  GET_INT("-slot_id", slot_id, i, "Specify time slot to be used");
	      GET_INT("-imm_acc", imm_acc, i, "Specify dot4 mode of operation - continues, immidiet, alternated value set TO by multiplying with full sync interval");
	  }


	  request.if_index = if_index;
	  request.channel_id.op_class = op_class;
	  request.channel_id.channel_num = chan_id;
	  request.channel_id.time_slot = slot_id;
	  request.immediate_access = imm_acc;

 rc = wdm_service_get(NULL, &wdm_service_ptr);
  if (atlk_error(rc)) {
    (void)fprintf(stderr, "wdm_service_get failed: %d\n", rc);
    return EXIT_FAILURE;
  }

 rc = wdm_interface_state_set(wdm_service_ptr,
                                 0,
                                 WDM_INTERFACE_STATE_ENABLED);
    if(atlk_error(rc)) {
      printf("Could not enable interface %d\n", 0);
    }
    else {
      printf("Interface %d enabled\n", 0);
    }

     rc = v2x_dot4_channel_start(v2x_service, &request, &atlk_wait_forever);
     if (rc != ATLK_OK) {
    	 cli_print(cli, "ERROR : dot4 channel request : %s\n", atlk_rc_to_str(rc));
    	 return CLI_ERROR;
     }
	 return CLI_OK;

}

int cli_v2x_dot4_channel_end_req(struct cli_def *cli, const char *command, char *argv[], int argc)
{

	v2x_dot4_channel_end_request_t request;
wdm_service_t *wdm_service_ptr = NULL;
	atlk_wait_t wait;
    int i = 0;
	atlk_rc_t      rc = ATLK_OK;
	int32_t       if_index = 1;
    int32_t       ch_id  = 0;
    int32_t       op_class = 1;
    (void) command;


  /* get user context */
  //user_context *myctx = (user_context *) cli_get_context(cli);

  IS_HELP_ARG("link dot4 ch_end [if_index 1 2] [ch_id see 1609.4] [op_class 0-4]");

  CHECK_NUM_ARGS /* make sure all parameter are there */



	  for (i = 0; i < argc; i += 2) {
	      GET_INT("-if_index", if_index, i, "RF index");
	      GET_INT("-ch_id", ch_id, i, "Sets the channel number (band) to be used");
	      GET_INT("-op_class", op_class, i, "operation class");
	  }


	  request.if_index = if_index;
	  request.channel_id.op_class = op_class;
	  request.channel_id.channel_num = ch_id;


 rc = wdm_service_get(NULL, &wdm_service_ptr);
  if (atlk_error(rc)) {
    (void)fprintf(stderr, "wdm_service_get failed: %d\n", rc);
    return EXIT_FAILURE;
  }
     rc = v2x_dot4_channel_end(v2x_service, &request, &wait);
     if (rc != ATLK_OK) {
    	 cli_print(cli, "ERROR : v2x channel request : %s\n", atlk_rc_to_str(rc));
    	 return CLI_ERROR;
     }
	 return CLI_OK;

}


//trying:

/* Show SDK version via CLI */
int cli_v2x_cmd_sdk_version(struct cli_def *cli,
                    const char *command,
                    char *argv[],
                    int argc)
{
(void) command;
(void) argv;
(void) argc;

  char sdk_version[256];
  size_t sdk_version_size;
  ddm_service_t *ddm_service = 
    ((diag_cli_services_t *)cli_get_context(cli))->ddm_service_ptr;
  wdm_service_t *wdm_service = 
    ((diag_cli_services_t *)cli_get_context(cli))->wdm_service_ptr;
  char *sdk_ver = sdk_version;
  atlk_rc_t rc;
  wdm_dsp_version_t dsp_version;

  sdk_version_size = sizeof(sdk_version);
  rc = ddm_version_get(ddm_service, sdk_version, &sdk_version_size);
  if (atlk_error(rc)) {
    cli_print(cli, "failed to get SDK version rc=%d", rc);
  //  return CLI_ERROR;
  }

  cli_print(cli, "Host:");
  cli_print(cli,
            "  SDK version: %d.%d.%d",
            ATLK_SDK_VER_MAJOR,
            ATLK_SDK_VER_MINOR,
            ATLK_SDK_VER_PATCH);
  cli_print(cli, "  Software version: %s", HOST_SDK_REV_ID);

  cli_print(cli, "Device:");

  cli_print(cli, "  SDK version: %s", sdk_ver);
  
  sdk_ver = sdk_ver + strlen(sdk_version) + 1;
  cli_print(cli, "  Software version: %s", sdk_ver);

  rc = wdm_dsp_version_get(wdm_service, &dsp_version);
  if (atlk_error(rc)) {
    cli_print(cli, "failed to get WDM DSP version rc=%d", rc);
    return CLI_ERROR;
  }

  cli_print(cli, "DSP:");

  cli_print(cli,
            "  Software version: %"PRIu8".%"PRIu8".%"PRIu8,
            dsp_version.major,
            dsp_version.minor,
            dsp_version.sw_revision);

  return CLI_OK;
}

//till here


// #ifdef __THREADX__
// void
// end_indication_thread_entry(ULONG input)

// #elif defined (__linux__)

// void *end_indication_thread_entry( void *thread_desc )

// #endif
// {
  // /* Autotalks return code */
  // atlk_rc_t rc = ATLK_OK;

  // /* DOT4 channel end indication */
  // v2x_dot4_channel_end_indication_t indication;
// #ifdef __THREADX__
  // /* Not using input */
  // (void)input;
// #elif defined (__linux__)
  // (void)thread_desc;
// #endif
  // while (1) {

    // /* polling for channel indication */
    // rc = v2x_dot4_channel_end_receive(v2x_service, &indication, NULL);
    // switch (rc) {
    // case ATLK_OK:
    	// printf("\nCHEND.ind:if %d ch %d re %d\n",
             // indication.if_index,
             // indication.channel_id.channel_num,
             // indication.reason);


      // /* Inform TX thread that we have sync loss */
      // is_sync_loss = 1;
      // break;

    // case ATLK_E_NOT_READY:
      // break;

    // default:
      // printf("v2x_dot4_channel_end_receive: %s", atlk_rc_to_str(rc));
    // }

    // /* sleep 10msec between each poll */
    // usleep(10000);
  // }
 // dot4_ch_end_ind_thx_is_up = false;
// }

#ifdef __THREADX__
int cli_v2x_wave6_init(struct cli_def *cli, const char *command, char *argv[], int argc)
{

  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* IPv6 address */
  NXD_ADDRESS ipv6_address;
  /* IPv4 address */
  ULONG ipv4_address;
  (void)command;
  static bool already_initilized = false;
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

  if (already_initilized)
	  return rc;

  /* Set V2X access profile of untrusted network interface #0 */
  v2x_netif_profile_t netif_profile = {
    .if_index = myctx->if_idx,
    .channel_id = V2X_CHANNEL_ID_INIT,
    .datarate = V2X_DATARATE_6MBPS,
    .power_dbm8 = -80
  };

  for (int i = 0; i < argc; i += 2) {
    GET_INT("-data_rate", netif_profile.datarate, i, "Specify the frame user priority, range 0:7");
    GET_INT("-op_class", netif_profile.channel_id.op_class, i, "Specify operational class");
    GET_INT("-ch_idx", netif_profile.channel_id.channel_num, i, "Sets the channel number (band) to be used");
  }

  rc = v2x_netif_profile_set(v2x_service, 0, &netif_profile);
  if (atlk_error(rc)) {
	  cli_print(cli, "v2x_netif_profile_set: %s\n", atlk_rc_to_str(rc));
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }
  /* Get `untrusted` IP instance */
  rc = net_ip_untrusted_instance_get(&untrusted_instance);
  if (atlk_error(rc)) {
	  cli_print(cli, "net_ip_untrusted_instance_get: %s\n", atlk_rc_to_str(rc));
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;

  }

  /* Enable IPv6 on the `untrusted` IP instance */
  nrv = nxd_ipv6_enable(untrusted_instance);
  if (nrv != NX_SUCCESS){
	  cli_print(cli, "v2x wave 6 ipv6 enable nrv (0x%02X)", (unsigned int)nrv);
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }

  /* Get IPv4 address of the `untrusted` IP instance */
  nrv = nx_ip_interface_info_get(untrusted_instance, 0, NULL, &ipv4_address,
                                 NULL, NULL, NULL, NULL);
  if (nrv != NX_SUCCESS) {
	  cli_print(cli, "v2x wave 6 if info get nrv (0x%02X)", (unsigned int)nrv);
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }

  /* Set IPv6 address based on retrieved IPv4 address */
  ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;
  ipv6_address.nxd_ip_address.v6[0] = 0x20010000;
  ipv6_address.nxd_ip_address.v6[1] = 0x0;
  ipv6_address.nxd_ip_address.v6[2] = 0x0;
  ipv6_address.nxd_ip_address.v6[3] = ipv4_address;

  /* Set IPv6 address of `untrusted` IP instance */
  nrv = nxd_ipv6_global_address_set(untrusted_instance, &ipv6_address, 64);
  if (nrv != NX_SUCCESS) {
	  cli_print(cli, "v2x wave 6 global addr set nrv (0x%02X)", (unsigned int)nrv);
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }
  already_initilized = true;
  error:
    return atlk_error(rc);


}


int cli_v2x_wave6_tx(struct cli_def *cli, const char *command, char *argv[], int argc)
{
  atlk_rc_t  rc = ATLK_OK;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* NetX UDP socket */
  NX_UDP_SOCKET udp_socket;
  /* NetX UDP packet pointer */
  NX_PACKET *udp_packet = NULL;
  /* NetX packet pool pointer */
  NX_PACKET_POOL *packet_pool = NULL;
  /* IPv6 multicast address */
  NXD_ADDRESS ipv6_multicast_address;
  int32_t       num_frames = 1;     /* total num frames to send */
  int32_t       rate_hz = 1;        /* rate of sending frames   */
  long unsigned int i = 0;
  int32_t       payload_len		 	= -1;
  char buf[wave6_msg_size_max];
  size_t size = 0;
  char tx_data[MAX_TX_MSG_LEN] = "";
  //v2x_send_params_t link_sk_tx_param = V2X_SEND_PARAMS_INIT;
  (void) command;

/*
    get user context
  user_context *myctx = (user_context *) cli_get_context(cli);
*/

  IS_HELP_ARG("link wave6 tx [-frames 1- ...] [-rate_hz 1 - ...] ");

  CHECK_NUM_ARGS /* make sure all parameter are there */

  for (i = 0; i < (long unsigned int)argc; i += 2) {
    GET_INT("-frames", num_frames, i, "Specify the number of frames to send");
    GET_INT("-rate_hz", rate_hz, i, "Specify the rate of frames to send");
    GET_INT("-payload_len", payload_len, i, "Sets the payload length insted of tx data parameter");
  }


  /* Point to the `untrusted` IP instance packet pool */
  packet_pool = untrusted_instance->nx_ip_default_packet_pool;

  /* Arbitrary multicast IPv6 address */
  ipv6_multicast_address.nxd_ip_version = NX_IP_VERSION_V6;
  ipv6_multicast_address.nxd_ip_address.v6[0] = 0xFF020000;
  ipv6_multicast_address.nxd_ip_address.v6[1] = 0x0;
  ipv6_multicast_address.nxd_ip_address.v6[2] = 0x0;
  ipv6_multicast_address.nxd_ip_address.v6[3] = 0x1;

  /* Create a UDP socket for sending UDP packets */
  nrv = nx_udp_socket_create(untrusted_instance, &udp_socket, "udp_tx_socket",
                             NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 5);
  if (nrv != NX_SUCCESS) {
	  cli_print(cli, "v2x wave 6 tx sock create nrv (0x%02X)", (unsigned int)nrv);
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }


  /* Bind UDP socket to client port */
  nrv = nx_udp_socket_bind(&udp_socket, WAVE6_CLIENT_PORT, TX_WAIT_FOREVER);
  if (nrv != NX_SUCCESS) {
	  cli_print(cli, "v2x wave 6 sock bind nrv (0x%02X)", (unsigned int)nrv);
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }


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
  cli_print(cli, "[%d] memset tx_data with %d 'F's", __LINE__, (int)payload_len);

  for (i = 0; i < (  long unsigned int)num_frames; ++i) {
      /* TX buffer */

	  /* Print message into buffer (with terminating \0) and update its size */

	  size += sprintf(buf+size, wave6_msg_fmt, i+1);
	  size += 1 + sprintf(buf+size, tx_data);
	  /* Allocate a packet from the packet pool */
	  //cli_print(cli, "[%d] size is %d", __LINE__, (int)size);
	  //cli_print(cli, "[%d] buffer is:   %s", __LINE__, (char *)&(buf[0]));
	  nrv = nx_packet_allocate(packet_pool, &udp_packet, NX_UDP_PACKET,
	                           TX_WAIT_FOREVER);
	  //cli_print(cli, "[%d] nx_packet_allocate %d", __LINE__, (unsigned int)nrv);
	  if (nrv != NX_SUCCESS) {
	  	cli_print(cli, "v2x wave 6 packet alloc nrv (0x%02X)", (unsigned int)nrv);
		  rc = ATLK_E_UNSPECIFIED;
		  goto error;
	  }


	  /* Write an arbitrary message */
	  nrv = nx_packet_data_append(udp_packet, buf, size, packet_pool, TX_WAIT_FOREVER);
	  //cli_print(cli, "[%d] nx_packet_data_append %d", __LINE__, (unsigned int)nrv);

    /* Send packet to the server */
    nrv = nxd_udp_socket_send(&udp_socket, udp_packet, &ipv6_multicast_address,
                              WAVE6_SERVER_PORT);
    //cli_print(cli, "[%d] nxd_udp_socket_send %d", __LINE__, (unsigned int)nrv);
    if (nrv != NX_SUCCESS) {
    	cli_print(cli, "v2x wave 6 sock send nrv (0x%02X)", (unsigned int)nrv);
  	  rc = ATLK_E_UNSPECIFIED;
  	  goto error;
    }
    buf[0] = '\0';
    size = 0;
    //cli_print(cli, "[%d]buf:  %s", __LINE__, (char *)&(buf[0]));
	/* Sleep 100 ms between transmissions */
	if ( (num_frames >= 1) && (rate_hz >= 1) ){
		int sleep_time_uSec = (1e6 / rate_hz );
		usleep( sleep_time_uSec );
	}
  }
  error:
  nx_udp_socket_unbind(&udp_socket);
  nx_udp_socket_delete(&udp_socket);
  return rc;
}

int cli_v2x_wave6_rx(struct cli_def *cli, const char *command, char *argv[], int argc)
{
  atlk_rc_t  rc = ATLK_OK;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* NetX UDP socket */
  NX_UDP_SOCKET udp_socket;
  /* NetX UDP packet */
  NX_PACKET *udp_packet = NULL;
  int32_t  frames = 1,i = 0, timeout = 50000;
  struct timeval session_start, current_cycle;

  (void) command;
/*
   get user context
  user_context *myctx = (user_context *) cli_get_context(cli);
*/
  IS_HELP_ARG("link wave6 rx [-frames 1- ...] [-timeout_ms (0-1e6)");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", frames, i, "Specify the number of frames to receive");
    GET_INT("-timeout_ms", timeout, i, "Define timeout of last frame");
  }

  /* Create a UDP socket for receiving UDP packets */
  nrv = nx_udp_socket_create(untrusted_instance, &udp_socket, "udp_rx_socket",
                             NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 5);
  if (nrv != NX_SUCCESS) {
	  cli_print(cli, "v2x wave 6 rx sock create nrv (0x%02X)", (unsigned int)nrv);
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }


  /* Bind UDP socket to server port */
  nrv = nx_udp_socket_bind(&udp_socket, WAVE6_SERVER_PORT, TX_WAIT_FOREVER);
  if (nrv != NX_SUCCESS) {
	  cli_print(cli, "v2x wave 6 rx sock bind nrv (0x%02X)", (unsigned int)nrv);
	  rc = ATLK_E_UNSPECIFIED;
	  goto error;
  }


  gettimeofday (&session_start, NULL);
  while ( frames-- ) {

    /* Receive a UDP packet */
    nrv = nx_udp_socket_receive(&udp_socket, &udp_packet, timeout);

    if (nrv != NX_SUCCESS) {
    	cli_print(cli, "v2x wave 6 rx recv nrv (0x%02X)", (unsigned int)nrv);
  	  rc = ATLK_E_UNSPECIFIED;
  	  goto error;
    }

	gettimeofday (&current_cycle, NULL);
    double elapsed_from_session_start = (current_cycle.tv_sec - session_start.tv_sec) * 1000.0;
    if (elapsed_from_session_start > timeout) {
		cli_print(cli, "ERROR : rx session timed out:");
		goto error;
    }

    /* Print length and first bytes of received packet */
    if (udp_packet->nx_packet_prepend_ptr[udp_packet->nx_packet_length - 1]
        != '\0') {
    	cli_print(cli, "Received a bad message (not zero-terminated)");
    }
/*
    else {
    	cli_print(cli, "Received message: \"%s\"", udp_packet->nx_packet_prepend_ptr);
    }
*/

    /* Release the packet */
    nx_packet_release(udp_packet);
  }
  error:
    nx_udp_socket_unbind(&udp_socket);
    nx_udp_socket_delete(&udp_socket);
    return atlk_error(rc);

}
#endif


