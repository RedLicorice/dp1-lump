#include "../lunp.h"
#include "../types.h"

#define SERVER_PORT_ARG argv[1] // server port

#define CHILD_COUNT 5

void childTask(SOCKET sockfd);
void xdrFunctionWarning(const char *functionName, XDR *xdrs_r, XDR *xdrs_w);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerPreforked(sockfd, CHILD_COUNT, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  uint32_t fileSize;
  XDR xdrs_w, xdrs_r;
  FILE *fd_w, *fd_r, *fd;
  clientReq_t clientReq;
  serverRes_t serverRes;
  
  fd_w = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs_w, fd_w, XDR_ENCODE);
  
  fd_r = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs_r, fd_r, XDR_DECODE);
  
  while (1) {
    
    // CLIENT REQUEST
    clientReq.fileName = NULL;
    
    if (xdr_clientReq_t(&xdrs_r, &clientReq) == 0) {
      xdrFunctionWarning("xdr_clientReq_t", &xdrs_r, &xdrs_w);
      return;
    }
    
    if (clientReq.message == QUIT) {
      myWarning("Connection closed (QUIT)", "childTask");
      xdr_destroy(&xdrs_r);
      xdr_destroy(&xdrs_w);
      return;
    }
    
    // else: clientReq = GET...
    
    // SERVER RESPONSE
    if (fileExists(clientReq.fileName) == false) {
      myWarning("The file does not exist", "childTask");
      
      serverRes.message = ERR;
      serverRes.fileData_t.fileData_t_val = NULL;
      serverRes.fileData_t.fileData_t_len = 0;
      
      if (xdr_serverRes_t(&xdrs_w, &serverRes) == 0) {
	xdrFunctionWarning("xdr_serverRes_t", &xdrs_r, &xdrs_w);
	return;
      }
      fflush(fd_w);
      
    } else { // The file exists
      myWarning("Sending the file to the client...", "childTask");
      
      serverRes.message = OK;
      
      fileSize = getFileSize(clientReq.fileName);
      serverRes.fileData_t.fileData_t_len = fileSize;
      
      serverRes.fileData_t.fileData_t_val = (char*)malloc(sizeof(char) * fileSize);
      fd = fopen(clientReq.fileName, "r");
      fread((void*)serverRes.fileData_t.fileData_t_val, 1, fileSize, fd);
      fclose(fd);
      
      if (xdr_serverRes_t(&xdrs_w, &serverRes) == 0) {
	xdrFunctionWarning("xdr_serverRes_t", &xdrs_r, &xdrs_w);
	return;
      }
      fflush(fd_w);
      
      free(serverRes.fileData_t.fileData_t_val);
      myWarning("File sent successfully to the client...", "childTask");
    }
    
  }
}

void xdrFunctionWarning(const char *functionName, XDR *xdrs_r, XDR *xdrs_w) {
  myFunctionWarning(functionName, NULL, "childTask");
  xdr_destroy(xdrs_r);
  xdr_destroy(xdrs_w);
}