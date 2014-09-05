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
#include <sys/stat.h>
#include "common.h"

int serveClient(int sockfd);

int main(int argc, char *argv[]) {
  socklen_t addrlen;
  int sockfd;
  struct sockaddr_in saddr;
  
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }

  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(argv[1]));
  saddr.sin_addr.s_addr = INADDR_ANY;
  
  addrlen = sizeof(struct sockaddr_in);
  if (bind(sockfd, (struct sockaddr*)&saddr, addrlen) == -1) {
    printf("Errore nella bind(): %s\n", strerror(errno));
    return myClose(1, sockfd);
  }

  while (1) {
    printf("\nServer on listening to port %d...\n", atoi(argv[1]));
    if (serveClient(sockfd) == 1)
      return myClose(1, sockfd);
  }
}

int serveClient(int sockfd) {
  ssize_t numberOfReadBytes;
  socklen_t addrlen;
  struct sockaddr_in daddr;
  FILE *fd;
  uint32_t fileSize, fileSizeConverted;
  struct stat st;
  
  //char resMessage[strlen(GET) + N + strlen("\r\n")];
  //char reqMessage[max(strlen(ERR), strlen(OK))];
  char resMessage[N + 5], reqMessage[6], fileName[N], *ptr, *bufferReply;
  
  addrlen = sizeof(struct sockaddr_in);
  
  numberOfReadBytes = recvfrom(sockfd, (void*)resMessage, N + 5, 0, (struct sockaddr*)&daddr, &addrlen);
  if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
    if (numberOfReadBytes == -1)
      printf("Errore nella recvfrom(): %s\n", strerror(errno));
    else // numberOfReadBytes == 0
      printf("Errore nella recvfrom(): the peer has performed an orderly shutdown\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (strcmp(resMessage, QUIT) == 0) {
    printf("The client closed\n");
    return 0;
  }
  
  // else: resMessage = GET<filename>\r\n
  
  //strcpy(fileName, resMessage + strlen(GET));
  strcpy(fileName, resMessage + 3);
  ptr = fileName;
  while (*ptr != '\r')
    ptr++;
  *ptr = '\0';
  
  printf("The client asked for the following file: %s\n", fileName);
  
  if (access(fileName, R_OK) == -1) { // The file does not exist
    strcpy(reqMessage, ERR);
    
    if (sendto(sockfd, (void*)reqMessage, strlen(reqMessage), MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
      printf("Errore nella sendto(): %s\n", strerror(errno));
      return 1;
    }
    
    return 0;
  }
  
  // else: Il file esiste
  strcpy(reqMessage, OK);
    
  if (sendto(sockfd, (void*)reqMessage, strlen(reqMessage), MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
    printf("Errore nella sendto(): %s\n", strerror(errno));
    return 1;
  }
  
  stat(fileName, &st);
  fileSize = (uint32_t)st.st_size; // HP: not too big files
  fileSizeConverted = htonl(fileSize);
  
  printf("Sending the file size to the client (file size = %u)...\n", fileSize);
  if (sendto(sockfd, (void*)(&fileSizeConverted), 4, MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
    printf("Errore nella sendto(): %s\n", strerror(errno));
    return 1;
  }
  
  bufferReply = (char*)malloc(sizeof(char) * fileSize);
  
  fd = fopen(fileName, "r");
  fread((void*)bufferReply, 1, fileSize, fd);
  
  printf("Sending the file to the client (file size = %u)...\n", fileSize);
  if (sendto(sockfd, (void*)bufferReply, fileSize, MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
    printf("Errore nella sendto(): %s\n", strerror(errno));
    return 1;
  }
    
  fclose(fd);
  free(bufferReply);
    
  return 0;
}