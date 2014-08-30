#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

#define N 32

int main(int argc, char *argv[]) {
  int sockfd, numberOfReadBytes;
  char bufferReply[N];
  struct sockaddr_in daddr;
  
  sockfd = myUdpServerStartup(SERVER_PORT_ARG);

  while (1) {
    
    myWarning("\nServer on listening on port %d...", NULL, atoi(SERVER_PORT_ARG));
    
    myUdpReadBytes(sockfd, (void*)bufferReply, N, &daddr, &numberOfReadBytes);
    
    myUdpWriteBytes(sockfd, (void*)bufferReply, numberOfReadBytes, daddr);
    
  }
}
