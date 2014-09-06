#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

void clientRequest(SOCKET sockfd, int num1, int num2);
void serverResponse(SOCKET sockfd, int *result);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  int num1, num2, result;
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
  
  printf("Please type the first number: ");
  scanf("%d", &num1);
  
  printf("Please type the second number: ");
  scanf("%d", &num2);
  
  clientRequest(sockfd, num1, num2);
  
  serverResponse(sockfd, &result);
  
  printf("The server replied: %d\n", result);
  
  Close(sockfd);
  return 0;
}

void clientRequest(SOCKET sockfd, int num1, int num2) {
  if (myTcpWriteXdr(sockfd, (myXdrFunction)&xdr_int, (void*)&num1) == false)
    myFunctionError("xdr_int", NULL, "clientRequest");
  
  if (myTcpWriteXdr(sockfd, (myXdrFunction)&xdr_int, (void*)&num2) == false)
    myFunctionError("xdr_int", NULL, "clientRequest");
}

void serverResponse(SOCKET sockfd, int *result) {
  if (myTcpReadXdr(sockfd, (myXdrFunction)&xdr_int, (void*)result) == false)
    myFunctionError("xdr_int", NULL, "serverResponse");
}