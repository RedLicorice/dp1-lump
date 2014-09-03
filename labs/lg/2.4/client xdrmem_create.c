#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int num1, num2, result, serverResLength, bufferPos;
  XDR xdrs1, xdrs2;
  char clientReq[BUFFSIZE], serverRes[BUFFSIZE];
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the first number: ");
  scanf("%d", &num1);
  
  printf("Please type the second number: ");
  scanf("%d", &num2);
  
  // CLIENT RESPONSE
  xdrmem_create(&xdrs1, clientReq, BUFFSIZE, XDR_ENCODE);
  
  if (xdr_int(&xdrs1, &num1) == 0)
    myFunctionError("xdr_int", NULL, "main");
  
  if (xdr_int(&xdrs1, &num2) == 0)
    myFunctionError("xdr_int", NULL, "main");
  
  bufferPos = xdr_getpos(&xdrs1);
  myTcpWriteBytes(sockfd, (void*)clientReq, bufferPos);
  
  xdr_destroy(&xdrs1);
  
  // SERVER RESPONSE
  myTcpReadBytes(sockfd, (void*)serverRes, BUFFSIZE, &serverResLength);
  // WARNING: This returns when the server closes the connection (orderly shutdown). If the server does not close the connection, this never returns.
  
  xdrmem_create(&xdrs2, serverRes, serverResLength, XDR_DECODE);
  
  if (xdr_int(&xdrs2, &result) == 0)
    myFunctionError("xdr_int", NULL, "main");
  
  xdr_destroy(&xdrs2);
  
  printf("The server replied: %d\n", result);
  
  Close(sockfd);
  return 0;
}