#include "lunp.h"

int
udpcli_simple(int argc, char **argv, int sigc, void (*sigv[])(int), int intv[], void(*dg_cli)(FILE *ifp, int sockfd, SA *cliaddr, socklen_t clilen))
{
  int                     sockfd;
  struct sockaddr_in      servaddr;//, cliaddr;

  if(argc == 2){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(atoi(argv[1]));
    Inet_pton(AF_INET, LOOPBACK_STR, &servaddr.sin_addr);
  }else if(argc == 3){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(atoi(argv[2]));
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  }else
    err_quit("usage: %s [<IPaddress>] <port#>", argv[0]);

  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

  exit(0);
}
