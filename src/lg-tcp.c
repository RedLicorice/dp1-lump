#include "lunp.h"

SOCKET myTcpClientStartup(const char *serverAddress, const char *serverPort) {
  return (SOCKET)Tcp_connect(serverAddress, serverPort); // tcp_connect.c
}

SOCKET myTcpServerStartup(const char *serverPort) {
  SOCKET sockfd;
  struct sockaddr_in saddr;
  
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1)
    err_sys("Error in socket(): %s\n", strerror(errno));
  
  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(serverPort));
  saddr.sin_addr.s_addr = INADDR_ANY;
  
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1)
    err_sys("Error in bind(): %s\n", strerror(errno));
  
  return sockfd;
}

bool myTcpReadBytes(SOCKET sockfd, void *buffer, int byteCount, int *readByteCount) {
  ssize_t readByteCountTmp;
  
  if (buffer == NULL)
    buffer = (void*)malloc(sizeof(void) * byteCount);
  
  readByteCountTmp = Readn((int)sockfd, buffer, (size_t)byteCount); // readn.c
  
  if (readByteCount != NULL)
    *readByteCount = (int)readByteCountTmp;
  
  if (readByteCountTmp < byteCount)
    return false;
  else
    return true;
}

void myTcpWriteBytes(SOCKET sockfd, void *data, int byteCount) {
  Writen((int)sockfd, data, (size_t)byteCount); // writen.c
}

bool myTcpReadString(SOCKET sockfd, char *buffer, int charCount, int *readCharCount) {
  bool reply;
  
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (charCount + 1));
  
  reply = myTcpReadBytes(sockfd, (void*)buffer, charCount, readCharCount);
  
  if (charCount == 0 || buffer[charCount - 1] != '\0')
    buffer[charCount] = '\0';
  
  return reply;
}

void myTcpWriteString(SOCKET sockfd, char *string) {
  Writen((int)sockfd, (void*)string, strlen(string));
}

ssize_t myTcpReadLine(SOCKET sockfd, char *buffer, int maxLength) {
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * maxLength);
  //TODO non bufferizzata
}

ssize_t myTcpBufferedReadLine(SOCKET sockfd, char *buffer, int maxLength) {
  if (buffer == NULL)
    buffer = (char*)malloc(sizeof(char) * (maxLength + 1));
  return Readline((int)sockfd, (void*)buffer, (size_t)(maxLength + 1)) - 1; // readline.c
}