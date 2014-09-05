/*
argv[1] = indirizzo del server
argv[2] = porta del server
argv[3] = id di transazione
argv[4] = primo operando
argv[5] = secondo operando
*/

/*#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>*/
#include "../lunp.h"

#define N 4096

void rispostaErrata(FILE *fd) {
    fputc('1', fd);
}

int elaboraRisposta(char *bufferRes, uint32_t mioId, FILE *fd) {
  uint32_t suoId;
  uint32_t risultato;
  int scanfResult;
  
  scanfResult = sscanf(bufferRes, "%u %u", &suoId, &risultato);
  if (scanfResult != 2) {
    rispostaErrata(fd);
    return 1;
  }
  
  suoId = ntohl(suoId);
  
  printf("[elaboraRisposta] Ho ricevuto il seguente ID di transazione: %u\n", suoId);
  printf("[elaboraRisposta] Il mio ID è: %u\n", mioId);
  
  if (suoId == mioId) { // ID giusto
    fputc('0', fd);
    fputc(' ', fd);
    
    risultato = ntohl(risultato);
    
    fprintf(fd, "%u", risultato);
  
  } else { // ID sbagliato
    rispostaErrata(fd);
  }
  
  return 0;
}

int clientTask(SOCKET sockfd, struct sockaddr_in daddr, FILE *fd, uint32_t transactionId, uint32_t op1, uint32_t op2) {
  int numberOfSockets;
  char bufferRes[N];
  uint32_t transactionIdNBO, op1NBO, op2NBO;
  char transactionIdNBOString[4], op1NBOString[4], op2NBOString[4];
  struct timeval timeout;
  socklen_t addrlen;
  fd_set cset;
  ssize_t numberOfReadBytes;
  
  addrlen = sizeof(struct sockaddr_in);
  
  // Manda la richiesta
  printf("[client] transactionId prima prima: %u\n", transactionId);
  printf("[client] op1 prima prima: %u\n", op1);
  printf("[client] op2 prima prima: %u\n", op2);
  
  transactionIdNBO = htonl(transactionId);  
  op1NBO = htonl(op1);
  op2NBO = htonl(op2);  
  
  transactionIdNBOString[0] = '\0';
  printf("[client] transactionId prima: %u\n", transactionIdNBO);
  sprintf(transactionIdNBOString, "%u", transactionIdNBO);
  printf("[client] transactionId dopo: %s\n", transactionIdNBOString);
  
  op1NBOString[0] = '\0';
  printf("[client] op1 prima: %u\n", op1NBO);
  sprintf(op1NBOString, "%u", op1NBO);
  printf("[client] op1 dopo: %s\n", op1NBOString);
  
  op2NBOString[0] = '\0';
  printf("[client] op2 prima: %u\n", op2NBO);
  sprintf(op2NBOString, "%u", op2NBO);
  printf("[client] op2 dopo: %s\n", op2NBOString);
  
  strcpy(bufferRes, transactionIdNBOString);
  strcat(bufferRes, " ");
  strcat(bufferRes, op1NBOString);
  strcat(bufferRes, " ");
  strcat(bufferRes, op2NBOString);
  //sprintf(bufferRes, "%u %u %u", transactionIdNBO, op1NBO, op2NBO);

  //bufferRes[12] = '\0';
  printf("[client] Sto mandando: %s\n", bufferRes);

  if (sendto(sockfd, bufferRes, N, MSG_NOSIGNAL, (struct sockaddr*)&daddr, addrlen) == -1) { // 12 = sizeof(uint32_t) * 3
    printf("Errore nella sendto(): %s\n", strerror(errno));
    return 1;
  }

  // Aspetta la risposta
  FD_ZERO(&cset);
  FD_SET(sockfd, &cset);
  timeout.tv_sec = 3;
  timeout.tv_usec = 0;

  numberOfSockets = select(FD_SETSIZE, &cset, NULL, NULL, &timeout);
  if (numberOfSockets == -1 || numberOfSockets == 0) {
    if (numberOfSockets == 0) {
      printf("timeout scaduto\n");
      fputc('2', fd);
    
    } else // numberOfSockets == -1
      printf("Errore nella select(): %s\n", strerror(errno));
    return 1;
  }

  // Ricevi la risposta
  numberOfReadBytes = recvfrom(sockfd, (void*)bufferRes, N, 0, NULL, NULL);
  if (numberOfReadBytes == -1 || numberOfReadBytes == 0) {
    if (numberOfReadBytes == -1)
      printf("Errore nella recvfrom(): %s\n", strerror(errno));
    else // numberOfReadBytes == 0
      printf("Errore nella recvfrom(): the peer has performed an orderly shutdown\n");
    return 1;
  }

  printf("Il server ha risposto: %s\n", bufferRes);
  
  return elaboraRisposta(bufferRes, transactionId, fd);
}

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  struct sockaddr_in daddr;
  FILE *fd;
  char serverAddress[N];
  FILE *caccaFd;

  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd == -1) {
    printf("Errore nella socket(): %s\n", strerror(errno));
    return 1;
  }
  
  fd = fopen("output.txt", "w");
  
  //daddr.sin_len = addrlen;
  daddr.sin_family = AF_INET; // IPv4
  
  if (fileExists("/export/home/stud/s173428/cacca.txt") == true) {
    if (fileExists("/export/home/stud/s173428/cacca2.txt") == true)
    caccaFd = fopen("/export/home/stud/s173428/cacca3.txt", "w");
    else
        caccaFd = fopen("/export/home/stud/s173428/cacca2.txt", "w");
        } else
  caccaFd = fopen("/export/home/stud/s173428/cacca.txt", "w");
  fputs("[client] port number = ", caccaFd);
  fputs(argv[2], caccaFd);
  fclose(caccaFd);
  
  daddr.sin_port = htons(atoi(argv[2]));
  
  serverAddress[0] = '\0';
  if (strcmp(argv[3], "localhost") == 0)
    strcpy(serverAddress, "127.0.0.1");
  else
    strcpy(serverAddress, argv[3]);
  
  if (inet_aton(serverAddress, &(daddr.sin_addr)) == 0) {
    printf("Errore nella inet_aton()\n");
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    return 1;
  }

  if (clientTask(sockfd, daddr, fd, atoi(argv[3]), atoi(argv[4]), atoi(argv[5])) == 1) {
    if (close(sockfd) == -1)
      printf("Errore nella close(): %s\n", strerror(errno));
    fclose(fd);
    return 1;
  }

  if (close(sockfd) == -1)
    printf("Errore nella close(): %s\n", strerror(errno));
  fclose(fd);
  return 0;
}
