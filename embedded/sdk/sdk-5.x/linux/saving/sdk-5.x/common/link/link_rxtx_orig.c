#include <stdio.h>
#include "../general/general.h"
#include "../v2x_cli/v2x_cli.h"
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <tx_api.h>
#include <stdbool.h>
//#include <crtdefs.h>

#include "link.h"
#include "../../linux/remote/remote.h"

/* threads priorities */
#define TX_THREAD_PRIORITY 40


#define MAX_V2X_THREADS 2
/* RXTX thread */
//static TX_THREAD rxtx_thread;
//static TX_THREAD rxtx_thread_2;

//static uint8_t rxtx_thread_stack[0x1000];
static void rxtx_thread_entry(ULONG input);


/* Interface index for non loop back session*/
#define IF_INDEX_1 1
#define IF_INDEX_2 2

/* default Protocol identifier used */
#define PROTO_ID 0x102ULL

/* txrx Shared V2X sockets for two link ifs */
//static v2x_socket_t *v2x_socket_1 = NULL;
//static v2x_socket_t *v2x_socket_2 = NULL;

/* Example message format string: Example <seq_num> */
static const char msg_fmt[] = "Example %" PRIu32;

/* Example message string maximum length */
static const size_t msg_size_max = sizeof(msg_fmt) + 10;

/* V2X service */
static v2x_service_t *v2x_service = NULL;

//static bool endThread = false;


//static struct cli_def *link_cli;

struct v2xThreadParam{
	v2x_socket_t *v2x_socket;
	v2x_socket_config_t socket_config;
	struct cli_def *cli;
	uint8_t rxtx_thread_stack[0x1000];
	TX_THREAD rxtx_thread;
	bool endThread;
};

static struct v2xThreadParam v2xThreadParams[MAX_V2X_THREADS];
 /* loop back flag */
static bool lb_on = false;

