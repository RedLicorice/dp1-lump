#include "lunp.h"

/*SOCKET myUdpClientStartup(const char *serverAddress, const char *serverPort, struct sockaddr_in *serverStruct) {
  SOCKET sockfd;
  
  sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  
  //addrlen = sizeof(struct sockaddr_in);
  //serverStruct->sin_len = addrlen;
  serverStruct->sin_family = AF_INET; // IPv4
  serverStruct->sin_port = htons(atoi(serverPort));
  if (inet_aton(serverAddress, &(serverStruct->sin_addr)) == 0)
    myFunctionError("inet_aton", NULL, "myUdpClientStartup");
  
  return sockfd;
}*/

SOCKET myUdpClientStartup(const char *serverAddress, const char *serverPort, struct sockaddr_in **serverStruct) {
  socklen_t junk;
  return Udp_client(serverAddress, serverPort, (SA**)serverStruct, &junk);
}

bool myUdpReadBytes(SOCKET sockfd, void *buffer, int maxByteCount, struct sockaddr_in *sourceStruct, int *readByteCount) {
  ssize_t readByteCountTmp;
  socklen_t addrlen;
  
  if (sourceStruct == NULL)
    readByteCountTmp = recvfrom(sockfd, buffer, maxByteCount, 0, NULL, NULL);
  else { // sourceStruct != NULL
    addrlen = sizeof(struct sockaddr_in);
    readByteCountTmp = recvfrom(sockfd, buffer, maxByteCount, 0, (struct sockaddr*)sourceStruct, &addrlen);
  }
  
  if (readByteCount != NULL)
    *readByteCount = (int)readByteCountTmp;
  
  if (readByteCountTmp < maxByteCount)
    return false;
  else
    return true;
}

void myUdpWriteBytes(SOCKET sockfd, void *data, int byteCount, struct sockaddr_in destStruct) {
  Sendto(sockfd, data, (size_t)byteCount, MSG_NOSIGNAL, (struct sockaddr*)&destStruct, sizeof(struct sockaddr_in));
}

bool myUdpReadString(SOCKET sockfd, char *buffer, int maxCharCount, struct sockaddr_in *sourceStruct, int *readCharCount) {
  int readCharCountTmp;
  bool reply;
  
  reply = myUdpReadBytes(sockfd, (void*)buffer, maxCharCount - 1, sourceStruct, &readCharCountTmp);
  
  buffer[readCharCountTmp] = '\0';
  
  if (readCharCount != NULL)
    *readCharCount = readCharCountTmp;
  
  return reply;
}

void myUdpWriteString(SOCKET sockfd, char *string, struct sockaddr_in destStruct) {
  myUdpWriteBytes(sockfd, (void*)string, strlen(string), destStruct);
}