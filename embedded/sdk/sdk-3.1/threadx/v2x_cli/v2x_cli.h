
#ifndef __V2X_CLI_H__
#define __V2X_CLI_H__

#include <libcli.h>
#include <atlk/sdk.h>
#include <atlk/v2x/wave.h>
#include <atlk/nav.h>
#include <atlk/v2x/link.h>
#include <tx_api.h>
/* Define user context variables for the cli */

#include "list.h"

typedef struct  {
    TX_THREAD           nav_thread;
    uint8_t             nav_thread_stack[1024];
    int8_t              loop_flag;
    nav_se_t            *nav_se;
} nav_op_data_t;

typedef struct {
	unsigned int		link_rx_packets;
	unsigned int		link_tx_packets;
	
} cli_cntrs;


typedef struct {
  v2x_se_t        *v2x_se;
  v2x_wsmp_sk_t   *wsmp_sk;
  nav_op_data_t   nav_data;
  v2x_link_sk_t   *ll_se;
  unsigned int    test;
  cli_cntrs		  cntrs;

} user_context;



#define CLITEST_PORT                8000
#define MODE_CONFIG_INT             10

#define USERNAME					"root"
#define PASSWORD					"root"


#define MAX_CLIENT_SUPPORTED 20

/** Set cli thread priority and time slice */
#define CLI_THREAD_PRIO 21
#define CLI_THREAD_TIME_SLICE 50

/** Defualt startup value for QA units */
#define RF_IF_1_ASSIGN_FREQ 		5890
#define RF_IF_2_ASSIGN_FREQ 		5920
#define RF_DEFUALT_POWER				-5


#define NX_THREAD_PRIO 1
#define MAIN_THREAD_PRIO 2
#define MAIN_THREAD_TIME_SLICE 50
#define MAIN_THREAD_STACK_SIZE (1 << 15)
#define MAIN_BUF_SIZE 500


#define CLI_THREAD_STACKSIZE 1024*16

typedef struct cli_thread_info_struct {
	struct list_head list;
	TX_THREAD    				thread;
	uint8_t      				stack[CLI_THREAD_STACKSIZE];
	int							socket;
	int							idx;
	char						is_active;
} cli_thread_info_t;


#define MAX_TX_MSG_LEN		1024







#ifdef __GNUC__
  #define UNUSED(d) d __attribute__ ((unused))
#else
  #define UNUSED(d) d
#endif

#define __USE_API_DEFAULTS__



#define GET_VALUE(_parameter_,_var_,_idx_,_user_msg_,_fmt_) {  \
    if ( (argv[_idx_] != NULL) && (strcmp(argv[_idx_], _parameter_) == 0) ) {           \
      unsigned int value = 0;                           \
      int rc = 0;                               \
      int par_idx = (_idx_+1);                          \
      if (!argv[par_idx] && !&argv[par_idx]) {          \
          cli_print(cli, _user_msg_);                   \
          return CLI_OK;                                \
      }                                                 \
      rc = sscanf(argv[par_idx], _fmt_, &value);              \
      if ( rc > 0 ) { \
        _var_ = value;                                   \
        /*cli_print( cli, "DEBUG : Processed parameter %s, value %d" , _parameter_ , value );*/\
      } \
      else { \
        cli_print( cli, "ERROR : Processed parameter %s, value #_fmt_ failed" , _parameter_ , value );\
      }\
    }                                                   \
}

#define GET_INT(_parameter_,_var_,_idx_,_user_msg_) GET_TYPE_INT(_parameter_,_var_,_idx_,_user_msg_,"%d")


#define GET_TYPE_INT(_parameter_,_var_,_idx_,_user_msg_,_fmt_str_) {  \
    if ( (argv[_idx_] != NULL) && (strcmp(argv[_idx_], _parameter_) == 0) ) {           \
      int value = 0;                           \
      int par_idx = (_idx_+1);                          \
      if (!argv[par_idx] && !&argv[par_idx]) {          \
          cli_print(cli, _user_msg_);                   \
          return CLI_OK;                                \
      }                                                 \
      sscanf(argv[par_idx], _fmt_str_, &value);              \
      _var_ = value;                                   \
	  cli_print( cli, "DEBUG : Processed parameter %s, value %d" , _parameter_ , value );\
    }                                                   \
}


#define GET_STRING(_parameter_,_var_,_idx_,_user_msg_) {  \
    memset( _var_, 0 , sizeof(_var_) );\
    if ( (argv[_idx_] != NULL) && (strcmp(argv[_idx_], _parameter_) == 0) ) {           \
      int par_idx = (_idx_+1);                          \
      if (!argv[par_idx] && !&argv[par_idx]) {          \
          cli_print(cli, _user_msg_);                   \
          return CLI_OK;                                \
      }     											\
	  sscanf(argv[par_idx], "%s", (char*) &_var_);    	\
	  /*cli_print( cli, "DEBUG : Processed parameter %s, value %s" , _parameter_ , _var_ );*/\
    }                                                   \
}

#define GET_STRING_VALUE(_parameter_,_var_,_user_msg_) {  \
    int j = 0; \
    for ( j = 0; j < argc; j++) { \
      if ( (argv[j] != NULL) && (strcmp(argv[j], _parameter_) == 0) ) {           \
        int par_idx = (j+1);                          \
        if (!argv[par_idx] && !&argv[par_idx]) {          \
            cli_print(cli, _user_msg_);                   \
            return CLI_OK;                                \
        }     											\
        memset( _var_, 0 , sizeof(_var_) );\
        sscanf(argv[par_idx], "%s", (char*) &_var_);    	\
        /*cli_print( cli, "DEBUG : Processed parameter %s, value %s" , _parameter_ , _var_ );*/\
      }                                                   \
   } \
}

#define GET_NUMERIC_VALUE(_parameter_,_var_,_user_msg_) {  \
    int j = 0; \
    for ( j = 0; j < argc; j++) { \
      int value = 0;                           \
      if ( (argv[j] != NULL) && (strcmp(argv[j], _parameter_) == 0) ) {           \
        int par_idx = (j+1);                          \
        if (!argv[par_idx] && !&argv[par_idx]) {          \
            cli_print(cli, _user_msg_);                   \
            return CLI_OK;                                \
        }     											\
        sscanf(argv[par_idx], "%d", &value);              \
        _var_ = value;                                   \
        /*cli_print( cli, "DEBUG : Processed parameter %s, value %d" , _parameter_ , value );*/\
      }                                                   \
   } \
}


#define CHECK_NUM_ARGS \
	if ( (argc % 2) != 0 ) { \
    cli_print(cli, "Error : number of argument is mismatch" ); \
    return CLI_ERROR_ARG; \
	}

#define IS_HELP_ARG(_err_usage_)\
  if ( (argv[0] != NULL) && (strcmp(argv[0], "?") == 0) ) {\
    cli_print(cli, "usage : %s" , _err_usage_ );\
    return CLI_OK;\
  }
  
  
#define IF_ID_MIN 1
#define IF_ID_MAX 2

#define ERR_VALUE (-1)

#endif /* __V2X_CLI_H__ */


