#include "lunp.h"
#include "ldf-srv.h"

int
  tcpsrv_ocpc(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int))
  {
    int			listenfd, connfd;
    pid_t			childpid;
    socklen_t		clilen, addrlen;
    struct sockaddr	*cliaddr;
    int                   i;

    if (argc == 2)
      listenfd = Tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 3)
      listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    else
      err_quit("usage: %s [ <host> ] <port#>", argv[0]);
    cliaddr = Malloc(addrlen);

    for (i = 0; i < sigc; i++)
      Signal(intv[i], sigv[i]);

    for ( ; ; ) {
      clilen = addrlen;
      if ( (connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
        if (errno == EINTR)
  	continue;		/* back to for() */
        else
  	err_sys("accept error");
      }

      if ( (childpid = Fork()) == 0) {	/* child process */
        Close(listenfd);	/* close listening socket */
        child_task(connfd);	/* process request */
        exit(0);
      }

      Close(connfd);			/* parent closes connected socket */
    }
    return 0;
  }
  /*
    PAY ATTENTION. Killing pids[i] that are equals to -1 will provoke a SYSTEM CRASH.
    Handle it with very much caution.
    CAN NOT WAIT PROCESS gracefully with SIG_INT
   */
int
  tcpsrv_ocpc_n(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int), int nchildren_max)
  {
    int			listenfd, connfd;
    pid_t			childpid;
    pid_t                 *pids_local;
    socklen_t		clilen, addrlen;
    struct sockaddr	*cliaddr;
    int                   nchildren_count;
    int                   i;
    sigset_t new_mask;
    sigset_t old_mask;
    int     stat;

    if (argc == 2)
      listenfd = Tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 3)
      listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    else
      err_quit("usage: %s [ <host> ] <port#>", argv[0]);
    cliaddr = Malloc(addrlen);

    /* Initialize data structure to memorize the pids */
    /* pids and nchildren are global variables defined in srv.h */
    pids_local = (int *)malloc(nchildren_max*sizeof(int));
    nchildren_count = 0;

    sigemptyset (&new_mask);
    sigaddset (&new_mask, SIGCHLD);

    for (i = 0; i < sigc; i++)
      Signal(intv[i], sigv[i]);

    for(i = 0; i <nchildren_max; i++)
      pids_local[i] = -1;

    for ( ; ; ) {
      clilen = addrlen;
      /* BLOCK THE FATHER if (nchildren_count >= nchildren_max) */
      Sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
      for (i = 0; i < nchildren_max; i++){
        if(pids_local[i]>0 && Waitpid(pids_local[i], NULL, WNOHANG) > 0){
  	     pids_local[i] = -1;
  	     nchildren_count--;
        }
      }
      if(nchildren_count >= nchildren_max) {
        childpid = waitpid(-1, &stat, WNOHANG);
        for(i = 0; i < nchildren_max; i++){
  	if(childpid == pids_local[i]) 
  	  pids_local[i] = -1;
        }
        nchildren_count--;
      }
      Sigprocmask(SIG_BLOCK, &old_mask, NULL);
      if ( (connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
        if (errno == EINTR)
  	continue;                       /* back to for() */
        else
  	err_sys("accept error");
      }

      if ( (childpid = Fork()) == 0) {	/* child process */
        Signal(SIGINT, NULL);             /* disable SIGINT handler in the child*/
        for(i=0; i<nchildren_max; i++){
  	if(pids_local[i] == -1) 
  	  pids_local[i] = childpid;
        }
        Close(listenfd);	                /* close listening socket */
        child_task(connfd);	        /* process request */
        exit(0);
      }

      Close(connfd);			/* parent closes connected socket */
      nchildren_count ++;
    }
    free(pids_local);
  }

int
  tcpsrv_pre(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int))
  {
    int             listenfd, i;
    socklen_t       addrlen;
    //pid_t           pid;

    if (argc == 3)
      listenfd = Tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 4)
      listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    else
      err_quit("usage: %s [ <host> ] <port#> <#children>", argv[0]);
    nchildren = atoi(argv[argc-1]);
    pids = Calloc(nchildren, sizeof(pid_t));

    for (i = 0; i < nchildren; i++)
      pids[i] = child_make(i, listenfd, addrlen, child_task);     /* parent returns */

    /* Father */
    /* 
       Signal handler of the father only. 
       The children do not handle them in this way.
    */
    for (i = 0; i < sigc; i++)
      Signal(intv[i], sigv[i]);

    for ( ; ; ){
      ;
    }
    return 0;
  }

