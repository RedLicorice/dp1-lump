#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"


//NOTES
//1. readline functions exactly like fgets -> always check for first char '\0'

//TODO
//1. handle sign_int
//2. more graceful leaving
//3. fixing crashing menu

#define IBS 1024
#define OBS 1024
#define FALSE 0
#define TRUE  1
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
  u_int32_t size;
  size_t n;
  size_t tot=0;

  /* Internal variables */
  char tbuf[BS+1];
  char ifn[BS+1];
  char path[BS+1];
  FILE *ifp;
  int fsize;

  /* Flags*/
  int flag_quit = FALSE;

  /* Indexes */
  int i;
            
  /* Path */
  strcpy(path, "/Users/ldf/Desktop/srv/");
  
  fprintf(stdout, "Server starts.\n");

  /* Main loop MENU */
  while (!flag_quit) {
    fprintf(stdout, "* Ready to receive a command\n");
    n = Readexp(connfd, ibuf, IBS);
    fprintf(stdout, "> Message: %7s\n", ibuf);

    // The readline returns n=1 
    if(ibuf[0] == '\0'  || strncmp(ibuf, "QUIT", 4)==0){ //Close the client's socket
      printf("Closing the connection!\n");
      flag_quit = TRUE;
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
      ifp = fopen(ifn, "rb");
            
      if (ifp==NULL) {
	fprintf(stderr, "  File not found!\n");
	strcpy(obuf, "-ERR\r\n");
	Writen(connfd, obuf, strlen(obuf));
      }else{
	strcpy(obuf, "+OK\r\n");
	Writen(connfd, obuf, strlen(obuf));

	/* Get size of the file */
	struct stat st;
        stat(ifn, &st);
        size = st.st_size;
	fsize = size;
	fprintf(stdout, "File's size: %d\n", size);

	size = htonl(size);

	memcpy(obuf, &size, sizeof(size));
	Writen(connfd, obuf, sizeof(size));

	while ((n=fread(obuf, 1, OBS, ifp))) {
	  Writen(connfd, obuf, (int)n); //Check the cast... 
	  tot += n;
	}
	fprintf(stdout, "Total bytes trasfered: %zd/%d\n", tot, fsize);
	fprintf(stdout, "Transfer is completed!\n");
      }
    }else{ //default
      fprintf(stdout, "Non recognised message!\n");
    }
  }
  fprintf(stdout, "Server quits.\n");
  return;
}

int main(int argc, char *argv[]){
  void ( *sigv[2] ) (int) = {sig_int, sig_usr1};
  int intv[2] = {SIGINT, SIGUSR1};
  tcpsrv_pre(argc, argv, 2, sigv, intv, child_task);
  return 0;
}
