#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port
#define FILE_NAME_ARG argv[3] // file name

void clientTask(SOCKET sockfd, char *fileName);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char fileName[MAXFILENAMELENGTH];
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
    
    clientTask(sockfd, FILE_NAME_ARG);
  
  Close(sockfd);
  return 0;
}

void clientTask(SOCKET sockfd, char *fileName) {
  char serverRes[BUFFSIZE];
  //uint32_t fileSize;
  
    if (fileExists(fileName) == false) {
      myWarning("The file does not exist", "clientTask");
      return;
    }
  
  myTcpWriteString(sockfd, PUT);
  myTcpWriteString(sockfd, fileName);
  myTcpWriteString(sockfd, "\r\n");
  
  myTcpReadLine(sockfd, serverRes, BUFFSIZE, NULL);
  
  if (strcmp(serverRes, ERR) == 0) {
    myWarning("Il server ha risposto ERR", "clientTask");
    return; // next file
  }
    
  // else: serverRes = OK
  myWarning("Sending the file to the server...", "clientTask");

      //fileSize = htonl(getFileSize(fileName));
      //myTcpWriteBytes(sockfd, (void*)(&fileSize), sizeof(uint32_t));
      
      if (myTcpReadFromFileAndWriteChunks(sockfd, fileName, NULL) == false) {
	myWarning("The server closed the connection abruptly", "clientTask");
	return;
      }
	
      myWarning("File sent successfully to the server", "clientTask");

    
  // else: serverRes = OK
//  myTcpReadBytes(sockfd, (void*)(&fileSize), sizeof(uint32_t), NULL);
//  fileSize = ntohl(fileSize);
  
//  if (myTcpReadChunksAndWriteToFile(sockfd, fileName, fileSize, NULL) == false)
//    myError("Cannot write the file", "clientTask");
}
