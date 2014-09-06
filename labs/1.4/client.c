#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port
#define NAME_ARG argv[3] // name

#define MAX_SECONDS 3

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char serverRes[BUFFSIZE];
  struct sockaddr_in *daddr;

  sockfd = myUdpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG, &daddr);
  
  myUdpWriteString(sockfd, NAME_ARG, *daddr);

  if (myWaitForSingleObject(MAX_SECONDS, sockfd) == false)
    myError("Expired timeout", "main");
  
  myUdpReadString(sockfd, serverRes, BUFFSIZE, NULL, NULL);
  
  printf("Reply from server: %s\n", serverRes);

  Close(sockfd);
  free(daddr);
  return 0;
}
