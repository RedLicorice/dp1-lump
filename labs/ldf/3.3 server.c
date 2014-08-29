#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"

#define IBS 1024
#define OBS 1024
#define BS    80

void
sig_int(int signo){
  int i;

  fprintf(stdout, "In the SIG_INT handler\n");

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
sig_chld(int signo)
{
  /* 
     To keep alive always N children the actual signal handler is performed
     by the father.
     The father blocks on a wait which is inside a never ending loop.
  */
  fprintf(stdout, "In the SIG_CHLD handler\n");
}

void
sig_usr1(int signo){
  /*
    Pay attention: SIGUSR1 calls also SIGTERM!
   */
  fprintf(stdout, "In the SIG_USR1 handler\n");
}

void
child_task(int connfd){
  /* Socket variables */
  char ibuf[IBS+1];
  char obuf[OBS+1];
  int size;
  size_t n;
  size_t tot=0;

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
  strcpy(path, "/Users/ldf/Desktop/srv/");
  
  fprintf(stdout, "Server starts.\n");

  /* Main loop MENU */
  while (!flag_quit) {
    fprintf(stdout, "* Ready to receive a command\n");
    n = Readline(connfd, ibuf, IBS);
    fprintf(stdout, "> Message: %7s\n", ibuf);

    // The readline returns n=1 
    if(ibuf[0] == '\0'  || strncmp(ibuf, "QUIT", 4)==0){ //Close the client's socket
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
      ifp = fopen(ifn, "rb");
            
      if (ifp==NULL) {
	fprintf(stderr, "  File not found!\n");
	strcpy(obuf, "-ERR\n");
	Writen(connfd, obuf, strlen(obuf));
      }else{
	strcpy(obuf, "+OK\n");
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
  void ( *sigv[3] ) (int) = {sig_int, sig_chld, sig_usr1};
  int intv[3] = {SIGINT, SIGCHLD, SIGUSR1};
  fprintf(stdout, "Main process starts.\n");
  tcpsrv_pre(argc, argv, 3, sigv, intv, child_task);
  fprintf(stdout, "Main process ends.\n");
  return 0;
}
