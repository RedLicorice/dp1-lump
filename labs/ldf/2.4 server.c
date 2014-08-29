#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"
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
server_task(int connfd){

  /* Main variables */
  uint16_t   op1, op2;  
  uint16_t   res;
  char       tbuf[BS+1];
  char       obuf[OBS+1];
  char       ibuf[IBS+1];
  int        quit;
  int        len;
  /* XDR */
  XDR        xdrs_in;
  XDR        xdrs_out;
  FILE       *xfp_r;
  FILE       *xfp_w;
  int        *xdr_op1, *xdr_op2, *xdr_res;


  /* XDR Bootstrap*/
  xfp_r = Fdopen(connfd, "r");
  xdrstdio_create(&xdrs_in, xfp_r, XDR_DECODE); 

  xfp_w = Fdopen(connfd, "w");
  xdrstdio_create(&xdrs_out, xfp_w, XDR_ENCODE); 

  setbuf(xfp_w, NULL);
  setbuf(xfp_r, NULL);

  /* Main */
  fprintf(stdout, "Client starts.\n");

  /* Get op1 and op2 from stdin */
  quit = 0;
  int count = 2;
  while(count-->0 && !quit){
    /* XDR transfer op1 and op2 */
    fprintf(stdout, "Sending message.\n");
    xdr_op1 = (int *)Malloc(sizeof(int));
    xdr_op2 = (int *)Malloc(sizeof(int));

    if(!xdr_int(&xdrs_in, xdr_op1)) 
      fprintf(stderr, "Error occurred while sending op1\n");
    if(!xdr_int(&xdrs_in, xdr_op2)) 
      fprintf(stderr, "Error occurred while sending op2\n");
    fprintf(stdout, "Message received correctly.\n");

    /* Validating input */
    // TODO
    fprintf(stdout, "Both operands are correct.\n");
    fprintf(stdout, "op1 = %d\n", *xdr_op1);
    fprintf(stdout, "op2 = %d\n", *xdr_op2);

    /* XDR receive res */
    xdr_res = (int *)malloc(sizeof(int));
    *xdr_res = *xdr_op1 + *xdr_op2;
    xdr_int(&xdrs_out, xdr_res);
    fprintf(stdout, "Server answered: %d.\n", *xdr_res);
  }
  xdr_destroy(&xdrs_in);
  xdr_destroy(&xdrs_out);
  fprintf(stdout, "Client ends.\n");

}

int main(int argc, char* argv[]){
  tcpsrv_select(argc, argv, 0, NULL, NULL, server_task);
  return 0;
}
