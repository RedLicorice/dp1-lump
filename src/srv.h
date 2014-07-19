#ifndef __srvh
#define __srvh
#include "lunp.h"

static int       nchildren;
static pid_t    *pids;

/* TCP SERVER */

/* 
 * usage: tcpsrv_ocpc [ <host> ] <port#>
 * One Child Per Client 
 * - father blocks on accept
 * - whenever a connection arrives it forks himself
 * -   the child executes `child_main' function
 * -   the father goes back to accept
 */
int tcpsrv_ocpc(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int));

/* 
 * usage: tcpsrv_ocpc_n [ <host> ] <port#>
 * One Child Per Client (up to nchildren_max childs)
 * - father blocks on accept
 * - whenever a connection arrives it forks himself
 * -   the child executes `child_main' function
 * -   the father goes back to accept
 * 
 * Notes: 
 * 1) waitpid WNOHANG : to refresh the nchildren_count while looping.
 *                      This is performed before the accept.
 * 2) blocking wait   : to prevent the father from accepting new connection.
 *                      This is performed after the blocking wait, before the accept.
 * 3) sigint handler  : if sigint occurs, just kill and wait all childs.
 * 
 * Warning:
 * TODO
 * 1) possible race condition! - THIS IS NOT YET SUPPORTED
 */
int
tcpsrv_ocpc_n(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int), int nchildren_max);


/* 
 * usage: tcpsrv_pre [ <host> ] <port#>
 * Preforked Server
 * - father gnerates `nchildren'
 * - each children has its task `children_task
 * - father pauses, everything is done by the children
 */
int tcpsrv_pre(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void(*child_task)(int));

/* 
 * usage: tcpsrv_pre [ <host> ] <port#>
 * Preforked Server
 * - father gnerates `nchildren'
 * - each children has its task `children_task
 * - father pauses, everything is done by the children
 */
int tcpsrv_proxy(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[]);


int
tcpsrv_select(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*server_task)(int));

/*
 * usage: child_make(child_idx, listenfd, addrlen)
 *  child_idx is not used by default (but may be useful)
 */
pid_t child_make(int, int, int, void (*child_task)(int));
void child_main(int, int, int, void (*child_task)(int));

/* UDP SERVER */
int
udpsrv_simple(int argc, char **argv, int sigc, void (*sigv[])(int), int intv[], void(*dg_cli)(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen));

int
udpcli_simple(int argc, char **argv, int sigc, void (*sigv[])(int), int intv[], void(*dg_cli)(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen));


#endif
