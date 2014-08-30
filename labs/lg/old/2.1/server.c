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

#define N 32

int main(int argc, char *argv[]) {
  int sockfd;
  char bufferReply[N];
  ssize_t numberOfReadBytes;
  socklen_t addrlen;
  struct sockaddr_in saddr, daddr;
  
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
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }

  while (1) {
    printf("\nServer in ascolto sulla porta %d...\n", atoi(argv[1]));

    // 1. Ricezione del datagram
    numberOfReadBytes = recvfrom(sockfd, (void*)bufferReply, N, 0, (struct sockaddr*)&daddr, &addrlen);
    if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
      if (numberOfReadBytes == -1)
        printf("Errore nella recvfrom(): %s\n", strerror(errno));
      else // numberOfReadBytes == 0
        printf("Errore nella recvfrom(): the peer has performed an orderly shutdown\n");
      if (close(sockfd) == -1)
        printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }

    // 2. Invio del datagram
    if (sendto(sockfd, bufferReply, numberOfReadBytes, MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
      printf("Errore nella sendto(): %s\n", strerror(errno));
      if (close(sockfd) == -1)
        printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
  }
}
