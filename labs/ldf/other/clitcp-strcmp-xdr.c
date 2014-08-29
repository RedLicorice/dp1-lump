#include "../../../lunpv13e/lib/lunp.h"
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <ctype.h>

#define IBS 1024
#define OBS 1024
#define BS    80

int
main(int argc, char **argv){
  /* Bootstrap variables */
  int                     sockfd;
  struct sockaddr_in      servaddr;
  /* Main variables */
  uint16_t   op1, op2;  
  uint16_t   res;
  char       tbuf[BS+1];
  char       obuf1[OBS+1];
  char       obuf2[OBS+1];
  char       ibuf[IBS+1];
  int        quit;
  int        len;
  /* XDR */
  XDR xdrs_in;
  XDR xdrs_out;
  FILE *xfp_r;
  FILE *xfp_w;
  char *xdr_obuf1;
  char *xdr_obuf2;
  char *xdr_ibuf;

  /* Bootstrap */
  if (argc != 3)
    err_quit("usage: %s <IPaddress> <Port>", argv[0]);

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

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
    fprintf(stdout, "Enter the first string:\n");
    fprintf(stdout, "> ");
    fgets(tbuf, BS, stdin);
    sscanf(tbuf, "%s", obuf1);
    fprintf(stdout, "Enter the second string:\n");
    fprintf(stdout, "> ");
    fgets(tbuf, BS, stdin);
    sscanf(tbuf, "%s", obuf2);
    /* Validating input */
    if(strncmp(obuf1, "QUIT", 4) == 0){
      fprintf(stdout, "Quitting the program.\n");
      quit = 1;
    }else{
      /* XDR transfer obuf1 and obuf2 */
      fprintf(stdout, "Sending message.\n");
      xdr_obuf1 = strdup(obuf1);
      xdr_obuf2 = strdup(obuf2);
      if(!xdr_string(&xdrs_out, &xdr_obuf1, OBS)) 
	fprintf(stderr, "Error occurred while sending obuf1\n");
      if(!xdr_string(&xdrs_out, &xdr_obuf2, OBS)) 
	fprintf(stderr, "Error occurred while sending obuf2\n");
      fprintf(stdout, "Message correctly sent.\n");

      /* XDR receive res */
      xdr_ibuf = (char *)Malloc(OBS*sizeof(char));
      if(!(xdr_string(&xdrs_in, &xdr_ibuf, IBS)))
	fprintf(stdout, "Error: no answer has been received.\n");
      else
	fprintf(stdout, "Server answered: %s.\n", xdr_ibuf);
    }
  }
  xdr_destroy(&xdrs_in);
  xdr_destroy(&xdrs_out);
  fprintf(stdout, "Client ends.\n");
  exit(0);
}
