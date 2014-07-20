/*
 * argv[1] = indirizzo del server
 * argv[2] = porta del server
 */

#include "../lunp.h"

#define BUFFER_SIZE 14
#define NUMBER_SIZE 6

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int num1, num2;
  char bufferOut[MAXLINE], bufferIn[BUFFER_SIZE];
  char num2string[NUMBER_SIZE];
  
  sockfd = myTcpClientStartup(argv[1], argv[2]);
  
  printf("Digitare il primo numero: ");
  scanf("%d", &num1);
  snprintf(bufferIn, NUMBER_SIZE, "%d", num1);
  
  strcat(bufferIn, " ");
  
  printf("Digitare il secondo numero: ");
  scanf("%d", &num2);
  snprintf(num2string, NUMBER_SIZE, "%d", num2);
  strcat(bufferIn, num2string);
  
  strcat(bufferIn, "\r\n");
  
  myTcpWriteString(sockfd, bufferIn);
  
  myTcpReadLine(sockfd, bufferOut, MAXLINE, NULL);
  
  printf("Il server ha risposto: %s", bufferOut);
  
  myClose(sockfd);
  return 0;
}