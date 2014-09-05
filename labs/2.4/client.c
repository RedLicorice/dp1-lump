#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

void clientRequest(SOCKET sockfd, int num1, int num2);
void serverResponse(SOCKET sockfd, int *result);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int num1, num2, result;
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the first number: ");
  scanf("%d", &num1);
  
  printf("Please type the second number: ");
  scanf("%d", &num2);
  
  clientRequest(sockfd, num1, num2);
  
  serverResponse(sockfd, &result);
  
  printf("The server replied: %d\n", result);
  
  Close(sockfd);
  return 0;
}

void clientRequest(SOCKET sockfd, int num1, int num2) {
  XDR xdrs;
  FILE *fd;
  
  fd = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
  
  if (xdr_int(&xdrs, &num1) == FALSE)
    myFunctionError("xdr_int", NULL, "clientRequest");
  
  if (xdr_int(&xdrs, &num2) == FALSE)
    myFunctionError("xdr_int", NULL, "clientRequest");
  
  fflush(fd);
  xdr_destroy(&xdrs);
}

void serverResponse(SOCKET sockfd, int *result) {
  XDR xdrs;
  FILE *fd;
  
  fd = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs, fd, XDR_DECODE);
  
  if (xdr_int(&xdrs, result) == FALSE)
    myFunctionError("xdr_int", NULL, "serverResponse");
  
  xdr_destroy(&xdrs);
}