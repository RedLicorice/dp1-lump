#include "../lunp.h"
#include "../types.h"

#define SERVER_PORT_ARG argv[1] // server port
#define KAPPA_ARG argv[2] // K (float)

#define CHILD_COUNT 5

float K;

void childTask(SOCKET sockfd);
bool clientRequest(SOCKET sockfd, Request *clientReq);
bool processClientRequest(Request clientReq, float **data, u_int *dataLen);
bool serverResponse_Fail(SOCKET sockfd);
bool serverResponse_Success(SOCKET sockfd, float *data, u_int dataLen);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  K = atof(KAPPA_ARG);
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerPreforked(sockfd, CHILD_COUNT, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd) {
  Request clientReq;
  bool valid;
  float *data;
  u_int dataLen;
  
  while(1) {
    
    if (clientRequest(sockfd, &clientReq) == false)
      return;
      
    valid = processClientRequest(clientReq, &data, &dataLen);
    
    if (valid == true) {
      if (serverResponse_Success(sockfd, data, dataLen) == false) {
	free(clientReq.data.data_val);
	free(data);
	return;
      }
      
    } else { // valid == false
      if (serverResponse_Fail(sockfd) == false) {
	free(clientReq.data.data_val);
	free(data);
	return;
      }
    }
      
    free(clientReq.data.data_val);
    free(data);
    
  }
}

bool clientRequest(SOCKET sockfd, Request *clientReq) {
  XDR xdrs;
  FILE *fd;
  
  clientReq->data.data_val = NULL;
  
  fd = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs, fd, XDR_DECODE);
  
  if (xdr_Request(&xdrs, clientReq) == FALSE) {
    xdr_destroy(&xdrs);
    return false;
  }
    
  xdr_destroy(&xdrs);
  return true;
}

bool processClientRequest(Request clientReq, float **data, u_int *dataLen) {
  float K2;
  int i;
  
  if (clientReq.op == ENCODE)
    K2 = K;
  else if (clientReq.op == DECODE)
    K2 = -K;
  else
    return false;
  
  *dataLen = clientReq.data.data_len;
  *data = (float*)malloc(sizeof(float) * *dataLen);
  
  for (i = 0; i < *dataLen; ++i) {
    (*data)[i] = clientReq.data.data_val[i] + K2;
  }
  
  return true;
}

bool serverResponse_Fail(SOCKET sockfd) {
  XDR xdrs;
  FILE *fd;
  Response serverRes;
  
  myWarning(NULL, "serverResponse_Fail");
  
  serverRes.success = FALSE;
  serverRes.data.data_val = (float*)malloc(0);
  serverRes.data.data_len = 0;
  
  fd = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
    
  if (xdr_Response(&xdrs, &serverRes) == FALSE) {
    xdr_destroy(&xdrs);
    free(serverRes.data.data_val);
    return false;
  }
    
  fflush(fd);
  xdr_destroy(&xdrs);
  
  free(serverRes.data.data_val);
  
  return true;
}

bool serverResponse_Success(SOCKET sockfd, float *data, u_int dataLen) {
  XDR xdrs;
  FILE *fd;
  Response serverRes;
  
  myWarning(NULL, "serverResponse_Success");
  
  serverRes.success = TRUE;
  serverRes.data.data_val = data;
  serverRes.data.data_len = dataLen;
  
  fd = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
    
  if (xdr_Response(&xdrs, &serverRes) == FALSE) {
    xdr_destroy(&xdrs);
    return false;
  }
    
  fflush(fd);
  xdr_destroy(&xdrs);
  
  return true;
}