#ifndef _COMMON_H

#define _COMMON_H

#define N 65 // 64 characters + \0

#define GET "GET"
#define OK "+OK\r\n"
#define QUIT "QUIT\r\n"
#define ERR "-ERR\r\n"

int myClose(int reply, int sockfd);

#endif