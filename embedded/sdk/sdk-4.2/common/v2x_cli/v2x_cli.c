
//#include <sys/types.h>

#ifdef WIN32
  #include <winsock2.h>
  #include <windows.h>
  
#elif defined(__THREADX__)
  #include <libcli.h>
  #include <nxd_bsd.h>
  #include <atlk/sdk.h>
	#include <atlk/mibs/wlan-mib.h>
	#include <atlk/mibs/vca-mib.h>
	#include "../../common/management/mng_api.h"

#elif defined (__LINUX__)
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
	#include <pthread.h>

#endif

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "list.h"
#include "v2x_cli.h"
//#include "../../common/session/session.h"
#include "../../common/nav_api/nav_api.h"
#include "../../common/link/link.h"

#ifdef __THREADX__
#include "../../threadx/phy/phy_cca_control.h"
#include "../../threadx/cpu/cpu_load.h"
#include "../../threadx/can/can.h"
#include "../../threadx/ecc/ecc.h"

#endif	



#ifdef __THREADX__
void v2x_cli_main(ULONG arg);
void connection_handler(ULONG socket_desc);
#elif defined (__LINUX__)
void *connection_handler( void *thread_desc );
#endif

void cli_cleanup( struct cli_def *cli );

#ifdef __THREADX__
static TX_THREAD    cli_main_thread;
static uint8_t      cli_main_thread_stack[CLI_THREAD_STACKSIZE];
#endif

struct list_head cli_thread_list;
int               thread_cnt;

/* in threadx this functions defined in the library */
int cmd_config_int_exit(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc));
int get_user_context_addr( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) );
int set_user_context_addr( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ); 

/* profiling function in uut cli */
int cmd_prof(struct cli_def *cli, const char *command, char *argv[], int argc);
int prof_reset(struct cli_def *cli);


int qa_check_enable(const char *password)
{
  return !strcasecmp(password, PASSWORD);
}


void qa_pc(UNUSED(struct cli_def *cli), const char *string)
{
  printf("%s\n", string);
}

int qa_idle_timeout(struct cli_def *cli)
{
  cli_print(cli, "Custom idle timeout");
  return CLI_QUIT;
}

int qa_check_auth(const char *username, const char *password)
{
  if (strcasecmp(username, USERNAME) != 0)
    return CLI_ERROR;
  if (strcasecmp(password, PASSWORD) != 0)
    return CLI_ERROR;
  return CLI_OK;
}

int show_version( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  cli_print( cli, "Version : 3.2" );
  return CLI_OK;
}


