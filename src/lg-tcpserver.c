#include "lunp.h"

static void waitForZombieChildren();

static void sigusr1Handler(int s);

int numChildren = 0;

SOCKET myTcpServerAccept(SOCKET sockfd, struct sockaddr_in *clientStruct) {
  socklen_t addrlen;
  
  addrlen = sizeof(struct sockaddr_in);
  
  return Accept(sockfd, (struct sockaddr*)clientStruct, &addrlen);
}

SOCKET myTcpServerStartup(const char *serverPort) {
  SOCKET sockfd;
  struct sockaddr_in saddr;
  
  sockfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(serverPort));
  saddr.sin_addr.s_addr = INADDR_ANY;
  
  Bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in));
  
  Listen(sockfd, 2);
  
  return sockfd;
}

void myTcpServerSimple(SOCKET sockfd, myTcpServerChildTask childTask) {
  SOCKET sockfd_copy;
  struct sockaddr_in clientAddr;
  
  while (1) {
    
    printf("\n");
    myWarning("Server on listening...", "myTcpServerSimple");
    
    sockfd_copy = myTcpServerAccept(sockfd, &clientAddr);
    
    childTask(sockfd_copy);
    
    Close(sockfd_copy);
    
  }
}

void myTcpServerOCPC(SOCKET sockfd, myTcpServerChildTask childTask) {
  pid_t childpid;
  SOCKET sockfd_copy;
  struct sockaddr_in clientAddr;
  
  if (signal(SIGUSR1, sigusr1Handler) == SIG_ERR)
    mySystemError("signal", "myTcpServerOCPC");
  
  while (1) {
    
    printf("\n");
    myWarning("Server on listening...  [%d children]", "myTcpServerOCPC", numChildren);
    
    sockfd_copy = myTcpServerAccept(sockfd, &clientAddr);
    
    childpid = fork();
    if (childpid == -1)
      mySystemError("fork", "myTcpServerOCPC");
    
    if (childpid == 0) {
      // child
      Close(sockfd);
      childTask(sockfd_copy);
      Close(sockfd_copy);
      return;
    }
    
    // father
    numChildren++;
    
    Close(sockfd_copy);
    
  }
}

void myTcpServerOCPCMax(SOCKET sockfd, int maxChildCount, myTcpServerChildTask childTask) {
  pid_t childpid;
  SOCKET sockfd_copy;
  struct sockaddr_in clientAddr;
  
  if (signal(SIGUSR1, sigusr1Handler) == SIG_ERR)
    mySystemError("signal", "myTcpServerOCPCMax");
  
  while (1) {
    
    if (numChildren == maxChildCount) {
      waitForZombieChildren();
      
      myWarning("Server on waiting for a client to terminate...  [%d children]", "myTcpServerOCPCMax", numChildren); // numChildren = 3
      if (wait(NULL) == -1)
	mySystemError("wait", "myTcpServerOCPCMax");
    }
    
    printf("\n");
    myWarning("Server on listening...  [%d children]", "myTcpServerOCPCMax", numChildren);
    
    sockfd_copy = myTcpServerAccept(sockfd, &clientAddr);
    
    childpid = fork();
    if (childpid == -1)
      mySystemError("fork", "myTcpServerOCPCMax");
    
    if (childpid == 0) {
      // child
      Close(sockfd);
      childTask(sockfd_copy);
      
      if (kill(getppid(), SIGUSR1) == -1)
	mySystemError("kill", "myTcpServerOCPCMax");
      
      Close(sockfd_copy);
      return;
    }
    
    // father
    numChildren++;
    
    Close(sockfd_copy);
    
  }
}

static void waitForZombieChildren() {
  pid_t childpid;
  
    childpid = waitpid(-1, NULL, WNOHANG);
    while (childpid > 0) {
      myWarning("Child %d was zombie  [%d children]", "waitForZombieChildren", childpid, numChildren);
      childpid = waitpid(-1, NULL, WNOHANG);
    }
    
    if (childpid == -1)
      mySystemError("waitpid", "waitForZombieChildren");
    else // childpid = 0
      myWarning("There are no zombie children  [%d children]", "waitForZombieChildren", numChildren);
}

static void sigusr1Handler(int s) {
  if (s == SIGUSR1) {
    numChildren--;
    myWarning("Received SIGUSR1 signal  [%d children]", "sigusr1Handler", numChildren);
    if (numChildren == 0)
      return;
    
    waitForZombieChildren();
  }
}