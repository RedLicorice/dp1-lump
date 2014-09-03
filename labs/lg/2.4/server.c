#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

void childTask(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerSimple(sockfd, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  int num1, num2, result;
  XDR xdrs1, xdrs2;
  FILE *fd1, *fd2;
  
  // CLIENT REQUEST
  fd1 = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs1, fd1, XDR_DECODE);
  
  if (xdr_int(&xdrs1, &num1) == 0) {
    myFunctionWarning("xdr_int", NULL, "childTask");
    xdr_destroy(&xdrs1);
    return;
  }
  
  if (xdr_int(&xdrs1, &num2) == 0) {
    myFunctionWarning("xdr_int", NULL, "childTask");
    xdr_destroy(&xdrs1);
    return;
  }
  
  xdr_destroy(&xdrs1);
  
  result = num1 + num2;
  
  // SERVER RESPONSE
  fd2 = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs2, fd2, XDR_ENCODE);
  
  if (xdr_int(&xdrs2, &result) == 0) {
    myFunctionWarning("xdr_int", NULL, "childTask");
    xdr_destroy(&xdrs2);
    return;
  }
  
  fflush(fd2);
  xdr_destroy(&xdrs2);
}