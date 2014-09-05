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

#define N 512
#define lunghMaxFile 64

int socketLibero(int sockfd, char *comando, int *stato, int flagQuit);
int stdinputLibero(int sockfd, char *comando, int *stato, int *flagQuit);
int inviaMessaggioQuit(int sockfd);
size_t readnUnaVolta(int s, void *ptr, size_t len, size_t *nleft);
size_t readn(int s, void *ptr, size_t len);
size_t writen(int s, const void *ptr, size_t nbytes);

int main(int argc, char *argv[]) {
  int sockfd, reply, flagQuit;
  struct sockaddr_in saddr, daddr;
  char comando[3 + lunghMaxFile + 1];
  fd_set cset;
  //int maxfd;
  
  int stato;
  /*
   * 0 = sto facendo la GET
   * 1 = sto leggendo il messaggio +OK/-ERR
   * 2 = sto leggendo il numero di byte del file
   * 3 = sto leggendo il file
   */
  
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(argv[3]));
  if (inet_aton("127.0.0.1", &(saddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton()\n");
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
    printf("Errore nella inet_aton()\n");
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
  
  printf("Digitare un comando [GET <file>, Q, A]: ");
  fflush(stdout); // Forza la scrittura dello stdout
  stato = 0;
  flagQuit = 0;
  
  while (1) {
    FD_ZERO(&cset);
    FD_SET(sockfd, &cset);
    FD_SET(fileno(stdin), &cset); // fileno(stdin) = 0
    
    if (select(FD_SETSIZE, &cset, NULL, NULL, NULL) == -1) {
      printf("Errore nella select(): %s\n", strerror(errno));
      if (close(sockfd) == -1)
	printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    
    if (FD_ISSET(sockfd, &cset))
      reply = socketLibero(sockfd, comando, &stato, flagQuit);
    else if (FD_ISSET(fileno(stdin), &cset))
      reply = stdinputLibero(sockfd, comando, &stato, &flagQuit);
    else {
      printf("Nessuno dei due è settato!\n");
      return 1;
    }
    
    if (reply != 2)
      return reply;
  }
}

int socketLibero(int sockfd, char *comando, int *stato, int flagQuit) {
  static size_t leftBytes;
  ssize_t numberOfReadBytes;
  static void *ptr;
  static char bufferReply[N];
  static int bufferReplyLast;
  static uint32_t numberOfBytes; // unsigned long int
  static FILE *fd;
  
  switch (*stato) {
    case 0: // sto facendo la GET
      // 2. Ricezione messaggio di OK/ERR
      bufferReplyLast = -1;
      
      *stato = 1;
      return 2;
      
    case 1: // sto leggendo il messaggio +OK/-ERR
      bufferReplyLast++;
      numberOfReadBytes = readn(sockfd, bufferReply + bufferReplyLast, 1); // 1 byte per volta
      if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
	if (close(sockfd) == -1)
	  printf("Errore nella close(): %s\n", strerror(errno));
	return 1;
      }
	
      if (bufferReply[bufferReplyLast] != '\n')
	return 2;
	
      bufferReply[bufferReplyLast + 1] = '\0';
      
      if (strcmp(bufferReply, "-ERR\r\n") == 0) {
	printf("Comando illegale o file non esistente\n");
	if (close(sockfd) == -1) {
	  printf("Errore nella close(): %s\n", strerror(errno));
	  return 1;
	}
	return 0;
      }
      
      // 3. Ricezione del numero di byte
      leftBytes = 4;
      ptr = (void*)&numberOfBytes;
      
      *stato = 2;
      return 2;
      
      
    case 2: // sto leggendo il numero di byte del file
      numberOfReadBytes = readnUnaVolta(sockfd, ptr, leftBytes, &leftBytes);
      if (numberOfReadBytes <= 0) {
	if (close(sockfd) == -1)
	  printf("Errore nella close(): %s\n", strerror(errno));
	return 1;
      }
      
      if (leftBytes > 0)
	return 2;
      
      numberOfBytes = ntohl(numberOfBytes); // network-to-host long
      // HP: numberOfBytes > 0
	
      // 4. Apertura del file
      fd = fopen(comando + 4, "w");
      leftBytes = numberOfBytes;
      
      ptr = (void*)bufferReply;
      
      *stato = 3;
      return 2;
      
      
    case 3: // sto leggendo il file
      if (leftBytes < N - 1)
        numberOfReadBytes = readnUnaVolta(sockfd, ptr, leftBytes, &leftBytes);
      else
        numberOfReadBytes = readnUnaVolta(sockfd, ptr, N - 1, &leftBytes);
	
      if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
        fclose(fd);
        if (close(sockfd) == -1)
          printf("(q) Errore nella close(): %s\n", strerror(errno));
        return 1;
      }
	  
      // 6. Scrittura del file
      fwrite((void*)bufferReply, 1, numberOfReadBytes, fd);

      if (leftBytes > 0)
	return 2;
      
      fclose(fd);
      
      if (flagQuit == 1)
	return inviaMessaggioQuit(sockfd);
      
      printf("\nDigitare un comando [GET <file>, Q, A]: ");
      fflush(stdout);
      *stato = 0;
      return 2;
  }
}

int stdinputLibero(int sockfd, char *comando, int *stato, int *flagQuit) {
  char bufferQuery[70];
  
  gets(comando);
      
  if (strcmp(comando, "Q") == 0) {
    if (*stato == 0)
      return inviaMessaggioQuit(sockfd);
    
    // else: *stato > 0
    *flagQuit = 1;
    return 2;
  }
	
  if (strcmp(comando, "A") == 0) {
    if (close(sockfd) == -1) {
      printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    return 0;
  }
  
  // GET <file>
  // 1. Richiesta del file
  bufferQuery[0] = '\0';
  strcat(bufferQuery, "GET");
  strcat(bufferQuery, comando + 4);
  strcat(bufferQuery, "\r\n");
	
  if (writen(sockfd, bufferQuery, strlen(bufferQuery)) == -1) {
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
	
  socketLibero(sockfd, comando, stato, *flagQuit);
	
  printf("Digitare un comando [Q, A]: "); // Due GET in contemporanea non sono ammesse
  fflush(stdout);
  return 2;
}

int inviaMessaggioQuit(int sockfd) {
  char *bufferQuery;
  
  // 8. Invia messaggio di QUIT
  bufferQuery = "QUIT\r\n";
	  
  if (writen(sockfd, bufferQuery, strlen(bufferQuery)) == -1) {
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (close(sockfd) == -1) {
    printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  return 0;
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
  *nleft -= nread;

  return nread;
}

size_t readn(int s, void *ptr, size_t len) {
  /* Valori di ritorno:
   * -1 = altro errore
   * 0 = orderly shutdown
   * > 0 = numero di caratteri letti
   */
  ssize_t nread;
  size_t nleft;
  
  nleft = len;
  while (nleft > 0) {
    nread = recv(s, ptr, nleft, 0);
    
    if (nread == 0) {
      printf("Errore nella recv(): The peer has performed an orderly shutdown\n");
      return 0;
    }
    
    if (nread == -1) {
      printf("Errore nella recv(): %s\n", strerror(errno));
      return -1;
    }
    
    // else: nread > 0
    nleft -= nread;
    ptr += nread;
  }
  
  return (len - nleft);
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