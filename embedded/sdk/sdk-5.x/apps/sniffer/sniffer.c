#ifdef __THREADX__
#include "../../common/v2x_cli/v2x_cli.h"

#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <atlk/sdk.h>
#include <nxd_bsd.h>

#include <tx_api.h>
#include <nx_api.h>

#include <craton/net.h>

#include <craton/wlan_driver.h>
#include "sniffer.h"
//#include "../common/counters.h"

#define TX 0 
#define RX 1
int				hwd[2]=	{0,0};

struct sockaddr_in 		destAddr[2];

int				sniffer_id = -1;




#define MAX_MSG_SIZE 1518

//static tx_rx_counters_t	counters[2] = {0};

#define MAX_MSG_SIZE 1518
#define ALL_RF_IF 2
#define packed __attribute__((packed))


typedef struct packed {

	/* PCAP frame header */
	uint32_t 	ts_sec;
	uint32_t 	ts_usec;
	uint32_t 	incl_len;
	uint32_t 	orig_len;

} pcacp_header_st;

enum ieee80211_radiotap_type {
  IEEE80211_RADIOTAP_TSFT = 0,
  IEEE80211_RADIOTAP_FLAGS = 1,
  IEEE80211_RADIOTAP_RATE = 2,
  IEEE80211_RADIOTAP_CHANNEL = 3,
  IEEE80211_RADIOTAP_FHSS = 4,
  IEEE80211_RADIOTAP_DBM_ANTSIGNAL = 5,
  IEEE80211_RADIOTAP_DBM_ANTNOISE = 6,
  IEEE80211_RADIOTAP_LOCK_QUALITY = 7,
  IEEE80211_RADIOTAP_TX_ATTENUATION = 8,
  IEEE80211_RADIOTAP_DB_TX_ATTENUATION = 9,
  IEEE80211_RADIOTAP_DBM_TX_POWER = 10,
  IEEE80211_RADIOTAP_ANTENNA = 11,
  IEEE80211_RADIOTAP_DB_ANTSIGNAL = 12,
  IEEE80211_RADIOTAP_DB_ANTNOISE = 13,
  IEEE80211_RADIOTAP_FCS = 14,
  IEEE80211_RADIOTAP_XCHANNEL = 18,
  IEEE80211_RADIOTAP_EXT = 31
};


// enum ieee80211_radiotap_type {
  // IEEE80211_RADIOTAP_TSFT = 0,
  // IEEE80211_RADIOTAP_FLAGS = 1,
  // IEEE80211_RADIOTAP_RATE = 2,
  // IEEE80211_RADIOTAP_CHANNEL = 3,
  // IEEE80211_RADIOTAP_FHSS = 4,
  // IEEE80211_RADIOTAP_DBM_ANTSIGNAL = 5,
  // IEEE80211_RADIOTAP_DBM_ANTNOISE = 6,
  // IEEE80211_RADIOTAP_LOCK_QUALITY = 7,
  // IEEE80211_RADIOTAP_TX_ATTENUATION = 8,
  // IEEE80211_RADIOTAP_DB_TX_ATTENUATION = 9,
  // IEEE80211_RADIOTAP_DBM_TX_POWER = 10,
  // IEEE80211_RADIOTAP_ANTENNA = 11,
  // IEEE80211_RADIOTAP_DB_ANTSIGNAL = 12,
  // IEEE80211_RADIOTAP_DB_ANTNOISE = 13,
  // IEEE80211_RADIOTAP_RX_FLAGS = 14,
  // IEEE80211_RADIOTAP_TX_FLAGS = 15,
  // IEEE80211_RADIOTAP_RTS_RETRIES = 16,
  // IEEE80211_RADIOTAP_DATA_RETRIES = 17,
  // IEEE80211_RADIOTAP_EXT = 31

// };


typedef struct packed {
	
	uint8_t       it_version;     /* set to 0 */
	uint8_t       it_pad;
	uint16_t      it_len;         /* entire length */
	uint32_t      it_present;     /* fields present */

	uint64_t 			mac_timestamp;
	uint8_t 			data_rate;
	uint8_t 			tx_power;
	uint8_t 			antenna;
	// uint8_t             ch_num;
	// uint8_t             op_class;
} radiotap_header_st;


