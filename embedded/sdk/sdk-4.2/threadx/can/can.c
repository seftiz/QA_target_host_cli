
#include "../../common/v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "can.h"

/* CAN service */
static can_service_t *can_service = NULL;
can_params_t          can_info;



static unsigned int 			m_can_rx_packets = 0,
													m_can_tx_packets = 0;


/* CAN bus service init */
int cli_v2x_can_service_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t 		rc 						= ATLK_OK;
  char          str_data[256] = "hw";
  int32_t       i             = 0;
  
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  (void) myctx; /* not used  */
  IS_HELP_ARG("can service create -type hw|remote [-server_addr ip_address]")
  CHECK_NUM_ARGS
  GET_STRING("-type", str_data, i, "Specify service type, local or remote");
	
  if ( strcmp( (char*) str_data,  "hw") == 0 ) {

		/* Get CAN service instance */
		rc = can_hw_service_get(&can_service);
		if (atlk_error(rc)) {
			cli_print( cli, "can_hw_service_get: %s\n", atlk_rc_to_str(rc));
			goto error;
		}
  } 
  else if ( strcmp( (char*) str_data, "remote") == 0 ) {
    cli_print( cli, "ERROR : Remote can is not implemented");
  } 
  else {
    cli_print( cli, "ERROR : unknown mode of can api");
  }
	
	return CLI_OK;
	
error:
  return atlk_error(rc);
}

int cli_v2x_can_service_delete( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
	(void) myctx; /* not used  */

  IS_HELP_ARG("can service delete")

  can_service_delete(can_service);
  can_service = NULL;
	
	return CLI_OK;
}

