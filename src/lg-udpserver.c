#include "lunp.h"

typedef struct client_s {
  struct sockaddr_in clientStruct;
  int numberOfDatagrams;
} client_t;

static int numChildren = 0;

static pid_t *childpids;

static int searchForClient(struct sockaddr_in clientStruct, client_t *clientList, int clientListIndex);
static bool areSockaddrinEqual(struct sockaddr_in address1, struct sockaddr_in address2);

static void sigintHandler(int s);

/*SOCKET myUdpServerStartup(const char *serverPort) {
  SOCKET sockfd;
  struct sockaddr_in saddr;
  
  sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  //saddr.sin_len = sizeof(struct sockaddr_in);
  saddr.sin_family = AF_INET; // IPv4
  saddr.sin_port = htons(atoi(serverPort));
  saddr.sin_addr.s_addr = INADDR_ANY;
  
  Bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in));
  
  return sockfd;
}*/

SOCKET myUdpServerStartup(const char *serverPort) {
  return Udp_server(NULL, serverPort, NULL);
}

void myUdpServerSimple(SOCKET sockfd, myTcpServerChildTask childTask) {
  while (1) {
    
    printf("\n");
    myWarning("Server on listening...", "myUdpServerSimple");
    
    childTask(sockfd);
    
  }
}

void myUdpServerPreforked(SOCKET sockfd, int childCount, myTcpServerChildTask childTask) {
  int i;
  pid_t childpid;
  
  numChildren = childCount - 1;
  childpids = (pid_t*)malloc(sizeof(pid_t) * numChildren);
  
  for (i = 0; i < numChildren; ++i) {
    
    childpid = fork();
    if (childpid == -1)
      mySystemError("fork", "myUdpServerPreforked");
    
    if (childpid == 0)
      // child
      myUdpServerSimple(sockfd, childTask);
    
    // father
    childpids[i] = childpid;
    
  }
  
  if (signal(SIGINT, sigintHandler) == SIG_ERR)
    mySystemError("signal", "myUdpServerPreforked");
  
  // The father becomes the last child
  myUdpServerSimple(sockfd, childTask);
}

bool myUdpLimitClients(struct sockaddr_in clientStruct, int maxDatagrams, int maxClients) {
  static client_t *clientList;
  static int clientListIndex = -1;
  
  int foundIndex, i;
  
  if (clientListIndex == -1) // first time
    clientList = (client_t*)malloc(sizeof(client_t) * maxClients);
  
  else {
    foundIndex = searchForClient(clientStruct, clientList, clientListIndex);
    
    if (foundIndex > -1) { // found
      if (clientList[foundIndex].numberOfDatagrams == maxDatagrams)
	return false;
      
      else {
	clientList[foundIndex].numberOfDatagrams++;
	return true;
      }
    }
  }
  
  if (clientListIndex == maxClients - 1)
    for (i = 0; i < clientListIndex; ++i)
      clientList[i] = clientList[i + 1];
  else
    clientListIndex++;
    
  clientList[clientListIndex].clientStruct = clientStruct;
  clientList[clientListIndex].numberOfDatagrams = 1;
  
  return true;
}

static int searchForClient(struct sockaddr_in clientStruct, client_t *clientList, int clientListIndex) {
  int i;
  for (i = 0; i <= clientListIndex; ++i)
    if (areSockaddrinEqual(clientStruct, clientList[i].clientStruct) == true)
      return i;
  return -1;
}

static bool areSockaddrinEqual(struct sockaddr_in struct1, struct sockaddr_in struct2) {
  if (struct1.sin_family != struct2.sin_family)
    return false;
  if (struct1.sin_port != struct2.sin_port)
    return false;
  if (struct1.sin_addr.s_addr != struct2.sin_addr.s_addr)
    return false;
  return true; // equal
}

static void sigintHandler(int s) {
  int i;
  if (s == SIGINT) {
    myWarning("Received SIGINT signal", "sigintHandler");
    
    for (i = 0; i < numChildren; ++i)
      kill(childpids[i], SIGINT);
    
    for (i = 0; i < numChildren; ++i)
      waitpid(childpids[i], NULL, 0);
    
    exit(0);
  }
}