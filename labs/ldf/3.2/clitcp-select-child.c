#include "../../../lunpv13e/lib/lunp.h"

#define IBS 1024
#define OBS 1024
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
  pid_t pid;

  /* Flags*/
  int flag_quit = 0;

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

  pid = -1; // No child at bootstrap

  /* Main loop MENU */
  while (!flag_quit) {
    fprintf(stdout, "> ");
    fscanf(stdin, "%s", obuf);
    strcat(obuf, "\n\0");
        
    if(pid == -1 && strncmp(obuf, "GET", 3)==0){
      Writen(connfd, obuf, strlen(obuf));

      /* +OK\n\0 -->  5 characters */
      /* -ERR\n\0 --> 6 characters */
      Breadline(bread, ibuf, 6);

      /* Answer checking */
      if(strncmp(ibuf, "-ERR", 3)==0){
	fprintf(stdout, "File error!\n");
	Breadline(bread, ibuf, 1); //+ERR -> 1 character more than OK
      }else if(strncmp(ibuf, "+OK", 3)==0){

	/*
	  Fork. The child will handle the actual file transfer.
	*/
	if((pid = fork())!=0){
	  /* FATHER */
	  fprintf(stdout, "Child %d has been forked.\n", pid);
	  fprintf(stdout, "He will send you the file.\n");
	  fprintf(stdout, "I'm the father and I will handle your commands.\n");
	}else{
	  /* CHILD */
	  fprintf(stdout, "+OK receiving data!\n");

	  /* Opening the file */
	  strncat(ifn, obuf+3, strlen(obuf)-4);
	  fprintf(stdout, "FNAME %s\n", ifn);
	  ifp = fopen(ifn, "wb");
	  if (ifp==NULL) {
	    fprintf(stderr, "File opening error!\n");
	    return;
	  }

	  /* Read the size */
	  Breadn(bread, ibuf, 4);
	  memcpy(&size, ibuf, 4);
	  size = ntohl(size);
	  //fprintf(stdout, "Size: %zd\n", size);

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
	    fprintf(stdout, "received %zd\n", received);
	    fwrite(ibuf, 1, received, ifp);
	    received_tot+=received;
	  }
	  fclose(ifp);
	  fprintf(stdout, "File transfer completed!\n");
	  exit (0);
	}
      }
            
    }else if(pid == -1 && strncmp(obuf, "QUIT", 4)==0){
      flag_quit = 1;
      /* Send the QUIT command back to the server*/
      /* Closes the connection */
      Writen(connfd, obuf, strlen(obuf));
    }else if(strncmp(obuf, "Q", 1)==0){
      /* WAIT for the child to finish the file transfer */
      fprintf(stdout, "Executing command Q\n");
      wait(NULL);
      flag_quit = 1;
      Writen(connfd, obuf, strlen(obuf));
    }else if(strncmp(obuf, "A", 1)==0){
      fprintf(stdout, "Executing command A\n");
      kill(pid, SIGTERM);
      wait(NULL);
      flag_quit = 1;
    }else{
      fprintf(stdout, "Non recognized command\n");
    }
  }
  fprintf(stdout, "Client ends.\n");
  return;
}



int main(int argc, char *argv[]){
  int			sockfd;
  struct sockaddr_in	servaddr;

  if (argc != 2)
    err_quit("usage: tcpcli <IPaddress>");

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  
  /* Perform the task - do all*/
  client_task(sockfd);

  Close(sockfd);

  return 0;
}
