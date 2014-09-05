#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

#define NUMBER_SIZE 6 // strlen(2^16 - 1)

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  uint16_t num1, num2;
  char serverRes[BUFFSIZE], num2string[NUMBER_SIZE];
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the first number: ");
  scanf("%hu", &num1);
  
  printf("Please type the second number: ");
  scanf("%hu", &num2);
  
  snprintf(num2string, NUMBER_SIZE, "%hu", num1);
  myTcpWriteString(sockfd, num2string);
  
  myTcpWriteString(sockfd, " ");

  snprintf(num2string, NUMBER_SIZE, "%hu", num2);
  myTcpWriteString(sockfd, num2string);
  
  myTcpWriteString(sockfd, "\r\n");
  
  myTcpReadLine(sockfd, serverRes, BUFFSIZE, NULL);
  
  printf("The server replied: %s", serverRes);
  
  Close(sockfd);
  return 0;
}