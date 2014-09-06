#include "lunp.h"

bool myTcpReadXdr(SOCKET sockfd, myXdrFunction xdrFunction, void *data) {
  XDR xdrs;
  FILE *fd;
  bool_t success;
  
  fd = fdopen(dup(sockfd), "r");
  xdrstdio_create(&xdrs, fd, XDR_DECODE);
  setbuf(fd, NULL);
  
  success = xdrFunction(&xdrs, data);
  
  xdr_destroy(&xdrs);
  fclose(fd);
  
  if (success == FALSE)
    return false;
  return true;
}

bool myTcpWriteXdr(SOCKET sockfd, myXdrFunction xdrFunction, void *data) {
  XDR xdrs;
  FILE *fd;
  bool_t success;
  
  fd = fdopen(dup(sockfd), "w");
  xdrstdio_create(&xdrs, fd, XDR_ENCODE);
  setbuf(fd, NULL);
  
  success = xdrFunction(&xdrs, data);
  
  xdr_destroy(&xdrs);
  fclose(fd);
  
  if (success == FALSE)
    return false;
  return true;
}

XDR *myUdpReadXdrStartup(char *buffer, int bufferPos) {
  XDR *xdrs;
  xdrs = (XDR*)malloc(sizeof(XDR));
  xdrmem_create(xdrs, buffer, bufferPos, XDR_DECODE);
  return xdrs;
}

bool myUdpReadXdr(XDR *xdrs, myXdrFunction xdrFunction, void *data) {
  if (xdrFunction(xdrs, data) == FALSE)
    return false;
  return true;
}

void myUdpReadXdrCleanup(XDR *xdrs) {
  xdr_destroy(xdrs);
  free(xdrs);
}

XDR *myUdpWriteXdrStartup(char *buffer, int bufferSize) {
  XDR *xdrs;
  xdrs = (XDR*)malloc(sizeof(XDR));
  xdrmem_create(xdrs, buffer, bufferSize, XDR_ENCODE);
  return xdrs;
}

bool myUdpWriteXdr(XDR *xdrs, myXdrFunction xdrFunction, void *data) {
  if (xdrFunction(xdrs, data) == FALSE)
    return false;
  return true;
  if (xdrFunction(xdrs, data) == FALSE)
    return false;
  return true;
}

int myUdpWriteXdrCleanup(XDR *xdrs, bool success) {
  int bufferPos;
  if (success)
    bufferPos = xdr_getpos(xdrs);
  else
    bufferPos = -1;
  xdr_destroy(xdrs);
  free(xdrs);
  return bufferPos;
}