/* Registered rx traffic monitor handler */
static void rx_traffic_monitor_handler(const wlan_frame_t *frame, const wlan_rx_frame_info_t *info)
{
	char					msg[MAX_MSG_SIZE], *pos = NULL;
		radiotap_header_st		radio_tap_hdr;
		int										rc = 0;
		if ( (frame->frame_body_size + frame->frame_header_size + sizeof(radio_tap_hdr) ) > (MAX_MSG_SIZE - 124) ) {
			printf ( "NOTE : Oversize frame, handle it later since its not shuld be common\n");
			return;
		}

		/* Build radio tamp header frame */
		radio_tap_hdr.it_version = 0;
		radio_tap_hdr.it_pad = 0;
		radio_tap_hdr.it_len = sizeof(radiotap_header_st);
	radio_tap_hdr.it_present = ( (1<<IEEE80211_RADIOTAP_TSFT) | (1<<IEEE80211_RADIOTAP_RATE)  | (1<<IEEE80211_RADIOTAP_DBM_TX_POWER) | (1<<IEEE80211_RADIOTAP_ANTENNA) );
		radio_tap_hdr.mac_timestamp = info->rx_isr_time_us;
		radio_tap_hdr.data_rate = info->datarate;
		radio_tap_hdr.tx_power = info->power_dbm8;
		radio_tap_hdr.antenna = info->device_id  & 0x0f;
    // radio_tap_hdr.ch_num = info->v2x_channel_id.channel_num;
    // radio_tap_hdr.op_class = info->v2x_channel_id.op_class;
	if (sniffer_id != -1)
		radio_tap_hdr.antenna |= ((sniffer_id<<4) & 0xf0);

	pos = msg;


	memcpy( pos , &radio_tap_hdr, sizeof(radio_tap_hdr) ); pos += sizeof(radio_tap_hdr);
	memcpy( pos, frame->frame_header_ptr, frame->frame_header_size ); pos += frame->frame_header_size;
	memcpy( pos, frame->frame_body_ptr, frame->frame_body_size ); pos += frame->frame_body_size;
	if (hwd[RX] != 0)
		rc = sendto(hwd[RX], msg, (pos - msg), 0, (struct sockaddr *)&destAddr[RX], sizeof(destAddr[RX]));
	
	if ( rc < 1 ) {
		printf("ERROR : failed to send frame rc %d, size %d, hwd[RX] %d\n", rc, (int) (pos - msg),  (int)hwd[RX]);
	}
	//counters[info->device_id].rx.pass++;
	
}


static void tx_traffic_monitor_handler(const wlan_frame_t *frame, const wlan_tx_frame_info_t *info)
{
	char					msg[MAX_MSG_SIZE], *pos = NULL;
		radiotap_header_st		radio_tap_hdr;
		int										rc = 0;
		if ( (frame->frame_body_size + frame->frame_header_size + sizeof(radio_tap_hdr) ) > (MAX_MSG_SIZE - 124) ) {
			printf ( "NOTE : Oversize frame, handle it later since its not shuld be common\n");
			return;
		}

		/* Build radio tamp header frame */
		radio_tap_hdr.it_version = 0;
		radio_tap_hdr.it_pad = 0;
		radio_tap_hdr.it_len = sizeof(radiotap_header_st);
		radio_tap_hdr.it_present = ( (1<<IEEE80211_RADIOTAP_TSFT) | (1<<IEEE80211_RADIOTAP_RATE)  | (1<<IEEE80211_RADIOTAP_DBM_TX_POWER) | (1<<IEEE80211_RADIOTAP_ANTENNA) );
		radio_tap_hdr.mac_timestamp = info->tx_isr_time_us;
		radio_tap_hdr.data_rate = info->datarate;
		radio_tap_hdr.tx_power = info->power_dbm8;
		radio_tap_hdr.antenna = info->device_id  & 0x0f;
	    if (sniffer_id != -1)
		  radio_tap_hdr.antenna |= ((sniffer_id<<4) & 0xf0);
		pos = msg;


		memcpy( pos , &radio_tap_hdr, sizeof(radio_tap_hdr) ); pos += sizeof(radio_tap_hdr);
		memcpy( pos, frame->frame_header_ptr, frame->frame_header_size ); pos += frame->frame_header_size;
		memcpy( pos, frame->frame_body_ptr, frame->frame_body_size ); pos += frame->frame_body_size;

        if (hwd[TX] != 0)
		  rc = sendto(hwd[TX], msg, (pos - msg), 0, (struct sockaddr *)&destAddr[TX], sizeof(destAddr[TX]));
	    if ( rc < 1 ) {
		  printf("ERROR : failed to send frame rc %d, size %d, hwd[TX] %d\n", rc, (int) (pos - msg), (int)hwd[TX] );
	    }
		//counters[info->device_id].rx.pass++;
	
}



