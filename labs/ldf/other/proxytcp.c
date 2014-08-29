#include "../../../lunpv13e/lib/lunp.h"
#include "../../../lunpv13e/lib/srv.h"
void
sig_int(int signo){
  int i;

  fprintf(stdout, "In the SIG_INT handler\n");

  /* terminate all children */
  for (i = 0; i < nchildren; i++)
    kill(pids[i], SIGTERM);
  while (wait(NULL) > 0)          /* wait for all children */
    ;
  if (errno != ECHILD)
    err_sys("wait error");

  exit(0);
}

void
sig_usr1(int signo){
  fprintf(stdout, "In the SIG_USR1 handler\n");
}

int main(int argc, char *argv[]){
  void ( *sigv[2] ) (int) = {sig_int, sig_usr1};
  int intv[2] = {SIGINT, SIGUSR1};
  tcpsrv_proxy(argc, argv, 2, sigv, intv);
  return 0;
}
