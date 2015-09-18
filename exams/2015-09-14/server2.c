#include "../lunp.h"
#include "../my_mytcp.h"

#define SERVER_PORT_ARG 	  argv[1]

int n_arg;
char* server_addr2;
char* server_port2;

void childTask(SOCKET sockfd);

int main(int argc, char *argv[]) {
  SOCKET sockfd;
  sockfd = myTcpServerStartup(SERVER_PORT_ARG);

  n_arg = argc;

  if (n_arg > 2) {
	  server_addr2 = argv[2];
	  server_port2 = argv[3];
  }

  myTcpServerPreforked(sockfd, 3, &childTask);

  return 0;
}

void childTask(SOCKET sockfd) {
  char clientReq[BUFFSIZE];
  char address[BUFFSIZE];
  char nomeFile[MAXFILENAMELENGTH];
  uint32_t fileSize;


    if (!myTcpReadLine(sockfd, clientReq, BUFFSIZE, NULL))
      return;

    if (strcmp(clientReq, QUIT) == 0) {
      myWarning("Connection closed (QUIT)", "childTask");
      return;
    }

    // else: clientReq = GET
    strcpy(nomeFile, clientReq + strlen(GET));
    nomeFile[strlen(nomeFile) - strlen("\r\n")] = '\0';

    if (!fileExists(nomeFile)) {
      myWarning("The file does not exist", "childTask");
      if (n_arg == 2)
    	  myTcpWriteString(sockfd, ERR);
      else {
    	  sprintf(address, "%s %s", server_addr2, server_port2);
    	  myTcpWriteString(sockfd, RED);
    	  myTcpWriteString(sockfd, address);
    	  myTcpWriteString(sockfd, "\r\n");
      }
    }
    else
    {
      myWarning("Sending the file to the client...", "childTask");
      myTcpWriteString(sockfd, OK);

      fileSize = htonl(getFileSize(nomeFile));
      myTcpWriteBytes(sockfd, (void*)(&fileSize), sizeof(uint32_t));

      if (myTcpReadFromFileAndWriteChunks(sockfd, nomeFile, NULL) == false) {
		myWarning("The client closed the connection abruptly", "childTask");
		return;
      }

      myWarning("File sent successfully to the client", "childTask");
    }

}