/* CAN bus service init */
int cli_v2x_can_socket_create( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t 		rc 						= ATLK_OK;
  int32_t       i             = 0,
								filter_count	= 0;
	int 					filter_idx;
								
  /* get user context */
  // user_context *myctx = (user_context *) cli_get_context(cli);

	IS_HELP_ARG("can socket create -filter_count 0|N [can_id_1 0x1 can_id_mask_1 0xFFFFFFFFU .... can_id_N 0x1 can_id_mask_N 0xFFFFFFFFU]")
  CHECK_NUM_ARGS

	can_info.can_params.device_id = DEFAULT_CAN_IF_INDEX;
	can_info.can_params.filter_array_ptr = NULL;
	
			//CAN_SOCKET_CREATE_PARAMS_INIT;
	
	GET_INT("-filter_count", filter_count, 0, "Specify filter fields");
	if ( filter_count != (argc-2) ) {
		cli_print( cli, "ERROR : Mismatch count of filter, count %d, number of filter args is %d", (int) filter_count, (argc-2) );
		goto error;
	}
	
	if ( filter_count > 0 ) {
		can_info.can_params.filter_array_size = filter_count;
		can_info.filter_array = calloc( filter_count , sizeof(can_id_filter_t) );
  
		for ( i=2,filter_idx=0 ; i < argc; i += 2, filter_idx++ ) {

      char 	filter_can_name[20] = "",
            filter_can_mask_name[20] = "";

			sprintf(filter_can_name, "-can_id_%d", filter_idx+1);
			sprintf(filter_can_mask_name, "-can_id_mask_%d", filter_idx+1);
						
			/* CAN_ID_FILTER_ONE_ID */
			
			GET_TYPE_INT(filter_can_name, can_info.filter_array[filter_idx].can_id, unsigned int, i, "Specify the number of frames to send", "%x");
			GET_TYPE_INT(filter_can_mask_name, can_info.filter_array[filter_idx].can_id_mask, unsigned int, i, "Specify the number of frames to send", "%x");
		} 
	}
	else {
		/* Initilize no filter */
    can_info.can_params.filter_array_size = 1;
		can_info.filter_array = calloc( can_info.can_params.filter_array_size , sizeof(can_id_filter_t) );
		can_info.filter_array[0].can_id = 0;
		can_info.filter_array[0].can_id_mask = 0;
	}
	
	can_info.can_params.filter_array_ptr = can_info.filter_array;
	if ( can_info.can_params.filter_array_ptr == NULL ) {
		cli_print( cli, "ERROR :filter_array not initilized properly" );
		goto error;
	}
	
	/* Create CAN socket */
  rc = can_socket_create(can_service, &can_info.can_socket, &can_info.can_params);
  if (atlk_error(rc)) {
    cli_print( cli, "can_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

	return CLI_OK;
	
error:
  return atlk_error(rc);
}


/* CAN bus service init */
int cli_v2x_can_socket_delete( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  
  //user_context *myctx = (user_context *) cli_get_context(cli);
	IS_HELP_ARG("can socket delete")
	
 
	if ( can_info.filter_array != NULL ) {
		free(can_info.filter_array);
	}
  
	can_socket_delete( can_info.can_socket );
	can_info.can_socket = NULL;

	
	return CLI_OK;
}

int cli_v2x_can_rx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
  atlk_rc_t      	rc = ATLK_OK;
  int32_t         frames   		= 1,
                  i        		= 0,
									print_frms	=	0;
	
	uint8_t 				data[CAN_DATA_SIZE_MAX];
  size_t 					data_size = sizeof(data);
  can_id_t 				can_id;
									

  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  IS_HELP_ARG("can socket rx [-frames 1- ... [-print (0|1)]");
  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", frames, i, "Specify the number of can frames to receive");
    GET_INT("-print", print_frms, i, "Sets frames printing");
  } 
  i = 0;
//	myctx->cntrs.can_rx_packets = 0;
	
  while ( i < frames ) {

		memset( data, 0, sizeof(data) );
    rc = can_receive(can_info.can_socket, data, &data_size, &can_id, &atlk_wait_forever);
    if (atlk_error(rc)) {
      cli_print ( cli, "can_receive: %s\n", atlk_rc_to_str(rc) );
      continue;
    }
    
    i++;
  	myctx->cntrs.can_rx_packets ++;
		m_can_rx_packets ++;
		
		if ( print_frms ) {
      cli_print( cli, "TIME : %u, CAN RX %d : ID = 0x%x, DLC = %d, Data[0:7] = 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x", (unsigned int) tx_time_get(), (int) i,
                 (unsigned int)can_id, data_size,
				 data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
		}
  }
  
  return atlk_error(rc);
}

int cli_v2x_can_tx( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
  int32_t       num_frames 		= 1;     /* total num frames to send */
  int32_t       rate_hz 			= 0;        /* rate of sending frames, 0 Max speed   */
  int32_t       data_size		 	= -1; 
  can_id_t 			can_id				= 0x50;
								
	size_t				msg_size = 0;
  char          can_data[CAN_DATA_SIZE_MAX * 2 + 1] = { 0 },
								hex_arr[CAN_DATA_SIZE_MAX] = { 0 };
                
  atlk_rc_t      rc = ATLK_OK;
	int						 i;
  
  struct timeval start, current;

  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("can socket tx [-frames 1- ...] [-rate_hz 1 - ...] [-can_id 0x1 -can_data '000102030405' | -data_size 8]");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
    GET_INT("-frames", num_frames, i, "Specify the number of frames to send");
    GET_INT("-rate_hz", rate_hz, i, "Specify the rate of frames to send");
		GET_INT("-data_size", data_size, i, "Sets the payload length instead of tx data parameter");
		GET_HEX("-can_id", can_id, i, "Specify the can id of frame");
  } 
   
  GET_STRING_VALUE("-can_data", can_data,"Define data to send over the link layer");
  
  if ( strlen(can_data) == 0 ) {
		if (data_size == -1 ) {
			data_size = CAN_DATA_SIZE_MAX;
		}

		sprintf(can_data , "%.*s", (int) data_size*2, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
  }
  else {
	  if (data_size == -1) {
		  data_size = (size_t)(strlen(can_data) / 2);
	  }
  }

  if ( data_size > CAN_DATA_SIZE_MAX ) {
    data_size = CAN_DATA_SIZE_MAX;
  } 
  
  /* verify user data is not to long */
  if ( strlen(can_data) > (CAN_DATA_SIZE_MAX*2) ) {
    can_data[(CAN_DATA_SIZE_MAX*2)] = '\0';
  }
  
  
	msg_size = (size_t) (strlen(can_data) / 2);
	hexstr_to_buffer(can_data, strlen(can_data), (uint8_t*) hex_arr, &msg_size);

  for (i = 0; i < num_frames; i++) {
 		gettimeofday (&start, NULL);	
		rc = can_send(can_info.can_socket, hex_arr, data_size, can_id, NULL);
		if (atlk_error(rc)) {
			cli_print(cli, "can_send: %s\n", atlk_rc_to_str(rc));
			goto error;
		}
		else {
      /* Calc blocking time for can send */
      gettimeofday (&current, NULL);
      double elapsedTime = (current.tv_sec - start.tv_sec) * 1e6;

			myctx->cntrs.can_tx_packets ++;
			m_can_tx_packets ++;
			/* Sleep 100 ms between transmissions */
			if ( (num_frames >= 1) && (rate_hz >= 1) ){
				int sleep_time_uSec = ( (1e6 - elapsedTime) / rate_hz );
				usleep( sleep_time_uSec );
			}
		}
	}
error:
  return rc;
}


int cli_v2x_can_reset_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)  ) 
{
	/* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);

	myctx->cntrs.can_rx_packets = 0;
	myctx->cntrs.can_tx_packets = 0;
	m_can_rx_packets = 0;
	m_can_tx_packets = 0;
	
	return ATLK_OK;
}

int cli_v2x_can_print_cntrs( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
 /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
	cli_print(cli, "TX : module = %u, session = %u", m_can_tx_packets, myctx->cntrs.can_tx_packets ); 
	cli_print(cli, "RX : module = %u, session = %u", m_can_rx_packets, myctx->cntrs.can_rx_packets ); 

	return ATLK_OK;
}
