#include <stdio.h>
#include "../utils.h"
#include "../lunp.h"
#include "../my_mytcp.h"

#define SERVER_PORT_ARG  argv[1]
#define K_ARG			 argv[2]
uint32_t K;

void service(SOCKET sockfd);


int main(int argc, char *argv[]) {

	SOCKET sockfd;
	sockfd = myUdpServerStartup(SERVER_PORT_ARG);

	K = (uint32_t)atoi(K_ARG);

	myUdpServerSimple(sockfd, service);

	Close(sockfd);
	return 0;
}

void service(SOCKET sockfd) {
	uint32_t res, id;
	struct sockaddr_in cliaddr;

	Request req;

	if (!myUdpReadBytes(sockfd, (void*)&req, sizeof(Request), &cliaddr, NULL))
		return;

	res = (ntohl(req.op1) + ntohl(req.op2))%K;

	Response resp;
	resp.res = htonl(res);
	resp.id = req.id;
	myUdpWriteBytes(sockfd, (void*)&resp, sizeof(Response), cliaddr);
}
