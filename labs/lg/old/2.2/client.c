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
  int sockfd, numberOfSockets, numeroDiTentativi, attesa;
  struct sockaddr_in daddr;
  char bufferReply[N], input[5];
  ssize_t numberOfReadBytes;
  fd_set cset;
  struct timeval timeout;
  socklen_t addrlen;

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

  do {
    numeroDiTentativi = 1;
    attesa = 1;

    while (numeroDiTentativi < 6 && attesa == 1) {
      if (sendto(sockfd, argv[3], sizeof(argv[3]), MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) {
	printf("Errore nella sendto(): %s\n", strerror(errno));
	if (close(sockfd) == -1)
	  printf("Errore nella close(): %s\n", strerror(errno));
	return 1;
      }

      FD_ZERO(&cset);
      FD_SET(sockfd, &cset);    
      timeout.tv_sec = 3;
      timeout.tv_usec = 0;
      numberOfSockets = select(FD_SETSIZE, &cset, NULL, NULL, &timeout);
      
      if (numberOfSockets == -1) {
	printf("Errore nella select(): %s\n", strerror(errno));
	if (close(sockfd) == -1)
	  printf("Errore nella close(): %s\n", strerror(errno));
	return 1;
      }

      if (numberOfSockets == 0) { // Timeout scaduto
	numeroDiTentativi++;
	printf("Timeout scaduto, tentativo %d in corso...\n", numeroDiTentativi);
      } else // Il server ha risposto
	attesa = 0;
    }
    
    if (attesa == 1) {
      printf("Non ho ricevuto risposta dal server.\n");
      if (close(sockfd) == -1)
	printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }

    numberOfReadBytes = recvfrom(sockfd, (void*)bufferReply, N, 0, NULL, NULL);
    if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
      if (numberOfReadBytes == -1)
	printf("Errore nella recvfrom(): %s\n", strerror(errno));
      else // numberOfReadBytes == 0
	printf("Errore nella recvfrom(): the peer has performed an orderly shutdown\n");
      if (close(sockfd) == -1)
	printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }

    printf("Il server ha risposto: %s\n", bufferReply);
    
    printf("Premere Invio per rispedire il pacchetto UDP, o digitare \"quit\" per uscire: "); // In realtà basta qualsiasi stringa, purché non superi i 4 caratteri ('\0' escluso) del buffer "input"
    gets(input);
  } while (strcmp(input, "") == 0);

  if (close(sockfd) == -1)
    printf("Errore nella close(): %s\n", strerror(errno));
  return 0;
}
