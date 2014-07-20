#include "../../../lunpv13e/lib/lunp.h"
#include <ctype.h>

#define IBS 1024
#define OBS 1024
#define BS    80

int get_uint16(char * tbuf, uint16_t *uint16){
  int i;
  int value;
  int max_value = 65536-1;                // 2^16-1
  /* Checking if it is below 5 char */
  printf("%s %d\n", tbuf, strlen(tbuf));
  if(strlen(tbuf) == 0 || strlen(tbuf) > 5) return 0;
  /* Checking if tbuf is a number */
  i = 0;
  while(tbuf[i]!='\0' && isdigit(tbuf[i])) i++;
  if(i != strlen(tbuf)) return 0;      //FAIL tbuf is not even a number
  /* Checking parsing it as an integer */
  sscanf(tbuf, "%d", &value);
  if(value > max_value) return 0;
  sscanf(tbuf, "%hu", uint16);
  
  return 1;
}

int
main(int argc, char **argv){
  /* Bootstrap variables */
  int                     sockfd;
  struct sockaddr_in      servaddr;
  /* Main variables */
  uint16_t   op1, op2;  
  uint16_t   res;
  char       tbuf[BS+1];
  char       obuf[OBS+1];
  char       ibuf[IBS+1];
  char       op1buf[BS+1];
  char       op2buf[BS+1];
  int        quit;
  int        len;

  /* Bootstrap */
  if (argc != 3)
    err_quit("usage: %s <IPaddress> <Port>", argv[0]);

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

  /* Main */
  fprintf(stdout, "Client starts.\n");

  /* Get op1 and op2 from stdin */
  quit = 0;
  while(!quit){
    op1 = 0; op1buf[0] = '\0';
    op2 = 0; op2buf[0] = '\0';
    fprintf(stdout, "Enter the two operands (unsigned int 16):\n");
    fprintf(stdout, "> ");
    fgets(tbuf, BS, stdin);
    sscanf(tbuf, "%s %s\n", op1buf, op2buf);
    if(strncmp(op1buf, "QUIT", 4) == 0){
      fprintf(stdout, "Quitting the program.\n");
      quit = 1;
    }else if( !(get_uint16(op1buf, &op1) && get_uint16(op2buf, &op2)) ){
      fprintf(stdout, "There is an error in one or both of the operands.\n");
    }else{
      fprintf(stdout, "Both operands are correct.\n");
      fprintf(stdout, "op1 = %hu\n", op1);
      fprintf(stdout, "op2 = %hu\n", op2);

      /* Transfer input */
      /* Raw data encoding: 'op1\ op2\r\n\0' */
      len = strlen(op1buf);
      op1buf[len] = ' ';
      op1buf[len+1] = '\0';
      strcpy(obuf, op1buf);
      strcat(obuf, op2buf);
      strcat(obuf, "\r\n\0");
      fprintf(stdout, "Sending message.\n");

      /* Sending obuf */
      Writen(sockfd, obuf, strlen(obuf));
      fprintf(stdout, "Message correctly sent.\n");

      /* Waiting for an answer */
      Readexp(sockfd, ibuf, IBS);
      sscanf(ibuf, "%s", tbuf);
      fprintf(stdout, "Server answered: %s.\n", tbuf);
    }
  }
  fprintf(stdout, "Client ends.\n");
  exit(0);
}
