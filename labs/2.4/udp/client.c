#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

void clientRequest(SOCKET sockfd, int num1, int num2, struct sockaddr_in daddr);
void serverResponse(SOCKET sockfd, int *result);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int num1, num2, result;
  struct sockaddr_in *daddr;
  
  sockfd = myUdpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG, &daddr);
  
  printf("Please type the first number: ");
  scanf("%d", &num1);
  
  printf("Please type the second number: ");
  scanf("%d", &num2);
  
  clientRequest(sockfd, num1, num2, *daddr);
  
  serverResponse(sockfd, &result);
  
  printf("The server replied: %d\n", result);
  
  Close(sockfd);
  free(daddr);
  return 0;
}

void clientRequest(SOCKET sockfd, int num1, int num2, struct sockaddr_in daddr) {
  XDR *xdrs;
  char clientReq[BUFFSIZE];
  int bufferPos;
  
  xdrs = myUdpWriteXdrStartup(clientReq, BUFFSIZE);
  
  if (myUdpWriteXdr(xdrs, (myXdrFunction)&xdr_int, (void*)&num1) == false)
    myFunctionError("xdr_int", NULL, "clientRequest");
  
  if (myUdpWriteXdr(xdrs, (myXdrFunction)&xdr_int, (void*)&num2) == false)
    myFunctionError("xdr_int", NULL, "clientRequest");
  
  bufferPos = myUdpWriteXdrCleanup(xdrs, true);
  
  myUdpWriteBytes(sockfd, (void*)clientReq, bufferPos, daddr);
}

void serverResponse(SOCKET sockfd, int *result) {
  XDR *xdrs;
  char serverRes[BUFFSIZE];
  int bufferPos;
  
  myUdpReadBytes(sockfd, (void*)serverRes, BUFFSIZE, NULL, &bufferPos);
  
  xdrs = myUdpReadXdrStartup(serverRes, bufferPos);
  
  if (myUdpReadXdr(xdrs, (myXdrFunction)&xdr_int, (void*)result) == false)
    myFunctionError("xdr_int", NULL, "serverResponse");
  
  myUdpReadXdrCleanup(xdrs);
}