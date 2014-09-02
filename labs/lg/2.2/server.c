#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

#define MAX_DATAGRAMS 3
#define MAX_CLIENTS 10

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char clientReq[BUFFSIZE];
  struct sockaddr_in daddr;
  
  sockfd = myUdpServerStartup(SERVER_PORT_ARG);

  while (1) {
    
    myWarning("\nServer on listening on port %d...", NULL, atoi(SERVER_PORT_ARG));
    
    myUdpReadString(sockfd, clientReq, BUFFSIZE, &daddr, NULL);
    
    if (myUdpLimitClients(daddr, MAX_DATAGRAMS, MAX_CLIENTS) == false)
      myWarning("Too many requests from this client", "main");
    
    else
      myUdpWriteString(sockfd, clientReq, daddr);
    
  }
}