//int cli_qa_sniffer_print_counters( struct cli_def *cli, const char *command, char *argv[], int argc ) 
//{
//	int			if_idx  = 0;
//	
//	(void) command;
//
  //IS_HELP_ARG("sniffer start -if_idx 1|2")
//	CHECK_NUM_ARGS /* make sure all parameter are there */
//	 
//	GET_INT("-if_idx", if_idx, 0, "Specify interface index");
//	if ( if_idx < 1 || if_idx > 3) {
//	 cli_print(cli, "ERROR : if_index is not optional and must be in range of 1,2 or 3 to set both rf_if immidiatly");
//	 return CLI_ERROR;
//	}
//
//	cli_print( cli, "Interface : %d", if_idx);
//	cli_print( cli, "RX : %d", counters[if_idx].rx.pass);
//
//	return CLI_OK;
//
//}


//int cli_qa_sniffer_reset_counters( struct cli_def *cli, const char *command, char *argv[], int argc ) 
//{
//	int			if_idx  = 0;
//	
//	(void) command;
//
  //IS_HELP_ARG("sniffer start -if_idx 1|2")
//	CHECK_NUM_ARGS /* make sure all parameter are there */
//	 
//	GET_INT("-if_idx", if_idx, 0, "Specify interface index");
//	if ( if_idx < 1 || if_idx > 3) {
//	 cli_print(cli, "ERROR : if_index is not optional and must be in range of 1,2 or 3 to set both rf_if immidiatly");
//	 return CLI_ERROR;
//	}
//	
//	memset( &counters[if_idx], 0 , sizeof(tx_rx_counters_t) );
//
//	return CLI_OK;
//
//}

int cli_qa_sniffer_start( struct cli_def *cli, const char *command, char *argv[], int argc ) 
{
  int32_t               i     				= 0;
  atlk_rc_t             rc    				= ATLK_OK;
	int										rv						= 0;
	char									server_ip[20] = {0};
	int										server_port 	= 8030;
	int										udp_port			= 0;
	struct sockaddr_in		_sockaddr_in;
	int										if_idx				= -1;
	char                                    sniffer_type[3] = {0};

  /* get user context */
  // user_context *myctx = (user_context *) cli_get_context(cli);
  (void) command;
  
  IS_HELP_ARG("sniffer start -if_idx 1|2|3 (3 - both 1 and 2 simultanously) -server_ip 10.10.1.131 [-server_port 8030] -sniffer_type TX|RX (in any case all rf_idx frames will be redirect to the first configured port...)")

  CHECK_NUM_ARGS /* make sure all parameter are there */
  
	GET_INT("-if_idx", if_idx, i, "Specify interface index");
	if ( if_idx < 1 || if_idx > 3) {
	  cli_print(cli, "ERROR : if_index is not optional and must be in range of 1,2 or 3 to set both rf_if immidiatly");
	  return CLI_ERROR;
	}

	for (i = 0; i < argc; i += 2) {
		GET_TYPE_INT("-server_port", server_port, unsigned int, i, "Local udp port number", "%u");
    GET_STRING("-server_ip", server_ip, i, "Set destination mac address");
    GET_STRING("-sniffer_type", sniffer_type, i, "set which CB to register TX or RX monitor");
	}
	
	cli_print(cli, "NOTE : no socket handle found - creating new socket...");

	/* Create socket */
	int rxtx = (strstr(sniffer_type, "RX") != NULL) ? RX : TX;

	if (hwd[rxtx] == 0){
	  hwd[rxtx] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	  if (hwd[rxtx] == -1) {
		  cli_print(cli, "ERROR : failed to create udp socket");
		  return CLI_ERROR;
	  }

	  /* Specify the address family */
	  destAddr[rxtx].sin_family = AF_INET;
	  /* Specify the destination port */
	  destAddr[rxtx].sin_port = htons(server_port);
	  /* Specify the destination IP address */
	  destAddr[rxtx].sin_addr.s_addr = inet_addr(server_ip);
	  	/* Bind socket */
	  memset(&_sockaddr_in, 0, sizeof(_sockaddr_in));
	  _sockaddr_in.sin_family = AF_INET;
	  _sockaddr_in.sin_port = htons(udp_port);
	  _sockaddr_in.sin_addr.s_addr = INADDR_ANY;
	  rv = bind(hwd[rxtx], (struct sockaddr *)&_sockaddr_in, sizeof(_sockaddr_in));
	  if (rv == -1) {
		  cli_print(cli, "ERROR : failed to bind udp socket");
		  return CLI_ERROR;
	  }
	}
	


	if (if_idx == 3){
		
		for ( int i = 0; i < 1; i++ ) {
			
			//memset( &counters[i], 0 , sizeof(tx_rx_counters_t) );
			if ((strstr(sniffer_type, "RX") != NULL) || (sniffer_type[0] == '\0')) {
				cli_print(cli, "wlan_traffic_monitor_set to RX");
				rc = wlan_rx_traffic_monitor_set( i, rx_traffic_monitor_handler);
			}
			if ((strstr(sniffer_type, "TX") != NULL) || (sniffer_type[0] == '\0')) {
				cli_print(cli, "wlan_traffic_monitor_set to TX");
				rc = wlan_tx_traffic_monitor_set( i, tx_traffic_monitor_handler);
			}
			if (atlk_error(rc)) {
				cli_print(cli, "wlan_traffic_monitor_set: %d, %s\n", i, atlk_rc_to_str(rc) );
				goto error;
			}
		}

	} 
	else {
		//memset( &counters[i], 0 , sizeof(tx_rx_counters_t) );
		if ((strstr(sniffer_type, "RX") != NULL) || (sniffer_type[0] == '\0' )) {
			cli_print(cli, "wlan_traffic_monitor_set to RX");
			rc = wlan_rx_traffic_monitor_set( (if_idx - 1), rx_traffic_monitor_handler);
		}
		if ((strstr(sniffer_type, "TX") != NULL) || (sniffer_type[0] == '\0')) {
			cli_print(cli, "wlan_traffic_monitor_set to TX");
			rc = wlan_tx_traffic_monitor_set( (if_idx - 1), tx_traffic_monitor_handler);
		}
		if (atlk_error(rc)) {
			cli_print(cli, "wlan_traffic_monitor_set: %d, %s", (int)(if_idx - 1), atlk_rc_to_str(rc) );
			goto error;
		}
	}

	cli_print(cli, "NOTE : Sniffer on i %d started, server %s:%d, sniffer id %d", if_idx, server_ip, server_port, sniffer_id);
	return CLI_OK;
	
error:
	
  return atlk_error(rc);
}


