#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"

#define IBS 1024
#define OBS 1024

#define MAXSTR 31

#define DEB 1

#define STOREDLENGTH 10
#define DROPPACKETS 3

typedef struct stored_t{
  struct sockaddr_in addr;
  int counter;
}store;

/* Initialize a socket. */
void nullsocket(struct sockaddr_in *socket){
  if(socket!=NULL){
    socket->sin_addr.s_addr=0;
    socket->sin_port=0;
  }else
    err_sys("nullsocket function fails: socket pointer null.");
}

/* Check if a socket is null socket. */
int socket_isnull(const struct sockaddr_in *socket){
  if (socket->sin_addr.s_addr==0 && socket->sin_port==0) {
    return 1;
  }
  return 0;
}

/* Check if two sockets are equal. */
int socket_isequal(struct sockaddr_in *socket1, struct sockaddr_in *socket2){
  if (socket1->sin_addr.s_addr  == socket2->sin_addr.s_addr &&
      socket1->sin_port == socket2->sin_port) {
    return 1;
  }
  return 0;
}

/* Print socket informations. */
void socket_print(const struct sockaddr_in socket){
  printf("IP: %s, Port: %d", inet_ntoa(socket.sin_addr), ntohs(socket.sin_port));
}

/* Print all sockets informations. */
void socket_printall(const store *stored){
  printf("Print sockets info of all sockets:\n");
  int i;
  for (i=0; i<STOREDLENGTH; i++) {
    socket_print(stored[i].addr);
    printf("* %d\n", stored[i].counter);
  }
}

/* It copies socket information into another one. */
void socket_copy (struct sockaddr_in *socket1, const struct sockaddr_in *socket2){
  socket1->sin_addr.s_addr = socket2->sin_addr.s_addr;
  socket1->sin_port = socket2->sin_port;
}

/* It inizializes the socket's storing structure. */
void socket_initialize(store *stored){
  int i;
  for (i=0; i<STOREDLENGTH; i++) {
    stored[i].counter=0;
    nullsocket(&stored[i].addr);
  }
}


void dg_srv(FILE *ifp, int sockfd, SA* cliaddr, socklen_t clilen){

  char       ibuf[IBS+1];
  char       obuf[IBS+1];
  socklen_t  len;
  int        n;
  int        drop;
  int        found;
  int        i;
  int        r;
  struct sockaddr_in *sin;

  /* Structure where to store the sockets received */
  store stored[STOREDLENGTH];
  int to_insert = 0;

  socket_initialize(stored);
    
  fprintf(stdout, "UDP Server starts.\n");

  for ( ; ; ) {
    drop   = 0;
    found  = 0;
    len    = clilen;

    n = Recvfrom(sockfd, ibuf, MAXSTR, 0, cliaddr, &len);
    fprintf(stdout, "Received %d bytes\n", n);
    ibuf[strlen(ibuf)] = '\0';
        
    /* First, I search between those which already exist. */
    for (i=0; i<STOREDLENGTH && !found; i++) {
      /* Compare the two sockets. */
      if (socket_isequal(&stored[i].addr, (struct sockaddr_in *)cliaddr)) {
	found = 1;
	if (stored[i].counter<DROPPACKETS) {
	  stored[i].counter++;
	  fprintf(stdout, "The socket has been found and updated!\n");
	  socket_printall(stored);
	}else{
	  drop = 1;
	  printf("Maximum received packets nuber was overtaken (%d)\n", DROPPACKETS);
	}
      }
    }
        
    /* If I couldn't find it, I insert the new socket. */
    if (!found) {
      fprintf(stdout, "The socket has NOT been found.\n");
      socket_copy(&stored[to_insert].addr, (struct sockaddr_in *)cliaddr);
      stored[to_insert].counter = 1;
      fprintf(stdout, "The socket has been inserted in position %d.\n", to_insert);

      socket_printall(stored);
            
      to_insert++;
      to_insert%=STOREDLENGTH;
    }
        
    /* If the max retrasmission number is not yet reached. */
    if (!drop) {
      sleep(1);
      strncpy(obuf, ibuf, n);
      Sendto(sockfd, obuf, n, 0, cliaddr, len);
      fprintf(stdout, "Sending again the buffer.\n");
    }
  }
}


void
sig_int(int signo){
  fprintf(stdout, "In the SIG_INT handler\n");
}

void
sig_usr1(int signo){
  fprintf(stdout, "In the SIG_USR1 handler\n");
}

int main(int argc, char *argv[]){
  void ( *sigv[2] ) (int) = {sig_int, sig_usr1};
  int intv[2] = {SIGINT, SIGUSR1};
  udpsrv_simple(argc, argv, 2, sigv, intv, dg_srv);
  return 0;
}
