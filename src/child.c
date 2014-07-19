#include "srv.h"

pid_t
child_make(int i, int listenfd, int addrlen, void (*child_task)(int))
{
  pid_t   pid;
  void    child_main(int, int, int, void (*child_task)(int));

  if ( (pid = Fork()) > 0)
    return(pid);            /* parent */

  child_main(i, listenfd, addrlen, child_task);       /* never returns */
  return 0; /* just to shut down the debugger! */
}
/* end child_make */

/* include child_main */
void
child_main(int i, int listenfd, int addrlen, void (*child_task)(int))
{
  int              connfd;
  void             web_child(int);
  socklen_t        clilen;
  struct sockaddr *cliaddr;

  cliaddr = Malloc(addrlen);

  printf("child %ld starting\n", (long) getpid());
  for ( ; ; ) {
    clilen = addrlen;
    connfd = Accept(listenfd, cliaddr, &clilen);

    child_task(connfd);              /* process the request */
    Close(connfd);
  }
  return;
}
/* end child_main */
