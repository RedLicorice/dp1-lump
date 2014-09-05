#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

void childTask(SOCKET sockfd);
bool clientRequest(SOCKET sockfd, int *num1, int *num2);
bool serverResponse(SOCKET sockfd, int result);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerSimple(sockfd, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  int num1, num2, result;
  
  if (clientRequest(sockfd, &num1, &num2) == false)
    return;

  result = num1 + num2;

  if (serverResponse(sockfd, result) == false)
    return;
}

bool clientRequest(SOCKET sockfd, int *num1, int *num2) {
  XDR xdrs;
  FILE *fd;
  
  fd = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs, fd, XDR_DECODE);
  
  if (xdr_int(&xdrs, num1) == FALSE)
    return false;
  
  if (xdr_int(&xdrs, num2) == FALSE)
    return false;
  
  xdr_destroy(&xdrs);
  return true;
}

bool serverResponse(SOCKET sockfd, int result) {
  XDR xdrs;
  FILE *fd;
  
  fd = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
  
  if (xdr_int(&xdrs, &result) == FALSE)
    return false;
  
  fflush(fd);
  xdr_destroy(&xdrs);
  return true;
}