#include "../lunp.h"
#include "../my_mytcp.h"

#define SERVER_ADDRESS_ARG 	argv[1] // server address
#define SERVER_PORT_ARG 	argv[2] // server port
#define FILE_NAME 		argv[3]

static int count = 0;

void clientTask(SOCKET sockfd, char *fileName);


int main(int argc, char *argv[]) {
  SOCKET sockfd;
  sockfd = myTcpClientStartup(SERVER_ADDRESS_ARG, SERVER_PORT_ARG);

  clientTask(sockfd, FILE_NAME);

  myTcpWriteString(sockfd, QUIT);

  Close(sockfd);
  return 0;
}

void clientTask(SOCKET sockfd, char *fileName) {
  char serverRes[BUFFSIZE];
  char address[BUFFSIZE];
  char serv2addr[BUFFSIZE], serv2port[BUFFSIZE];
  uint32_t fileSize;

  myTcpWriteString(sockfd, GET);
  myTcpWriteString(sockfd, fileName);
  myTcpWriteString(sockfd, "\r\n");

  myTcpReadLine(sockfd, serverRes, BUFFSIZE, NULL);

  //-ERR\r\n
  if (strcmp(serverRes, ERR) == 0) {
    myWarning("Illegal command or non-existing file", "clientTask");
    return; // next file
  }

  //+OK\r\n
  if (!memcmp(serverRes, OK, strlen(OK))) {
	  myTcpReadBytes(sockfd, (void*)(&fileSize), sizeof(uint32_t), NULL);
	  fileSize = ntohl(fileSize);

	  if (myTcpReadChunksAndWriteToFile(sockfd, fileName, fileSize, NULL) == false)
		myError("Cannot write the file", "clientTask");
  }

  //-REDserv2addr serv2port\r\n
  if (!memcmp(serverRes, RED, strlen(RED))) {
	    count++;
	    if (count <= 4) {
			strcpy(address, serverRes + strlen(RED));
			address[strlen(address) - strlen("\r\n")] = '\0';

			sscanf(address, "%s %s", serv2addr, serv2port);

			sockfd = myTcpClientStartup(serv2addr, serv2port);
			clientTask(sockfd, fileName);
	    }

  }

}
