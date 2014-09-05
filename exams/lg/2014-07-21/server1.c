#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port
#define KAPPA_ARG argv[2] // K (int)

int K;

bool clientRequest(SOCKET sockfd, struct sockaddr_in *saddr, uint32_t *transactionId, uint32_t *op1, uint32_t *op2);
bool processClientRequest(uint32_t op1, uint32_t op2, uint32_t *result);
void serverResponse(SOCKET sockfd, struct sockaddr_in saddr, uint32_t transactionId, uint32_t result);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  struct sockaddr_in saddr;
  uint32_t transactionId, op1, op2, result;
  
  K = atoi(KAPPA_ARG);
  
  sockfd = myUdpServerStartup(SERVER_PORT_ARG);
  
  while (1) {
    
    if (clientRequest(sockfd, &saddr, &transactionId, &op1, &op2) == true) {
      
      if (processClientRequest(op1, op2, &result) == true) {
      
	serverResponse(sockfd, saddr, transactionId, result);
      
      } // else: ignore
      
    } // else: ignore
    
  }
  
  return 0;
}

bool clientRequest(SOCKET sockfd, struct sockaddr_in *saddr, uint32_t *transactionId, uint32_t *op1, uint32_t *op2) {
  uint32_t clientReq[3];
  
  printf("\n");
  myWarning("Server on listening...", "clientRequest");
  
  if (myUdpReadBytes(sockfd, (void*)&clientReq, sizeof(uint32_t) * 3, saddr, NULL) == false) {
    myWarning("The number of read bytes is less than the expected one (%d)", "serverResponse", sizeof(uint32_t) * 3);
    return false;
  }
  
  myWarning("Request datagram received successfully", "clientRequest");
  
  *transactionId = ntohl(clientReq[0]);
  *op1 = ntohl(clientReq[1]);
  *op2 = ntohl(clientReq[2]);
  
  return true;
}

bool processClientRequest(uint32_t op1, uint32_t op2, uint32_t *result) {
  *result = (op1 + op2) % K;
  return true;
}

void serverResponse(SOCKET sockfd, struct sockaddr_in saddr, uint32_t transactionId, uint32_t result) {
  uint32_t serverResponse[2];
  
  serverResponse[0] = htonl(transactionId);
  serverResponse[1] = htonl(result);
  
  myUdpWriteBytes(sockfd, (void*)&serverResponse, sizeof(uint32_t) * 2, saddr);
  
  myWarning("Response datagram sent successfully", "serverResponse");
}