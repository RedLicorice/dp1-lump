/*
argv[1] = porta del server
argv[2] = K
*/

/*#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>*/
#include "../lunp.h"

#define N 4096

int elaboraRichiesta(char *bufferReply, char *Kstring) {
  uint32_t id, op1, op2, result, K;
  int sscanfReply;
  
  id = 0;
  op1 = 1;
  op2 = 2;
  
  printf("[server] La richiesta del client è: %s\n", bufferReply);
  
  //bufferReply[12] = '\0';
  //sscanfReply = sscanf(bufferReply, "%u", &id);
  sscanfReply = sscanf(bufferReply, "%u %u %u", &id, &op1, &op2);
  
  printf("[server] sscanfReply: %d (id = %u, op1 = %u, op2 = %u)\n", sscanfReply, ntohl(id), ntohl(op1), ntohl(op2));
  
  if (sscanfReply != 3) {
    // Ignora
    printf("[server] Input errato\n");
    return 1;
  }
  
  op1 = ntohl(op1);
  op2 = ntohl(op2);
  K = atoi(Kstring);
  
  result = (op1 + op2) % K;
  
  sprintf(bufferReply, "%u %u", id, htonl(result));
  return 0;
}

int serviClient(SOCKET sockfd, char *K) {
  char bufferReply[N];
  ssize_t numberOfReadBytes;
  socklen_t addrlen;
  struct sockaddr_in daddr;

  addrlen = sizeof(struct sockaddr_in);
  
    // Ricezione del datagram
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
    printf("[server] Ho letto %d byte\n", (int)numberOfReadBytes);

    if (elaboraRichiesta(bufferReply, K) == 1)
      return 1;

    // Invio del datagram
    if (sendto(sockfd, bufferReply, numberOfReadBytes, MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
      printf("Errore nella sendto(): %s\n", strerror(errno));
      if (close(sockfd) == -1)
        printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  struct sockaddr_in saddr;
  socklen_t addrlen;
  
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

    serviClient(sockfd, argv[2]);
  }
}
