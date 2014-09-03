#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int num1, num2, result;
  XDR xdrs1, xdrs2;
  FILE *fd1, *fd2;
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the first number: ");
  scanf("%d", &num1);
  
  printf("Please type the second number: ");
  scanf("%d", &num2);
  
  // CLIENT REQUEST
  fd1 = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs1, fd1, XDR_ENCODE);
  
  if (xdr_int(&xdrs1, &num1) == 0)
    myFunctionError("xdr_int", NULL, "main");
  
  if (xdr_int(&xdrs1, &num2) == 0)
    myFunctionError("xdr_int", NULL, "main");
  
  fflush(fd1);
  xdr_destroy(&xdrs1);
  
  // SERVER RESPONSE
  fd2 = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs2, fd2, XDR_DECODE);
  
  if (xdr_int(&xdrs2, &result) == 0)
    myFunctionError("xdr_int", NULL, "main");
  
  xdr_destroy(&xdrs2);
  
  printf("The server replied: %d\n", result);
  
  Close(sockfd);
  return 0;
}