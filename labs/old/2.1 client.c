#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"

#define IBS 1024
#define OBS 1024
#define BS    80

#define MAXSTR    31
#define MAXCOUNT   3
#define TIMEOUT    5

void
dg_cli(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen)
{
  socklen_t       len;
  char            tbuf[BS+1];
  char            ibuf[IBS+1];
  char            obuf[OBS+1];
  int             count;
  int             countdown;
  int             i, idx, n, sn;
  int             flag_recv;
  count = 0;

  /* Variables to implement the timeout */
  fd_set cset;
  struct timeval time;
  FD_ZERO(&cset);
  FD_SET(sockfd, &cset);

  time.tv_sec = TIMEOUT;
  time.tv_usec = 0;

  fprintf(stdout, "Udp server starts\n");
  for ( ; ; ) {
    len = clilen;
    
    /* Getting the input from stdin */
    fprintf(stdout, "Enter string (max %d char):\n", MAXSTR);
    fprintf(stdout, "> ");
    fgets(tbuf, IBS, stdin);
    idx = strlen(tbuf);
    for(i = idx; i < MAXSTR; i++) tbuf[i]='\0';
    if(idx > MAXSTR){
      fprintf(stdout, "The input string is greater then the maximun (%d).\n", MAXSTR);
      fprintf(stdout, "Please retry.\n");
    }else{
      countdown = MAXCOUNT;
      flag_recv = 0;
      for( ; countdown>0 && !flag_recv ; countdown--){
	fprintf(stdout, "Input string is correct.\n");
	/* Sending */
	strncpy(obuf, tbuf, idx);
	Sendto(sockfd, obuf, idx, 0, cliaddr, len);

      again:
	if ( (sn = select(FD_SETSIZE, &cset, NULL, NULL, &time)) < 0) {
	  if (errno == EINTR)
	    goto again;/* back to for() */
	  else
	    err_sys("select error");
	}
	if( sn==0 ){
	  fprintf(stdout, "Timeout expired.\n");
	}else{
	  /* Receiving */
	  n = Recvfrom(sockfd, ibuf, MAXLINE, 0, cliaddr, &len);
	  fprintf(stdout, "Received %d bytes\n", n); 
	  strncpy(tbuf, ibuf, n);
	  fprintf(stdout, "Server has replied: %s\n", tbuf);

	  count++;
	  fprintf(stdout, "%d datagram(s) sent\n", count);
	  flag_recv = 1;
	}
      }
      if(countdown == 0)
	fprintf(stdout, "Maximum number of attempt reached.\n");
    }
  }
  fprintf(stdout, "Udp server ends\n");
  return;
}


int main(int argc, char *argv[]){
  udpcli_simple(argc, argv, 0, NULL, NULL, dg_cli);
  return 0;
}
