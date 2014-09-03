#include "../lunp.h"
#include "../types.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

#define MAXFILENAMELENGTH MAXLINE

void clientTask(SOCKET sockfd, char *fileName, XDR *xdrs_r, XDR *xdrs_w, FILE *fd_w);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char fileName[MAXFILENAMELENGTH];
  XDR xdrs_w, xdrs_r;
  FILE *fd_w, *fd_r;
  clientReq_t clientReq;
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  fd_w = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs_w, fd_w, XDR_ENCODE);
  
  fd_r = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs_r, fd_r, XDR_DECODE);
  
  printf("Please type the file name (maximum %d characters, empty string to close): ", MAXFILENAMELENGTH - 1);
  gets(fileName);
  
  while (strcmp(fileName, "") != 0) {
    
    clientTask(sockfd, fileName, &xdrs_r, &xdrs_w, fd_w);
    
    printf("\nPlease type the file name (maximum %d characters, empty string to close): ", MAXFILENAMELENGTH - 1);
    gets(fileName);
    
  }
  
  clientReq.message = QUIT;
  clientReq.fileName = NULL;
  xdr_clientReq_t(&xdrs_w, &clientReq);

  xdr_destroy(&xdrs_r);
  xdr_destroy(&xdrs_w);  
  Close(sockfd);
  return 0;
}

void clientTask(SOCKET sockfd, char *fileName, XDR *xdrs_r, XDR *xdrs_w, FILE *fd_w) {
  uint32_t fileSize;
  FILE *fd;
  clientReq_t clientReq;
  serverRes_t serverRes;
  
  // CLIENT REQUEST
  clientReq.message = GET;
  clientReq.fileName = fileName;
  
  if (xdr_clientReq_t(xdrs_w, &clientReq) == 0)
    myFunctionError("xdr_clientReq_t", NULL, "clientTask");
  fflush(fd_w);
  
  // SERVER RESPONSE
  serverRes.fileData_t.fileData_t_val = NULL;
  
  if (xdr_serverRes_t(xdrs_r, &serverRes) == 0)
    myFunctionError("xdr_serverRes_t", NULL, "clientTask");
  
  if (serverRes.message == ERR) {
    myWarning("Illegal command or non-existing file", "clientTask");
    return; // next file
  }
    
  // else: serverRes = OK
  fileSize = serverRes.fileData_t.fileData_t_len;
  
  fd = fopen(fileName, "w");
  fwrite((void*)serverRes.fileData_t.fileData_t_val, 1, fileSize, fd);
  fclose(fd);
}