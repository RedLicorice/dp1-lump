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
#include <signal.h>
#include <errno.h>
#include <string.h>

#define N 512
#define lunghMaxFile 64

int stop = 0;

int figlio(int pipefd[2], char *argv[]);
static void sig_usr(int s);
size_t readnConAbort(int s, void *ptr, size_t len);
size_t readn(int s, void *ptr, size_t len);
size_t writen(int s, const void *ptr, size_t nbytes);

int main(int argc, char *argv[]) {
  int pipefd[2];
  char comando[3 + lunghMaxFile + 1];
  pid_t childpid;
  
  if (pipe(pipefd) == -1) {
    printf("Errore nella pipe(): %s\n", strerror(errno));
    return 1;
  }
  
  childpid = fork();
  
  if (childpid == 0)
    return figlio(pipefd, argv);
  
  close(pipefd[0]);
  
  printf("Digitare un comando [GET <file>, Q, A]: ");
  gets(comando);
  while (strcmp(comando, "Q") != 0 && strcmp(comando, "A") != 0) {
    // GET <file>
    write(pipefd[1], (void*)comando, strlen(comando) + 1); // + 1 = '\0'
    
    printf("Digitare un comando [GET <file>, Q, A]: ");
    gets(comando);
  }
  
  write(pipefd[1], (void*)comando, 1 + 1); // + 1 = '\0'
  if (strcmp(comando, "A") == 0)
    kill(childpid, SIGUSR1);
  
  return 0;
}

int figlio(int pipefd[2], char *argv[]) {
  int sockfd, bufferReplyLast, i, trovatoSlash0;
  size_t leftBytes;
  ssize_t numberOfReadBytes;
  uint32_t numberOfBytes; // unsigned long int
  struct sockaddr_in saddr, daddr;
  char bufferQuery[70];
  char bufferReply[N];
  char comando[3 + lunghMaxFile + 1];
  FILE *fd;
  
  signal(SIGUSR1, sig_usr);
  
  close(pipefd[1]);
  
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
  
  trovatoSlash0 = 0;
  for (i = 0; i < 3 + lunghMaxFile + 1 && trovatoSlash0 == 0; ++i) {
    if (read(pipefd[0], (void*)(comando + i), 1) == -1) {
      printf("Errore nella read(): %s\n", strerror(errno));
      return 1;
    }
    if (comando[i] == '\0')
      trovatoSlash0 = 1;
  }
  
  while (strcmp(comando, "Q") != 0 && strcmp(comando, "A") != 0) {
    
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
    
    // 2. Ricezione messaggio di OK/ERR
    bufferReplyLast = -1;
    do {
      bufferReplyLast++;
      numberOfReadBytes = readn(sockfd, bufferReply + bufferReplyLast, 1); // 1 byte per volta
      if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
        if (close(sockfd) == -1)
          printf("Errore nella close(): %s\n", strerror(errno));
        return 1;
      }
    } while(bufferReply[bufferReplyLast] != '\n');
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
    if (readn(sockfd, (void*)&numberOfBytes, 4) <= 0) {
      if (close(sockfd) == -1)
        printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    numberOfBytes = ntohl(numberOfBytes); // network-to-host long
      
    // 4. Apertura del file
    fd = fopen(comando + 4, "w");
    leftBytes = numberOfBytes;
      
    while (leftBytes > 0) {
      // 5. Ricezione del file
      if (leftBytes < N - 1)
        numberOfReadBytes = readnConAbort(sockfd, (void*)bufferReply, leftBytes);
      else
        numberOfReadBytes = readnConAbort(sockfd, (void*)bufferReply, N - 1);
	
      if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
        fclose(fd);
        if (close(sockfd) == -1)
          printf("Errore nella close(): %s\n", strerror(errno));
        return 1;
      }
	
      if (numberOfReadBytes == -2) {
	printf("Ricevuto comando di interruzione immediata\n");
        fclose(fd);
        if (close(sockfd) == -1)
          printf("Errore nella close(): %s\n", strerror(errno));
        return 0;
      }
	  
      // 6. Scrittura del file
      leftBytes -= numberOfReadBytes;
      fwrite((void*)bufferReply, 1, numberOfReadBytes, fd);
    }
      
    // 7. Chiusura del file
    fclose(fd);
      
    trovatoSlash0 = 0;
    for (i = 0; i < 3 + lunghMaxFile + 1 && trovatoSlash0 == 0; ++i) {
      read(pipefd[0], (void*)(comando + i), 1);
      if (comando[i] == '\0')
	trovatoSlash0 = 1;
    }
  }
  
  if (strcmp(comando, "Q") == 0) {
    // 8. Invia messaggio di QUIT
    bufferQuery[0] = '\0';
    strcat(bufferQuery, "QUIT\r\n");
      
    if (writen(sockfd, bufferQuery, strlen(bufferQuery)) == -1)
      return 1;
  } // else: comando = "A"
  
  if (close(sockfd) == -1) {
    printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  return 0;
}

static void sig_usr (int s) {
  if (s == SIGUSR1) {
    stop = 1;
    printf("Ricevuto segnale SIGUSR1\n");
  }
}

size_t readnConAbort(int s, void *ptr, size_t len) {
  /* Valori di ritorno:
   * -2 = termina immediatamente (opzione A)
   * -1 = altro errore
   * 0 = orderly shutdown
   * > 0 = numero di caratteri letti
   */
  ssize_t nread;
  size_t nleft;
  
  nleft = len;
  while (nleft > 0) {
    if (stop == 1)
      return -2;
    
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