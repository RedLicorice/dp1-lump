#include "../lunp.h"

#define SERVER_PORT_ARG argv[1] // server port

typedef void (*myTcpServerChildTask2)(SOCKET sockfd, struct sockaddr_in clientAddr);
static int numChildren_ocpc2 = 0;

void childTask(SOCKET sockfd, struct sockaddr_in clientAddr);

bool myTcpReadChunks2(SOCKET sockfd, int *readByteCount, myTcpReadChunksCallback callback, void *callbackParam);
bool myTcpReadChunksAndWriteToFile2(SOCKET sockfd, const char *filePath, int *readByteCount);
static bool myTcpReadChunksAndWriteToFileCallback2(void *chunk, int chunkSize, void *param);

void myTcpServerOCPC2(SOCKET sockfd, myTcpServerChildTask2 childTask);
static void tcpServerOCPC2(SOCKET sockfd, myTcpServerChildTask2 childTask, bool resetSigInt);
static void sigchldHandler2(int s);
static void waitForZombieChildren2();


int main(int argc, char *argv[]) {
  SOCKET sockfd;
  
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);
  
  myTcpServerOCPC2(sockfd, &childTask);
  
  return 0;
}

void childTask(SOCKET sockfd, struct sockaddr_in clientAddr) {
  char clientReq[BUFFSIZE], fileName[MAXFILENAMELENGTH], solofile[MAXFILENAMELENGTH];
  //uint32_t fileSize;
  bool valid;
  
  uint16_t porta;
  uint32_t indirizzo;
  char *indirizzoString, portaString[BUFFSIZE];
  
  porta = clientAddr.sin_port;
  indirizzo = clientAddr.sin_addr.s_addr;
  
  printf("DEBUG2 porta = %u\nindirizzo = %u\n", porta, indirizzo);
  
  indirizzoString = inet_ntoa(clientAddr.sin_addr);
  printf("%s\n", indirizzoString);
  
  
  
  fileName[0] = '\0';
  strcat(fileName, indirizzoString);
  strcat(fileName, "_");
  sprintf(portaString, "%u", porta);
  strcat(fileName, portaString);
  
  printf("La cartella è: %s\n", fileName);
  if (mkdir (fileName, 0777) == -1)
        mySystemError("mkdir", NULL);
  
  
//  while (1) {
    
    valid = true;
    
    if (myTcpReadLine(sockfd, clientReq, BUFFSIZE, NULL) == false)
      valid = false;
      
      
    if (valid == true) {
      
            if (strlen(clientReq) < strlen(PUT) + strlen("\r\n"))
                valid = false;
                
            else {
                
              if (strncmp(clientReq, PUT, strlen(PUT)) != 0)
                valid = false;
                
              else {
                valid = true;
                strcpy(solofile, clientReq + strlen(PUT));
                solofile[strlen(solofile) - strlen("\r\n")] = '\0';
                
                strcat(fileName, "/");
                strcat(fileName, solofile);
              }
            }
    }
    
    if (valid == false) {
      myWarning("Wrong format", "childTask");
      myTcpWriteString(sockfd, ERR);
      
    } else { // The file exists
      myWarning("Receiving the file from the client...", "childTask");
      myTcpWriteString(sockfd, OK);
	
  if (myTcpReadChunksAndWriteToFile2(sockfd, fileName, NULL) == false)
    myError("Cannot write the file", "clientTask");
	
      myWarning("File received successfully from the client", "childTask");
    }
    
//  }
}



