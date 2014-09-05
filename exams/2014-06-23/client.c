#include "../lunp.h"
#include "../types.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port
#define OPERATION_ARG argv[3] // operation ("ENCODE" or "DECODE")
#define INPUT_FILE_ARG argv[4] // input file

#define OUTPUT_FILE "output.txt"
#define ENCODE_OP "ENCODE"
#define DECODE_OP "DECODE"

void clientRequest(SOCKET sockfd, char *operation, char *inputFile);
void serverResponse(SOCKET sockfd);

void readInputFile(char *inputFile, Request *clientReq);
void writeOutputFile(char *outputFile, Response serverRes);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);

  clientRequest(sockfd, OPERATION_ARG, INPUT_FILE_ARG);
  
  serverResponse(sockfd);
  
  Close(sockfd);
  return 0;
}

void clientRequest(SOCKET sockfd, char *operation, char *inputFile) {
  XDR xdrs;
  FILE *fd;
  Request clientReq;
  
  if (strcmp(operation, ENCODE_OP) == 0)
    clientReq.op = ENCODE;
  else // strcmp(operation, DECODE_OP) == 0
    clientReq.op = DECODE;
  
  readInputFile(inputFile, &clientReq);
  
  fd = fdopen(sockfd, "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
  
  if (xdr_Request(&xdrs, &clientReq) == FALSE)
    myFunctionError("xdr_Request", NULL, "clientRequest");
  
  fflush(fd);
  xdr_destroy(&xdrs);
  
  myWarning("Request sent successfully", "clientRequest");
  
  free(clientReq.data.data_val);
}

void serverResponse(SOCKET sockfd) {
  XDR xdrs;
  FILE *fd;
  Response serverRes;

  serverRes.data.data_val = NULL;
  
  fd = fdopen(sockfd, "r");
  xdrstdio_create(&xdrs, fd, XDR_DECODE);
  
  if (xdr_Response(&xdrs, &serverRes) == FALSE)
    myFunctionError("xdr_Response", NULL, "serverResponse");
  
  xdr_destroy(&xdrs);
  
  if (serverRes.success == FALSE)
    printf("Error\n");
  
  else { // serverRes.success == TRUE
    myWarning("Response received successfully", "serverResponse");
    writeOutputFile(OUTPUT_FILE, serverRes);
  }
  
  free(serverRes.data.data_val);
}

void readInputFile(char *inputFile, Request *clientReq) {
  FILE *fd;
  float num;
  int i;
  u_int count;
  
  // Preliminary reading
  fd = fopen(inputFile, "r");
  count = 0;
  while(fscanf(fd,"%f", &num) != EOF) {
    count++;
  }
  fclose(fd);
  
  clientReq->data.data_len = count;
  clientReq->data.data_val = (float*)malloc(sizeof(float) * count);
  
  // Second reading
  fd = fopen(inputFile, "r");
  for (i = 0; i < count; ++i) {
    fscanf(fd,"%f", &(clientReq->data.data_val[i]));
  }
  fclose(fd);
}

void writeOutputFile(char *outputFile, Response serverRes) {
  FILE *fd;
  int i;
  
  fd = fopen(outputFile, "w");
  for (i = 0; i < serverRes.data.data_len; ++i) {
    fprintf(fd, "%f\n", serverRes.data.data_val[i]);
  }
  fclose(fd);
}