int cli_v2x_link_rxtx_start(struct cli_def *cli, const char *command, char *argv[], int argc)
{
	/* ThreadX return value */
	//ULONG trv = TX_SUCCESS;
	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	/* V2X socket configuration */
	//v2x_socket_config_t socket_config = V2X_SOCKET_CONFIG_INIT;
	static const char ThreadName_fmt[] = "rxtx_thread %" PRIu32;
	
	ULONG param;
	char ThreadName[128];
	(void)command;
	/* Get default V2X service instance */
	rc = v2x_default_service_get(&v2x_service);
	if (atlk_error(rc)) {
		cli_print(cli, "v2x_default_service_get: %s\n", atlk_rc_to_str(rc));
		goto error;
	}

	
	if (argc > 0)
		lb_on = (strcmp(argv[0], "-lb") == 0) ? true : false;

	//
	///* Set socket configuration */
	//socket_config.if_index = IF_INDEX_1;
	//socket_config.protocol.protocol_id = PROTO_ID;
	///* Create a V2X socket */
	//rc = v2x_socket_create(v2x_service, &v2x_socket_1, &socket_config);
	//if (atlk_error(rc)) {
	//	cli_print(cli, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
	//	goto error;
	//}

	///*save global cli pointer to be used by the threads*/
	//link_cli = cli;

	///* Create RXTX thread for first v2x rxtx generator*/
	//trv = tx_thread_create(&rxtx_thread, "rxtx_thread",
	//	rxtx_thread_entry, 0,
	//	rxtx_thread_stack,
	//	sizeof(rxtx_thread_stack),
	//	TX_THREAD_PRIORITY,
	//	TX_THREAD_PRIORITY,
	//	TX_NO_TIME_SLICE, TX_AUTO_START);
	//if (trv != TX_SUCCESS) {
	//	cli_print(cli, "tx_thread_create: %s\n", atlk_rc_to_str(rc));
	//	goto error;
	//}


	//if (lb_on){
	//	socket_config.if_index = IF_INDEX_2;
	//	rc = v2x_socket_create(v2x_service, &v2x_socket_2, &socket_config);
	//	cli_print(cli, "v2x_socket_2 created\n");
	//	if (atlk_error(rc)) {
	//		cli_print(cli, "v2x_socket(2)_create: %s\n", atlk_rc_to_str(rc));
	//		goto error;
	//	}
	//	cli_print(cli, "v2x_socket_2 created\n");
	//		/* Create RXTX thread for second v2x rxtx generator*/
	//	trv = tx_thread_create(&rxtx_thread_2, "rxtx_thread_2",
	//		rxtx_thread_entry, 1,
	//		rxtx_thread_2_stack,
	//		sizeof(rxtx_thread_2_stack),
	//		TX_THREAD_PRIORITY,
	//		TX_THREAD_PRIORITY,
	//		TX_NO_TIME_SLICE, TX_AUTO_START);
	//	
	//	if (trv != TX_SUCCESS) {
	//		cli_print(cli, "tx_thread_create: %s\n", atlk_rc_to_str(rc));
	//		goto error;
	//	}
	//}
	//
	int ii = (lb_on) ? MAX_V2X_THREADS : 1;
	for (int i = 0; i < ii; i++){
		v2xThreadParams[i].endThread = false;
		v2xThreadParams[i].socket_config.if_index = i+1;
		v2xThreadParams[i].socket_config.protocol.protocol_id = PROTO_ID;
		v2xThreadParams[i].cli = cli;
		param = (ULONG)&(v2xThreadParams[i]);
		snprintf(ThreadName, sizeof(ThreadName), ThreadName_fmt, (ULONG)i);
		
		tx_thread_create(&(v2xThreadParams[i].rxtx_thread), ThreadName,
			rxtx_thread_entry, param,
			v2xThreadParams[i].rxtx_thread_stack,
			sizeof(v2xThreadParams[i].rxtx_thread_stack),
			TX_THREAD_PRIORITY,
			TX_THREAD_PRIORITY,
			TX_NO_TIME_SLICE, TX_AUTO_START);
	}
	
	return ATLK_OK;
error:
/* Clean-up resources */
   v2x_service_delete(v2x_service);
   return atlk_error(rc);
}


 void rxtx_thread_entry(ULONG input)
{
	/* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Send parameters */
  v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
  /* Message counter */
  uint32_t msg_count = 0;
  /* Not using input */
  struct v2xThreadParam *v2xThreadPar;// = (struct v2xThreadParam *)input;
  v2xThreadPar = (struct v2xThreadParam *)input;
  atlk_wait_t atlk_wait;
  atlk_wait.wait_type = ATLK_WAIT_INTERVAL;
  atlk_wait.wait_usec = 1000000;

  /* Set transmit power to -10 dBm */
  // send_params.power_dbm8 = -80;

  /* TX buffer */
  char buf[msg_size_max];

  /* RX buffer */
  char rxbuf[msg_size_max];

  /* RX size */
  size_t rxsize = sizeof(rxbuf);
  
  /*extract current thread socket*/
  /*v2x_socket_t *v2x_socket = NULL;
  if (input == 1){
	  v2x_socket = v2x_socket_2;
  }
  else{
	  v2x_socket = v2x_socket_1;
  }*/
  cli_print(v2xThreadPar->cli, "link_rxtx - [%s] : %d", __FUNCTION__, (int)input);

  

  rc = v2x_socket_create(v2x_service, &(v2xThreadPar->v2x_socket), &(v2xThreadPar->socket_config));
   if (atlk_error(rc)) {
	  cli_print(v2xThreadPar->cli, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
	  goto error;
  }
  cli_print(v2xThreadPar->cli, "current socket [%p]\n\n", (void *)(v2xThreadPar->v2x_socket));

  while (!(v2xThreadPar->endThread)) {
    
    size_t size = 1 + snprintf(buf, sizeof(buf), msg_fmt, msg_count);
    msg_count++;

	cli_print(v2xThreadPar->cli, "[%p]link_rxtx: Broadcast TX : \"%s\"\n", (void *)(v2xThreadPar->v2x_socket), buf);

    /* Transmit V2X PDU */
	//rc = v2x_send(v2x_socket, buf, size, &send_params, NULL);
	rc = v2x_send(v2xThreadPar->v2x_socket, buf, size, &send_params, NULL);
    if (atlk_error(rc)) {
		cli_print(v2xThreadPar->cli, "v2x_send error: %s\n", atlk_rc_to_str(rc));
      //return;
    }

	tx_thread_sleep(100);
	/* Received V2X parameters */
    v2x_receive_params_t receive_params = V2X_RECEIVE_PARAMS_INIT;
	(void)atlk_wait;
    /* Receive frame (wait forever until it arrives) */
	//rc = v2x_receive(v2x_socket, rxbuf, &rxsize, &receive_params, NULL /*&atlk_wait*/);
	rc = v2x_receive(v2xThreadPar->v2x_socket, rxbuf, &rxsize, &receive_params, NULL /*&atlk_wait*/);

    if (atlk_error(rc)) {
		cli_print(v2xThreadPar->cli, "[%p]v2x_receive error: id %d, desc : %s\n", (void *)v2xThreadPar->v2x_socket, rc, atlk_rc_to_str(rc));
      //return;
    }
	else {
		/* Print source address of received frame */
		cli_print(v2xThreadPar->cli, "link_rxtx: RX from %02x:%02x:%02x:%02x:%02x:%02x\n",
			receive_params.source_address.octets[0],
			receive_params.source_address.octets[1],
			receive_params.source_address.octets[2],
			receive_params.source_address.octets[3],
			receive_params.source_address.octets[4],
			receive_params.source_address.octets[5]);


	}

	/* Sleep 1 second between transmissions */
	tx_thread_sleep(100);
	// usleep(1000000);

   
  }
error:
  /* Clean-up resources */
  v2x_socket_delete(v2xThreadPar->v2x_socket);
  
}

 int cli_v2x_link_rxtx_stop(struct cli_def *cli, const char *command, char *argv[], int argc)
 {
	 /* get user context */
	 (void)command;
	 (void)argv;
	 (void)argc;
	 //ULONG trv = TX_SUCCESS;
	 int i = 0;
	 //endThread = true;
	 /* Autotalks return code */
	 //atlk_rc_t rc = ATLK_OK;
	 //trv = tx_thread_terminate(&rxtx_thread_2);
	 //if (trv != TX_SUCCESS) {
		// cli_print(cli, "link_rxtx: v2x_socket(2)_terminate: %s\n", atlk_rc_to_str(rc));
		// goto error;
	 //}
	 //trv = tx_thread_terminate(&rxtx_thread);
	 //if (trv != TX_SUCCESS) {
		// cli_print(cli, "v2x_socket_terminate: %s\n", atlk_rc_to_str(rc));
		// goto error;
	 //}
	 //cli_print(cli, "link rxtx thread terminated");
	
	 int ii = (lb_on) ? MAX_V2X_THREADS : 1;
	 for (i = 0; i > ii; i++){
		 v2xThreadParams[i].endThread = true;
	 }

	 cli_print(cli, "link rxtx thread terminated");

	 return ATLK_OK;

 //error:
	// /* Clean-up resources */
	// v2x_socket_delete(v2xThreadParams[i].v2x_socket);
	// v2x_service_delete(v2x_service);

	// return atlk_error(trv);
 }
 


