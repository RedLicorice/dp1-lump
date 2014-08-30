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

#define N 512

int serviClient(int sockfd_copy, struct sockaddr_in caddr);
int chiudiClient(int sockfd_copy);
size_t readn(int s, void *ptr, size_t len);
size_t writen(int s, const void *ptr, size_t nbytes);

int main(int argc, char *argv[]) {
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
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  if (listen(sockfd, 2) == -1) {
    printf("Errore nella listen(): %s\n", strerror(errno));
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  addrlen = sizeof(struct sockaddr_in);
  while (1) {
    printf("\nServer in ascolto sulla porta %d...\n", atoi(argv[1]));
    sockfd_copy = accept(sockfd, (struct sockaddr*)&caddr, &addrlen);
    if (sockfd_copy == -1) {
      printf("Errore nella accept(): %s\n", strerror(errno));
      if (close(sockfd) == -1)
	printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    
    if (serviClient(sockfd_copy, caddr) == 1) {
      if (close(sockfd) == -1)
        printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
  }
}

int serviClient(int sockfd_copy, struct sockaddr_in caddr) {
  int bufferReplyLast, valido;
  ssize_t numberOfReadBytes;
  char bufferReply[70], nomeFile[67], bufferQuery[N], byte;
  struct stat st;
  uint32_t fileSize;
  FILE *fd;
  
  printf("(serviClient)  Connessione accettata da un client con indirizzo = %s e porta = %d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
      
  while (1) {
	
    // 1. Ricezione del nome del file
    bufferReplyLast = -1;
    valido = 1;
    do {
      bufferReplyLast++;
      if (bufferReplyLast >= 69)
	valido = 0;
      else {
	numberOfReadBytes = readn(sockfd_copy, bufferReply + bufferReplyLast, 1);
	if (numberOfReadBytes == -1 || numberOfReadBytes == 0)
	  return chiudiClient(sockfd_copy);
      }
    } while (valido == 1 && bufferReply[bufferReplyLast] != '\n');
	
    // 2. Verifica se il comando è legale
    if (valido == 1) {
      bufferReply[bufferReplyLast + 1] = '\0';
      if (strcmp(bufferReply, "QUIT\r\n") == 0)
	return chiudiClient(sockfd_copy);
      if (strncmp(bufferReply, "GET", 3) != 0)
	valido = 0;
      // else: valido = 1
    }
    
    if (valido == 1) {
      // 3. Estrazione del nome del file e verifica dell'esistenza del file (http://stackoverflow.com/a/230068)
      strcpy(nomeFile, bufferReply + 3);
      nomeFile[strlen(nomeFile) - 2] = '\0';
      if (access(nomeFile, R_OK) == -1) // Il file non esiste
	valido = 0;
    }
	  
    if (valido == 0) {
      // Invio del messaggio -ERR
      printf("(serviClient)  Comando illegale o file non esistente\n");
      bufferQuery[0] = '\0';
      strcat(bufferQuery, "-ERR\r\n");
      //if (writen(sockfd_copy, bufferQuery, strlen(bufferQuery)) == -1)
      writen(sockfd_copy, bufferQuery, strlen(bufferQuery));
      return chiudiClient(sockfd_copy);
    }
	
    // 4. Invio del messaggio +OK
    bufferQuery[0] = '\0';
    strcat(bufferQuery, "+OK\r\n");
    if (writen(sockfd_copy, bufferQuery, strlen(bufferQuery)) == -1)
      return chiudiClient(sockfd_copy);

    // 5. Invio della dimensione del file
    stat(nomeFile, &st);
    fileSize = (uint32_t)st.st_size; // HP: file non troppo grandi
    printf("(serviClient)  Invio del file \"%s\" (%u byte) al client %s:%d in corso...\n", nomeFile, fileSize, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
    fileSize = htonl(fileSize);
    if (writen(sockfd_copy, (void*)&fileSize, 4) == -1)
      return chiudiClient(sockfd_copy);
	
    // 6. Invio del file
    fd = fopen(nomeFile, "r");
    while (fread((void*)&byte, 1, 1, fd) > 0) {
      if (writen(sockfd_copy, (void*)&byte, 1) == -1) {
	fclose(fd);
	return chiudiClient(sockfd_copy);
      }
    }
    
    fclose(fd);
    printf("(serviClient)  File \"%s\" correttamente inviato al client %s:%d\n", nomeFile, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
	  
  }
}

int chiudiClient(int sockfd_copy) {
  if (close(sockfd_copy) == -1) {
    printf("Errore nella close(): %s\n", strerror(errno));
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
      printf("Errore nella send(): %s\n", strerror(errno));
      return (nwritten);
    } else {
      nleft -= nwritten;
      ptr += nwritten;
    }
  }
  
  return (nbytes - nleft);
}