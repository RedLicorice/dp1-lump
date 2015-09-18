#include <stdio.h>
#include "../utils.h"
#include "../lunp.h"
#include "../my_mytcp.h"

#define SERVER_ADDR_ARG  argv[1]
#define SERVER_PORT_ARG  argv[2]

#define ID_ARG			 argv[3]
#define OP1_ARG			 argv[4]
#define OP2_ARG			 argv[5]

uint32_t id, op1, op2, res;
struct sockaddr_in *daddr;

void clientRequest(SOCKET sockfd, struct sockaddr_in *daddr);
exit_code serverResponse(SOCKET sockfd);

void scriviFile(exit_code ec);

int main(int argc, char *argv[])
{
	SOCKET sockfd;
	sockfd = myUdpClientStartup(SERVER_ADDR_ARG, SERVER_PORT_ARG, &daddr);

	id = (uint32_t)atoi(ID_ARG);
	op1 = (uint32_t)atoi(OP1_ARG);
	op2 = (uint32_t)atoi(OP2_ARG);

	clientRequest(sockfd, daddr);
	exit_code ec = serverResponse(sockfd);

	scriviFile(ec);

	Close(sockfd);
	return 0;
}

void clientRequest(SOCKET sockfd, struct sockaddr_in *daddr)
{
	Request req;
	req.id = htonl(id);
	req.op1 = htonl(op1);
	req.op2 = htonl(op2);

	myUdpWriteBytes(sockfd, (void*)&req, sizeof(Request), *daddr);
}

exit_code serverResponse(SOCKET sockfd)
{
	uint attempt = 0;
	exit_code ec = wrong_response;

	Response resp;

	while (ec == wrong_response) {
		if (!myWaitForSingleObject(3, sockfd)) {
			if (attempt < 3) {
				clientRequest(sockfd, daddr);
				attempt++;
			}
			else
				return expired_timeout;
		}


		if (!myUdpReadBytes(sockfd, (void*)&resp, sizeof(Response), NULL, NULL)) {
			ec = wrong_response;
			continue;
		}

		if (ntohl(resp.id) != id) {
			ec = wrong_response;
			continue;
		}

		res = ntohl(resp.res);
		ec = success;
	}

	return ec;
}

void scriviFile(exit_code ec) {
	FILE* fp;

	fp = fopen("output.txt", "w");
	fprintf(fp, "%d", ec);

	if(ec == success)
		fprintf(fp, " %u", res);

	fclose(fp);
}
