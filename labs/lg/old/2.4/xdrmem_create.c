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

#define M 100

int tcpConnect(int *sockfd, char *argv[]);
int closeAll1(int reply, int sockfd, XDR *xdrs1);
int closeAll2(int reply, int sockfd, XDR *xdrs1, XDR *xdrs2);
size_t readnUnaVolta(int s, void *ptr, size_t len, size_t *nleft);
size_t writen(int s, const void *ptr, size_t nbytes);

int main(int argc, char *argv[]) {
  int sockfd, num1, num2, result;
  XDR xdrs1, xdrs2; // pointer to XDR stream
  char buffer1[M], buffer2[M];
  unsigned int bufferPos;
  int n;
  
  if (tcpConnect(&sockfd, argv) == 1)
    return 1;
  
  printf("Digitare il primo numero: ");
  scanf("%d", &num1);
  
  printf("Digitare il secondo numero: ");
  scanf("%d", &num2);
  
  // 1. Invio dei dati
  memset(buffer1, '\0', M);
  
  xdrmem_create(&xdrs1, buffer1, M, XDR_ENCODE);
  
  if (xdr_int(&xdrs1, &num1) == 0) {
    printf("Errore nella xdr_int()\n");
    return closeAll1(1, sockfd, &xdrs1);
  }
  
  if (xdr_int(&xdrs1, &num2) == 0) {
    printf("Errore nella xdr_int()\n");
    return closeAll1(1, sockfd, &xdrs1);
  }
  
  bufferPos = xdr_getpos(&xdrs1); // the amount of buffer used
  
  if (writen(sockfd, buffer1, bufferPos) == -1) {
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  // 2. Ricezione dei dati
  memset(buffer2, '\0', M);
  
  n = readnUnaVolta(sockfd, buffer2, M, NULL);
  if (n <= 0)
    return closeAll1(1, sockfd, &xdrs1);
  
  xdrmem_create(&xdrs2, buffer2, n, XDR_DECODE);
  
  if (xdr_int(&xdrs2, &result) == 0) {
    printf("Errore nella xdr_int()\n");
    return closeAll2(1, sockfd, &xdrs1, &xdrs2);
  }
  
  printf("Il server ha risposto: %d\n", result);
  
  return closeAll2(0, sockfd, &xdrs1, &xdrs2);
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

int closeAll1(int reply, int sockfd, XDR *xdrs1) {
  xdr_destroy(xdrs1);
  if (close(sockfd) == -1)
    printf("Errore nella close(): %s\n", strerror(errno));
  return reply;
}

int closeAll2(int reply, int sockfd, XDR *xdrs1, XDR *xdrs2) {
  xdr_destroy(xdrs1);
  xdr_destroy(xdrs2);
  if (close(sockfd) == -1)
    printf("Errore nella close(): %s\n", strerror(errno));
  return reply;
}

size_t readnUnaVolta(int s, void *ptr, size_t len, size_t *nleft) {
  /* Valori di ritorno:
   * -1 = altro errore
   * 0 = orderly shutdown
   * > 0 = numero di caratteri letti
   */
  ssize_t nread;
  
  nread = recv(s, ptr, len, 0);
    
  if (nread == 0) {
    printf("Errore nella recv(): The peer has performed an orderly shutdown\n");
    return 0;
  }
    
  if (nread == -1) {
    printf("Errore nella recv(): %s\n", strerror(errno));
    return -1;
  }
    
  // else: nread > 0
  if (nleft != NULL)
    *nleft -= nread;

  return nread;
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