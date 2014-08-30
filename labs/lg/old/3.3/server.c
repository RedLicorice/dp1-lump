/*
 * argv[1] = porta
 * argv[2] = numero di figli
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define N 512
#define maxNumberOfChildren 10
#define maxNumberOfSeconds 10

pid_t childpids[maxNumberOfChildren - 1];
int numberOfChildren;

int figlio(int childNumber, int sockfd, struct sockaddr_in caddr, int porta);
int serviClient(int childNumber, int sockfd_copy, struct sockaddr_in caddr);
int chiudiClient(int sockfd_copy);
static void sig_int(int s);
size_t readn(int s, void *ptr, size_t len);
size_t writen(int s, const void *ptr, size_t nbytes);

int main(int argc, char *argv[]) {
  int sockfd, i;
  struct sockaddr_in saddr, caddr;
  pid_t childpid;
  
  numberOfChildren = atoi(argv[2]);
  if (numberOfChildren > maxNumberOfChildren) {
    printf("Questo server supporta un numero massimo di figli pari a %d\n", maxNumberOfChildren);
    return 1;
  }
  
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
  
  for (i = 0; i < numberOfChildren - 1; ++i) {
    childpid = fork();
    
    if (childpid == -1) {
      printf("Errore nella fork(): %s\n", strerror(errno));
      if (close(sockfd) == -1)
        printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    
    if (childpid == 0)
      return figlio(i + 1, sockfd, caddr, atoi(argv[1]));
    else
      childpids[i] = childpid;
  }
  
  if (signal(SIGINT, sig_int) == SIG_ERR) {
    printf("Errore nella signal(): %s\n", strerror(errno));
    return 1;
  }
  
  // Il padre diventa l'ultimo figlio
  return figlio(i + 1, sockfd, caddr, atoi(argv[1]));
}

int figlio(int childNumber, int sockfd, struct sockaddr_in caddr, int porta) {
  int sockfd_copy;
  socklen_t addrlen;
  
  addrlen = sizeof(struct sockaddr_in);
  while (1) {
    printf("(figlio %d)  Server in ascolto sulla porta %d...\n", childNumber, porta);
    sockfd_copy = accept(sockfd, (struct sockaddr*)&caddr, &addrlen);
    if (sockfd_copy == -1) {
      printf("Errore nella accept(): %s\n", strerror(errno));
      if (close(sockfd) == -1)
	printf("Errore nella close(): %s\n", strerror(errno));
      return 1;
    }
    
    if (serviClient(childNumber, sockfd_copy, caddr) == 1)
      return 1;
  }
}

int serviClient(int childNumber, int sockfd_copy, struct sockaddr_in caddr) {
  int bufferReplyLast, valido, numberOfElapsedSeconds;
  ssize_t numberOfReadBytes;
  char bufferReply[70], nomeFile[67], bufferQuery[N], byte;
  struct stat st;
  uint32_t fileSize;
  FILE *fd;
  struct timespec begin, end;
  
  clock_gettime(CLOCK_REALTIME, &begin);
  
  printf("(figlio %d)  Connessione accettata da un client con indirizzo = %s e porta = %d\n", childNumber, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
      
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
      printf("(figlio %d)  Comando illegale o file non esistente\n", childNumber);
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
    printf("(figlio %d)  Invio del file \"%s\" (%u byte) al client %s:%d in corso...\n", childNumber, nomeFile, fileSize, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
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
    printf("(figlio %d)  File \"%s\" correttamente inviato al client %s:%d\n", childNumber, nomeFile, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
	  
    clock_gettime(CLOCK_REALTIME, &end);
    numberOfElapsedSeconds = (int)(end.tv_sec - begin.tv_sec);
    if (numberOfElapsedSeconds > maxNumberOfSeconds) {
      printf("(figlio %d)  Tempo scaduto\n", childNumber);
      return chiudiClient(sockfd_copy);
    }
  }
}

int chiudiClient(int sockfd_copy) {
  if (close(sockfd_copy) == -1) {
    printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }
  
  return 0;
}

static void sig_int (int s) {
  int i;
  if (s == SIGINT) {
    printf("Segnale SIGINT ricevuto\n");
    for (i = 0; i < numberOfChildren - 1; ++i)
      kill(childpids[i], SIGINT);
    for (i = 0; i < numberOfChildren - 1; ++i)
      waitpid(childpids[i], NULL, 0);
    
    exit(0); // Questo killa il processo padre
    
    /* Alternativa: (dal manuale: If pid equals 0, then sig is sent to every process in the process group of the calling process.)
    kill(0, SIGINT);
    waitpid(0, NULL, 0);
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) // altrimenti continua a entrare in questa funzione
      printf("Errore nella signal(): %s\n", strerror(errno));
    */
  }
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