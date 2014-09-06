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
  if (myTcpReadXdr(sockfd, (myXdrFunction)&xdr_int, (void*)num1) == false)
    return false;
  
  if (myTcpReadXdr(sockfd, (myXdrFunction)&xdr_int, (void*)num2) == false)
    return false;

  myWarning("Request received successfully", "clientRequest");
  return true;
}

bool serverResponse(SOCKET sockfd, int result) {
  if (myTcpWriteXdr(sockfd, (myXdrFunction)&xdr_int, (void*)&result) == false)
    return false;

  myWarning("Response sent successfully", "serverResponse");
  return true;
}