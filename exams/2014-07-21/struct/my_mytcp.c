#include "lunp.h"

bool myMyTcpReadFromFileAndWriteChunks(SOCKET sockfd, char* fileName, int *byteLetti) {
	int byteDaInviare;
	int byteDaLeggere = getFileSize(fileName);
	int chunkSize = DEFAULT_CHUNK_SIZE;
	char *buffer = (char*)malloc(chunkSize*sizeof(char));

	if (byteLetti != NULL)
		*byteLetti = 0;

	FILE *fp = fopen(fileName, "r");

	while(byteDaLeggere > 0) {
		bzero(buffer, chunkSize);
		byteDaInviare = fread(buffer, 1, chunkSize, fp);

		if (!myTcpWriteBytes(sockfd, buffer, byteDaInviare)) {
			free(buffer);
			fclose(fp);
			return false;
		}

		if (byteLetti != NULL)
			*byteLetti += byteDaInviare;

		byteDaLeggere -= byteDaInviare;
	}

	free(buffer);
	fclose(fp);

	return true;
}

void myMyTcpReadChunksAndWriteToFile(SOCKET sockfd, char* fileName, int *byteLetti) {
	int readBytes;
	int chunkSize = DEFAULT_CHUNK_SIZE;
	bool readReply = true;
	char* buffer = (char*)malloc(chunkSize*sizeof(char));

	if (byteLetti != NULL)
		*byteLetti = 0;

	FILE *fp = fopen(fileName, "w");

	while (readReply) {
	  bzero(buffer, chunkSize);
	  readBytes = 0;

	  readReply = myTcpReadBytesAsync(sockfd, buffer, chunkSize, &readBytes);
	  fwrite(buffer, 1, readBytes, fp);

	  if (byteLetti != NULL)
		  *byteLetti += readBytes;

	}

	free(buffer);
	fclose(fp);
}

