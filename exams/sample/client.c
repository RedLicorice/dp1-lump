#include "../lunp.h"
#include "../types.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

#define MAXFILENAMELENGTH MAXLINE

void clientRequest_Get(SOCKET sockfd, char *fileName);
void clientRequest_Quit(SOCKET sockfd);
void serverResponse(SOCKET sockfd, char *fileName);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  char fileName[MAXFILENAMELENGTH];
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the file name (maximum %d characters, empty string to close): ", MAXFILENAMELENGTH - 1);
  gets(fileName);
  
  while (strcmp(fileName, "") != 0) {
    
    clientRequest_Get(sockfd, fileName);
  
    serverResponse(sockfd, fileName);
    
    printf("\nPlease type the file name (maximum %d characters, empty string to close): ", MAXFILENAMELENGTH - 1);
    gets(fileName);
    
  }
  
  clientRequest_Quit(sockfd);
  
  Close(sockfd);
  return 0;
}

void clientRequest_Get(SOCKET sockfd, char *fileName) {
  XDR xdrs;
  FILE *fd;
  Request clientReq;
  
  clientReq.op = GET;
  clientReq.data = fileName;
  
  fd = fdopen(dup(sockfd), "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
  setbuf(fd, NULL);
  
  if (xdr_Request(&xdrs, &clientReq) == FALSE)
    myFunctionError("xdr_Request", NULL, "clientRequest_Get");
  
  xdr_destroy(&xdrs);
  fclose(fd);
}

void clientRequest_Quit(SOCKET sockfd) {
  XDR xdrs;
  FILE *fd;
  Request clientReq;
  
  clientReq.op = QUIT;
  clientReq.data = (char*)malloc(0);
  
  fd = fdopen(dup(sockfd), "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
  setbuf(fd, NULL);
  
  if (xdr_Request(&xdrs, &clientReq) == FALSE)
    myFunctionError("xdr_Request", NULL, "clientRequest_Quit");
  
  xdr_destroy(&xdrs);
  fclose(fd);
  
  free(clientReq.data);
}

void serverResponse(SOCKET sockfd, char *fileName) {
  XDR xdrs;
  FILE *fd;
  Response serverRes;
  
  u_int fileSize;
    
  serverRes.data.data_val = NULL;
  
  fd = fdopen(dup(sockfd), "r");
  xdrstdio_create(&xdrs, fd, XDR_DECODE);
  
  if (xdr_Response(&xdrs, &serverRes) == FALSE)
    myFunctionError("xdr_Response", NULL, "serverResponse");
  
  xdr_destroy(&xdrs);
  fclose(fd);
  
  if (serverRes.success == FALSE) {
    myWarning("Illegal command or non-existing file", "serverResponse");
    free(serverRes.data.data_val);
    return; // next file
  }
    
  // else: serverRes.success == TRUE
  fileSize = serverRes.data.data_len;
  
  fd = fopen(fileName, "w");
  fwrite((void*)serverRes.data.data_val, 1, fileSize, fd);
  fclose(fd);
  
  free(serverRes.data.data_val);
}