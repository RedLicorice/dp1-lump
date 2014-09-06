#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

#define MAX_DATAGRAMS 3
#define MAX_CLIENTS 10

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
    
  if (myUdpLimitClients(daddr, MAX_DATAGRAMS, MAX_CLIENTS) == false)
    myWarning("Too many requests from this client", "main");
    
  else {
    myWarning("Request datagram received successfully", "childTask");
    
    myUdpWriteString(sockfd, clientReq, daddr);
    myWarning("Response datagram sent successfully", "childTask");
  }
}