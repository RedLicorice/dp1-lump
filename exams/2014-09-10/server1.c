#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

void childTask(SOCKET sockfd);

bool myTcpReadChunks2(SOCKET sockfd, int *readByteCount, myTcpReadChunksCallback callback, void *callbackParam);
bool myTcpReadChunksAndWriteToFile2(SOCKET sockfd, const char *filePath, int *readByteCount);
static bool myTcpReadChunksAndWriteToFileCallback2(void *chunk, int chunkSize, void *param);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerSimple(sockfd, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  char clientReq[BUFFSIZE], fileName[MAXFILENAMELENGTH];
  //uint32_t fileSize;
  bool valid;
  
//  while (1) {
    
    valid = true;
    
    if (myTcpReadLine(sockfd, clientReq, BUFFSIZE, NULL) == false)
      valid = false;
      
      
    if (valid == true) {
      
            if (strlen(clientReq) < strlen(PUT) + strlen("\r\n"))
                valid = false;
                
            else {
                
              if (strncmp(clientReq, PUT, strlen(PUT)) != 0)
                valid = false;
                
              else {
                valid = true;
                strcpy(fileName, clientReq + strlen(PUT));
                fileName[strlen(fileName) - strlen("\r\n")] = '\0';
              }
            }
    }
    
    if (valid == false) {
      myWarning("Wrong format", "childTask");
      myTcpWriteString(sockfd, ERR);
      
    } else { // The file exists
      myWarning("Receiving the file from the client...", "childTask");
      myTcpWriteString(sockfd, OK);
	
  if (myTcpReadChunksAndWriteToFile2(sockfd, fileName, NULL) == false)
    myError("Cannot write the file", "clientTask");
	
      myWarning("File received successfully from the client", "childTask");
    }
    
//  }
}



bool myTcpReadChunks2(SOCKET sockfd, int *readByteCount, myTcpReadChunksCallback callback, void *callbackParam) {
  int numberOfReadBytes, chunkSize;
  void *buffer;
  bool readReply;
  
  buffer = (void*)malloc(sizeof(void) * DEFAULT_CHUNK_SIZE);
  
  if (readByteCount != NULL)
    *readByteCount = 0;
  readReply = true;
  
  while (1) {

      chunkSize = DEFAULT_CHUNK_SIZE;
    
    readReply = myTcpReadBytes(sockfd, buffer, chunkSize, &numberOfReadBytes);
    
    if (readByteCount != NULL)
      *readByteCount = *readByteCount + numberOfReadBytes;

    if (callback != NULL) {
      if (callback(buffer, numberOfReadBytes, callbackParam) == false) {
	free(buffer);
	return false;
      }
    }
    
    if (readReply == false) {
      free(buffer);
      return true;
    }
    
  }
}

bool myTcpReadChunksAndWriteToFile2(SOCKET sockfd, const char *filePath, int *readByteCount) {
  FILE *fd;
  bool reply;
  
  fd = fopen(filePath, "w");
  if (fd == NULL)
    mySystemError("fopen", "myTcpReadChunksAndWriteToFile");
  
  reply = myTcpReadChunks2(sockfd, readByteCount, &myTcpReadChunksAndWriteToFileCallback2, (void*)fd);
  
  if (fclose(fd) == EOF)
    mySystemError("fclose", "myTcpReadChunksAndWriteToFile");
  return reply;
}

static bool myTcpReadChunksAndWriteToFileCallback2(void *chunk, int chunkSize, void *param) {
  if (fwrite(chunk, 1, chunkSize, (FILE*)param) != chunkSize)
    return myFunctionWarning("fwrite", "myTcpReadChunksAndWriteToFileCallback", NULL);
  return true;
}
