#include "lunp.h"

SOCKET myUdpClientStartup(const char *serverAddress, const char *serverPort, struct sockaddr_in *serverStruct) {
  SOCKET sockfd;
  
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd == -1)
    mySystemError("socket", "myUdpClientStartup");
  
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
  
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd == -1)
    mySystemError("socket", "myUdpServerStartup");

  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(serverPort));
  saddr.sin_addr.s_addr = INADDR_ANY;
  
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1)
    mySystemError("bind", "myUdpServerStartup");
  
  return sockfd;
}