bool myTcpReadChunks2(SOCKET sockfd, int *readByteCount, myTcpReadChunksCallback callback, void *callbackParam) {
  int numberOfReadBytes, chunkSize;
  void *buffer;
  bool readReply;
  
  buffer = (void*)malloc(sizeof(void) * DEFAULT_CHUNK_SIZE);
  
  if (readByteCount != NULL)
    *readByteCount = 0;
  readReply = true;
  
  while (1) {

      chunkSize = DEFAULT_CHUNK_SIZE;
    
    readReply = myTcpReadBytes(sockfd, buffer, chunkSize, &numberOfReadBytes);
    
    if (readByteCount != NULL)
      *readByteCount = *readByteCount + numberOfReadBytes;

    if (callback != NULL) {
      if (callback(buffer, numberOfReadBytes, callbackParam) == false) {
	free(buffer);
	return false;
      }
    }
    
    if (readReply == false) {
      free(buffer);
      return true;
    }
    
  }
}

bool myTcpReadChunksAndWriteToFile2(SOCKET sockfd, const char *filePath, int *readByteCount) {
  FILE *fd;
  bool reply;
  
  fd = fopen(filePath, "w");
  if (fd == NULL)
    mySystemError("fopen", "myTcpReadChunksAndWriteToFile");
  
  reply = myTcpReadChunks2(sockfd, readByteCount, &myTcpReadChunksAndWriteToFileCallback2, (void*)fd);
  
  if (fclose(fd) == EOF)
    mySystemError("fclose", "myTcpReadChunksAndWriteToFile");
  return reply;
}

static bool myTcpReadChunksAndWriteToFileCallback2(void *chunk, int chunkSize, void *param) {
  if (fwrite(chunk, 1, chunkSize, (FILE*)param) != chunkSize)
    return myFunctionWarning("fwrite", "myTcpReadChunksAndWriteToFileCallback", NULL);
  return true;
}






void myTcpServerOCPC2(SOCKET sockfd, myTcpServerChildTask2 childTask) {
  tcpServerOCPC2(sockfd, childTask, false);
}

static void tcpServerOCPC2(SOCKET sockfd, myTcpServerChildTask2 childTask, bool resetSigInt) {
  pid_t childpid;
  SOCKET sockfd_copy;
  struct sockaddr_in clientAddr;
  
  if (signal(SIGCHLD, sigchldHandler2) == SIG_ERR)
    mySystemError("signal", "tcpServerOCPC");
  
  while (1) {
    
    printf("\n");
    myWarning("Server on listening...  [%d children]", "tcpServerOCPC", numChildren_ocpc2);
    
    sockfd_copy = myTcpServerAccept(sockfd, &clientAddr);
    myWarning("Connection accepted", "tcpServerOCPC");
    
    childpid = fork();
    if (childpid == -1)
      mySystemError("fork", "tcpServerOCPC");
    
    if (childpid == 0) {
      // child
      Close(sockfd);
      
      if (resetSigInt == true && signal(SIGINT, SIG_DFL) == SIG_ERR)
	mySystemError("signal", "tcpServerOCPC");
      
      childTask(sockfd_copy, clientAddr);
      Close(sockfd_copy);
      return;
    }
    
    // father
    numChildren_ocpc2++;
    
    Close(sockfd_copy);
    
  }
}

static void sigchldHandler2(int s) {
  if (s == SIGCHLD) {
    numChildren_ocpc2--;
    myWarning("Received SIGCHLD signal  [%d children]", "sigchldHandler2", numChildren_ocpc2);
    waitForZombieChildren2();
  }
}


static void waitForZombieChildren2() {
  pid_t childpid;
  
    childpid = waitpid(-1, NULL, WNOHANG);
    while (childpid > 0) {
      myWarning("Child %d was zombie  [%d children]", "waitForZombieChildren2", childpid, numChildren_ocpc2);
      childpid = waitpid(-1, NULL, WNOHANG);
    }
    
    if (childpid == -1) {
      if (errno == 10) // No child processes
	myWarning("Now there are no children  [%d children]", "waitForZombieChildren2", numChildren_ocpc2);
      else
	mySystemError("waitpid", "waitForZombieChildren2");
      
    } else // childpid = 0
      myWarning("Now there are no zombie children  [%d children]", "waitForZombieChildren2", numChildren_ocpc2);
}
