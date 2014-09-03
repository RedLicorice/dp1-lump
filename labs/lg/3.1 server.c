#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

#define MAX_CHILD_COUNT 3

void childTask(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerOCPCMax(sockfd, MAX_CHILD_COUNT, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  char clientReq[BUFFSIZE], nomeFile[MAXFILENAMELENGTH];
  uint32_t fileSize;
  
  while (1) {
    
    if (myTcpReadLine(sockfd, clientReq, BUFFSIZE, NULL) == false)
      return;
    
    if (strcmp(clientReq, QUIT) == 0) {
      myWarning("Connection closed (QUIT)", "childTask");
      return;
    }
    
    // else: clientReq = GET...
    
    strcpy(nomeFile, clientReq + strlen(GET));
    nomeFile[strlen(nomeFile) - strlen("\r\n")] = '\0';
    
    if (fileExists(nomeFile) == false) {
      myWarning("The file does not exist", "childTask");
      myTcpWriteString(sockfd, ERR);
      
    } else { // The file exists
      myWarning("Sending the file to the client...", "childTask");
      myTcpWriteString(sockfd, OK);
      
      fileSize = htonl(getFileSize(nomeFile));
      myTcpWriteBytes(sockfd, (void*)(&fileSize), sizeof(uint32_t));
      
      myTcpReadFromFileAndWriteChunks(sockfd, nomeFile);
      myWarning("File sent successfully to the client...", "childTask");
    }
    
  }
}