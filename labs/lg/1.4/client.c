#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port
#define NAME_ARG argv[3] // name

#define N 32

#define MAX_SECONDS 3

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char bufferReply[N];
  struct sockaddr_in daddr;

  sockfd = myUdpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG, &daddr);
  
  myUdpWriteBytes(sockfd, (void*)NAME_ARG, sizeof(NAME_ARG), daddr);

  if (myWaitForSingleObject(MAX_SECONDS, sockfd) == false)
    myError("Expired timeout", "main");
  
  myUdpReadBytes(sockfd, (void*)bufferReply, N, NULL, NULL);
  
  printf("Reply from server: %s\n", bufferReply);

  Close(sockfd);
  return 0;
}
