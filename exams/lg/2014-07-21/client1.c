#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port
#define TRANSACTION_ID_ARG argv[3] // transaction ID
#define OP1_ARG argv[4] // operand 1
#define OP2_ARG argv[5] // operand 2

#define OUTPUT_FILE "output.txt"
#define MAX_SECONDS 3

void clientRequest(SOCKET sockfd, struct sockaddr_in daddr, char *transactionIdArg, char *op1Arg, char *op2Arg, uint32_t *transactionId);
int serverResponse(SOCKET sockfd, uint32_t transactionId, uint32_t *result);
void writeOutputFile(int reply, uint32_t result);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  struct sockaddr_in *daddr;
  uint32_t transactionId, result;
  int reply;
  
  sockfd = myUdpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG, &daddr);
  
  clientRequest(sockfd, *daddr, TRANSACTION_ID_ARG, OP1_ARG, OP2_ARG, &transactionId);
  
  reply = serverResponse(sockfd, transactionId, &result);
  
  writeOutputFile(reply, result);
  
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

int serverResponse(SOCKET sockfd, uint32_t transactionId, uint32_t *result) {
  /* Return values:
   * 0 = success
   * 1 = wrong transaction ID or wrong format
   * 2 = expired timeout
   */
  
  uint32_t receivedTransactionId, serverResponse[2];
  
  if (myWaitForSingleObject(MAX_SECONDS, sockfd) == false) {
    myWarning("Expired timeout", "serverResponse");
    return 2;
  }
  
  if (myUdpReadBytes(sockfd, (void*)&serverResponse, sizeof(uint32_t) * 2, NULL, NULL) == false) {
    myWarning("The number of read bytes is less than the expected one (%d)", "serverResponse", sizeof(uint32_t) * 2);
    return 1;
  }
  
  myWarning("Response datagram received successfully", "serverResponse");
  
  receivedTransactionId = ntohl(serverResponse[0]);
  if (receivedTransactionId != transactionId) {
    myWarning("Wrong transaction ID", "serverResponse");
    return 1;
  }
  
  *result = ntohl(serverResponse[1]);
  return 0;
}

void writeOutputFile(int reply, uint32_t result) {
  FILE *fd;
  
  fd = fopen(OUTPUT_FILE, "w");
  
  fprintf(fd, "%d", reply);
  
  if (reply == 0)
    fprintf(fd, " %u", result);
  
  fclose(fd);
}