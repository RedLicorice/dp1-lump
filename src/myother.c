#include "lunp.h"
#include <stdarg.h>

bool myWaitForSingleObject(int maxSeconds, int fileDescriptor) {
  fd_set cset;
  struct timeval timeout;
  int selectReply;
  
  FD_ZERO(&cset);
  FD_SET(fileDescriptor, &cset);
  
  if (maxSeconds > -1) {
    timeout.tv_sec = maxSeconds;
    timeout.tv_usec = 0;
    selectReply = select(FD_SETSIZE, &cset, NULL, NULL, &timeout);
  
  } else // maxSeconds == -1 (infinite timeout)
    selectReply = select(FD_SETSIZE, &cset, NULL, NULL, NULL);
    
  if (selectReply == -1)
    mySystemError("select", "myWaitForSingleObject");
  
  if (selectReply == 0)
    return false; // expired timeout
    
  return true;
}

int myWaitForMultipleObjects(int maxSeconds, int fileDescriptorCount, ...) { // http://msdn.microsoft.com/en-us/library/kb57fad8.aspx
  va_list ap, ap2;
  fd_set cset;
  struct timeval timeout;
  int selectReply, fileDescriptor, i;
  
  FD_ZERO(&cset);
  
  va_start(ap, fileDescriptorCount);
  va_copy(ap2, ap);
  
  for (i = 0; i < fileDescriptorCount; ++i) {
    fileDescriptor = va_arg(ap, int);
    FD_SET(fileDescriptor, &cset);
  }
  
  va_end(ap);
  
  if (maxSeconds > -1) {
    timeout.tv_sec = maxSeconds;
    timeout.tv_usec = 0;
    selectReply = select(FD_SETSIZE, &cset, NULL, NULL, &timeout);
  
  } else // maxSeconds == -1 (infinite timeout)
    selectReply = select(FD_SETSIZE, &cset, NULL, NULL, NULL);
    
  if (selectReply == -1)
    mySystemError("select", "myWaitForMultipleObjects");
  
  if (selectReply == 0) {
    va_end(ap2);
    return 0; // expired timeout
  }
    
  for (i = 0; i < fileDescriptorCount; ++i) {
    fileDescriptor = va_arg(ap2, int);
    if (FD_ISSET(fileDescriptor, &cset)) {
      va_end(ap2);
      return (i + 1);
    }
  }
  
  myError("Can not determine which file descriptor has become ready", "myWaitForMultipleObjects");
  return -1; // just to shut down a compiler warning
}

int myWaitForMultipleObjectsArray(int maxSeconds, int fileDescriptorCount, int *fileDescriptorArray) {
  fd_set cset;
  struct timeval timeout;
  int i, selectReply;
  
  FD_ZERO(&cset);
  
  for (i = 0; i < fileDescriptorCount; ++i)
    FD_SET(fileDescriptorArray[i], &cset);
  
  if (maxSeconds > -1) {
    timeout.tv_sec = maxSeconds;
    timeout.tv_usec = 0;
    selectReply = select(FD_SETSIZE, &cset, NULL, NULL, &timeout);
  
  } else // maxSeconds == -1 (infinite timeout)
    selectReply = select(FD_SETSIZE, &cset, NULL, NULL, NULL);
    
  if (selectReply == -1)
    mySystemError("select", "myWaitForMultipleObjectsArray");
  
  if (selectReply == 0)
    return 0; // expired timeout
    
  for (i = 0; i < fileDescriptorCount; ++i)
    if (FD_ISSET(fileDescriptorArray[i], &cset))
      return (i + 1);
  
  myError("Can not determine which file descriptor has become ready", "myWaitForMultipleObjectsArray");
  return -1; // just to shut down a compiler warning
}

bool fileExists(const char *filePath) {
  // http://stackoverflow.com/a/230068
  if (access(filePath, R_OK) == -1)
    return false;
  else
    return true;
}

uint32_t getFileSize(const char *filePath) {
  struct stat st;
  stat(filePath, &st);
  return (uint32_t)st.st_size; // HP: not too big files
}