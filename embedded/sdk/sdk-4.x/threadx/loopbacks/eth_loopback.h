#ifndef __ETH_LOOPBACK_H__
#define __ETH_LOOPBACK_H__


int loopback_bsd_ip_udp( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc );
int loopback_raw_socket(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc);
int cli_loopback_reset_cntrs(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc));
int cli_loopback_print_cntrs(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc));

#endif