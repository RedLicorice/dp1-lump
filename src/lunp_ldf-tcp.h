#ifndef __lunp_ldf_tcp_h
#define __lunp_ldf_tcp_h

/* TCP CLIENT */

int tcpcli_simple(int argc, char *argv[], void (*client_task)(int));


/* TCP SERVER */

/* 
 * usage: tcpsrv_ocpc [ <host> ] <port#>
 * One Child Per Client 
 * - father blocks on accept
 * - whenever a connection arrives it forks himself
 * -   the child executes `child_main' function
 * -   the father goes back to accept
 */
void tcpsrv_ocpc(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int));

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

/* ka: keep alive*/
int
  tcpsrv_preka(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int));

/* 
 * usage: tcpsrv_pre [ <host> ] <port#>
 * Preforked Server
 * - father gnerates `nchildren'
 * - each children has its task `children_task
 * - father pauses, everything is done by the children
 */
int tcpsrv_proxy(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[]);

int
  tcpsrv_mixed(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int));

int
tcpsrv_select(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*server_task)(int));

#endif
