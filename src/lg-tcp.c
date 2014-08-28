#include "lunp.h"

bool myTcpReadChunksAndWriteToFileCallback(void *chunk, int chunkSize, void *param);
bool myTcpReadFromFileAndWriteChunksCallback(void *chunk, int *chunkSize, void *param);

SOCKET myTcpClientStartup(const char *serverAddress, const char *serverPort) {
  return Tcp_connect(serverAddress, serverPort); // tcp_connect.c
}

bool myTcpReadBytes(SOCKET sockfd, void *buffer, int byteCount, int *readByteCount) {
  ssize_t readByteCountTmp;
  
  if (buffer == NULL)
    buffer = (void*)malloc(sizeof(void) * byteCount);
  
  readByteCountTmp = Readn(sockfd, buffer, (size_t)byteCount); // readn.c
  
  if (readByteCount != NULL)
    *readByteCount = (int)readByteCountTmp;
  
  if (readByteCountTmp < byteCount)
    return false;
  else
    return true;
}

void myTcpWriteBytes(SOCKET sockfd, void *data, int byteCount) {
  Writen(sockfd, data, (size_t)byteCount); // writen.c
}

bool myTcpReadString(SOCKET sockfd, char *buffer, int charCount, int *readCharCount) {
  bool reply;
  
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (charCount + 1));
  
  reply = myTcpReadBytes(sockfd, (void*)buffer, charCount, readCharCount);
  
  if (*readCharCount > 0 && buffer[*readCharCount - 1] == '\0') {
    do {
      (*readCharCount)--;
    } while (*readCharCount > 0 && buffer[*readCharCount - 1] == '\0');
  } else
    buffer[*readCharCount] = '\0';
  
  return reply;
}

void myTcpWriteString(SOCKET sockfd, char *string) {
  Writen(sockfd, (void*)string, strlen(string));
}

bool myTcpReadLine(SOCKET sockfd, char *buffer, int maxLength, int *readCharCount) {
  char *ptr;
  int readCharCount_local;
  
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (maxLength + 1));
  
  ptr = buffer;
  readCharCount_local = 0;
  bool found = false;
  
  while ((found == false) && (readCharCount_local < maxLength - 1) && (myTcpReadBytes(sockfd, (void*)ptr, 1, NULL) == true)) {
    if (*ptr == '\n')
      found = true;
    ptr++;
    readCharCount_local++;
  }
  
  if (readCharCount_local > 0 && buffer[readCharCount_local - 1] == '\0') {
    do {
      readCharCount_local--;
    } while (readCharCount_local > 0 && buffer[readCharCount_local - 1] == '\0');
  } else
    buffer[readCharCount_local] = '\0';
    
  if (readCharCount != NULL)
    *readCharCount = readCharCount_local;
  
  return found;
}

int myTcpBufferedReadLine(SOCKET sockfd, char *buffer, int maxLength) {
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (maxLength + 1));
  return (int)Readline(sockfd, (void*)buffer, (size_t)(maxLength + 1)) - 1; // readline.c
}

bool myTcpReadChunks(SOCKET sockfd, int byteCount, int *readByteCount, myTcpReadChunksCallback callback, void *callbackParam) {
  int leftBytes, numberOfReadBytes;
  void *buffer;
  bool readReply;
  
  buffer = (void*)malloc(sizeof(void) * DEFAULT_CHUNK_SIZE);
  
  leftBytes = byteCount;
  if (readByteCount != NULL)
    *readByteCount = 0;
  readReply = true;
  
  while (leftBytes > 0) {
    
    if (readReply == false) {
      free(buffer);
      return myWarning("Function myTcpReadBytes() returned false (end-of-file) before reading all data", "myTcpReadChunks");
    }
    
    if (leftBytes < DEFAULT_CHUNK_SIZE)
      readReply = myTcpReadBytes(sockfd, buffer, leftBytes, &numberOfReadBytes);
    else
      readReply = myTcpReadBytes(sockfd, buffer, DEFAULT_CHUNK_SIZE, &numberOfReadBytes);
    
    if (readReply == false && numberOfReadBytes == 0) {
      free(buffer);
      return false;
    }
    
    if (readByteCount != NULL)
      *readByteCount = *readByteCount + numberOfReadBytes;

    if (callback != NULL) {
      if (callback(buffer, numberOfReadBytes, callbackParam) == false) {
	free(buffer);
	return false;
      }
    }
    
    leftBytes -= numberOfReadBytes;
  }
  
  free(buffer);
  return true;
}

bool myTcpReadChunksAndWriteToFile(SOCKET sockfd, const char *filePath, int fileSize, int *readByteCount) {
  FILE *fd;
  bool reply;
  
  fd = fopen(filePath, "w");
  if (fd == NULL)
    mySystemError("fopen", "myTcpReadChunksAndWriteToFile");
  
  reply = myTcpReadChunks(sockfd, fileSize, readByteCount, &myTcpReadChunksAndWriteToFileCallback, (void*)fd);
  
  if (fclose(fd) == EOF)
    mySystemError("fclose", "myTcpReadChunksAndWriteToFile");
  return reply;
}

bool myTcpReadChunksAndWriteToFileCallback(void *chunk, int chunkSize, void *param) {
  if (fwrite(chunk, 1, chunkSize, (FILE*)param) != chunkSize)
    return myFunctionWarning("fwrite", "myTcpReadChunksAndWriteToFileCallback", NULL);
  return true;
}

int myTcpWriteChunks(SOCKET sockfd, myTcpWriteChunksCallback callback, void *callbackParam) {
  int numberOfWrittenBytes, chunkSize;
  void *buffer;
  
  buffer = (void*)malloc(sizeof(void) * DEFAULT_CHUNK_SIZE);
  
  while (callback(buffer, &chunkSize, callbackParam) == true) {
    myTcpWriteBytes(sockfd, buffer, chunkSize);
    numberOfWrittenBytes += chunkSize;
  }
  
  free(buffer);
  return numberOfWrittenBytes;
}

int myTcpReadFromFileAndWriteChunks(SOCKET sockfd, const char *filePath) {
  FILE *fd;
  int numberOfWrittenBytes;
  
  fd = fopen(filePath, "r");
  if (fd == NULL)
    mySystemError("fopen", "myTcpReadFromFileAndWriteChunks");
  
  numberOfWrittenBytes = myTcpWriteChunks(sockfd, myTcpReadFromFileAndWriteChunksCallback, (void*)fd);
  
  if (fclose(fd) == EOF)
    mySystemError("fclose", "myTcpReadFromFileAndWriteChunks");
  
  return numberOfWrittenBytes;
}

bool myTcpReadFromFileAndWriteChunksCallback(void *chunk, int *chunkSize, void *param) {
  *chunkSize = 1;
  if (fread(chunk, 1, 1, (FILE*)param) > 0)
    return true;
  else
    return false;
}