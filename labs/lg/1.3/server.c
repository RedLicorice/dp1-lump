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
  char clientReq[MAXLINE + 1], serverRes[MAXLINE + 1];
  uint16_t op1, op2;
  uint32_t result;
  
  printf("\n");
  myWarning("Connection accepted", "childTask");
  
  while (1) {
    
    if (myTcpReadLine(sockfd, clientReq, MAXLINE, NULL) == false)
      return;
    
    if (sscanf(clientReq, "%hu %hu", &op1, &op2) != 2) // h = unsigned int -> uint16_t (https://en.wikipedia.org/wiki/Printf_format_string#Format_placeholders)
      myTcpWriteString(sockfd, "incorrect operands\r\n");
    
    else {
      result = op1 + op2;
      
      if (result > 65535) // = 2^16 - 1
	myTcpWriteString(sockfd, "overflow\r\n");
      
      else {
	sprintf(serverRes, "%u\r\n", result);
	myTcpWriteString(sockfd, serverRes);
      }
    }
    
  }
}