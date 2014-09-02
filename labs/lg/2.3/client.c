#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

bool clientTask(SOCKET sockfd, char *fileName);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char fileName[MAXFILENAMELENGTH];
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the file name (maximum %d characters, empty string to close): ", MAXFILENAMELENGTH - 1);
  gets(fileName);
  
  while (strcmp(fileName, "") != 0) {
    
    if (clientTask(sockfd, fileName) == false) {
      Close(sockfd);
      return 1;
    }
    
    printf("\nPlease type the file name (maximum %d characters, empty string to close): ", MAXFILENAMELENGTH - 1);
    gets(fileName);
    
  }
  
  myTcpWriteString(sockfd, QUIT);
  
  Close(sockfd);
  return 0;
}

bool clientTask(SOCKET sockfd, char *fileName) {
  char serverRes[BUFFSIZE];
  uint32_t fileSize;
  
  myTcpWriteString(sockfd, GET);
  myTcpWriteString(sockfd, fileName);
  myTcpWriteString(sockfd, "\r\n");
  
  myTcpReadLine(sockfd, serverRes, BUFFSIZE, NULL);
  
  if (strcmp(serverRes, ERR) == 0) {
    myWarning("Illegal command or non-existing file", "clientTask");
    return true; // prossimo file
  }
    
  // else: serverRes = OK
  myTcpReadBytes(sockfd, (void*)(&fileSize), sizeof(uint32_t), NULL);
  fileSize = ntohl(fileSize);
  
  if (myTcpReadChunksAndWriteToFile(sockfd, fileName, fileSize, NULL) == false) {
    myWarning("Cannot write the file", "clientTask");
    return true; // prossimo file
  }
    
  return true;
}