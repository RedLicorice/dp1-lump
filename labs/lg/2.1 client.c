#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port
#define NAME_ARG argv[3] // name

#define N 32

#define MAX_SECONDS 3
#define MAX_ATTEMPTS 5

bool clientTask(SOCKET sockfd, char *name, struct sockaddr_in daddr, char *bufferReply);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char bufferReply[N];
  struct sockaddr_in daddr;

  sockfd = myUdpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG, &daddr);
  
  if (clientTask(sockfd, NAME_ARG, daddr, bufferReply) == true)
    printf("Reply from server: %s\n", bufferReply);
  
  else // false
    printf("No replies from server\n");

  Close(sockfd);
  return 0;
}

bool clientTask(SOCKET sockfd, char *name, struct sockaddr_in daddr, char *bufferReply) {
  int i;
  
  for (i = 0; i < MAX_ATTEMPTS; ++i) {
    
    printf("Attempt %d...\n", i + 1);
    
    myUdpWriteBytes(sockfd, (void*)name, sizeof(name), daddr);

    if (myWaitForSingleObject(MAX_SECONDS, sockfd) == true) {
      myUdpReadBytes(sockfd, (void*)bufferReply, N, NULL, NULL);
      return true;
    }
    
  }
  
  return false; // expired timeout
}