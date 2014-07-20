#include "lunp.h"

int
tcpcli_simple(int argc, char *argv[], void (*client_task)(int))
{
  int			sockfd;
  struct sockaddr_in	servaddr;

  if(argc == 2){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    Inet_pton(AF_INET, LOOPBACK_STR, &servaddr.sin_addr);
  }else if(argc == 3){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  }
  else
    err_quit("usage: %s [<IPaddress>] <port#>", argv[0]);

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  
  /* Perform the task - do all*/
  client_task(sockfd);

  Close(sockfd);
  return 0;
}
