#include "lunp.h"

// ocpc
static int numChildren_ocpc = 0;

// preforked
static int numChildren_preforked = 0;
static pid_t *childpids;

static void tcpServerPreforked(SOCKET sockfd, int childCount, myTcpServerChildTask childTask);
static void tcpServerOCPC(SOCKET sockfd, myTcpServerChildTask childTask, bool resetSigInt);
static void tcpServerOCPCMax(SOCKET sockfd, int maxChildCount, myTcpServerChildTask childTask, bool resetSigInt);
static void waitForZombieChildren();

static void sigchldHandler(int s);
static void sigintHandler(int s);

SOCKET myTcpServerAccept(SOCKET sockfd, struct sockaddr_in *clientStruct) {
  socklen_t addrlen;
  
  addrlen = sizeof(struct sockaddr_in);
  
  return Accept(sockfd, (struct sockaddr*)clientStruct, &addrlen);
}

/*SOCKET myTcpServerStartup(const char *serverPort) {
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
}*/

SOCKET myTcpServerStartup(const char *serverPort) {
  return Tcp_listen(NULL, serverPort, NULL);
}

void myTcpServerSimple(SOCKET sockfd, myTcpServerChildTask childTask) {
  SOCKET sockfd_copy;
  struct sockaddr_in clientAddr;
  
  while (1) {
    
    printf("\n");
    myWarning("Server on listening...", "myTcpServerSimple");
    
    sockfd_copy = myTcpServerAccept(sockfd, &clientAddr);
    myWarning("Connection accepted", "myTcpServerSimple");
    
    childTask(sockfd_copy);
    
    Close(sockfd_copy);
    
  }
}

void myTcpServerOCPC(SOCKET sockfd, myTcpServerChildTask childTask) {
  tcpServerOCPC(sockfd, childTask, false);
}

void myTcpServerOCPCMax(SOCKET sockfd, int maxChildCount, myTcpServerChildTask childTask) {
  tcpServerOCPCMax(sockfd, maxChildCount, childTask, false);
}

void myTcpServerPreforked(SOCKET sockfd, int childCount, myTcpServerChildTask childTask) {
  tcpServerPreforked(sockfd, childCount - 1, childTask);
  
  // The father becomes the last child
  myTcpServerSimple(sockfd, childTask);
}

void myTcpServerMixed(SOCKET sockfd, int minChildCount, myTcpServerChildTask childTask) {
  tcpServerPreforked(sockfd, minChildCount, childTask);
  
  tcpServerOCPC(sockfd, childTask, true);
}

void myTcpServerMixedMax(SOCKET sockfd, int minChildCount, int maxChildCount, myTcpServerChildTask childTask) {
  tcpServerPreforked(sockfd, minChildCount, childTask);
  
  tcpServerOCPCMax(sockfd, maxChildCount, childTask, true);
}

static void tcpServerPreforked(SOCKET sockfd, int childCount, myTcpServerChildTask childTask) {
  int i;
  pid_t childpid;
  
  numChildren_preforked = childCount;
  childpids = (pid_t*)malloc(sizeof(pid_t) * numChildren_preforked);
  
  for (i = 0; i < numChildren_preforked; ++i) {
    
    childpid = fork();
    if (childpid == -1)
      mySystemError("fork", "tcpServerPreforked");
    
    if (childpid == 0)
      // child
      myTcpServerSimple(sockfd, childTask);
    
    // father
    childpids[i] = childpid;
    
  }
  
  if (signal(SIGINT, sigintHandler) == SIG_ERR)
    mySystemError("signal", "tcpServerPreforked");
}

static void tcpServerOCPC(SOCKET sockfd, myTcpServerChildTask childTask, bool resetSigInt) {
  pid_t childpid;
  SOCKET sockfd_copy;
  struct sockaddr_in clientAddr;
  
  if (signal(SIGCHLD, sigchldHandler) == SIG_ERR)
    mySystemError("signal", "tcpServerOCPC");
  
  while (1) {
    
    printf("\n");
    myWarning("Server on listening...  [%d children]", "tcpServerOCPC", numChildren_ocpc);
    
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
      
      childTask(sockfd_copy);
      Close(sockfd_copy);
      return;
    }
    
    // father
    numChildren_ocpc++;
    
    Close(sockfd_copy);
    
  }
}

static void tcpServerOCPCMax(SOCKET sockfd, int maxChildCount, myTcpServerChildTask childTask, bool resetSigInt) {
  pid_t childpid;
  SOCKET sockfd_copy;
  struct sockaddr_in clientAddr;
  sigset_t new_mask, old_mask;
  
  sigemptyset(&new_mask);
  sigaddset(&new_mask, SIGCHLD);
  
  if (signal(SIGCHLD, sigchldHandler) == SIG_ERR)
    mySystemError("signal", "tcpServerOCPCMax");
  
  while (1) {
    
    if (numChildren_ocpc == maxChildCount) {
      Sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
      waitForZombieChildren();
      
      myWarning("Server on waiting for a client to terminate...  [%d children]", "tcpServerOCPCMax", numChildren_ocpc); // numChildren_ocpc = 3
      if (wait(NULL) == -1)
	mySystemError("wait", "tcpServerOCPCMax");
      Sigprocmask(SIG_BLOCK, &old_mask, NULL);
    }
    
    printf("\n");
    myWarning("Server on listening...  [%d children]", "tcpServerOCPCMax", numChildren_ocpc);
    
    sockfd_copy = myTcpServerAccept(sockfd, &clientAddr);
    myWarning("Connection accepted", "tcpServerOCPCMax");
    
    childpid = fork();
    if (childpid == -1)
      mySystemError("fork", "tcpServerOCPCMax");
    
    if (childpid == 0) {
      // child
      Close(sockfd);
      
      if (resetSigInt == true && signal(SIGINT, SIG_DFL) == SIG_ERR)
	mySystemError("signal", "tcpServerOCPCMax");
      
      childTask(sockfd_copy);
      Close(sockfd_copy);
      return;
    }
    
    // father
    numChildren_ocpc++;
    
    Close(sockfd_copy);
    
  }
}

static void waitForZombieChildren() {
  pid_t childpid;
  
    childpid = waitpid(-1, NULL, WNOHANG);
    while (childpid > 0) {
      myWarning("Child %d was zombie  [%d children]", "waitForZombieChildren", childpid, numChildren_ocpc);
      childpid = waitpid(-1, NULL, WNOHANG);
    }
    
    if (childpid == -1) {
      if (errno == 10) // No child processes
	myWarning("Now there are no children  [%d children]", "waitForZombieChildren", numChildren_ocpc);
      else
	mySystemError("waitpid", "waitForZombieChildren");
      
    } else // childpid = 0
      myWarning("Now there are no zombie children  [%d children]", "waitForZombieChildren", numChildren_ocpc);
}

static void sigchldHandler(int s) {
  if (s == SIGCHLD) {
    numChildren_ocpc--;
    myWarning("Received SIGCHLD signal  [%d children]", "sigchldHandler", numChildren_ocpc);
    waitForZombieChildren();
  }
}

static void sigintHandler(int s) {
  int i;
  if (s == SIGINT) {
    myWarning("Received SIGINT signal", "sigintHandler");
    
    for (i = 0; i < numChildren_preforked; ++i)
      kill(childpids[i], SIGINT);
    
    for (i = 0; i < numChildren_preforked; ++i)
      waitpid(childpids[i], NULL, 0);
    
    exit(0);
  }
}