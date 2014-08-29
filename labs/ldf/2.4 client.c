#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/cli.h"

#include <ctype.h>

#include <rpc/types.h>
#include <rpc/xdr.h>

#define IBS 1024
#define OBS 1024
#define BS    80

int get_uint16(char * tbuf, uint16_t *uint16){
  int i;
  int value;
  int max_value = 65536-1;                // 2^16-1
  /* Checking if it is below 5 char */
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

void
client_task(int sockfd){
  /* Bootstrap variables */
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
  /* XDR */
  XDR xdrs_in;
  XDR xdrs_out;
  FILE *xfp_r;
  FILE *xfp_w;
  int *xdr_op1, *xdr_op2, *xdr_res;

  /* XDR Bootstrap*/
  xfp_r = Fdopen(sockfd, "r");
  xdrstdio_create(&xdrs_in, xfp_r, XDR_DECODE); 

  xfp_w = Fdopen(sockfd, "w");
  xdrstdio_create(&xdrs_out, xfp_w, XDR_ENCODE); 

  setbuf(xfp_w, NULL);
  setbuf(xfp_r, NULL);

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
    /* Validating input */
    if(strncmp(op1buf, "QUIT", 4) == 0){
      fprintf(stdout, "Quitting the program.\n");
      quit = 1;
    }else if( !(get_uint16(op1buf, &op1) && get_uint16(op2buf, &op2)) ){
      fprintf(stdout, "There is an error in one or both of the operands.\n");
    }else{
      fprintf(stdout, "Both operands are correct.\n");
      fprintf(stdout, "op1 = %hu\n", op1);
      fprintf(stdout, "op2 = %hu\n", op2);

      /* XDR transfer op1 and op2 */
      fprintf(stdout, "Sending message.\n");
      xdr_op1 = (int *)Malloc(sizeof(int));
      *xdr_op1 = op1;
      xdr_op2 = (int *)Malloc(sizeof(int));
      *xdr_op2 = op2;
      if(!xdr_int(&xdrs_out, xdr_op1)) fprintf(stderr, "Error occurred while sending op1\n");
      if(!xdr_int(&xdrs_out, xdr_op2)) fprintf(stderr, "Error occurred while sending op2\n");
      fprintf(stdout, "Message correctly sent.\n");

      /* XDR receive res */
      xdr_res = (int *)malloc(sizeof(int));
      xdr_int(&xdrs_in, xdr_res);
      res = *xdr_res;
      fprintf(stdout, "Server answered: %d.\n", *xdr_res);
    }
  }
  xdr_destroy(&xdrs_in);
  xdr_destroy(&xdrs_out);
  fprintf(stdout, "Client ends.\n");
  exit(0);
}

int main(int argc, char* argv[]){
  tcpcli_simple(argc, argv, client_task);
  return 0;
}
