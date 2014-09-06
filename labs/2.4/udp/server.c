#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

void childTask(SOCKET sockfd);
bool clientRequest(SOCKET sockfd, int *num1, int *num2, struct sockaddr_in *saddr);
bool serverResponse(SOCKET sockfd, int result, struct sockaddr_in saddr);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myUdpServerStartup(SERVER_PORT_ARG);
  
  myUdpServerSimple(sockfd, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  int num1, num2, result;
  struct sockaddr_in saddr;
  
  if (clientRequest(sockfd, &num1, &num2, &saddr) == false)
    return;

  result = num1 + num2;

  if (serverResponse(sockfd, result, saddr) == false)
    return;
}

bool clientRequest(SOCKET sockfd, int *num1, int *num2, struct sockaddr_in *saddr) {
  XDR *xdrs;
  char clientReq[BUFFSIZE];
  int bufferPos;
  bool success;
  
  myUdpReadBytes(sockfd, (void*)clientReq, BUFFSIZE, saddr, &bufferPos);
  
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

bool serverResponse(SOCKET sockfd, int result, struct sockaddr_in saddr) {
  XDR *xdrs;
  char serverRes[BUFFSIZE];
  int bufferPos;
  bool success;
  
  xdrs = myUdpWriteXdrStartup(serverRes, BUFFSIZE);
  
  success = myUdpWriteXdr(xdrs, (myXdrFunction)&xdr_int, (void*)&result);
  
  bufferPos = myUdpWriteXdrCleanup(xdrs, success);
  
  if (success == false)
    return false;
  
  myUdpWriteBytes(sockfd, (void*)serverRes, bufferPos, saddr);
  
  myWarning("Response sent successfully", "serverResponse");
  return true;
}