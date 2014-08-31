#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

#define N 32
#define MAX_DATAGRAMS 3
#define MAX_CLIENTS 10

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int numberOfReadBytes;
  char bufferReply[N];
  struct sockaddr_in daddr;
  
  sockfd = myUdpServerStartup(SERVER_PORT_ARG);

  while (1) {
    
    myWarning("\nServer on listening on port %d...", NULL, atoi(SERVER_PORT_ARG));
    
    myUdpReadBytes(sockfd, (void*)bufferReply, N, &daddr, &numberOfReadBytes);
    
    if (myUdpLimitClients(daddr, MAX_DATAGRAMS, MAX_CLIENTS) == false)
      myWarning("Too many requests from this client", "main");
    
    else
      myUdpWriteBytes(sockfd, (void*)bufferReply, numberOfReadBytes, daddr);
    
  }
}