/* ka: keep alive*/
int
  tcpsrv_preka(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int))
  {
    int             listenfd, i;
    socklen_t       addrlen;
    pid_t           pid;

    if (argc == 3)
      listenfd = Tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 4)
      listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    else
      err_quit("usage: %s [ <host> ] <port#> <#children>", argv[0]);
    nchildren = atoi(argv[argc-1]);
    pids = Calloc(nchildren, sizeof(pid_t));

    for (i = 0; i < nchildren; i++)
      pids[i] = child_make(i, listenfd, addrlen, child_task);     /* parent returns */

    /* Father */
    /* 
       Signal handler of the father only. 
       The children do not handle them in this way.
    */
    for (i = 0; i < sigc; i++)
      Signal(intv[i], sigv[i]);

    /* 
       Never ending loop to handle SIGCHLD.
       In case a child dies, it forks a new one.
    */
    for ( ; ; ){
      pid = wait(NULL);
      if(pid > 0){
        fprintf(stdout, "Child %d died.\n", pid);
        for(i = 0; i < nchildren; i++){
  	if(pids[i] == pid){
  	  pids[i] = child_make(i, listenfd, addrlen, child_task);
  	}
        }
      }
    }
    return 0;
  }

#define PROXY_BS 8192

static int
  streamer(int from, int to)
  {
    char buf[PROXY_BS];
    ssize_t rc;
    ssize_t amt;
    ssize_t offset;

    while (1) {
      rc = read(from, buf, sizeof(buf));
      if (rc <= 0) {
        if (rc == 0) break;
        if (errno == EINTR || errno == EAGAIN) continue;
        perror("read error");
        return -1;
      }
      offset = 0;
      amt = rc;
      while (amt) {
        rc = write(to, buf+offset, amt);
        if (rc < 0) {
  	if (errno == EINTR || errno == EAGAIN) continue;
  	perror("write error");
  	return -1;
        }
        offset += rc;
        amt -= rc;
      }
    }
    return 0;
  }

int tcpsrv_proxy(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[]){
    /* Connection params */
    int             in_listenfd, in_connfd;
    int             out_connfd;
    //int             i;
    struct sockaddr	*cliaddr;
    socklen_t       clilen, addrlen;
    //pid_t           pid;

    /* Internal variables */

    if (argc == 5){
      in_listenfd  = Tcp_listen(argv[1], argv[2], &addrlen);
    }
    else
      err_quit("usage: %s <host_in> <port_in#> <host_out> <port_out#>", argv[0]);

    nchildren = 1;
    pids = Calloc(nchildren, sizeof(pid_t));
    cliaddr = Malloc(addrlen);
    
    out_connfd   = Tcp_connect(argv[3], argv[4]);

    fprintf(stdout, "TCP Proxy server starts.\n");
    for ( ; ; ) {
      clilen = addrlen;
      if ( (in_connfd = accept(in_listenfd, cliaddr, &clilen)) < 0) {
        if (errno == EINTR)
  	continue;		/* back to for() */
        else
  	err_sys("accept error");
      }
      
      if ( (pids[0] = Fork()) == 0) {
        /* Child - handles IN -> OUT*/
        fprintf(stdout, "Child.\n");
        streamer(out_connfd, in_connfd);
        exit(0);
      }else{
        /* Father handles OUT -> IN */
        fprintf(stdout, "Father.\n");
        streamer(in_connfd, out_connfd);
      }
    }
    fprintf(stdout, "TCP Proxy server ends.");
    return 0;
  }


