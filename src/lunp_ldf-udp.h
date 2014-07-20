#ifndef __lunp_ldf_udp_h
#define __lunp_ldf_udp_h

/* UDP CLIENT */

int udpcli_simple(int argc, char **argv, int sigc, void (*sigv[])(int), int intv[], void(*dg_cli)(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen));


/* UDP SERVER */

int
udpsrv_simple(int argc, char **argv, int sigc, void (*sigv[])(int), int intv[], void(*dg_cli)(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen));

#endif
