#include "lunp.h"

void myError(const char *errorMsg, const char *bracketText) {
  if (errorMsg == NULL) {
    if (bracketText == NULL)
      err_quit("Error\n");
    else
      err_quit("[%s] Error\n", bracketText);
  } else {
    if (bracketText == NULL)
      err_quit("%s\n", errorMsg);
    else
      err_quit("[%s] %s\n", bracketText, errorMsg);
  }
}

void myFunctionError(const char *functionName, const char *errorMsg, const char *bracketText) {
  if (bracketText == NULL) {
    if (errorMsg == NULL)
      err_quit("Error in function %s()\n", functionName);
    else
      err_quit("Error in function %s(): %s\n", functionName, errorMsg);
  } else {
    if (errorMsg == NULL)
      err_quit("[%s] Error in function %s()\n", bracketText, functionName);
    else
      err_quit("[%s] Error in function %s(): %s\n", bracketText, functionName, errorMsg);
  }
}

void mySystemError(const char *systemCallName, const char *bracketText) {
  if (bracketText == NULL)
    err_quit("Error in system call %s(): %s\n", systemCallName, strerror(errno));
  else
    err_quit("[%s] Error in system call %s(): %s\n", bracketText, systemCallName, strerror(errno));
}

bool myWarning(const char *warningMsg, const char *bracketText) {
  if (warningMsg == NULL) {
    if (bracketText == NULL)
      printf("Warning\n");
    else
      printf("[%s] Warning\n", bracketText);
  } else {
    if (bracketText == NULL)
      printf("%s\n", warningMsg);
    else
      printf("[%s] %s\n", bracketText, warningMsg);
  }
  return false;
}

bool myFunctionWarning(const char *functionName, const char *warningMsg, const char *bracketText) {
  if (bracketText == NULL) {
    if (warningMsg == NULL)
      printf("Warning in function %s()\n", functionName);
    else
      printf("Warning in function %s(): %s\n", functionName, warningMsg);
  } else {
    if (warningMsg == NULL)
      printf("[%s] Warning in function %s()\n", bracketText, functionName);
    else
      printf("[%s] Warning in function %s(): %s\n", bracketText, functionName, warningMsg);
  }
  return false;
}

bool mySystemWarning(const char *systemCallName, const char *bracketText) {
  if (bracketText == NULL)
    printf("Warning in system call %s(): %s\n", systemCallName, strerror(errno));
  else
    printf("[%s] Warning in system call %s(): %s\n", bracketText, systemCallName, strerror(errno));
  return false;
}