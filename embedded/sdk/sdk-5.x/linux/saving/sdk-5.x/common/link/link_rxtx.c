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

#include "link.h"
#include "../../linux/remote/remote.h"
#include "../management/mng_api.h"

/* threads priorities */
#define TX_THREAD_PRIORITY 40
#define MAX_V2X_THREADS 2

#define V2X_IF_1 1
#define V2X_IF_2 2

static void rxtx_thread_entry(ULONG input);
static int configure_linkrxtx_lb_session(int *num_of_threads, struct cli_def *cli);


/* default Protocol identifier used */
#define PROTO_ID 0x102ULL

/* link_rxtx message format string: Example <seq_num> */
static const char msg_fmt[] = "link_rxtx %" PRIu32;

/* Example message string maximum length */
static const size_t msg_size_max = sizeof(msg_fmt) + 10;

/* V2X service */
static v2x_service_t *v2x_service = NULL;


struct v2x_thread_param{
	v2x_socket_t *v2x_socket;
	v2x_socket_config_t socket_config;
	struct cli_def *cli;
	uint8_t rxtx_thread_stack[0x1000];
	TX_THREAD rxtx_thread;
	bool end_thread;
};

static struct v2x_thread_param v2x_thread_params[MAX_V2X_THREADS];

/* loop back flag */
static bool lb_on = false;

static int orig_wlan_freq_data = -1;

/* MIB service */
mib_service_t 	*mib_service;


