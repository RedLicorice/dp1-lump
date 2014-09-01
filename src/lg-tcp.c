#include "lunp.h"

bool myTcpReadChunksAndWriteToFileCallback(void *chunk, int chunkSize, void *param);
bool myTcpReadFromFileAndWriteChunksCallback(void *chunk, int *chunkSize, void *param);

ssize_t readnOnce(int fd, void *vptr, size_t n);
ssize_t ReadnOnce(int fd, void *ptr, size_t nbytes);

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
  
  /*if (*readCharCount > 0 && buffer[*readCharCount - 1] == '\0') {
    do {
      (*readCharCount)--;
    } while (*readCharCount > 0 && buffer[*readCharCount - 1] == '\0');
  } else*/
    buffer[*readCharCount] = '\0';
  
  return reply;
}

void myTcpWriteString(SOCKET sockfd, char *string) {
  Writen(sockfd, (void*)string, strlen(string));
}

bool myTcpReadLine(SOCKET sockfd, char *buffer, int maxLength, int *readCharCount) {
  char *ptr;
  int readCharCount_local;
  bool found;
  
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (maxLength + 1));
  
  ptr = buffer;
  readCharCount_local = 0;
  found = false;
  
  while ((found == false) && (readCharCount_local < maxLength - 1) && (myTcpReadBytes(sockfd, (void*)ptr, 1, NULL) == true)) {
    if (*ptr == '\n')
      found = true;
    ptr++;
    readCharCount_local++;
  }
  
  /*if (readCharCount_local > 0 && buffer[readCharCount_local - 1] == '\0') {
    do {
      readCharCount_local--;
    } while (readCharCount_local > 0 && buffer[readCharCount_local - 1] == '\0');
  } else*/
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
  int leftBytes, numberOfReadBytes, chunkSize;
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
      chunkSize = leftBytes;
    else
      chunkSize = DEFAULT_CHUNK_SIZE;
    
    readReply = myTcpReadBytes(sockfd, buffer, chunkSize, &numberOfReadBytes);
    
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

bool myTcpReadBytesOnce(SOCKET sockfd, void *buffer, int maxByteCount, int *readByteCount) {
  ssize_t readByteCountTmp;
  
  if (buffer == NULL)
    buffer = (void*)malloc(sizeof(void) * maxByteCount);
  
  readByteCountTmp = ReadnOnce(sockfd, buffer, (size_t)maxByteCount); // readn.c
  
  if (readByteCount != NULL)
    *readByteCount = (int)readByteCountTmp;
  
  if (readByteCountTmp == 0)
    return false;
  else // readByteCountTmp == -2 || readByteCountTmp > 0
    return true;
}

bool myTcpReadBytesAsync(SOCKET sockfd, void *buffer, int byteCount, int *readByteCount) {
  int leftBytes, readByteCountTmp;
  bool reply;
  
  if (readByteCount == NULL)
    myError("Parameter \"readByteCount\" can not be NULL", "myTcpReadBytesAsync");
  
  leftBytes = byteCount - *readByteCount;
  
  reply = myTcpReadBytesOnce(sockfd, buffer + *readByteCount, leftBytes, &readByteCountTmp);
  
  *readByteCount += readByteCountTmp;
  
  if (reply == false || *readByteCount == byteCount)
    return true; // operation completed
  else
    return false;
}

bool myTcpReadStringAsync(SOCKET sockfd, char *buffer, int charCount, int *readCharCount) {
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (charCount + 1));
  
  if (myTcpReadBytesAsync(sockfd, (void*)buffer, charCount, readCharCount) == true) {
    buffer[*readCharCount] = '\0';
    return true;
  }
  
  return false;
}

bool myTcpReadLineAsync(SOCKET sockfd, char *buffer, int maxLength, int *readCharCount) {
  bool completed;
  int readCharCountTmp;
  
  if (readCharCount == NULL)
    myError("Parameter \"readCharCount\" can not be NULL", "myTcpReadLineAsync");
  
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (maxLength + 1));
  
  if (myTcpReadBytes(sockfd, buffer + *readCharCount, 1, &readCharCountTmp) == false) {
    completed = true; // end-of-file
    myWarning("Function myTcpReadBytes() returned false (end-of-file) before reading all data", "myTcpReadLineAsync");
  
  } else if (buffer[*readCharCount] == '\n')
    completed = true; // reached '\n' character
  
  else if (*readCharCount == maxLength - 1)
    completed = true; // reached maxLength
  
  else
    completed = false;
  
  *readCharCount += readCharCountTmp;
  
  if (completed == true) {
    buffer[*readCharCount] = '\0';
    return true;
  }
  
  return false;
}

bool myTcpReadChunksAsync(SOCKET sockfd, int byteCount, int *readByteCount, myTcpReadChunksCallback callback, void *callbackParam) {
  int chunkSize, readByteCountTmp;
  void *buffer;
  bool readReply;
  
  if (readByteCount == NULL)
    myError("Parameter \"readCharCount\" can not be NULL", "myTcpReadChunksAsync");
  
  chunkSize = byteCount - *readByteCount;
  if (chunkSize > DEFAULT_CHUNK_SIZE)
    chunkSize = DEFAULT_CHUNK_SIZE;
  buffer = (void*)malloc(sizeof(void) * chunkSize);
  
  readByteCountTmp = 0;
  readReply = myTcpReadBytesAsync(sockfd, buffer, chunkSize, &readByteCountTmp);
  
  if (readByteCountTmp == 0) {
    free(buffer);
    return readReply;
  }
  
  // else: readByteCountTmp > 0
  *readByteCount += readByteCountTmp;
  
  if (callback != NULL) {
    if (callback(buffer, readByteCountTmp, callbackParam) == false) {
      free(buffer);
      return true;
    }
  }
    
  free(buffer);
  
  if (readReply == true && readByteCountTmp < chunkSize)
    return true; // end-of-file
    
  if (*readByteCount == byteCount)
    return true;
    
  return false;
}

bool myTcpReadChunksAndWriteToFileAsync(SOCKET sockfd, const char *filePath, int fileSize, int *readByteCount) {
  static FILE *fd;
  static bool open = false;
  bool reply;
  
  if (open == false) {
    fd = fopen(filePath, "w");
    if (fd == NULL)
      mySystemError("fopen", "myTcpReadChunksAndWriteToFile");
    open = true;
  }
  
  reply = myTcpReadChunksAsync(sockfd, fileSize, readByteCount, &myTcpReadChunksAndWriteToFileCallback, (void*)fd);
  
  if (reply == true) {
    if (fclose(fd) == EOF)
      mySystemError("fclose", "myTcpReadChunksAndWriteToFile");
    open = false;
  }
  
  return reply;
}

ssize_t readnOnce(int fd, void *vptr, size_t n) {
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	//while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				//nread = 0;		/* and call read() again */
				return -2;
			else
				return(-1);
		} //else if (nread == 0)
			//break;				/* EOF */

		nleft -= nread;
		//ptr   += nread;
	//}
	return(n - nleft);		/* return >= 0 */
}

ssize_t ReadnOnce(int fd, void *ptr, size_t nbytes) {
	ssize_t		n;

	if ( (n = readn(fd, ptr, nbytes)) == -1)
		err_sys("readnOnce error");
	return(n);
}