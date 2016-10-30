
#include "../../common/v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <nxd_bsd.h>

#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <tx_api.h>

#include <nx_api.h>
#include <craton/net.h>



#define MAX_MSG_SIZE 1518

#define TM_DEST_ADDR "10.10.1.131"
#define TM_DEST_PORT 8020

unsigned int 	m_udp_rx_packets = 0,
							m_udp_tx_packets = 0,
							m_raw_tx_packets = 0,
							m_raw_rx_packets = 0;


void hexdump_packet(struct cli_def *cli, const NX_PACKET *packet);



int loopback_bsd_ip_udp( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc )
{
	int 		rc			= ATLK_OK;
	int32_t      	udp_port = 0;
	struct				sockaddr_in	_sockaddr_in, destAddr;
	char					server_ip[20] = {0};
	int						server_port = -1;
	int						sd = -1, i = 0, loop = 1, exit_loop = 0, print_packets = 0;
	// const int			on = 1;
	char					msg[MAX_MSG_SIZE];


	/* get user context */
	user_context *myctx = (user_context *)cli_get_context(cli);
	(void)myctx; /* not used  */

	IS_HELP_ARG("loopback ip -port 8020 -server_ip 10.10.1.131 [-server_port 8020 = port as default] [-print 0(default) | 1 ]")
	CHECK_NUM_ARGS
	/* Get port number*/
	for (i = 0; i < argc; i += 2) {
		GET_TYPE_INT("-port", udp_port, unsigned int, i, "Local udp port number", "%u");
		GET_TYPE_INT("-server_port", server_port, unsigned int, i, "Local udp port number", "%u");
    GET_STRING("-server_ip", server_ip, i, "Set destination mac address");
    GET_TYPE_INT("-print", print_packets, int, i, "Print frames flag", "%d");
	}

	if (server_port == -1) {
		server_port = udp_port;
		cli_print(cli, "NOTE : HOst udp port set to %d", server_port);
	}

	/* Create socket */
	sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sd == -1) {
		cli_print(cli, "ERROR : failed to create udp socket");
		return CLI_ERROR;
	}
	/*
	if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
	cli_print(cli, "ERROR : failed to set socket option IP_HDRINCL");
	return CLI_ERROR;
	}
	*/

	/* Specify the address family */
	destAddr.sin_family = AF_INET;
	/* Specify the destination port */
	destAddr.sin_port = htons(server_port);
	/* Specify the destination IP address */
	destAddr.sin_addr.s_addr = inet_addr(server_ip);

	/* Bind socket */
	memset(&_sockaddr_in, 0, sizeof(_sockaddr_in));
	_sockaddr_in.sin_family = AF_INET;
	_sockaddr_in.sin_port = htons(udp_port);
	_sockaddr_in.sin_addr.s_addr = INADDR_ANY;
	rc = bind(sd, (struct sockaddr *)&_sockaddr_in, sizeof(_sockaddr_in));
	if (rc == -1) {
		cli_print(cli, "ERROR : failed to bind udp socket");
		return CLI_ERROR;
	}

	cli_print(cli, "NOTE : loopback server started, server %s:%d", server_ip, server_port);

  
	/* Receive and print incoming packets */
	while (loop) {
		memset(msg, 0, MAX_MSG_SIZE);
		rc = recv(sd, msg, sizeof(msg), 0);
		if (rc > 0) {
			m_udp_rx_packets++;
      if (print_packets == 1) {
        cli_print(cli, "Received packet (%d bytes): %.*s", rc, ((rc > 43) ? 43 : rc), msg);
      }
			rc = sendto(sd, msg, rc, 0, (struct sockaddr *)&destAddr, sizeof(destAddr));
			if (rc < 0) {
				cli_print(cli, "ERROR : failed to send data");
			}
			else {
				m_udp_tx_packets++;
			}
			if (strcmp(msg, "!EXIT") == 0) {
				exit_loop = 1;
				soc_close(sd);
			}
		}
		else {
			if ( exit_loop ) {
				loop = 0;
			}
		}
	}

	return CLI_OK;
}






/*
Ethernet interface index in trusted IP instance.
Currently this is the only supported interface for raw packets
*/
#define ETHERNET_INTERFACE_INDEX 0


