#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/cli.h"

#define IBS 1024
#define OBS 1024
#define FALSE 0
#define TRUE  1
#define BS    80

void
client_task(int connfd){
  /* Socket variables */
  char obuf[OBS+1];
  char ibuf[IBS+1];
  ssize_t n;
  u_int32_t size;

  /* Internal variables */
  char tbuf[BS+1];
  char ifn[BS+1];
  char command[BS+1];
  FILE *ifp;

  /* Flags*/
  int flag_quit = FALSE;

  /* Indexes */
  int i;
    
  /* Initialize the bread struct*/
  bread_t *bread;
  Bread_init(&bread, connfd);
  /* Path */
  strcpy(ifn, "/Users/ldf/Desktop/cli/");

  fprintf(stdout, "Client starts.\n");
  fprintf(stdout, "MENU'\n");
  fprintf(stdout, "GETfilename  -- to request a file\n");
  fprintf(stdout, "QUIT         -- to exit\n");

  /* Main loop MENU */
  while (!flag_quit) {
    fprintf(stdout, "> ");
    fscanf(stdin, "%s", obuf);
    strcat(obuf, "\n\0");
        
    if(strncmp(obuf, "GET", 3)==0){
      Writen(connfd, obuf, strlen(obuf));

      /* +OK\n\0 -->  5 characters */
      /* -ERR\n\0 --> 6 characters */
      Breadline(bread, ibuf, 6);

      /* Answer checking */
      if(strncmp(ibuf, "-ERR", 3)==0){
	fprintf(stdout, "File error!\n");
	Breadline(bread, ibuf, 1); //+ERR -> 1 character more than OK
      }else if(strncmp(ibuf, "+OK", 3)==0){
	fprintf(stdout, "+OK receiving data!\n");

	/* Opening the file */
	strncat(ifn, obuf+3, strlen(obuf)-4);
	ifp = fopen(ifn, "wb");
	if (ifp==NULL) {
	  fprintf(stderr, "File opening error!\n");
	  return;
	}

	/* Read the size */
	Breadn(bread, ibuf, 4);
	memcpy(&size, ibuf, 4);
	size = ntohl(size);
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
	  received = Breadn(bread, ibuf, how_many);
	  printf("received %zd\n", received);
	  fwrite(ibuf, 1, received, ifp);
	  received_tot+=received;
	}
	fclose(ifp);
	fprintf(stdout, "File transfer completed!\n");
      }
            
    }else if(strncmp(obuf, "QUIT", 4)==0){
      flag_quit = TRUE;
      /* Send the QUIT command back to the server*/
      /* Closes the connection */
      Writen(connfd, obuf, strlen(obuf));
    }else{
      fprintf(stdout, "Non recognized command\n");
    }
        
  }
  fprintf(stdout, "Client ends.\n");
  return;
}



int main(int argc, char *argv[]){
  tcpcli_simple(argc, argv, client_task);
  return 0;
}
