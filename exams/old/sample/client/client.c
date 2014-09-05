/*
 * argv[1] = indirizzo destinazione
 * argv[2] = porta destinazione
 * argv[3] = porta sorgente
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
#include <rpc/xdr.h>
#include "../types.h"

#define N 512

int chiudi(int reply, int sockfd, XDR *xdrs_r, XDR *xdrs_w);

int main(int argc, char *argv[]) {
  int sockfd;
  uint32_t numberOfBytes; // unsigned long int
  struct sockaddr_in saddr, daddr;
  char nomeFile[65];
  FILE *fd;
  
  XDR xdrs_w, xdrs_r;
  FILE *fd_w, *fd_r;
  clientQuery_t clientQuery;
  serverResponse_t serverResponse;
  
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(argv[3]));
  if (inet_aton("127.0.0.1", &(saddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton()\n");
    return chiudi(1, sockfd, NULL, NULL);
  }
  
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1) {
    printf("Errore nella bind(): %s\n", strerror(errno));
    return chiudi(1, sockfd, NULL, NULL);
  }
  
  daddr.sin_family = AF_INET; // IPv4
  daddr.sin_port = htons(atoi(argv[2]));
  if (inet_aton(argv[1], &(daddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton()\n");
    return chiudi(1, sockfd, NULL, NULL);
  }
  
  if (connect(sockfd, (struct sockaddr*)&daddr, sizeof(struct sockaddr_in)) == -1) {
    printf("Errore nella connect(): %s\n", strerror(errno));
    return chiudi(1, sockfd, NULL, NULL);
  }
  
  fd_w = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs_w, fd_w, XDR_ENCODE);
  
  fd_r = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs_r, fd_r, XDR_DECODE);
  
  printf("Digitare il nome del file (massimo 64 caratteri, stringa vuota per terminare): ");
  gets(nomeFile);
  while (strcmp(nomeFile, "") != 0) {
    
    // CLIENT QUERY
    clientQuery.message = GET;
    clientQuery.fileName = nomeFile;
    if (!xdr_clientQuery_t(&xdrs_w, &clientQuery))
      return chiudi(1, sockfd, &xdrs_r, &xdrs_w);
    fflush(fd_w);
    
    // SERVER RESPONSE
    serverResponse.fileData_t.fileData_t_val = NULL;
    xdr_serverResponse_t(&xdrs_r, &serverResponse);
    
    if (serverResponse.serverMessage == ERR) {
      printf("Comando illegale o file non esistente\n");
      return chiudi(1, sockfd, &xdrs_r, &xdrs_w);
    }
    
    numberOfBytes = serverResponse.fileData_t.fileData_t_len;
      
    fd = fopen(nomeFile, "w");
    fwrite((void*)serverResponse.fileData_t.fileData_t_val, 1, numberOfBytes, fd);
    fclose(fd);
      
    printf("Digitare il nome del file (massimo 64 caratteri, stringa vuota per terminare): ");
    gets(nomeFile);
  }
  
  clientQuery.message = QUIT;
  clientQuery.fileName = NULL;
  if (!xdr_clientQuery_t(&xdrs_w, &clientQuery))
    return chiudi(1, sockfd, &xdrs_r, &xdrs_w);
  fflush(fd_w);
  
  return chiudi(0, sockfd, &xdrs_r, &xdrs_w);
}

int chiudi(int reply, int sockfd, XDR *xdrs_r, XDR *xdrs_w) {
  if (xdrs_r != NULL)
    xdr_destroy(xdrs_r);
  if (xdrs_w != NULL)
    xdr_destroy(xdrs_w);
  if (close(sockfd) == -1) {
    printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  return reply;
}