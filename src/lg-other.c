#include "lunp.h"

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