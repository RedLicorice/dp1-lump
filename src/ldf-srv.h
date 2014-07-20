#ifndef __ldf_srv_h
#define __ldf_srv_h

static int       nchildren;
static pid_t    *pids;

/*
 * usage: child_make(child_idx, listenfd, addrlen)
 *  child_idx is not used by default (but may be useful)
 */
static pid_t child_make(int i, int listenfd, int addrlen, void (*child_task)(int));
static void child_main(int i, int listenfd, int addrlen, void (*child_task)(int));

static pid_t
child_make(int i, int listenfd, int addrlen, void (*child_task)(int))
{
  pid_t   pid;

  if ( (pid = Fork()) > 0)
    return(pid);            /* parent */

  child_main(i, listenfd, addrlen, child_task);       /* never returns */
  return 0; /* just to shut down the debugger! */
}

static void
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

#endif