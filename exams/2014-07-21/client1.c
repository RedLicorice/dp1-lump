#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port
#define TRANSACTION_ID_ARG argv[3] // transaction ID
#define OP1_ARG argv[4] // operand 1
#define OP2_ARG argv[5] // operand 2

#define OUTPUT_FILE "output.txt"
#define MAX_SECONDS 3

enum serverResStatus {
  success = 0, // success
  wrong = 1, // wrong transaction ID or wrong format
  expired = 2 // expired timeout
};
typedef enum serverResStatus serverResStatus;

void clientRequest(SOCKET sockfd, struct sockaddr_in daddr, char *transactionIdArg, char *op1Arg, char *op2Arg, uint32_t *transactionId);
serverResStatus serverResponse(SOCKET sockfd, uint32_t transactionId, uint32_t *result);
void writeOutputFile(serverResStatus reply, uint32_t result);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  struct sockaddr_in *daddr;
  uint32_t transactionId, result;
  serverResStatus reply;
  
  sockfd = myUdpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG, &daddr);
  
  clientRequest(sockfd, *daddr, TRANSACTION_ID_ARG, OP1_ARG, OP2_ARG, &transactionId);
  
  reply = serverResponse(sockfd, transactionId, &result);
  
  writeOutputFile(reply, result);
  
  Close(sockfd);
  free(daddr);
  return 0;
}

void clientRequest(SOCKET sockfd, struct sockaddr_in daddr, char *transactionIdArg, char *op1Arg, char *op2Arg, uint32_t *transactionId) {
  uint32_t clientReq[3];
  
  *transactionId = atoi(transactionIdArg);
  
  clientReq[0] = htonl(*transactionId);
  clientReq[1] = htonl(atoi(op1Arg));
  clientReq[2] = htonl(atoi(op2Arg));
  
  myUdpWriteBytes(sockfd, (void*)&clientReq, sizeof(uint32_t) * 3, daddr);
  
  myWarning("Request datagram sent successfully", "clientRequest");
}

serverResStatus serverResponse(SOCKET sockfd, uint32_t transactionId, uint32_t *result) {
  uint32_t receivedTransactionId, serverResponse[2];
  
  if (myWaitForSingleObject(MAX_SECONDS, sockfd) == false) {
    myWarning("Expired timeout", "serverResponse");
    return expired;
  }
  
  if (myUdpReadBytes(sockfd, (void*)&serverResponse, sizeof(uint32_t) * 2, NULL, NULL) == false) {
    myWarning("The number of read bytes is less than the expected one (%d)", "serverResponse", sizeof(uint32_t) * 2);
    return wrong;
  }
  
  myWarning("Response datagram received successfully", "serverResponse");
  
  receivedTransactionId = ntohl(serverResponse[0]);
  if (receivedTransactionId != transactionId) {
    myWarning("Wrong transaction ID", "serverResponse");
    return wrong;
  }
  
  *result = ntohl(serverResponse[1]);
  return success;
}

void writeOutputFile(serverResStatus reply, uint32_t result) {
  FILE *fd;
  
  fd = fopen(OUTPUT_FILE, "w");
  
  fprintf(fd, "%d", reply);
  
  if (reply == success)
    fprintf(fd, " %u", result);
  
  fclose(fd);
}