#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

#define BUFFER_SIZE 14
#define NUMBER_SIZE 6

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int num1, num2;
  char bufferOut[MAXLINE], bufferIn[BUFFER_SIZE];
  char num2string[NUMBER_SIZE];
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the first number: ");
  scanf("%d", &num1);
  snprintf(bufferIn, NUMBER_SIZE, "%d", num1);
  
  strcat(bufferIn, " ");
  
  printf("Please type the second number: ");
  scanf("%d", &num2);
  snprintf(num2string, NUMBER_SIZE, "%d", num2);
  strcat(bufferIn, num2string);
  
  strcat(bufferIn, "\r\n");
  
  myTcpWriteString(sockfd, bufferIn);
  
  myTcpReadLine(sockfd, bufferOut, MAXLINE, NULL);
  
  printf("The server replied: %s", bufferOut);
  
  myClose(sockfd);
  return 0;
}