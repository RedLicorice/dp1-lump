/* WHAT TO DO
 *
 * update PARAM_NUM
 * update PATH_BASE
 
 
 /* NOTE
 *
 * Readline:
 * Legge fino al carattere \n o al massimo n-1 byte.
 * Nell'n-esimo (partendo da uno) mette sempre il carattere di terminazione di stringa.
 * Ritorna il numero di caratteri letti senza contare il \0 finale.
 *
 *
 * Protocol:
 * The protocol expects that each command is sent using the \n.
 * The function Readline is used to parse those comand.
 * The \0 cannot be send, otherwise the Readline function doesn't work.
 *
 * Worker ending:
 * In the worker function is better to use err_ret instead of err_quit
 * in order to release correctly the resources.
 *
 **/




// By now the protocol use the \n only.
// Cambiare la scanf per l'inserimento del comando.
// Controllare se i wrapper di stevens ritornano anche l'errore (anche se gi' gestito) o no..
// Controllare che cosa succede se l'utente inserisce il / all'inizio del file. 
// Controllare come gestisce le risorse in caso di chiusura del server...
// Stesso di prima in casdo del client.

// provare a mettere come nel server un buffer di dimensione maggiore per la dimensione del comando da ricevere
// controllare cosa succede quando il server viene chiuso mentre il client sta lavorando.

// controllare che cosa fa fgets quando prende un comando

#include    "../../../lunpv13e/lib/lunp.h"

#define PARAM_NUM  2
#define PATH_BASE "/home/ldf/tmp/cli/"


/* client side -- lab 3.1 */
int main(int argc, char *argv[]){
    
  if(argc < PARAM_NUM+1){
    err_quit("usage: ./program_name server_address server_port");
  }
    
  u param;
    
  param.client_connect_TCP.server_address = argv[1];
  param.client_connect_TCP.server_port=atoi(argv[2]);
  param.client_connect_TCP.worker = worker;
    
  client_connection_TCP(& param.client_connect_TCP);
    
  return 0;
}


void worker(int fd){
    
  char buffer_send[BUFFER_SEND];
  char buffer_rcv[BUFFER_RECV];
  char nomefile[100];
  char temp[100];
    
  fd_set cset;
    
  ssize_t received;
  ssize_t received_tot;
  u_int32_t size;
    
  FILE *fp;
    
  FD_ZERO(&cset);
  FD_SET(STDIN_FILENO, &cset);
  FD_SET(fd, &cset);
    
    
  int quit = _FALSE;
    
  int file = _FALSE;
    
  // ricordarsi di rimetterlo a FALSE
  int command = _TRUE;
    
  int dimension = _FALSE;
    
    
  printf("******* MENU' ********\n");
  printf("GETfilename  -- to request a file\n");
  printf("QUIT         -- to exit\n");
    
  printf("enter your choice:\n");
    
  while (!quit) {
        
    printf("before select\n");
        
    int nready = Select(FD_SETSIZE, &cset, NULL, NULL, NULL);
        
    printf("nready: %d\n\n", nready);
        
    if ( FD_ISSET(fd, &cset) ){ // Something from server arrived.
            
      printf("received something from server\n");
            
      if (command) {
                
	printf("is command\n");
                
	// +OK\n\0 --> 5 characters in the buffer, the function must return 4.
	Readline_unbuffered(fd, buffer_rcv, BUFFER_RECV);
                
	printf("%s\n", buffer_rcv);
                
	if(strncmp(buffer_rcv, "-ERR", 3)==0){
	  printf("File error!\n");
                    
	  //+ERR -> 1 character more than OK, 2 for the \0 too. Must return 1.
	  Readline_unbuffered(fd, buffer_rcv, 2);
                    
	  command = _TRUE;
	  dimension = _FALSE;
                    
	}else if(strncmp(buffer_rcv, "+OK", 3)==0){
	  printf("OK, receiving data!\n");
                    
	  received_tot=0;
                    
	  /* Opening the file */
	  strcpy(nomefile, "/Users/andreamarcelli/Desktop/received/");
	  strncat(nomefile, buffer_send+3, strlen(buffer_send)-4);
	  fp = fopen(nomefile, "wb");
	  if (fp==NULL) {
	    err_ret("File opening error!");
	  }
                    
	  command = _FALSE;
	  dimension = _TRUE;
                    
	}else{
	  //printf("%s\n", buffer_rcv);
	  printf("Non recognized command from server\n");
                    
	  command = _TRUE;
	  dimension = _FALSE;
                    
	}
                
	file = _FALSE;
                
      }
            
      if (dimension) {
                
	printf("is dimension\n");
                
	/* Read the size */
	Readn_modified(fd, buffer_rcv, 4);
	memcpy(&size, buffer_rcv, 4);
	size = ntohl(size);
                
	printf("%d\n", size);
                
	command = _FALSE;
	dimension = _FALSE;
	file = _TRUE;
                
      }
            
            
      if (file) {
                
	printf("is file\n");
                
	while (received_tot<size) {
                    
	  /* This prevent starvation */
	  size_t how_many;
                    
	  if ((size-received_tot)>BUFFER_RECV) {
                        
	    how_many = BUFFER_RECV-1;
                        
	  }else{
                        
	    how_many = size-received_tot;
	  }
                    
	  received = Readn_modified(fd, buffer_rcv, how_many);
                    
	  fwrite(buffer_rcv, 1, received, fp);
                    
	  received_tot+=received;
	}
                
	fclose(fp);
                
	printf("File transfer completed!\n");
                
	command = _TRUE;
	dimension = _FALSE;
	file = _FALSE;
      }
    }
        
            
    if ( FD_ISSET(STDIN_FILENO, &cset) ) {
            
      printf("received something from user\n");
            
      if (fgets(buffer_send, BUFFER_SEND, stdin)) {
                
	// version 1
	strcat(buffer_send, "\n\0");
                
	if(strncmp(buffer_send, "GET", 3)==0){
	  Writen(fd, buffer_send, strlen(buffer_send));
	  printf("file requested!\n");
                    
	  command = _TRUE;
                    
	}else if(strncmp(buffer_send, "Q", 1)==0){
                    
	  quit = _TRUE;
	  Writen(fd, buffer_send, strlen(buffer_send));
	  printf("program closing!\n");
                
	}else if(strncmp(buffer_send, "A", 1)==0){
                    
	  // per ora lo stesso comportamento di Q,
	  // ma in futuro dovr' forzare il server a smettere di mandare pacchetti.
                    
	  quit = _TRUE;
	  Writen(fd, buffer_send, strlen(buffer_send));
	  printf("program closing!\n");
                    
	}else{
                    
	  printf("Non recognized command\n");
                    
	}

      }

    }
    
  }
            
  return;
}
