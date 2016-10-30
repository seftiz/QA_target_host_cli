#include <stdio.h>
#include <sys/types.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include "list.h"
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libcli/libcli.h"
#include "../session/session.h"
#include "../wsmp/wsmp.h"
#include "../gps_simulator/gps.h"
#include "../nav_api/nav_api.h"
#include "v2x_cli.h"

struct list_head cli_thread_list;
int               thread_cnt;


void *connection_handler(void *socket_desc);

int cmd_config_int_exit(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc))
{
    cli_set_configmode(cli, MODE_CONFIG, NULL);
    return CLI_OK;
}


int check_auth(const char *username, const char *password)
{
    if (strcasecmp(username, USERNAME) != 0)
        return CLI_ERROR;
    if (strcasecmp(password, PASSWORD) != 0)
        return CLI_ERROR;
    return CLI_OK;
}

int check_enable(const char *password)
{
    return !strcasecmp(password, PASSWORD);
}

int idle_timeout(struct cli_def *cli)
{
    cli_print(cli, "Custom idle timeout");
    return CLI_QUIT;
}

void pc(UNUSED(struct cli_def *cli), const char *string)
{
    printf("%s\n", string);
}

int show_version( struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc) ) 
{
  cli_print( cli, "Version : 0.0.0" );
  return CLI_OK;
}


