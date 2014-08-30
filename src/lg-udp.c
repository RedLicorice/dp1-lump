#include "lunp.h"

SOCKET myUdpClientStartup(const char *serverAddress, const char *serverPort, struct sockaddr_in *serverStruct) {
  SOCKET sockfd;
  
  sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  
  //addrlen = sizeof(struct sockaddr_in);
  //serverStruct->sin_len = addrlen;
  serverStruct->sin_family = AF_INET; // IPv4
  serverStruct->sin_port = htons(atoi(serverPort));
  if (inet_aton(serverAddress, &(serverStruct->sin_addr)) == 0)
    myFunctionError("inet_aton", NULL, "myUdpClientStartup");
  
  return sockfd;
}

SOCKET myUdpServerStartup(const char *serverPort) {
  SOCKET sockfd;
  struct sockaddr_in saddr;
  
  sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(serverPort));
  saddr.sin_addr.s_addr = INADDR_ANY;
  
  Bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in));
  
  return sockfd;
}

bool myUdpReadBytes(SOCKET sockfd, void *buffer, int byteCount, struct sockaddr_in *sourceStruct, int *readByteCount) {
  ssize_t readByteCountTmp;
  socklen_t addrlen;
  
  if (buffer == NULL)
    buffer = (void*)malloc(sizeof(void) * byteCount);
  
  if (sourceStruct == NULL)
    readByteCountTmp = recvfrom(sockfd, buffer, byteCount, 0, NULL, NULL);
  else { // sourceStruct != NULL
    addrlen = sizeof(struct sockaddr_in);
    readByteCountTmp = recvfrom(sockfd, buffer, byteCount, 0, (struct sockaddr*)sourceStruct, &addrlen);
  }
  
  if (readByteCount != NULL)
    *readByteCount = (int)readByteCountTmp;
  
  if (readByteCountTmp < byteCount)
    return false;
  else
    return true;
}

void myUdpWriteBytes(SOCKET sockfd, void *data, int byteCount, struct sockaddr_in destStruct) {
  Sendto(sockfd, data, (size_t)byteCount, MSG_NOSIGNAL, (struct sockaddr*)&destStruct, sizeof(struct sockaddr_in));
}

bool myUdpReadBytesTimeout(SOCKET sockfd, void *buffer, int byteCount, int maxSeconds, struct sockaddr_in *sourceStruct, int *readByteCount) {
  fd_set cset;
  struct timeval timeout;
  int numberOfSockets;
  
  FD_ZERO(&cset);
  FD_SET(sockfd, &cset);
  timeout.tv_sec = maxSeconds;
  timeout.tv_usec = 0;

  numberOfSockets = select(FD_SETSIZE, &cset, NULL, NULL, &timeout);
  
  if (numberOfSockets == 0)
    return false; // expired timeout
    
  if (numberOfSockets == -1)
    mySystemError("select", "myUdpReadBytesTimeout");
  
  myUdpReadBytes(sockfd, buffer, byteCount, sourceStruct, readByteCount);
  return true;
}