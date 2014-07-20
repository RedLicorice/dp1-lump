#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"

#define IBS 1024
#define OBS 1024

void
sig_int(int signo){
  fprintf(stdout, "In the SIG_INT handler\n");
}

void
sig_usr1(int signo){
  fprintf(stdout, "In the SIG_USR1 handler\n");
}

void
dg_srv(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen)
{
  socklen_t       len;
  char            ibuf[IBS+1];
  char            obuf[OBS+1];
  int             count;
  int             n;
  count = 0;
  
  fprintf(stdout, "Udp server starts\n");
  for ( ; ; ) {
    len = clilen;

    /* Receiving */
    n = Recvfrom(sockfd, ibuf, MAXLINE, 0, cliaddr, &len);
    fprintf(stdout, "Received %d bytes\n", n);
    
    ibuf[strlen(ibuf)] = '\0';

    /* Sending */
    strncpy(obuf, ibuf, n);
    Sendto(sockfd, obuf, n, 0, cliaddr, len);

    count++;
    fprintf(stdout, "%d datagram(s) sent\n", count);
  }
  fprintf(stdout, "Udp server ends\n");
  return;
}


int main(int argc, char *argv[]){
  void ( *sigv[2] ) (int) = {sig_int, sig_usr1};
  int intv[2] = {SIGINT, SIGUSR1};
  udpsrv_simple(argc, argv, 2, sigv, intv, dg_srv);
  return 0;
}
