#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <ctype.h>

#define IBS 1024
#define OBS 1024
#define BS    80

void
sig_int(int signo){
  int i;

  fprintf(stdout, "In the SIG_INT handler\n");

  /* terminate all children */
  for (i = 0; i < nchildren; i++)
    kill(pids[i], SIGTERM);
  while (wait(NULL) > 0)          /* wait for all children */
    ;
  if (errno != ECHILD)
    err_sys("wait error");

  exit(0);
}

void
sig_usr1(int signo){
  fprintf(stdout, "In the SIG_USR1 handler\n");
}

void
child_task(int connfd){
  /* Bootstrap variables */
  struct sockaddr_in      servaddr;
  /* Main variables */
  uint16_t   op1, op2;  
  uint16_t   res;
  char       tbuf[BS+1];
  char       ibuf1[OBS+1];
  char       ibuf2[OBS+1];
  char       obuf[IBS+1];
  int        quit;
  int        len;
  /* XDR */
  XDR xdrs_in;
  XDR xdrs_out;
  FILE *xfp_r;
  FILE *xfp_w;
  char *xdr_ibuf1;
  char *xdr_ibuf2;
  char *xdr_obuf;

  /* XDR Bootstrap*/
  xfp_r = Fdopen(connfd, "r");
  xdrstdio_create(&xdrs_in, xfp_r, XDR_DECODE); 

  xfp_w = Fdopen(connfd, "w");
  xdrstdio_create(&xdrs_out, xfp_w, XDR_ENCODE); 

  setbuf(xfp_w, NULL);
  setbuf(xfp_r, NULL);

  /* Main */
  fprintf(stdout, "Client starts.\n");

  /* Get op1 and op2 from stdin */
  quit = 0;
  while(!quit){
    /* Gathering input from the client */
    xdr_ibuf1 = (char *)Malloc(OBS*sizeof(char));
    xdr_ibuf2 = (char *)Malloc(OBS*sizeof(char));

    if(!xdr_string(&xdrs_in, &xdr_ibuf1, OBS)) 
      fprintf(stderr, "Error occurred while sending ibuf1\n");
    if(!xdr_string(&xdrs_in, &xdr_ibuf2, OBS)) 
      fprintf(stderr, "Error occurred while sending ibuf2\n");

    printf("Strings: %s, %s.", xdr_ibuf1, xdr_ibuf2);
    /* Validating input */
    if(strcmp(xdr_ibuf1, xdr_ibuf2)==0)
      xdr_obuf = strdup("Strings are equal\0");
    else
      xdr_obuf = strdup("Strings are NOT equal\0");

    /* XDR transfer obuf1 and obuf2 */
    fprintf(stdout, "Sending the response.\n");
    if(!(xdr_string(&xdrs_out, &xdr_obuf, OBS)))
      fprintf(stdout, "Error: the message has not been sent.\n");
    else
      fprintf(stdout, "Message correctly sent.\n");
  }
  xdr_destroy(&xdrs_in);
  xdr_destroy(&xdrs_out);
  fprintf(stdout, "Client ends.\n");
  exit(0);
}

int main(int argc, char *argv[]){
  void ( *sigv[2] ) (int) = {sig_int, sig_usr1};
  int intv[2] = {SIGINT, SIGUSR1};
  tcpsrv_pre(argc, argv, 2, sigv, intv, child_task);
  return 0;
}
