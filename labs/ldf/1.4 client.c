#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"

#define IBS 1024
#define OBS 1024
#define BS    80

#define MAXSTR    31

void
dg_cli(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen)
{
  socklen_t       len;
  char            tbuf[BS+1];
  char            ibuf[IBS+1];
  char            obuf[OBS+1];
  int             count;
  int             i, idx, n;
  count = 0;
  
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
      fprintf(stdout, "Input string is correct.\n");
      /* Sending */
      strncpy(obuf, tbuf, idx);
      Sendto(sockfd, obuf, idx, 0, cliaddr, len);

      /* Receiving */
      n = Recvfrom(sockfd, ibuf, MAXLINE, 0, cliaddr, &len);
      fprintf(stdout, "Received %d bytes\n", n); 
      strncpy(tbuf, ibuf, n);
      fprintf(stdout, "Server has replied: %s\n", tbuf);

      count++;
      fprintf(stdout, "%d datagram(s) sent\n", count);
    }
  }
  fprintf(stdout, "Udp server ends\n");
  return;
}


int main(int argc, char *argv[]){
  udpcli_simple(argc, argv, 0, NULL, NULL, dg_cli);
  return 0;
}
