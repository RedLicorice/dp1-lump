/*
 * argv[1] = indirizzo del server
 * argv[2] = porta del server
 */

#include "../lunp.h"

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  sockfd = myTcpClientStartup(argv[1], argv[2]);
  myClose(sockfd);
  return 0;
}