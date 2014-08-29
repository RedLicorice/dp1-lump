#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"
#include "xdr_ftp.h"

#include <rpc/xdr.h>
#include <string.h>

//NOTES
//1. readline functions exactly like fgets -> always check for first char '\0'

//TODO
//1. handle sign_int
//2. more graceful leaving
//3. fixing crashing menu

#define IBS 1024
#define OBS 1024
#define BS    80

void
sig_int(int signo){
  int i;

  fprintf(stdout, "\n\n");
  fprintf(stdout, "In the SIG_INT handler\n");
  fprintf(stdout, "\n\n");

  /* terminate all children */
  for (i = 0; i < nchildren; i++)
    kill(pids[i], SIGTERM);
  while (wait(NULL) > 0)          /* wait for all children */
    ;
  if (errno != ECHILD)
    err_sys("wait error");

  exit(0);
}

void
sig_usr1(int signo){
  fprintf(stdout, "\n\n");
  fprintf(stdout, "In the SIG_USR1 handler\n");
  fprintf(stdout, "\n\n");
}

void
child_task(int connfd){
  /* Socket variables */
  char ibuf[IBS+1];
  char obuf[OBS+1];
  unsigned int obuf_pos;
  int size;
  size_t n;
  size_t tot=0;

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
  char path[BS+1];
  FILE *ifp;
  int fsize;

  /* Flags*/
  int flag_quit = 0;

  /* Indexes */
  int i;
            
  /* Path */
  strcpy(path, "/home/ldf/tmp/srv/");

  /* XDR: linking xdrs to the socket */
  xfp_r = Fdopen(connfd, "r");
  xdrstdio_create(&xdrs_in, xfp_r, XDR_DECODE); 

  xfp_w = Fdopen(connfd, "w");
  xdrstdio_create(&xdrs_out, xfp_w, XDR_ENCODE); 

  setbuf(xfp_w, NULL);
  setbuf(xfp_r, NULL);

  idt.datatype_u.filename = (char *)Malloc(MAXNAMELEN * sizeof(char));
  odt.datatype_u.filename = (char *)Malloc(MAXNAMELEN * sizeof(char));

  fprintf(stdout, "Server starts.\n");

  /* Main loop MENU */
  while (!flag_quit) {

    idt.datatype_u.filename = (char *)malloc(MAXNAMELEN * sizeof(char));
    odt.datatype_u.filename = (char *)malloc(MAXNAMELEN * sizeof(char));

    fprintf(stdout, "* Ready to receive a command\n");

    /* XDR: converting the client command*/
    xdr_datatype(&xdrs_in, &idt);
    
    if(idt.select == COMMAND){
      strcpy(ibuf, idt.datatype_u.filename);
      fprintf(stdout, "> Message: %7s\n", ibuf);

      if(ibuf[0] == '\0'  || strncmp(ibuf, "QUIT", 4)==0){ //Close the client's socket
	printf("Command: %s\n", ibuf);
	printf("Closing the connection!\n");
	flag_quit = 1;
      } else if (strncmp(ibuf, "GET", 3)==0) {
	fprintf(stdout, "* Requested a file\n");
	for (i=3; i<strlen(ibuf) 
	       && ibuf[i]!=' '
	       && ibuf[i]!='\n'; i++) {
	  tbuf[i-3]=ibuf[i];
	}
	tbuf[i-3]='\0';
            
	strcpy(ifn, path);
	strcat(ifn, tbuf);
	fprintf(stdout, "nome file: %s\n", ifn);

	ifp = fopen(ifn, "rb"); //should NOT use Fopen
	if (ifp==NULL) {
	  fprintf(stderr, "  File NOT found!\n");

	  // OUT 2 PREPARE AND SEND
	  odt.select = COMMAND;
	  //	  strcpy(odt.datatype_u.filename, "-ERR\n");
	  odt.datatype_u.filename = strdup("-ERR\n");

	  xdr_datatype(&xdrs_out, &odt);
	  
	}else{
	  fprintf(stderr, "  File found!\n");
	  
	  // OUT 2 PREPARE AND SEND
	  odt.select = COMMAND;
	  //	  strcpy(odt.datatype_u.filename, "+OK\n");
	  odt.datatype_u.filename = strdup("+OK\n");

	  xdr_datatype(&xdrs_out, &odt);
	  
	  /* Get size of the file */
	  struct stat st;
	  stat(ifn, &st);
	  size = st.st_size;
	  fsize = size;
	  fprintf(stdout, "  File's size: %d\n", size);

	  // OUT 2 PREPARE (using the OUT 1)
	  odt.select = DATA;
	  odt.datatype_u.data = size;
	  xdr_datatype(&xdrs_out, &odt);

	  memset(obuf, '\0', OBS);

	  while ((n=fread(obuf, 1, OBS, ifp))) {
	    Writen(connfd, obuf, (int)n); //Check the cast... 
	    tot += n;
	    fprintf(stdout, "Sending %zu Bytes\n", n);
	  }
	  fprintf(stdout, "Total bytes trasfered: %zd/%d\n", tot, fsize);
	  fprintf(stdout, "Transfer is completed!\n");
	}
      }else{ //invalid command
	fprintf(stdout, "Non recognised command!\n");
      }
    }else{ //not a command
      fprintf(stdout, "Non recognised XDR message!\n");
      break;
    }
  }
  /* Free data */
  xdr_destroy(&xdrs_in);
  xdr_destroy(&xdrs_out);
  //  Fclose(xfp_r);
  //  Fclose(xfp_w);
  Fclose(ifp);
  fprintf(stdout, "Server quits.\n");
  return;
}

int main(int argc, char *argv[]){
  void ( *sigv[2] ) (int) = {sig_int, sig_usr1};
  int intv[2] = {SIGINT, SIGUSR1};

  tcpsrv_pre(argc, argv, 2, sigv, intv, child_task, 2);
  return 0;
}
