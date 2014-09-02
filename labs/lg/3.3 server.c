#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port
#define CHILD_COUNT_ARG argv[2] // number of children

#define MAX_CHILD_COUNT 10
#define MAX_SECOND_COUNT 10

void childTask(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int childCount;
  
  childCount = atoi(CHILD_COUNT_ARG);
  if (childCount > MAX_CHILD_COUNT)
    myError("This server supports a maximum number of children equal to %d", NULL, MAX_CHILD_COUNT);
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerPreforked(sockfd, childCount, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  char clientReq[BUFFSIZE], nomeFile[MAXFILENAMELENGTH];
  uint32_t fileSize;
  
  struct timespec begin, end;
  int numberOfElapsedSeconds;
  
  clock_gettime(CLOCK_REALTIME, &begin);
  
  printf("\n");
  myWarning("Connection accepted", "childTask");
  
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
	  
    clock_gettime(CLOCK_REALTIME, &end);
    numberOfElapsedSeconds = (int)(end.tv_sec - begin.tv_sec);
    if (numberOfElapsedSeconds > MAX_SECOND_COUNT) {
      myWarning("Expired time", "childTask");
      return;
    }
    
  }
}