#include "lunp.h"
#include <stdarg.h>

void myError(const char *errorMsg, const char *bracketText, ...) {
  va_list ap;
  char formattedErrorMsg[MAXLINE + 1];
  
  if (errorMsg == NULL) {
    if (bracketText == NULL)
      err_quit("Error\n");
    else
      err_quit("[%s] Error\n", bracketText);
  } else {
    va_start(ap, bracketText);
    vsprintf(formattedErrorMsg, errorMsg, ap);
    va_end(ap);
    
    if (bracketText == NULL)
      err_quit("%s\n", formattedErrorMsg);
    else
      err_quit("[%s] %s\n", bracketText, formattedErrorMsg);
  }
}

void myFunctionError(const char *functionName, const char *errorMsg, const char *bracketText, ...) {
  va_list ap;
  char formattedErrorMsg[MAXLINE + 1];
  
  if (errorMsg == NULL) {
    if (bracketText == NULL)
      err_quit("Error in function %s()\n", functionName);
    else
      err_quit("[%s] Error in function %s()\n", bracketText, functionName);
  } else {
    va_start(ap, bracketText);
    vsprintf(formattedErrorMsg, errorMsg, ap);
    va_end(ap);
    
    if (bracketText == NULL)
      err_quit("Error in function %s(): %s\n", functionName, formattedErrorMsg);
    else
      err_quit("[%s] Error in function %s(): %s\n", bracketText, functionName, formattedErrorMsg);
  }
}

void mySystemError(const char *systemCallName, const char *bracketText) {
  if (bracketText == NULL)
    err_quit("Error in system call %s(): %s\n", systemCallName, strerror(errno));
  else
    err_quit("[%s] Error in system call %s(): %s\n", bracketText, systemCallName, strerror(errno));
}

bool myWarning(const char *warningMsg, const char *bracketText, ...) {
  va_list ap;
  char formattedWarningMsg[MAXLINE + 1];
  
  if (warningMsg == NULL) {
    if (bracketText == NULL)
      printf("Warning\n");
    else
      printf("[%s] Warning\n", bracketText);
  } else {
    va_start(ap, bracketText);
    vsprintf(formattedWarningMsg, warningMsg, ap);
    va_end(ap);
    
    if (bracketText == NULL)
      printf("%s\n", formattedWarningMsg);
    else
      printf("[%s] %s\n", bracketText, formattedWarningMsg);
  }
  return false;
}

bool myFunctionWarning(const char *functionName, const char *warningMsg, const char *bracketText, ...) {
  va_list ap;
  char formattedWarningMsg[MAXLINE + 1];
  
  if (warningMsg == NULL) {
    if (bracketText == NULL)
      printf("Warning in function %s()\n", functionName);
    else
      printf("[%s] Warning in function %s()\n", bracketText, functionName);
  } else {
    va_start(ap, bracketText);
    vsprintf(formattedWarningMsg, warningMsg, ap);
    va_end(ap);
    
    if (bracketText == NULL)
      printf("Warning in function %s(): %s\n", functionName, formattedWarningMsg);
    else
      printf("[%s] Warning in function %s(): %s\n", bracketText, functionName, formattedWarningMsg);
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