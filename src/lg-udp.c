#include "lunp.h"

typedef struct client_s {
  struct sockaddr_in clientStruct;
  int numberOfDatagrams;
} client_t;

static int searchForClient(struct sockaddr_in clientStruct, client_t *clientList, int clientListIndex);
static bool areSockaddrinEqual(struct sockaddr_in address1, struct sockaddr_in address2);

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

bool myUdpReadBytes(SOCKET sockfd, void *buffer, int maxByteCount, struct sockaddr_in *sourceStruct, int *readByteCount) {
  ssize_t readByteCountTmp;
  socklen_t addrlen;
  
  if (buffer == NULL)
    buffer = (void*)malloc(sizeof(void) * maxByteCount);
  
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

bool myUdpLimitClients(struct sockaddr_in clientStruct, int maxDatagrams, int maxClients) {
  static client_t *clientList;
  static int clientListIndex = -1;
  
  int foundIndex, i;
  
  if (clientListIndex == -1) // first time
    clientList = (client_t*)malloc(sizeof(client_t) * maxClients);
  
  else {
    foundIndex = searchForClient(clientStruct, clientList, clientListIndex);
    
    if (foundIndex > -1) { // found
      if (clientList[foundIndex].numberOfDatagrams == maxDatagrams)
	return false;
      
      else {
	clientList[foundIndex].numberOfDatagrams++;
	return true;
      }
    }
  }
  
  if (clientListIndex == maxClients - 1)
    for (i = 0; i < clientListIndex; ++i)
      clientList[i] = clientList[i + 1];
  else
    clientListIndex++;
    
  clientList[clientListIndex].clientStruct = clientStruct;
  clientList[clientListIndex].numberOfDatagrams = 1;
  
  return true;
}

static int searchForClient(struct sockaddr_in clientStruct, client_t *clientList, int clientListIndex) {
  int i;
  for (i = 0; i <= clientListIndex; ++i)
    if (areSockaddrinEqual(clientStruct, clientList[i].clientStruct) == true)
      return i;
  return -1;
}

static bool areSockaddrinEqual(struct sockaddr_in struct1, struct sockaddr_in struct2) {
  if (struct1.sin_family != struct2.sin_family)
    return false;
  if (struct1.sin_port != struct2.sin_port)
    return false;
  if (struct1.sin_addr.s_addr != struct2.sin_addr.s_addr)
    return false;
  return true; // equal
}