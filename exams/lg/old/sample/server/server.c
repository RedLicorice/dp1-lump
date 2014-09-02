/*
 * argv[1] = porta
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <rpc/xdr.h>
#include "../types.h"

#include "unp.h"

#define nchildren 5

pid_t child_make(int i, int listenfd, int addrlen, void (*child_task)(int));
void child_main(int i, int listenfd, int addrlen, void (*child_task)(int));
int tcpsrv_pre(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int));
void serviClient(int sockfd_copy);
void chiudi(int sockfd, XDR *xdrs_r, XDR *xdrs_w);
static void sig_int(int s);

pid_t childpids[nchildren];

pid_t
child_make(int i, int listenfd, int addrlen, void (*child_task)(int))
{
  pid_t   pid;

  if ( (pid = Fork()) > 0)
    return(pid);            /* parent */

  child_main(i, listenfd, addrlen, child_task);       /* never returns */
  return 0; /* just to shut down the debugger! */
}

void
child_main(int i, int listenfd, int addrlen, void (*child_task)(int))
{
  int              connfd;
  socklen_t        clilen;
  struct sockaddr *cliaddr;

  cliaddr = Malloc(addrlen);

  printf("child %ld starting\n", (long) getpid());
  for ( ; ; ) {
    clilen = addrlen;
    connfd = Accept(listenfd, cliaddr, &clilen);

    child_task(connfd);              /* process the request */
    //Close(connfd);
  }
  
  // it nevers gets here
  printf("child %ld stopping\n", (long) getpid());
  free(cliaddr);
  return;
}

/*int
tcpsrv_pre(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int))*/
int main(int argc, char *argv[]) {
  int             listenfd, i;
  socklen_t       addrlen;

  listenfd = Tcp_listen(NULL, argv[1], &addrlen);

  for (i = 0; i < nchildren; i++)
    childpids[i] = child_make(i, listenfd, addrlen, serviClient);     /* parent returns */

  /* Father */
  /* 
     Signal handler of the father only. 
     The children do not handle them in this way.
  */
  if (signal(SIGINT, sig_int) == SIG_ERR) {
    printf("Errore nella signal(): %s\n", strerror(errno));
    return 1;
  }

  for ( ; ; ){
    ;
  }
  return 0;
}

/*int main(int argc, char *argv[]) {
  int sockfd, sockfd_copy;
  struct sockaddr_in saddr, caddr;
  socklen_t addrlen;
  
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(argv[1]));
  saddr.sin_addr.s_addr = INADDR_ANY;
  
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1) {
    printf("Errore nella bind(): %s\n", strerror(errno));
    chiudi(sockfd, NULL, NULL);
    return 1;
  }
  
  if (listen(sockfd, 2) == -1) {
    printf("Errore nella listen(): %s\n", strerror(errno));
    chiudi(sockfd, NULL, NULL);
    return 1;
  }
  
  addrlen = sizeof(struct sockaddr_in);
  while (1) {
    printf("\nServer in ascolto sulla porta %d...\n", atoi(argv[1]));
    sockfd_copy = accept(sockfd, (struct sockaddr*)&caddr, &addrlen);
    if (sockfd_copy == -1) {
      printf("Errore nella accept(): %s\n", strerror(errno));
      chiudi(sockfd, NULL, NULL);
      return 1;
    }
    
    if (serviClient(sockfd_copy, caddr) == 1) {
      chiudi(sockfd, NULL, NULL);
      return 1;
    }
  }
}*/

void serviClient(int sockfd_copy) {
  struct stat st;
  uint32_t fileSize;
  FILE *fd;
  
  XDR xdrs_w, xdrs_r;
  FILE *fd_w, *fd_r;
  clientQuery_t clientQuery;
  serverResponse_t serverResponse;
  
  fd_w = fdopen(sockfd_copy, "w");
  xdrstdio_create(&xdrs_w, fd_w, XDR_ENCODE);
  
  fd_r = fdopen(sockfd_copy, "r");
  xdrstdio_create(&xdrs_r, fd_r, XDR_DECODE);
  
  printf("(serviClient)  Connessione accettata da un client\n");
      
  while (1) {
    
    // CLIENT QUERY
    clientQuery.fileName = NULL;
    if (!xdr_clientQuery_t(&xdrs_r, &clientQuery)) {
      chiudi(sockfd_copy, &xdrs_r, &xdrs_w);
      return;
    }
	
    if (clientQuery.message == QUIT) {
      chiudi(sockfd_copy, &xdrs_r, &xdrs_w);
      return;
    }
    //clientQuery.message = GET

    // SERVER RESPONSE    
    // http://stackoverflow.com/a/230068
    if (access(clientQuery.fileName, R_OK) == -1) { // Il file non esiste
      printf("(serviClient)  Comando illegale o file non esistente\n");
      serverResponse.fileData_t.fileData_t_val = NULL;
      serverResponse.serverMessage = ERR;
      if (!xdr_serverResponse_t(&xdrs_w, &serverResponse)) {
	chiudi(sockfd_copy, &xdrs_r, &xdrs_w);
	return;
      }
      fflush(fd_w);
      chiudi(sockfd_copy, &xdrs_r, &xdrs_w);
      return;
    }

    serverResponse.serverMessage = OK;

    stat(clientQuery.fileName, &st);
    fileSize = (uint32_t)st.st_size; // HP: file non troppo grandi
    printf("(serviClient)  Invio del file \"%s\" (%u byte) al client in corso...\n", clientQuery.fileName, fileSize);
    
    serverResponse.fileData_t.fileData_t_len = fileSize;
    
    serverResponse.fileData_t.fileData_t_val = (char*)malloc(sizeof(char) * fileSize);
    fd = fopen(clientQuery.fileName, "r");
    fread((void*)serverResponse.fileData_t.fileData_t_val, 1, fileSize, fd);
    
    if (!xdr_serverResponse_t(&xdrs_w, &serverResponse)) {
      chiudi(sockfd_copy, &xdrs_r, &xdrs_w);
      return;
    }
    fflush(fd_w);
    
    fclose(fd);
    printf("(serviClient)  File \"%s\" correttamente inviato al client\n", clientQuery.fileName);
    free(serverResponse.fileData_t.fileData_t_val);
	  
  }
}

void chiudi(int sockfd, XDR *xdrs_r, XDR *xdrs_w) {
  if (xdrs_r != NULL)
    xdr_destroy(xdrs_r);
  if (xdrs_w != NULL)
    xdr_destroy(xdrs_w);
  if (close(sockfd) == -1)
    printf("Errore nella close(): %s\n", strerror(errno));
  return;
}

static void sig_int (int s) {
  int i;
  if (s == SIGINT) {
    printf("Segnale SIGINT ricevuto\n");
    for (i = 0; i < nchildren; ++i)
      kill(childpids[i], SIGINT);
    for (i = 0; i < nchildren; ++i)
      waitpid(childpids[i], NULL, 0);
    exit(0);
  }
}