int configure_linkrxtx_lb_session(int *num_of_threads, struct cli_def *cli)
{
	atlk_rc_t rc = ATLK_OK;
	int lb_wlan_freq_data = ERR_VALUE;

	*num_of_threads = MAX_V2X_THREADS;

	/*save wlan if#2 original freq*/
	rc = mib_get_wlanFrequency(mib_service, (int)V2X_IF_2, (int32_t*)&orig_wlan_freq_data);
	if (atlk_error(rc)) {
		cli_print(cli, "ERROR : mib_get_wlanDefaultTxDataRate failed for if V2X_IF_2 and value %d: %s\n", orig_wlan_freq_data, atlk_rc_to_str(rc));
		return rc;
	}
	cli_print(cli, "DBG : mib_get_wlanDefaultTxDataRate for if V2X_IF_2 (save) value %d: %s\n", orig_wlan_freq_data, atlk_rc_to_str(rc));

	/*extract wlan if#1 freq for loop back session*/
	rc = mib_get_wlanFrequency(mib_service, (int)V2X_IF_1, (int32_t*)&lb_wlan_freq_data);
	if (atlk_error(rc)) {
		cli_print(cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if V2X_IF_1 and value %d: %s\n", lb_wlan_freq_data, atlk_rc_to_str(rc));
		return rc;
	}
	cli_print(cli, "DBG : mib_get_wlanDefaultTxDataRate for if V2X_IF_1 value %d: %s\n", lb_wlan_freq_data, atlk_rc_to_str(rc));
	/*set if#1 freq to if#2*/
	rc = mib_set_wlanFrequency(mib_service, (int)V2X_IF_2, lb_wlan_freq_data);
	if (atlk_error(rc)) {
		cli_print(cli, "ERROR :  wlanFrequency  failed for if V2X_IF_2 and value %d: %s\n", lb_wlan_freq_data, atlk_rc_to_str(rc));
		return rc;
	}
	cli_print(cli, "DBG : mib_set_wlanDefaultTxDataRate for if V2X_IF_2 value %d: %s\n", lb_wlan_freq_data, atlk_rc_to_str(rc));

	return rc;
}

int cli_v2x_link_rxtx_start(struct cli_def *cli, const char *command, char *argv[], int argc)
{

	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	static const char ThreadName_fmt[] = "rxtx_thread %" PRIu32;
	ULONG param;
	char thread_name[128];
	ULONG trv = TX_SUCCESS;
	int   num_of_threads = 1, i = 0;

	(void)command;

	/* Get default V2X service instance */
	rc = v2x_default_service_get(&v2x_service);
	if (atlk_error(rc)) {
		cli_print(cli, "v2x_default_service_get: %s\n", atlk_rc_to_str(rc));
		goto error;
	}

	/* Get default MIB service */
	rc = mib_default_service_get(&mib_service);
	if (atlk_error(rc)) {
		cli_print(cli, "mib_default_service_get: %s , please change freq of if#2 manual for loop back mode support\n", atlk_rc_to_str(rc));
	}

	
	if (argc > 0)
		lb_on = (strcmp(argv[0], "-lb") == 0) ? true : false;
	
	if (lb_on)
		rc = configure_linkrxtx_lb_session(&(num_of_threads), cli);

	if (rc != TX_SUCCESS){
		cli_print(cli, "configure_linkrxtx_lb_session: %s\n", atlk_rc_to_str(rc));
		goto error;
	}

	
	for (i = 0; i < num_of_threads; i++){
		v2x_thread_params[i].end_thread = false;
		v2x_thread_params[i].socket_config.if_index = i+1;
		v2x_thread_params[i].socket_config.protocol.protocol_id = PROTO_ID;
		v2x_thread_params[i].cli = cli;
		
		param = (ULONG)&(v2x_thread_params[i]);
		snprintf(thread_name, sizeof(thread_name), ThreadName_fmt, (ULONG)i);
		
		trv = tx_thread_create(&(v2x_thread_params[i].rxtx_thread), thread_name,
			rxtx_thread_entry, param,
			v2x_thread_params[i].rxtx_thread_stack,
			sizeof(v2x_thread_params[i].rxtx_thread_stack),
			TX_THREAD_PRIORITY,
			TX_THREAD_PRIORITY,
			TX_NO_TIME_SLICE, TX_AUTO_START);
		if (trv != TX_SUCCESS){
			cli_print(cli, "tx_thread_create: %s\n", atlk_rc_to_str(trv));
				goto error;
		}
	}
	
	return ATLK_OK;
error:
/* Clean-up resources */
	/* clean already running threads...*/
	if (i > 0){
		while(i){
			v2x_thread_params[i].end_thread = true;
			cli_print(cli, "link rxtx thread #%d terminated\n", i);
			tx_thread_terminate(&(v2x_thread_params[i].rxtx_thread));
			tx_thread_delete(&(v2x_thread_params[i].rxtx_thread));
			i--;
		}

	}
   v2x_service_delete(v2x_service);

   if ((orig_wlan_freq_data != -1) && (lb_on == true)){
	   /* if loop back TC then try to fall back to original freq in v2x if#2 */
	   rc = mib_set_wlanFrequency(NULL, (int)V2X_IF_2, orig_wlan_freq_data);
	   if (atlk_error(rc)) {
		   cli_print(cli, "ERROR :  wlanFrequency  failed for if 2 orig freq value %d: %s\n", orig_wlan_freq_data, atlk_rc_to_str(rc));
	   }
	   orig_wlan_freq_data = -1;
   }
   return atlk_error(rc);
}


 void rxtx_thread_entry(ULONG input)
{
  
  atlk_rc_t rc = ATLK_OK;
 
  v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
  
  uint32_t msg_count = 0;

  struct v2x_thread_param *v2xthread_param_ptr = NULL;
  v2xthread_param_ptr = (struct v2x_thread_param *)input;
 
  char buf[msg_size_max];

  char rxbuf[msg_size_max];

  size_t rxsize = sizeof(rxbuf);
 
  v2x_receive_params_t receive_params = V2X_RECEIVE_PARAMS_INIT;

  printf("link_rxtx - [%s] : %lu\n", __FUNCTION__, input);
  rc = v2x_socket_create(v2x_service, &(v2xthread_param_ptr->v2x_socket), &(v2xthread_param_ptr->socket_config));
  if (atlk_error(rc)) {
	  printf("v2x_socket_create: %s\n", atlk_rc_to_str(rc));
  }

  printf("current socket [%p]\n\n", (void *)(v2xthread_param_ptr->v2x_socket));

  while (!(v2xthread_param_ptr->end_thread)) {
    
    size_t size = 1 + snprintf(buf, sizeof(buf), msg_fmt, msg_count);
    msg_count++;

    /* Transmit V2X PDU */
	rc = v2x_send(v2xthread_param_ptr->v2x_socket, buf, size, &send_params, NULL);
    if (atlk_error(rc)) {
		printf("v2x_send error: %s\n", atlk_rc_to_str(rc));
    }
	
	tx_thread_sleep(5);
	
    /* Receive frame (wait forever until it arrives) */
	rc = v2x_receive(v2xthread_param_ptr->v2x_socket, rxbuf, &rxsize, &receive_params, NULL /*&atlk_wait*/);

    if (atlk_error(rc)) {
		printf("[%p]v2x_receive error: id %d, desc : %s\n", (void *)v2xthread_param_ptr->v2x_socket, rc, atlk_rc_to_str(rc));
    }
  }
  
  /* Clean-up resources */
  v2x_socket_delete(v2xthread_param_ptr->v2x_socket);
  printf("[%p]v2x_socket@thread#%lu terminated...\n", (void *)v2xthread_param_ptr->v2x_socket, input);
  
}

 int cli_v2x_link_rxtx_stop(struct cli_def *cli, const char *command, char *argv[], int argc)
 {
	 /* Autotalks return code */
	 atlk_rc_t rc = ATLK_OK;

	 (void)command;
	 (void)argv;
	 (void)argc;
	 int i = 0;
	
	 int ii = (lb_on) ? MAX_V2X_THREADS : 1;
	 for (i = 0; i < ii; i++){
		 v2x_thread_params[i].end_thread = true;
		 cli_print(cli, "link rxtx thread #%d terminated\n", i);
		 tx_thread_terminate(&(v2x_thread_params[i].rxtx_thread));
		 tx_thread_delete(&(v2x_thread_params[i].rxtx_thread));
	 }

	 v2x_service_delete(v2x_service);
	 cli_print(cli, "link v2x_service terminated\n");

	 if (orig_wlan_freq_data != -1){
		 rc = mib_set_wlanFrequency(mib_service, (int)2, orig_wlan_freq_data);
		 if (atlk_error(rc)) {
			 cli_print(cli, "ERROR :  wlanFrequency  failed for if 2 orig freq value %d: %s\n", orig_wlan_freq_data, atlk_rc_to_str(rc));
		 }
		 else{
			 cli_print(cli, "mib_set_wlanFrequency set if#2 back to %d\n", orig_wlan_freq_data);
		 }
		 orig_wlan_freq_data = -1;
	 }
	 return ATLK_OK;
 }
 