int create_cli_struct( struct cli_def **cli )
{
  struct cli_command    *c        = NULL; 
  struct cli_command    *d        = NULL; 

  *cli = cli_init();

  cli_set_hostname(*cli, "v2x >");
  cli_set_banner(*cli, "Autotalks - Confidence of Knowing Ahead.\nWelcome to Auto-talks V2X cli\n\n");

  cli_telnet_protocol(*cli, 1);

  // cli_regular(*cli, regular_callback);
  cli_regular_interval(*cli, 5); // Defaults to 1 second
  cli_set_idle_timeout_callback(*cli, 0, qa_idle_timeout); /* 5 Minutes idle timeout */

	

  c = cli_register_command(*cli, NULL, "show", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
  cli_register_command(*cli, c, "version", show_version, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show version");
	
   /* link commands */
  c = cli_register_command(*cli, NULL, "link", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "send message frame via SDK");
  d = cli_register_command(*cli, c, "service", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set management mibs");
  cli_register_command(*cli, d, "create", cli_v2x_link_service_create, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "open link new socket");
  cli_register_command(*cli, d, "delete", cli_v2x_link_service_delete, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "close link socket");
  d = cli_register_command(*cli, c, "socket", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set management mibs");
  cli_register_command(*cli, d, "create", cli_v2x_link_socket_create, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "open link new socket");
  cli_register_command(*cli, d, "delete", cli_v2x_link_socket_delete, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "close link socket");
	cli_register_command(*cli, d, "tx", cli_v2x_link_tx, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Transmit Data via link socket");
  cli_register_command(*cli, d, "rx", cli_v2x_link_rx, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Receive Data via link socket");
  cli_register_command(*cli, d, "set", cli_v2x_set_link_socket_addr, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Receive Data via link socket");
  cli_register_command(*cli, d, "get", cli_v2x_get_link_socket_addr, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Receive Data via link socket");
	
	d = cli_register_command(*cli, c, "counters", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Counters");
	cli_register_command(*cli, d, "reset", cli_v2x_link_reset_cntrs, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Reset Internal link counters");
  cli_register_command(*cli, d, "print", cli_v2x_link_print_cntrs, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "print Internal link counters");

  cli_register_command(*cli, c, "reset_cntrs", cli_v2x_link_reset_cntrs, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Reset Internal link counters");
  cli_register_command(*cli, c, "print_cntrs", cli_v2x_link_print_cntrs, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "print Internal link counters");
   
  /* handle gps command */
  c = cli_register_command(*cli, NULL, "nav", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Navigation API operations");
  cli_register_command(*cli, c, "init", cli_v2x_nav_init, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "init nav api");
  cli_register_command(*cli, c, "start", cli_v2x_nav_start, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "start nav api handler");
  cli_register_command(*cli, c, "stop", cli_v2x_nav_stop, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "start nav api handler");
  
	d = cli_register_command(*cli, NULL, "mng", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Management mibs");
  c = cli_register_command(*cli, d, "set", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set management mibs");
  cli_register_command(*cli, c, "wlanDefaultTxDataRate", cli_v2x_set_wlanDefaultTxDataRate, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set default transmission data rate.");
  cli_register_command(*cli, c, "wlanDefaultTxPower", cli_v2x_set_wlanDefaultTxDataRate, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set default transmission power.");
  cli_register_command(*cli, c, "wlanFrequency", cli_v2x_set_wlanFrequency, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set current RX and TX frequency");
  cli_register_command(*cli, c, "wlanMacAddress", cli_v2x_set_wlanMacAddress, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set WLAN mac address");

 	c = cli_register_command(*cli, d, "get", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get management mibs");
  cli_register_command(*cli, c, "wlanDefaultTxDataRate", cli_v2x_get_wlanDefaultTxDataRate, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get default transmission data rate.");
  cli_register_command(*cli, c, "wlanDefaultTxPower", cli_v2x_get_wlanDefaultTxPower, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get default transmission power.");
  cli_register_command(*cli, c, "wlanFrameRxCnt", cli_v2x_get_wlanFrameRxCnt, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get received frame counter");
  cli_register_command(*cli, c, "wlanFrameTxCnt", cli_v2x_get_wlanFrameTxCnt, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get transmitted frame counter");
  cli_register_command(*cli, c, "wlanFrequency", cli_v2x_get_wlanFrequency, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get current RX and TX frequency");
	cli_register_command(*cli, c, "wlanMacAddress", cli_v2x_get_wlanMacAddress, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get WLAN mac address");

  /* can */
	d = cli_register_command(*cli, NULL, "can", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "can bus implementation");
  c = cli_register_command(*cli, d, "service", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set service");
  cli_register_command(*cli, c, "create", cli_v2x_can_service_create, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Create can service");
  cli_register_command(*cli, c, "delete", cli_v2x_can_service_delete, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Delete can service");
	c = cli_register_command(*cli, d, "socket", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set can socket");
  cli_register_command(*cli, c, "create", cli_v2x_can_socket_create, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Create can socket");
  cli_register_command(*cli, c, "delete", cli_v2x_can_socket_delete, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Delete can socket");
  cli_register_command(*cli, c, "tx", cli_v2x_can_tx, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Send can frames over socket");
  cli_register_command(*cli, c, "rx", cli_v2x_can_rx, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Receive can frames over socket");
	c = cli_register_command(*cli, d, "counters", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Counters");
	cli_register_command(*cli, c, "reset", cli_v2x_can_reset_cntrs, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Reset Internal link counters");
  cli_register_command(*cli, c, "print", cli_v2x_can_print_cntrs, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "print Internal link counters");
 	
	/* ecc */
	d = cli_register_command(*cli, NULL, "ecc", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "ecc implementation");
  c = cli_register_command(*cli, d, "service", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set service");
  cli_register_command(*cli, c, "create", cli_v2x_ecc_service_create, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Create ecc service");
  cli_register_command(*cli, c, "delete", cli_v2x_ecc_service_delete, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Delete ecc service");
    /* Curves */
	c = cli_register_command(*cli, d, "curve", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set curve type");
	cli_register_command(*cli, c, "set", cli_v2x_ecc_curve_type_set, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set curve type");
	cli_register_command(*cli, c, "get", cli_v2x_ecc_curve_type_get, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get curve type");
    /* Socket */
  c = cli_register_command(*cli, d, "socket", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set can socket");
  cli_register_command(*cli, c, "create", cli_v2x_ecc_socket_create, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Create ecc socket");
  cli_register_command(*cli, c, "delete", cli_v2x_ecc_socket_delete, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Delete ecc socket");
  
	/* Sign */
  c = cli_register_command(*cli, d, "sign", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Sign actions");
  cli_register_command(*cli, c, "key", cli_v2x_ecc_sign_private_key, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set sign key");
  cli_register_command(*cli, c, "request", cli_v2x_ecc_sign_request_send, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Send sign request");
  cli_register_command(*cli, c, "get-response", cli_v2x_ecc_sign_response_receive, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Request response");
  
	/* Verification */
  c = cli_register_command(*cli, d, "verification", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Verifications actions");
  cli_register_command(*cli, c, "key", cli_v2x_ecc_verification_set_public_key, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Public key settings");
  cli_register_command(*cli, c, "request", cli_v2x_ecc_verification_request_send, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Send sign request");
  cli_register_command(*cli, c, "signature", cli_v2x_ecc_verification_set_signature, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Signature settings");

	c = cli_register_command(*cli, NULL, "uc", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "User context");
  cli_register_command(*cli, c, "set", set_user_context_addr, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set User context address.");
  cli_register_command(*cli, c, "get", get_user_context_addr, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Get user context address.");

	c = cli_register_command(*cli, NULL, "set", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "some utilities");
	cli_register_command(*cli, c, "context-name", cli_v2x_set_thread_name, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "");
	cli_register_command(*cli, c, "cpu-load", cli_v2x_cpu_load_start, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "");
	d = cli_register_command(*cli, c, "cca", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "cca control");
	cli_register_command(*cli, d, "status", cli_v2x_phy_cca_set, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Set phy CCA status");
	cli_register_command(*cli, d, "control", cli_v2x_phy_cca_control_start, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "CCA control ");
	
	c = cli_register_command(*cli, NULL, "thread", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Kill system resources");
	cli_register_command(*cli, c, "kill", cli_v2x_thread_kill, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "");
	cli_register_command(*cli, c, "suspend", cli_v2x_thread_suspend, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "");
	cli_register_command(*cli, c, "resume", cli_v2x_thread_resume, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "");
	
	c = cli_register_command(*cli, NULL, "prof", cmd_prof, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "profiler");
	cli_register_command(*cli, c, "reset", cmd_prof, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "profiler reset");
	cli_register_command(*cli, c, "display", cmd_prof, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "profiler display");
  
  cli_set_auth_callback(*cli, qa_check_auth);
  cli_set_enable_callback(*cli, qa_check_enable);
	/* Test reading from a file */

 #ifdef __LINUX__
  FILE                  *fh       = NULL;
  
  if ((fh = fopen("clitest.txt", "r"))) {
    cli_print_callback(*cli, qa_pc);
    cli_file(*cli, fh, PRIVILEGE_UNPRIVILEGED, MODE_EXEC);
    cli_print_callback(*cli, NULL);
    fclose(fh);
  }
#endif

  return 0;

}



#ifdef __THREADX__	
void craton_user_init(void)
{
  int 				rv;
	atlk_rc_t 	rc        = ATLK_OK;
	int					rf_freqs[] = { RF_IF_1_ASSIGN_FREQ , RF_IF_2_ASSIGN_FREQ };
	int					i = 0, rf_if = 0;
	
	/* Get default MIB service */
  rc = cli_v2x_managment_init();
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
  }
	
	for ( i=0, rf_if=1; i< 2; i++, rf_if++ ) {
		
		printf("\nV2X-CLI setting RF %d to Frequency %d with power %d\n", rf_if, rf_freqs[i], RF_DEFUALT_POWER);
		rc = mib_set_wlanDefaultTxPower(cli_v2x_managment_service_get(), rf_if, RF_DEFUALT_POWER ); 
		if (atlk_error(rc)) {
			printf( "ERROR :  mib_set_wlanDefaultTxPower  failed for if %d and value %d: %s\n", rf_if, RF_DEFUALT_POWER, atlk_rc_to_str(rc));
		}
		
		rc = mib_set_wlanFrequency (cli_v2x_managment_service_get(), rf_if, rf_freqs[i]); 
		if (atlk_error(rc)) {
			printf( "ERROR :  wlanFrequency  failed for if %d and value %d: %s\n", rf_if, rf_freqs[i], atlk_rc_to_str(rc));
		}
		
	}
	
	printf("\nV2X-CLI is disable vca log\n");
	rc = mib_set_vcaLogMode (cli_v2x_managment_service_get(), MIB_vcaLogMode_off );
	if (atlk_error(rc)) {
    printf( "ERROR :  mib_set_vcaLogMode failed, %s\n", atlk_rc_to_str(rc));
  }
  
	
	INIT_LIST_HEAD(&cli_thread_list);
	
  /* Create a thread to run main server */
  rv = tx_thread_create(&cli_main_thread, "qa_main_thread",
                        v2x_cli_main, (ULONG)NULL,
                        &cli_main_thread_stack,
                        sizeof(cli_main_thread_stack),
                        MAIN_THREAD_PRIO, MAIN_THREAD_PRIO,
                        MAIN_THREAD_TIME_SLICE, TX_AUTO_START);
  if ( rv != TX_SUCCESS ) {
    fprintf( stderr, "Failed to create v2x-cli main thread");
  }

}
#endif

void threads_garbage_collector(void) 
{
	cli_thread_info_t			*curr, *temp;

	list_for_each_entry_safe(curr, temp, &cli_thread_list, list) {

		if (curr->is_active) {
			continue;
		}
#ifdef __THREADX__	
	  ULONG trv;	
		trv = tx_thread_terminate(&curr->thread);
		if (trv != TX_SUCCESS) {
			fprintf(stdout, "V2X-CLI : tx_thread_terminate failed, rc = %lx\n", trv);
			return;
		}

		trv = tx_thread_delete(&curr->thread);
		if (trv != TX_SUCCESS) {
			fprintf(stdout, "V2X-CLI : tx_thread_delete failed, rc = %lx\n", trv);
			return;
		}
#endif

		list_del(&curr->list);
		free(curr);
		thread_cnt--;
	}
	
}


#ifdef __THREADX__
void v2x_cli_main(ULONG arg)
#elif defined (__LINUX__)
void v2x_cli_main( )
#endif
{
  int                     s, client_sock;
  struct sockaddr_in      server;
	socklen_t len = sizeof(server);
	cli_thread_info_t			*new_thread			= NULL;

#ifdef __THREADX__													
  ULONG                   trv = (int) arg;
#elif defined (__LINUX__)
	int on = 1;
#endif
  printf("V2X-CLI Loaded, Listening on port %d\n", CLITEST_PORT);
  

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf( stderr, "socket failed");
    return;
  }
#ifdef __LINUX__
	/* Init the list only in linux, in threadx its initilized in the */
	INIT_LIST_HEAD(&cli_thread_list);
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
#endif
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = 0;
  server.sin_port = htons(CLITEST_PORT);
  if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("bind");
    return;
  }

  if (listen(s, 1) < 0) {
    perror("listen");
    return;
  }

#ifdef __LINUX__
	/* just for use in threadx */
	int sck_size = sizeof(struct sockaddr_in);
	struct sockaddr_in      client;

  while( (client_sock = accept(s, (struct sockaddr *)&client, (socklen_t*)&sck_size)) ) {
#elif __THREADX__
  while( (client_sock = accept(s, NULL, 0) ) ) {
#endif
	
		if (client_sock < 0) {
			fprintf(stdout, " * Connection Failed\n");
			continue;
		}
		
		if ( thread_cnt > MAX_CLIENT_SUPPORTED ) {
			fprintf( stdout, " ERROR : there are to many client connected ( < %d) !\n", MAX_CLIENT_SUPPORTED );
			continue;
		}
	
		/** Create new thread information */
		new_thread = calloc(1, sizeof(*new_thread));
		if (new_thread == NULL) {
		 fprintf(stdout, "Error alloc data for new thread\n");
#ifdef __THREADX__
		 abort();
#elif defined (__LINUX__)
		 return;
#endif
	  }
		
		new_thread->socket = client_sock;
		new_thread->idx = thread_cnt;
		new_thread->is_active = 1;
		
#ifdef __LINUX__
		/*
		pthread_attr_init(&new_thread->attr);
    pthread_attr_setstacksize(&new_thread->attr, CLI_THREAD_STACKSIZE);
		*/
		int err ;

		err = pthread_create( &new_thread->thread , NULL, connection_handler , (void*) new_thread);
    if ( err ) {
      printf("could not create thread %d\n", err );
      return;
		}
#elif defined(__THREADX__)
    trv = tx_thread_create(&new_thread->thread, "v2x_cli_thread",
                          connection_handler, (ULONG) new_thread,
                          new_thread->stack,
                          CLI_THREAD_STACKSIZE,
                          CLI_THREAD_PRIO, CLI_THREAD_PRIO,
                          CLI_THREAD_TIME_SLICE, TX_AUTO_START);
    if (trv != TX_SUCCESS) {
      fprintf(stdout, "tx_thread_create failed, rc=%lx\n", trv);
      abort();
    }
#endif
		
		if (getpeername(client_sock, (struct sockaddr *) &server, &len) >= 0) {
			fprintf(stdout, " * accepted connection from %s, sock id : %d\n", inet_ntoa(server.sin_addr), client_sock);
		}
		
		/* Add It to linked list */
		list_add(&new_thread->list, &cli_thread_list);
		
		thread_cnt++;
		threads_garbage_collector();

  }
	
	fprintf( stdout, "V2X-CLI Unloaded !, This is unexpected error, please contact administartor");
	return;
}

#ifdef __LINUX__
int main()
{
  v2x_cli_main();
	return 0;
}
#endif



/* This will handle connection for each client */
#ifdef __THREADX__
void connection_handler( ULONG thread_desc )
#elif defined (__LINUX__)
void *connection_handler( void *thread_desc )
#endif
{
	//Get the socket descriptor
  cli_thread_info_t *thread_info = (cli_thread_info_t *) thread_desc;
  struct cli_def *cli = NULL;
  user_context user_ctx;

  /* Create cli structure for each user */
  create_cli_struct( &cli );
	
  /* Initilize the user context */
  user_ctx.v2x_socket = NULL;
	memset( &user_ctx.cntrs , 0 , sizeof(cli_cntrs) );
  user_ctx.nav_info.nav_hwd = NULL;
  user_ctx.nav_info.loop_flag = 0;
  memset(&user_ctx.nav_info.nav_thread_stack, 0, sizeof(user_ctx.nav_info.nav_thread_stack) );

  cli_set_context(cli, (void*)&user_ctx);
  cli_loop(cli, thread_info->socket);
  
	cli_cleanup( cli );
#ifdef __THREADX__
  soc_close( thread_info->socket );
#endif
  
  cli_done( cli );
	/* Mark thread as ended, the main cli thread will clean on next client */
	thread_info->is_active = 0;
	
	fprintf( stdout, " ** Connection closed, socket handler id : %d released \n", thread_info->socket );
	
#ifdef __LINUX__
  pthread_exit(0);
  return NULL;
#endif
}

void cli_cleanup( struct cli_def *cli )
{
	/* get user context */
	user_context *user_ctx = (user_context *) cli_get_context(cli);
	
	/* Delete user socket for this session */
	if ( user_ctx->v2x_socket != NULL ) {
		v2x_socket_delete(user_ctx->v2x_socket);
		user_ctx->v2x_socket = NULL;
	}
	
	if ( user_ctx->nav_info.nav_hwd != NULL ) {
	
		nav_fix_subscriber_delete (user_ctx->nav_info.nav_hwd);
		user_ctx->nav_info.nav_hwd = NULL;
	}
	
}
  


int get_user_context_addr( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  /* get user context */
  user_context *myctx = (user_context *) cli_get_context(cli);
  cli_print( cli, "Context : %x", (unsigned int) myctx);
	
  return CLI_OK;
}

int set_user_context_addr( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  unsigned int    addr    = 0;
  int							i       = 0;
 
  IS_HELP_ARG("uc set -addr 0xNNNNN");

  CHECK_NUM_ARGS /* make sure all parameter are there */
    
  for ( i = 0 ; i < argc; i += 2 ) {
	   GET_TYPE_INT("-addr", addr, unsigned int, i, "Address of current context ", "%x"); 
  } 
  
  if ( addr == 0 ) {
    cli_print( cli, "ERROR : address is incorrect %x\n", (int) addr );
    return CLI_ERROR;
  }
  cli_set_context( cli, (user_context *) addr  );
  return CLI_OK;
	
}



