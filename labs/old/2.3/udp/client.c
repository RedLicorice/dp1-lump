/*
 * argv[1] = server address
 * argv[2] = server port number
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "common.h"

int fileTransfer(int sockfd, char *fileName, struct sockaddr_in daddr);

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in daddr;
  char nomeFile[N];
  socklen_t addrlen;
  
  //char reqMessage[strlen(QUIT)];
  char reqMessage[6];

  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  addrlen = sizeof(struct sockaddr_in);
  //daddr.sin_len = addrlen;
  daddr.sin_family = AF_INET; // IPv4
  daddr.sin_port = htons(atoi(argv[2]));
  if (inet_aton(argv[1], &(daddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton()\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }

  printf("Please type the filename (maximum 64 characters, empty string to close): ");
  gets(nomeFile);
  while (strcmp(nomeFile, "") != 0) {
    
    if (fileTransfer(sockfd, nomeFile, daddr) == 1)
      return myClose(1, sockfd);
    
    printf("\nPlease type the filename (maximum 64 characters, empty string to close): ");
    gets(nomeFile);
  }
    
  strcpy(reqMessage, QUIT);
  
  if (sendto(sockfd, (void*)reqMessage, strlen(reqMessage), MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
    printf("Errore nella sendto(): %s\n", strerror(errno));
    return myClose(1, sockfd);
  }
    
  return myClose(0, sockfd);
}

int fileTransfer(int sockfd, char *fileName, struct sockaddr_in daddr) {
  //char reqMessage[strlen(GET) + N + strlen("\r\n")];
  //char resMessage[max(strlen(ERR), strlen(OK))];
  char reqMessage[N + 5], resMessage[6], *bufferReply;
  
  int numberOfBytes;
  ssize_t numberOfReadBytes;
  socklen_t addrlen;
  FILE *fd;
  
  addrlen = sizeof(struct sockaddr_in);
  
  // REQUEST
  strcpy(reqMessage, "GET");
  strcat(reqMessage, fileName);
  strcat(reqMessage, "\r\n");
  
  if (sendto(sockfd, (void*)reqMessage, strlen(reqMessage), MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
    printf("Errore nella sendto(): %s\n", strerror(errno));
    return 1;
  }
  printf("Request sent to the server\n");
  
  // RESPONSE
  numberOfReadBytes = recvfrom(sockfd, (void*)resMessage, 6, 0, NULL, NULL);
  if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
    if (numberOfReadBytes == -1)
      printf("Errore nella recvfrom(): %s\n", strerror(errno));
    else // numberOfReadBytes == 0
      printf("Errore nella recvfrom(): the peer has performed an orderly shutdown\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (strcmp(resMessage, ERR) == 0) {
    printf("The server replied: -ERR\n");
    return 0;
  }
  
  // else: resMessage = OK
  printf("The server replied: +OK\n");
  
  numberOfReadBytes = recvfrom(sockfd, (void*)(&numberOfBytes), 4, 0, NULL, NULL);
  if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
    if (numberOfReadBytes == -1)
      printf("Errore nella recvfrom(): %s\n", strerror(errno));
    else // numberOfReadBytes == 0
      printf("Errore nella recvfrom(): the peer has performed an orderly shutdown\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  numberOfBytes = ntohl(numberOfBytes);
  printf("The file size is: %d\n", numberOfBytes);
  
  bufferReply = (char*)malloc(sizeof(char) * numberOfBytes);
  
  printf("Receiving the file from the server...\n");
  numberOfReadBytes = recvfrom(sockfd, (void*)(bufferReply), numberOfBytes, 0, NULL, NULL);
  if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
    if (numberOfReadBytes == -1)
      printf("Errore nella recvfrom(): %s\n", strerror(errno));
    else // numberOfReadBytes == 0
      printf("Errore nella recvfrom(): the peer has performed an orderly shutdown\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  fd = fopen(fileName, "w");
  fwrite((void*)bufferReply, 1, numberOfBytes, fd);
  
  free(bufferReply);
  fclose(fd);
  
  return 0;
}