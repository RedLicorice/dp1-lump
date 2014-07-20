#include "lunp.h"

void myClose(SOCKET sockfd) {
  Close((int)sockfd); // wrapunix.c
}