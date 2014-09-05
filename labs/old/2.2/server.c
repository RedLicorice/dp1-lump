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

typedef struct client_s {
  struct sockaddr_in address;
  int numberOfPackets;
} client_t;

#define N 32

int areSockaddrinEqual(struct sockaddr_in address1, struct sockaddr_in address2);

int main(int argc, char *argv[]) {
  int sockfd, clientsLastIndex, i, valid, found;
  char bufferReply[N];
  ssize_t numberOfReadBytes;
  socklen_t addrlen;
  struct sockaddr_in saddr, daddr;
  client_t clients[10];
  
  clientsLastIndex = -1;
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

    // 2. Verifica del vettore dei client passati
    valid = 1;
    found = 0;
    if (clientsLastIndex > -1) {
      for (i = 0; i <= clientsLastIndex && found == 0; ++i) {
	if (areSockaddrinEqual(clients[i].address, daddr) == 0) {
	  found = 1;
	  if (clients[i].numberOfPackets > 2)
	    valid = 0;
	  else {
	    clients[i].numberOfPackets++;
	    printf("Questo client finora ha mandato %d pacchetti.\n", clients[i].numberOfPackets);
	  }
	}
      }
    }
    
    if (found == 0) {
      printf("Questo client è nuovo.\n");
      if (clientsLastIndex == 9)
	for (i = 0; i < 9; ++i)
	  clients[i] = clients[i + 1];
	
      else
	clientsLastIndex++;
      
      clients[clientsLastIndex].address = daddr;
      clients[clientsLastIndex].numberOfPackets = 1;
      valid = 1;
    }
      
    if (valid == 0)
      printf("Il client ha superato il numero massimo di pacchetti inviabili a questo server.\n");
    else { // valid == 1
      // 3. Invio del datagram
      if (sendto(sockfd, bufferReply, numberOfReadBytes, MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
	printf("Errore nella sendto(): %s\n", strerror(errno));
	if (close(sockfd) == -1)
	  printf("Errore nella close(): %s\n", strerror(errno));
	return 1;
      }
    }
  }
}

int areSockaddrinEqual(struct sockaddr_in address1, struct sockaddr_in address2) {
  // Restituisce 0 se sono uguali, -1 se sono diversi.
  if (address1.sin_family != address2.sin_family)
    return -1;
  if (address1.sin_port != address2.sin_port)
    return -1;
  if (address1.sin_addr.s_addr != address2.sin_addr.s_addr)
    return -1;
  return 0;
}