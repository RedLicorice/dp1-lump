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

#define M 14

size_t writen(int s, const void *ptr, size_t nbytes);

int main(int argc, char *argv[]) {
  int sockfd, num1, num2;
  ssize_t numberOfReadBytes;
  struct sockaddr_in saddr, daddr;
  char bufferIn[M], bufferOut;
  char num2string[6];
  
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(43082);
  if (inet_aton("127.0.0.1", &(saddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton() (a)\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1) {
    printf("Errore nella bind(): %s\n", strerror(errno));
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  //daddr.sin_len = sizeof(struct sockaddr_in);
  daddr.sin_family = AF_INET; // IPv4
  daddr.sin_port = htons(atoi(argv[2]));
  if (inet_aton(argv[1], &(daddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton() (b)\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (connect(sockfd, (struct sockaddr*)&daddr, sizeof(struct sockaddr_in)) == -1) {
    printf("Errore nella connect(): %s\n", strerror(errno));
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  printf("Digitare il primo numero: ");
  scanf("%d", &num1);
  //itoa(num1, bufferIn, 10);
  snprintf(bufferIn, 6, "%d", num1);
  
  strcat(bufferIn, " ");
  
  printf("Digitare il secondo numero: ");
  scanf("%d", &num2);
  //itoa(num2, num2string, 10);
  snprintf(num2string, 6, "%d", num2);
  strcat(bufferIn, num2string);
  
  strcat(bufferIn, "\r\n");
  
  if (writen(sockfd, bufferIn, strlen(bufferIn)) == -1) {
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  printf("Il server ha risposto: ");
  do {
    numberOfReadBytes = recv(sockfd, (void*)&bufferOut, 1, 0);
    if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
      if (numberOfReadBytes == -1)
	printf("Errore nella recv(): %s\n", strerror(errno));
      else // numberOfReadBytes == 0
	printf("Errore nella recv(): the peer has performed an orderly shutdown\n");
      if (close(sockfd) == -1)
	printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    printf("%c", bufferOut);
  } while (bufferOut != '\n');
  
  if (close(sockfd) == -1) {
    printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  return 0;
}

size_t writen(int s, const void *ptr, size_t nbytes) {
  size_t nleft;
  ssize_t nwritten;
  
  nleft = nbytes;
  
  while (nleft > 0) {
    nwritten = send(s, ptr, nleft, MSG_NOSIGNAL);
    if (nwritten == -1) {
      printf("Errore nella send(): %s\n", strerror(errno));
      return (nwritten);
    } else {
      nleft -= nwritten;
      ptr += nwritten;
    }
  }
  
  return (nbytes - nleft);
}