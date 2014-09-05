/*
 * argv[1] = indirizzo destinazione
 * argv[2] = porta destinazione
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

size_t readn(int s, void *ptr, size_t len);
size_t writen(int s, const void *ptr, size_t nbytes);

int main(int argc, char *argv[]) {
  int sockfd, bufferReplyLast;
  size_t leftBytes;
  ssize_t numberOfReadBytes;
  uint32_t numberOfBytes; // unsigned long int
  struct sockaddr_in saddr, daddr;
  char nomeFile[65], bufferQuery[70];
  char bufferReply[N];
  FILE *fd;
  //struct linger optval;
  
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    printf("(a) Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  // http://developerweb.net/viewtopic.php?id=2982
  /*optval.l_onoff = 0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &optval, sizeof(struct linger)) == -1) {
    printf("(v) Errore nella setsockopt(): %s\n", strerror(errno));
    if (close(sockfd) == -1)
      printf("(w) Errore nella close(): %s\n", strerror(errno));
    return 1;
  }*/
  
  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(43082);
  if (inet_aton("127.0.0.1", &(saddr.sin_addr)) == 0) {
    printf("(b) Errore nella inet_aton()\n");
    if (close(sockfd) == -1)
      printf("(c) Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1) {
    printf("(d) Errore nella bind(): %s\n", strerror(errno));
    if (close(sockfd) == -1)
      printf("(e) Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  //daddr.sin_len = sizeof(struct sockaddr_in);
  daddr.sin_family = AF_INET; // IPv4
  daddr.sin_port = htons(atoi(argv[2]));
  if (inet_aton(argv[1], &(daddr.sin_addr)) == 0) {
    printf("(f) Errore nella inet_aton()\n");
    if (close(sockfd) == -1)
      printf("(g) Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (connect(sockfd, (struct sockaddr*)&daddr, sizeof(struct sockaddr_in)) == -1) {
    printf("(h) Errore nella connect(): %s\n", strerror(errno));
    if (close(sockfd) == -1)
      printf("(i) Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  printf("Digitare il nome del file (massimo 64 caratteri, stringa vuota per terminare): ");
  gets(nomeFile);
  while (strcmp(nomeFile, "") != 0) {
    
    // 1. Richiesta del file
    bufferQuery[0] = '\0';
    strcat(bufferQuery, "GET");
    strcat(bufferQuery, nomeFile);
    strcat(bufferQuery, "\r\n");
    
    if (writen(sockfd, bufferQuery, strlen(bufferQuery)) == -1) {
      if (close(sockfd) == -1)
	printf("(j) Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    
    // 2. Ricezione messaggio di OK/ERR
    bufferReplyLast = -1;
    do {
      bufferReplyLast++;
      numberOfReadBytes = readn(sockfd, bufferReply + bufferReplyLast, 1); // 1 byte per volta
      if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
        if (close(sockfd) == -1)
          printf("(m) Errore nella close(): %s\n", strerror(errno));
        return 1;
      }
    } while(bufferReply[bufferReplyLast] != '\n');
    bufferReply[bufferReplyLast + 1] = '\0';
    
    if (strcmp(bufferReply, "-ERR\r\n") == 0) {
      printf("Comando illegale o file non esistente\n");
      if (close(sockfd) == -1) {
	printf("(u) Errore nella close(): %s\n", strerror(errno));
	return 1;
      }
      return 0;
    }
    
    // 3. Ricezione del numero di byte
    if (readn(sockfd, (void*)&numberOfBytes, 4) <= 0) {
      if (close(sockfd) == -1)
        printf("(p) Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    numberOfBytes = ntohl(numberOfBytes); // network-to-host long
      
    // 4. Apertura del file
    fd = fopen(nomeFile, "w");
    leftBytes = numberOfBytes;
      
    while (leftBytes > 0) {
      // 5. Ricezione del file
      if (leftBytes < N - 1)
        numberOfReadBytes = readn(sockfd, (void*)bufferReply, leftBytes);
      else
        numberOfReadBytes = readn(sockfd, (void*)bufferReply, N - 1);
	
      if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
        fclose(fd);
        if (close(sockfd) == -1)
          printf("(q) Errore nella close(): %s\n", strerror(errno));
        return 1;
      }
	  
      // 6. Scrittura del file
      leftBytes -= numberOfReadBytes;
      fwrite((void*)bufferReply, 1, numberOfReadBytes, fd);
    }
      
    // 7. Chiusura del file
    fclose(fd);
      
    printf("Digitare il nome del file (massimo 64 caratteri, stringa vuota per terminare): ");
    gets(nomeFile);
  }
  
  // 8. Invia messaggio di QUIT
  bufferQuery[0] = '\0';
  strcat(bufferQuery, "QUIT\r\n");
    
  if (writen(sockfd, bufferQuery, strlen(bufferQuery)) == -1 || close(sockfd) == -1) {
    printf("(r) Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  return 0;
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
      printf("(u) Errore nella send(): %s\n", strerror(errno));
      return (nwritten);
    } else {
      nleft -= nwritten;
      ptr += nwritten;
    }
  }
  
  return (nbytes - nleft);
}