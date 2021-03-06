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
  XDR *xdrs;
  char clientReq[BUFFSIZE];
  int bufferPos;
  bool success;
  
  myTcpReadBytesOnce(sockfd, (void*)clientReq, BUFFSIZE, &bufferPos);
  
  xdrs = myUdpReadXdrStartup(clientReq, bufferPos);
  
  success = myUdpReadXdr(xdrs, (myXdrFunction)&xdr_int, (void*)num1);
  
  if (success == true)
    success = myUdpReadXdr(xdrs, (myXdrFunction)&xdr_int, (void*)num2);
  
  myUdpReadXdrCleanup(xdrs);
  
  if (success == false)
    return false;
  
  myWarning("Request received successfully", "clientRequest");
  return true;
}

bool serverResponse(SOCKET sockfd, int result) {
  XDR *xdrs;
  char serverRes[BUFFSIZE];
  int bufferPos;
  bool success;
  
  xdrs = myUdpWriteXdrStartup(serverRes, BUFFSIZE);
  
  success = myUdpWriteXdr(xdrs, (myXdrFunction)&xdr_int, (void*)&result);
  
  bufferPos = myUdpWriteXdrCleanup(xdrs, success);
  
  if (success == false)
    return false;
  
  myTcpWriteBytes(sockfd, (void*)serverRes, bufferPos);
  
  myWarning("Response sent successfully", "serverResponse");
  return true;
}