int loopback_raw_socket(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc)
{

	/* Pointer to trusted network IP instance */
	NX_IP					*trusted_instance = NULL;
	unsigned int	nrv = NX_SUCCESS;
	atlk_rc_t			rc = ATLK_OK;
	NX_PACKET			*raw_packet = NULL;
	int						i, print_packets = 0, loop = 1, set_sa = 0;
	char					lp_da[20] = "", lp_sa[20] = "";
	unsigned int	da_mac[6], sa_mac[6] = { 0 };

	IS_HELP_ARG("loopback raw -da XX:XX:XX:XX:XX:XX [-sa XX:XX:XX:XX:XX:XX] [-print 0(default) | 1 ]")
	CHECK_NUM_ARGS

	/* Get port number*/
	for (i = 0; i < argc; i += 2) {
		GET_STRING("-da", lp_da, i, "Set destination mac address");
		GET_STRING("-sa", lp_sa, i, "Set source mac address");
		GET_TYPE_INT("-print", print_packets, int, i, "Print frames flag", "%d");
	}

	if ( strlen(lp_da) == 0 ) {
		cli_print(cli, "ERROR : Missing parameter da");
		return CLI_ERROR;
	}

	/* Get trusted IP instance */
	rc = net_ip_trusted_instance_get(&trusted_instance);
	if (atlk_error(rc)) {
		cli_print(cli, "net_ip_trusted_instance_get: %s\n", atlk_rc_to_str(rc));
		return CLI_ERROR;
	}

	/* Enable raw packet support on trusted instance */
	nrv = nx_raw_packet_enable(trusted_instance);

	if (nrv != NX_SUCCESS) {
		cli_print(cli, "ERROR : nx_raw_packet_enable failed. error : %u\n", nrv);
		return CLI_ERROR;
	}


	rc = sscanf(lp_da, "%x:%x:%x:%x:%x:%x", &da_mac[0], &da_mac[1], &da_mac[2],
									&da_mac[3], &da_mac[4], &da_mac[5]);

	if (rc != 6) {
		cli_print(cli, "ERROR : Failed to retreive loopback da. ");
	}
	cli_print(cli, "DA will updated to: %02x:%02x:%02x:%02x:%02x:%02x", da_mac[0], da_mac[1], da_mac[2], da_mac[3], da_mac[4], da_mac[5]);

	if ( strlen(lp_sa) > 0 ) {
		rc = sscanf(lp_sa, "%x:%x:%x:%x:%x:%x", &sa_mac[0], &sa_mac[1], &sa_mac[2], &sa_mac[3], &sa_mac[4], &sa_mac[5]);
		if (rc == 6) {
			set_sa = 1;
		}
	}


	while ( loop ) {
		/*
		Receive a raw packet

		NOTE: Only packets with EtherType not used by IPv4, ARP and IPv6
		may be returned by nx_raw_packet_enable. Those 3 EtherType's are
		processed by the NetX stack internally.
		*/
		nrv = nx_raw_packet_receive(trusted_instance,
																ETHERNET_INTERFACE_INDEX,
																&raw_packet,
																TX_WAIT_FOREVER);
		if (nrv == NX_SUCCESS) {
			m_raw_rx_packets += 1;

			/* Update DA of sender */
			/*
			for (i = 0; i < 6; i++) {
				raw_packet->nx_packet_prepend_ptr[i] = (char)da_mac[i];
				if (set_sa == 1) {
					raw_packet->nx_packet_prepend_ptr[6 + i] = (char)sa_mac[i];
				}
			}
			*/
			(void)set_sa;


			// raw_packet->nx_packet_prepend_ptr[14] = 0xaa;
			raw_packet->nx_packet_prepend_ptr[12] = 0xdc;
			//raw_packet->nx_packet_prepend_ptr[13] = 0xdc;

			nrv = nx_raw_packet_send(trusted_instance, raw_packet, ETHERNET_INTERFACE_INDEX);

			if (nrv != NX_SUCCESS) {
				cli_print(cli, "ERROR : nx_raw_packet_send failed. error : %u\n", nrv);
				continue;
			}
			m_raw_tx_packets += 1;


		}
		else {
			cli_print(cli, "ERROR : nx_raw_packet_receive failed. error : %u\n", nrv);
			continue;
		}
		if (print_packets == 1) {
			hexdump_packet(cli, raw_packet);
		}

		// nx_packet_release( raw_packet);
	}

	return CLI_OK;

}


int cli_loopback_reset_cntrs(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc))
{
	cli_print(cli, "Counter reset");

	m_raw_tx_packets = 0;
	m_raw_rx_packets = 0;
	m_udp_tx_packets = 0;
	m_udp_rx_packets = 0;
	
	return ATLK_OK;
}

int cli_loopback_print_cntrs(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc))
{
	cli_print(cli, "RAW TX : module = %u", m_raw_tx_packets );
	cli_print(cli, "RAW RX : module = %u", m_raw_rx_packets );
	cli_print(cli, "UDP TX : module = %u", m_udp_tx_packets );
	cli_print(cli, "UDP RX : module = %u", m_udp_rx_packets );

	return ATLK_OK;
}

void hexdump_packet(struct cli_def *cli, const NX_PACKET *packet)
{
#define MAX_BYTES_PER_LINE 16
	uint32_t index;
	char		frame[2000] = { 0 };

	cli_print(cli, "Received raw packet data length %d:", (int) packet->nx_packet_length);

	for (index = 0; index < packet->nx_packet_length; index++) {
		char frm_byte[5] = { 0 };
		sprintf(frm_byte, "%02X ", packet->nx_packet_prepend_ptr[index]);
		strcat(frame, frm_byte);
		if ((index + 1) % MAX_BYTES_PER_LINE == 0) {
			strcat(frame, "\n");
		}
	}
}

