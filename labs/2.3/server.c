/*
 * argv[1] = porta
 */

#include "../lunp.h"

void childTask(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(argv[1]);
  
  myTcpServerSimple(sockfd, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  const int clientReqLen = strlen(GET) + MAXFILENAMELENGTH + strlen("\r\n");
  char clientReq[clientReqLen];
  uint32_t fileSize;
  char nomeFile[MAXFILENAMELENGTH];
  
  printf("\n");
  myWarning("Connection accepted", "childTask");
  
  while (1) {
    
    if (myTcpReadLine(sockfd, clientReq, clientReqLen, NULL) == false)
      return;
    
    if (strcmp(clientReq, QUIT) == 0) {
      myWarning("Connection closed (QUIT)", "childTask");
      return;
    }
    
    // else: clientReq = GET...
    
    strcpy(nomeFile, clientReq + 3);
    nomeFile[strlen(nomeFile) - 2] = '\0';
    
    if (fileExists(nomeFile) == false) {
      myWarning("The file does not exist", "childTask");
      myTcpWriteString(sockfd, ERR);
      
    } else { // The file exists
      myWarning("Sending the file to the client...", "childTask");
      myTcpWriteString(sockfd, OK);
      
      fileSize = htonl(getFileSize(nomeFile));
      myTcpWriteBytes(sockfd, (void*)(&fileSize), 4);
      
      myTcpReadFromFileAndWriteChunks(sockfd, nomeFile);
      myWarning("File sent successfully to the client...", "childTask");
    }
    
  }
}