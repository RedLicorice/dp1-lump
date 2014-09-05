#include "../lunp.h"

#define SERVER_ADDRESS_ARG argv[1] // server address
#define SERVER_PORT_ARG argv[2] // server port

enum status_t {
  init = 0, // I'm doing the GET
  okErr = 1, // I'm reading the +OK/-ERR message
  size = 2, // I'm reading the number of bytes of the file
  file = 3 // I'm reading the file
};

bool freeSocket(SOCKET sockfd, char *fileName, enum status_t *status, bool flagQuit);
bool freeStdInput(SOCKET sockfd, char *command, enum status_t *status, bool *flagQuit);
bool sendQuitMessage(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  bool flagQuit, reply;
  int freeFd;
  char command[4 + MAXFILENAMELENGTH]; // 4 = strlen("GET ")
  enum status_t status;
  
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);
    
  printf("Please type a command [GET <file>, Q, A]: ");
  fflush(stdout); // It forces stdout writing
  status = init;
  flagQuit = false;
  
  while (1) {
    
    freeFd = myWaitForMultipleObjects(-1, 2, sockfd, fileno(stdin));
    
    if (freeFd == 1) // sockfd
      reply = freeSocket(sockfd, command + 4, &status, flagQuit); // 4 = strlen("GET ");
    
    else // freeFd == 2 // fileno(stdin)
      reply = freeStdInput(sockfd, command, &status, &flagQuit);
    
    if (reply == false) { // exit
      Close(sockfd);
      return 0;
    } // else: continue
    
  }
}

bool freeSocket(SOCKET sockfd, char *fileName, enum status_t *status, bool flagQuit) {
  static char serverRes[BUFFSIZE];
  static int readByteCount;
  static uint32_t fileSize; // unsigned long int
  
  switch (*status) {
    case init:
      readByteCount = 0;
      *status = okErr;
      return true;
      
      
    case okErr:
      if (myTcpReadLineAsync(sockfd, serverRes, BUFFSIZE, &readByteCount) == false)
	return true;
      
      if (strcmp(serverRes, ERR) == 0) {
	myWarning("Illegal command or non-existing file", "freeSocket");
	return true; // next file
      }
      
      readByteCount = 0;
      *status = size;
      return true;
      
      
    case size:
      if (myTcpReadBytesAsync(sockfd, (void*)&fileSize, sizeof(uint32_t), &readByteCount) == false)
	return true;
      
      fileSize = ntohl(fileSize); // network-to-host long
      // HP: fileSize > 0
	
      readByteCount = 0;
      *status = file;
      return true;
      
      
    case file:
      if (myTcpReadChunksAndWriteToFileAsync(sockfd, fileName, fileSize, &readByteCount) == false)
	return true;
      
      if (flagQuit == true)
	return sendQuitMessage(sockfd);
      
      printf("\nPlease type a command [GET <file>, Q, A]: ");
      fflush(stdout);
      *status = init;
      return true;
      
    default:
      return false;
  }
}

bool freeStdInput(SOCKET sockfd, char *command, enum status_t *status, bool *flagQuit) {
  char *fileName;
  
  gets(command);
      
  if (strcmp(command, "Q") == 0) {
    if (*status == 0)
      return sendQuitMessage(sockfd);
    
    // else: *status > 0
    *flagQuit = 1;
    return true;
  }
	
  if (strcmp(command, "A") == 0)
    return false;
  
  // GET <file>
  fileName = command + 4; // 4 = strlen("GET ");
  myTcpWriteString(sockfd, GET);
  myTcpWriteString(sockfd, fileName);
  myTcpWriteString(sockfd, "\r\n");
	
  freeSocket(sockfd, fileName, status, *flagQuit); // *status == 0
	
  printf("Please type a command [Q, A]: "); // Two GETs at the same time are not allowed
  fflush(stdout);
  return true;
}

bool sendQuitMessage(SOCKET sockfd) {
  myTcpWriteString(sockfd, QUIT);
  return false;
}