int cli_qa_sniffer_stop( struct cli_def *cli, const char *command, char *argv[], int argc ) 
{
	  atlk_rc_t             rc    				= ATLK_OK;
		int										if_idx				= 0;

		(void) command;
		(void) argv;
		(void) argc;

	  IS_HELP_ARG("sniffer stop -if_idx 1|2")

		GET_INT("-if_idx", if_idx, 0, "Specify interface index");
		if ( if_idx < 1 || if_idx > 3) {
			cli_print(cli, "ERROR : if_index is not optional and must be in range of 1,2 or 3 to set both rf_if immidiatly");
			return CLI_ERROR;
		}


		/* Register a callback for WLAN device */
		if (if_idx == 3){

			for ( int i=0; i <= 1; i++ ) {
				rc = wlan_rx_traffic_monitor_set( i, NULL);
				if (atlk_error(rc)) {
					cli_print(cli, "wlan_rx_traffic_monitor_set: %d, %s\n", i, atlk_rc_to_str(rc) );
					goto error;
				}
			}

		}
		else {

			rc = wlan_rx_traffic_monitor_set( (if_idx-1), NULL);
			if (atlk_error(rc)) {
			 cli_print(cli, "wlan_traffic_monitor_set: %s\n", atlk_rc_to_str(rc) );
			 goto error;
			}
		}
		
		soc_close(hwd[RX]);
		soc_close(hwd[TX]);
		hwd[0] = 0;
		hwd[1] = 0;
		return CLI_OK;

	 error:
		 return atlk_error(rc);
}

int cli_qa_sniffer_settings(struct cli_def *cli, const char *command, char *argv[], int argc)
{
	int						id = 0;
	int32_t					i = 0;
	atlk_rc_t				rc = ATLK_OK;

	(void)command;
	
	IS_HELP_ARG("sniffer setting -id 1..4")

	CHECK_NUM_ARGS /* make sure all parameter are there */

	GET_INT("-id", id, i, "Specify sniffer index");
	if (id < 1 || id > 4) {
		cli_print(cli, "ERROR : id must be in range of 1-4");
		return CLI_ERROR;
	}
	sniffer_id = id;
	return rc;

}

 
#endif