int create_cli_struct( struct cli_def **cli )
{
  struct cli_command    *c        = NULL;
  FILE                  *fh       = NULL;

  *cli = cli_init();

  cli_set_hostname(*cli, "v2x >");
  cli_set_banner(*cli, "Autotalks - Confidence of Knowing Ahead.\nWelcome to Auto-talks V2X cli\n\n");
   
  cli_telnet_protocol(*cli, 1);

  // cli_regular(*cli, regular_callback);
  cli_regular_interval(*cli, 5); // Defaults to 1 second
  cli_set_idle_timeout_callback(*cli, 0, idle_timeout); /* 5 Minutes idle timeout */


  c = cli_register_command(*cli, NULL, "show", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
  cli_register_command(*cli, c, "version", show_version, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show version");

  c = cli_register_command(*cli, NULL, "session", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
  cli_register_command(*cli, c, "open", cli_v2x_session_open, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "open session");
  cli_register_command(*cli, c, "close", cli_v2x_session_close, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "close session");
  cli_register_command(*cli, c, "get", cli_v2x_get_session, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "get session");
  cli_register_command(*cli, c, "set", cli_v2x_set_session, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "set session");

  /* handle wsmp command */
  c = cli_register_command(*cli, NULL, "wsmp", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "send message frame via SDK");
  cli_register_command(*cli, c, "open", cli_v2x_wsmp_sk_open, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "open wsmp new socket");
  cli_register_command(*cli, c, "close", cli_v2x_wsmp_sk_close, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "close wsmp socket");
  cli_register_command(*cli, c, "tx", cli_v2x_wsmp_tx, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Transmit Data via wsmp socket");
  cli_register_command(*cli, c, "rx", cli_v2x_wsmp_rx, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Transmit Data via wsmp socket");

  /* handle gps command */
  c = cli_register_command(*cli, NULL, "nav", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Navigation API operations");
  cli_register_command(*cli, c, "init", cli_v2x_nav_init, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "init nav api");
  cli_register_command(*cli, c, "start", cli_v2x_nav_start, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "start nav api handler");
  cli_register_command(*cli, c, "stop", cli_v2x_nav_stop, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "start nav api handler");
 
  /*
  int_gps = cli_register_command(*cli, c, "internal-gps", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Handle inte");
  cli_register_command(*cli, int_gps, "start", cli_v2x_gps_start, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "start gps simulator");
  cli_register_command(*cli, int_gps, "stop", cli_v2x_gps_stop, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "stop gps simulator");
  cli_register_command(*cli, int_gps, "inject", cli_v2x_gps_inject, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "set gps data");
  cli_register_command(*cli, int_gps, "config", cli_v2x_gps_config, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "set gps configuration parameters");
  */

  cli_set_auth_callback(*cli, check_auth);
  cli_set_enable_callback(*cli, check_enable);

  /* Test reading from a file */
  if ((fh = fopen("clitest.txt", "r"))) {
    /* This sets a callback which just displays the cli_print() text to stdout */
    cli_print_callback(*cli, pc);
    cli_file(*cli, fh, PRIVILEGE_UNPRIVILEGED, MODE_EXEC);
    cli_print_callback(*cli, NULL);
    fclose(fh);
  }
  
  return 0;
}

void threads_garbage_collector(void) 
{
	cli_thread_info_t			*curr, *temp;

	fprintf(stdout, "!!!starting threads_garbage_collector\n" );

	list_for_each_entry_safe(curr, temp, &cli_thread_list, list) {

		if (curr->is_active) {
			continue;
		}

		list_del(&curr->list);
		free(curr);
		thread_cnt--;
	}
}


int main( )
{
            
  int                   s, client_sock , sck_size, 
                        on = 1;
  struct sockaddr_in    server, client;
  int                   thread_cnt      = 0;
 	socklen_t             len             = sizeof(server);
  cli_thread_info_t			*new_thread			= NULL;


  signal(SIGCHLD, SIG_IGN);
  
  INIT_LIST_HEAD(&cli_thread_list);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return 1;
  }
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(CLITEST_PORT);
  if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("bind");
    return 1;
  }

  if (listen(s, 50) < 0) {
    perror("listen");
    return 1;
  }

  printf("Listening on port %d\n", CLITEST_PORT);
  sck_size = sizeof(struct sockaddr_in);
  
  while( (client_sock = accept(s, (struct sockaddr *)&client, (socklen_t*)&sck_size)) ) {

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
		 return (-1);
	  }
    
 		
		new_thread->socket = client_sock;
		new_thread->idx = thread_cnt;
		new_thread->is_active = 1;
    
    pthread_attr_init(&new_thread->attr);
    pthread_attr_setstacksize(&new_thread->attr, CLI_THREAD_STACKSIZE);

    if( pthread_create( &new_thread->thread , &new_thread->attr, connection_handler , (void*) new_thread) < 0) {
      perror("could not create thread");
      return (-1);
    }
    
    if (getpeername(client_sock, (struct sockaddr *) &server, &len) >= 0) {
			fprintf(stdout, " * accepted connection from %s, sock id : %d\n", inet_ntoa(server.sin_addr), client_sock);
		}
		
		/* Add It to linked list */
		list_add(&new_thread->list, &cli_thread_list);
		
		thread_cnt++;
		
		threads_garbage_collector();
  }

 
	return 0;
}


void cli_cleanup( struct cli_def *cli )
{
	/* get user context */
	user_context *user_ctx = (user_context *) cli_get_context(cli);
	v2x_se_close( &user_ctx->v2x_se );
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *thread_desc)
{
	//Get the socket descriptor
  cli_thread_info_t *thread_info = (cli_thread_info_t *) thread_desc;
  struct cli_def *cli = NULL;
  
  user_context user_ctx;
  

  /* Create cli structure for each user */
  create_cli_struct( &cli );
  /* create user context */
  
  /* Initilize the user context */
  user_ctx.v2x_se = NULL;
	memset( &user_ctx.cntrs , 0 , sizeof(cli_cntrs) );

  cli_set_context(cli, (void*)&user_ctx);
  cli_loop(cli, thread_info->socket);
  
	cli_cleanup( cli );
  
	
  /* soc_close( thread_info->socket ); */
  
  cli_done( cli );
	/* Mark thread as ended, the main cli thread will clean on next client */
	thread_info->is_active = 0;

	fprintf( stdout, " ** Connection closed, socket handler id : %d released \n", thread_info->socket );

  pthread_exit(0);

  return (void*) NULL;
  
}