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
#include <rpc/xdr.h>

int tcpConnect(int *sockfd, char *argv[]);
int closeAll(int reply, int sockfd, XDR *xdrs1, XDR *xdrs2);

int main(int argc, char *argv[]) {
  int sockfd, num1, num2, result;
  XDR xdrs1, xdrs2; // pointer to XDR stream
  FILE *fd1, *fd2;
  
  if (tcpConnect(&sockfd, argv) == 1)
    return 1;
  
  printf("Digitare il primo numero: ");
  scanf("%d", &num1);
  
  printf("Digitare il secondo numero: ");
  scanf("%d", &num2);
  
  fd1 = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs1, fd1, XDR_ENCODE);
  
  fd2 = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs2, fd2, XDR_DECODE);
  
  // 1. Invio dei dati
  if (xdr_int(&xdrs1, &num1) == 0) {
    printf("Errore nella xdr_int()\n");
    return closeAll(1, sockfd, &xdrs1, &xdrs2);
  }
  
  if (xdr_int(&xdrs1, &num2) == 0) {
    printf("Errore nella xdr_int()\n");
    return closeAll(1, sockfd, &xdrs1, &xdrs2);
  }
  
  fflush(fd1);
  
  // 2. Ricezione dei dati
  if (xdr_int(&xdrs2, &result) == 0) {
    printf("Errore nella xdr_int()\n");
    return closeAll(1, sockfd, &xdrs1, &xdrs2);
  }
  
  printf("Il server ha risposto: %d\n", result);
  
  return closeAll(0, sockfd, &xdrs1, &xdrs2);
}

int tcpConnect(int *sockfd, char *argv[]) {
  struct sockaddr_in saddr, daddr;
  
  *sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(43082);
  if (inet_aton("127.0.0.1", &(saddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton() (a)\n");
    if (close(*sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (bind(*sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1) {
    printf("Errore nella bind(): %s\n", strerror(errno));
    if (close(*sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  //daddr.sin_len = sizeof(struct sockaddr_in);
  daddr.sin_family = AF_INET; // IPv4
  daddr.sin_port = htons(atoi(argv[2]));
  if (inet_aton(argv[1], &(daddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton() (b)\n");
    if (close(*sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (connect(*sockfd, (struct sockaddr*)&daddr, sizeof(struct sockaddr_in)) == -1) {
    printf("Errore nella connect(): %s\n", strerror(errno));
    if (close(*sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  return 0;
}

int closeAll(int reply, int sockfd, XDR *xdrs1, XDR *xdrs2) {
  xdr_destroy(xdrs1);
  xdr_destroy(xdrs2);
  if (close(sockfd) == -1)
    printf("Errore nella close(): %s\n", strerror(errno));
  return reply;
}