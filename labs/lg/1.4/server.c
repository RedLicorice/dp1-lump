#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

void childTask(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myUdpServerStartup(SERVER_PORT_ARG);

  myUdpServerSimple(sockfd, &childTask);
  
  return 0; // it shuts down a compiler warning
}

void childTask(SOCKET sockfd) {
  char clientReq[BUFFSIZE];
  struct sockaddr_in daddr;
  
  myUdpReadString(sockfd, clientReq, BUFFSIZE, &daddr, NULL);
    
  myUdpWriteString(sockfd, clientReq, daddr);
}