#include "lunp.h"

int
udpsrv_simple(int argc, char **argv, int sigc, void (*sigv[])(int), int intv[], void(*dg_cli)(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen))
{
  int                     sockfd;
  struct sockaddr_in      servaddr, cliaddr;

  if(argc == 2){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));
  }else if(argc == 3){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port        = htons(atoi(argv[2]));
  }else
    err_quit("usage: %s [<IPaddress>] <port#>", argv[0]);

  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

  dg_cli(stdin, sockfd, (SA *) &cliaddr, sizeof(cliaddr));

  exit(0);
}