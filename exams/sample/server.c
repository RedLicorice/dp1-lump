#include "../lunp.h"
#include "../types.h"

#define SERVER_PORT_ARG argv[1] // server port

#define CHILD_COUNT 5

void childTask(SOCKET sockfd);
bool clientRequest(SOCKET sockfd, Request *clientReq);
bool processClientRequest(Request clientReq, char **fileName);
bool serverResponse_Success(SOCKET sockfd, char *fileName);
bool serverResponse_Fail(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerPreforked(sockfd, CHILD_COUNT, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  Request clientReq;
  char *fileName;
  bool valid;
  
  while (1) {
    
    if (clientRequest(sockfd, &clientReq) == false)
      return;
    
    valid = processClientRequest(clientReq, &fileName);
    
    if (valid == true) {
      if (serverResponse_Success(sockfd, fileName) == false) {
	free(fileName);
	return;
      }
      
    } else { // valid == false
      if (serverResponse_Fail(sockfd) == false) {
	free(fileName);
	return;
      }
    }
    
    free(fileName);
    
  }
}

bool clientRequest(SOCKET sockfd, Request *clientReq) {
  XDR xdrs;
  FILE *fd;
  
  clientReq->data = NULL;
  
  fd = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs, fd, XDR_DECODE);
    
  if (xdr_Request(&xdrs, clientReq) == FALSE) {
    xdr_destroy(&xdrs);
    return false;
  }
  
  xdr_destroy(&xdrs);
  
  if (clientReq->op == QUIT) {
    myWarning("Connection closed (QUIT)", "clientRequest");
    free(clientReq->data);
    return false;
  }
  
  return true;
}

bool processClientRequest(Request clientReq, char **fileName) {
  if (clientReq.op != GET)
    return false;
  
  *fileName = clientReq.data;
  
  return fileExists(*fileName);
}

bool serverResponse_Success(SOCKET sockfd, char *fileName) {
  XDR xdrs;
  FILE *fd;
  Response serverRes;
  
  uint32_t fileSize;
  
  myWarning("Sending the file to the client...", "serverResponse_Success");
      
  serverRes.success = TRUE;
      
  fileSize = getFileSize(fileName);
  serverRes.data.data_len = fileSize;
      
  serverRes.data.data_val = (char*)malloc(sizeof(char) * fileSize);
  fd = fopen(fileName, "r");
  fread((void*)serverRes.data.data_val, 1, fileSize, fd);
  fclose(fd);
      
  fd = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
    
  if (xdr_Response(&xdrs, &serverRes) == FALSE) {
    xdr_destroy(&xdrs);
    return false;
  }
    
  xdr_destroy(&xdrs);
  fflush(fd);
      
  free(serverRes.data.data_val);
      
  myWarning("File sent successfully to the client...", "serverResponse_Success");
  
  return true;
}

bool serverResponse_Fail(SOCKET sockfd) {
  XDR xdrs;
  FILE *fd;
  Response serverRes;
  
  myWarning("Illegal command or non-existing file", "serverResponse_Fail");
      
  serverRes.success = FALSE;
  serverRes.data.data_val = NULL;
  serverRes.data.data_len = 0;
      
  fd = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
    
  if (xdr_Response(&xdrs, &serverRes) == FALSE) {
    xdr_destroy(&xdrs);
    return false;
  }
    
  xdr_destroy(&xdrs);
  fflush(fd);
    
  return true;
}