int
  tcpsrv_mixed(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*child_task)(int))
  {
    int                listenfd, connfd, i;
    struct sockaddr_in cliaddr;
    socklen_t          clilen, addrlen;
    pid_t              pid, childpid;
    int                nchildren_min;
    int                nchildren_count;
    sigset_t           new_mask;
    sigset_t           old_mask;

    if (argc == 4)
      listenfd = Tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 5)
      listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    else
      err_quit("usage: %s [ <host> ] <port#> <#children_min> <#children_max>", argv[0]);
    clilen = sizeof(struct sockaddr_in);
    nchildren = atoi(argv[argc-1]);
    nchildren_min = atoi(argv[argc-2]);
    pids = Calloc(nchildren_min, sizeof(pid_t));
    nchildren_count = 0;

    fprintf(stdout, "Main process start.\n");
    fprintf(stdout, "Mixed approach server.\n");
    fprintf(stdout, "%d children always on.\n", nchildren_min);
    fprintf(stdout, "%d children on demand.\n", nchildren-nchildren_min);
    fprintf(stdout, "%d children in total.\n\n", nchildren);

    for (i = 0; i < sigc; i++)
      Signal(intv[i], sigv[i]);

    for (i = 0; i < nchildren_min; i++){
      pids[i] = child_make(i, listenfd, addrlen, child_task);     /* parent returns */
      printf("HEREEE %d %d waitpid\n", i, pids[i]);
      nchildren_count++;
    }

    /* Father */  
    /* Initialize data structure to memorize the pids */
    /* pids and nchildren are global variables defined in srv.h */

    sigemptyset (&new_mask);
    sigaddset (&new_mask, SIGCHLD);
    for ( ; ; ) {
      /* BLOCK THE FATHER if (nchildren >= nchildren_max) */
      Sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
      while((pid = Waitpid(-1, NULL, WNOHANG)) > 0 ){
        for (i = 0; i < nchildren_min; i++){
  	if(pids[i] == pid){
  	  fprintf(stdout, "Child %d died.\n", pid);
  	  pids[i] = child_make(i, listenfd, addrlen, child_task);
  	}
        }
      }
      Sigprocmask(SIG_BLOCK, &old_mask, NULL);

      if(nchildren_count >= nchildren) {
        Wait(NULL);
        nchildren_count--;
      }

      if ( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0) {
        if (errno == EINTR)
  	continue;                       /* back to for() */
        else
  	err_sys("accept error");
      }

      if ( (childpid = Fork()) == 0) {	/* child process */
        Close(listenfd);	                /* close listening socket */
        child_task(connfd);	        /* process request */
        exit(0);
      }

      Close(connfd);			/* parent closes connected socket */
      nchildren_count ++;
    }

    /* 
       Signal handler of the father only. 
       The children do not handle them in this way.
    */
    for (i = 0; i < sigc; i++)
      Signal(intv[i], sigv[i]);

    /* 
       Never ending loop to handle SIGCHLD.
       In case a child dies, it forks a new one.
    */
    return 0;
  }

int
  tcpsrv_select(int argc, char *argv[], int sigc, void (*sigv[])(int), int intv[], void (*server_task)(int))
  {
    int			i, maxi, maxfd, listenfd, connfd, sockfd;
    int			nready, client[FD_SETSIZE];
    //ssize_t		n;
    fd_set		rset, allset;
    //char			buf[MAXLINE];
    socklen_t		clilen;
    struct sockaddr_in	cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    maxfd = listenfd;			/* initialize */
    maxi = -1;				/* index into client[] array */
    for (i = 0; i < FD_SETSIZE; i++)
      client[i] = -1;			/* -1 indicates available entry */
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    /* end fig01 */

    /* include fig02 */
    for ( ; ; ) {
      rset = allset;		/* structure assignment */
      nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

      if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
  #ifdef	NOTDEF
        printf("new client: %s, port %d\n",
  	     Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
  	     ntohs(cliaddr.sin_port));
  #endif

        for (i = 0; i < FD_SETSIZE; i++)
  	if (client[i] < 0) {
  	  client[i] = connfd;	        /* save descriptor */
  	  break;
  	}
        if (i == FD_SETSIZE)
  	err_quit("too many clients");

        FD_SET(connfd, &allset);        	/* add new descriptor to set */
        if (connfd > maxfd)
  	maxfd = connfd;			/* for select */
        if (i > maxi)
  	maxi = i;			/* max index in client[] array */

        if (--nready <= 0)
  	continue;			/* no more readable descriptors */
      }

      for (i = 0; i <= maxi; i++) {	/* check all clients for data */
        if ( (sockfd = client[i]) < 0)
  	continue;
        if (FD_ISSET(sockfd, &rset)) {
  	/* PAY */
  	server_task(sockfd);
  	
  	/* connection closed by client */
  	Close(sockfd);
  	FD_CLR(sockfd, &allset);
  	client[i] = -1;

  	if (--nready <= 0)
  	  break;			/* no more readable descriptors */
        }
      }
    }
  }