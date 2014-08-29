#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"
#include "xdr_ftp.h"

#include <rpc/types.h>
#include <rpc/xdr.h>
#include <string.h>

#define IBS 1024
#define OBS 1024
#define BS    80
#define LS    80

void
client_task(int connfd){
  /* Socket variables */
  char obuf[OBS+1];
  char ibuf[IBS+1];
  ssize_t n;
  u_int32_t size;

  /* XDR variables */
  XDR xdrs_in;
  XDR xdrs_out;
  enum dataselect ds;
  struct datatype idt;
  struct datatype odt;
  FILE *xfp_r;
  FILE *xfp_w;

  /* Internal variables */
  char tbuf[BS+1];
  char ifn[BS+1];
  char line[LS+1];
  FILE *ifp;

  /* Flags*/
  int flag_quit = 0;

  /* Indexes */
  int i;
    
  /* Path */
  strcpy(ifn, "/home/ldf/tmp/cli/");

  /* XDR: linking xdrs to the socket */
  idt.datatype_u.filename = (char *)malloc(MAXNAMELEN * sizeof(char));
  odt.datatype_u.filename = (char *)malloc(MAXNAMELEN * sizeof(char));

  xfp_r = Fdopen(connfd, "r");
  xdrstdio_create(&xdrs_in, xfp_r, XDR_DECODE);
  
  xfp_w = Fdopen(connfd, "w");
  xdrstdio_create(&xdrs_out, xfp_w, XDR_ENCODE); // create stream

  setbuf(xfp_w, NULL);
  setbuf(xfp_r, NULL);

  fprintf(stdout, "Client starts.\n");
  fprintf(stdout, "MENU'\n");
  fprintf(stdout, "GETfilename  -- to request a file\n");
  fprintf(stdout, "QUIT         -- to exit\n");

  /* Main loop MENU */
  while (!flag_quit) {

    idt.datatype_u.filename = (char *)Malloc(MAXNAMELEN * sizeof(char));
    odt.datatype_u.filename = (char *)Malloc(MAXNAMELEN * sizeof(char));

    fprintf(stdout, "> ");
    Fgets(line, BS, stdin);
    sscanf(line, "%s", tbuf);
    strcat(tbuf, "\n\0");

    /* Convert the command in XDR format */

    if(strncmp(tbuf, "GET", 3)==0){
      // OUT 2 PREPARE
      odt.select = COMMAND;
      odt.datatype_u.filename = strdup(tbuf);
      xdr_datatype(&xdrs_out, &odt);

      /* Convert the response in XDR format */

      // IN 2 RECEIVE
      xdr_datatype(&xdrs_in, &idt);
      strcpy(ibuf, idt.datatype_u.filename);
      printf("command: %s\n", idt.datatype_u.filename);

      /* Answer checking */
      if(strncmp(ibuf, "-ERR", 3)==0){
	fprintf(stdout, "File error!\n");
      }else if(strncmp(ibuf, "+OK", 3)==0){
	fprintf(stdout, "+OK receiving data!\n");

	/* Opening the file */
	strncat(ifn, tbuf+3, strlen(tbuf)-4);
	ifp = fopen(ifn, "wb");
	if (ifp==NULL) {
	  fprintf(stderr, "File opening error!\n");
	  return;
	}

	// IN 2 RECEIVE
	xdr_datatype(&xdrs_in, &idt);
	size = idt.datatype_u.data;

	fprintf(stdout, "Size: %zd\n", size);

	/* file transfer */
	ssize_t received;
	ssize_t received_tot=0;
	long how_many;

	while (received_tot<size) {
	  fprintf(stdout, "Byteleft %ld\n", size-received_tot);

	  /* This prevent starvation */
	  if ((size-received_tot)>IBS) {
	    how_many = IBS-1;
	  }else{
	    how_many = size-received_tot;
	  }
	  received = Readn(connfd, ibuf, how_many);
	  printf("received %zd\n", received);
	  fwrite(ibuf, 1, received, ifp);
	  received_tot+=received;
	}
	fclose(ifp);
	fprintf(stdout, "File transfer completed!\n");
      }
            
    }else if(strncmp(tbuf, "QUIT", 4)==0){
      flag_quit = 1;
      /* Send the QUIT command back to the server*/
      odt.select = COMMAND;
      //strcpy(odt.datatype_u.filename, "QUIT\n");
      odt.datatype_u.filename = strdup("QUIT\n");
      xdr_datatype(&xdrs_out, &odt);
    }else{
      fprintf(stdout, "Non recognized command\n");
    }
  }
  xdr_destroy(&xdrs_in);
  xdr_destroy(&xdrs_out);
  fclose(ifp);
  fprintf(stdout, "Client ends.\n");
  return;
}

int main(int argc, char *argv[]){
  tcpcli_simple(argc, argv, client_task);
  return 0;
}
