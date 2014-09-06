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
  char clientReq[BUFFSIZE];
  int bufferPos;
  bool_t success;
  
  myTcpReadBytesOnce(sockfd, (void*)clientReq, BUFFSIZE, &bufferPos);
  
  xdrmem_create(&xdrs, clientReq, bufferPos, XDR_DECODE);
  
  success = xdr_int(&xdrs, num1);
  
  if (success == TRUE)
    success = xdr_int(&xdrs, num2);
  
  xdr_destroy(&xdrs);
  
  if (success == FALSE)
    return false;
  
  myWarning("Request received successfully", "clientRequest");
  return true;
}

bool serverResponse(SOCKET sockfd, int result) {
  XDR xdrs;
  char serverRes[BUFFSIZE];
  int bufferPos;
  
  xdrmem_create(&xdrs, serverRes, BUFFSIZE, XDR_ENCODE);
  
  if (xdr_int(&xdrs, &result) == FALSE) {
    xdr_destroy(&xdrs);
    return false;
  }
  
  bufferPos = xdr_getpos(&xdrs);
  myTcpWriteBytes(sockfd, (void*)serverRes, bufferPos);
  
  xdr_destroy(&xdrs);
  
  myWarning("Response sent successfully", "serverResponse");
  return true;
}