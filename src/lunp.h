/* include lunph */

#ifndef __lunp_h
#define __lunp_h

#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>	/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>	/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>	/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>	/* for Unix domain sockets */
# include	<sys/select.h>	/* for convenience */
# include	<poll.h>	/* for convenience */
# include	<strings.h>	/* for convenience */

# include       <sys/ioctl.h>

#include <stdint.h>

/* Following shortens all the typecasts of pointer arguments: */
#define	SA	struct sockaddr
const struct in6_addr in6addr_any;

#define	LISTENQ		1024	/* 2nd argument to listen() */

#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */

/* Define some port number that  can be used for our examples */
#define	LOOPBACK_STR	"127.0.0.1"			/* TCP and UDP */
#define	SERV_PORT		 9877			/* TCP and UDP */
#define	SERV_PORT_STR	"9877"			/* TCP and UDP */
#define	UNIXSTR_PATH	"/tmp/unix.str"	/* Unix domain stream */
#define	UNIXDG_PATH		"/tmp/unix.dg"	/* Unix domain datagram */


int      tcp_connect(const char *, const char *);
int      tcp_listen(const char *, const char *, socklen_t *);

/* Signal handler */
void sig_int(int);
void sign_chld(int);

/* Wrappers */
/* wrapsock - wrappers to standard functions */
int	 Accept(int, SA *, socklen_t *);
void	 Bind(int, const SA *, socklen_t);
void	 Connect(int, const SA *, socklen_t);
void	 Getpeername(int, SA *, socklen_t *);
void	 Getsockname(int, SA *, socklen_t *);
void	 Getsockopt(int, int, int, void *, socklen_t *);
void	 Listen(int, int);
int	 Poll(struct pollfd *, unsigned long, int);
ssize_t	 Readline(int, void *, size_t);
ssize_t	 Readexp(int, void *, size_t);
ssize_t	 Readn(int, void *, size_t);
ssize_t	 Recv(int, void *, size_t, int);
ssize_t	 Recvfrom(int, void *, size_t, int, SA *, socklen_t *);
ssize_t	 Recvmsg(int, struct msghdr *, int);
int	 Select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
void	 Send(int, const void *, size_t, int);
void	 Sendto(int, const void *, size_t, int, const SA *, socklen_t);
void	 Sendmsg(int, const struct msghdr *, int);
void	 Setsockopt(int, int, int, const void *, socklen_t);
void	 Shutdown(int, int);
int	 Sockatmark(int);
int	 Socket(int, int, int);
void	 Socketpair(int, int, int, int *);
void	 Writen(int, void *, size_t);

/* wrappers to non-standard functions */
int      Tcp_connect(const char *, const char *);
int      Tcp_listen(const char *, const char *, socklen_t *);

ssize_t	 readline(int, void *, size_t);
ssize_t	 readexp(int, void *, size_t);
ssize_t	 readn(int, void *, size_t);

void	 str_cli(FILE *, int);
void     str_echo(int);

/* wraplib functions (are they Unix standard?)*/
const char	*Inet_ntop(int, const void *, char *, size_t);
void	        Inet_pton(int, const char *, void *);

/* Unix wrapper functions */
void	*Calloc(size_t, size_t);
void	 Close(int);
void	 Dup2(int, int);
int	 Fcntl(int, int, int);
void	 Gettimeofday(struct timeval *, void *);
int	 Ioctl(int, int, void *);
pid_t	 Fork(void);
void	*Malloc(size_t);
int	 Mkstemp(char *);
void	*Mmap(void *, size_t, int, int, int, off_t);
int	 Open(const char *, int, mode_t);
void	 Pipe(int *fds);
ssize_t	 Read(int, void *, size_t);
void	 Sigaddset(sigset_t *, int);
void	 Sigdelset(sigset_t *, int);
void	 Sigemptyset(sigset_t *);
void	 Sigfillset(sigset_t *);
int	 Sigismember(const sigset_t *, int);
void	 Sigpending(sigset_t *);
void	 Sigprocmask(int, const sigset_t *, sigset_t *);
char	*Strdup(const char *);
long	 Sysconf(int);
void	 Sysctl(int *, u_int, void *, size_t *, void *, size_t);
void	 Unlink(const char *);
pid_t	 Wait(int *);
pid_t	 Waitpid(pid_t, int *, int);
void	 Write(int, void *, size_t);

/* stdio wrappers functions*/
void	 Fclose(FILE *);
FILE	*Fdopen(int, const char *);
char	*Fgets(char *, int, FILE *);
FILE	*Fopen(const char *, const char *);
void	 Fputs(const char *, FILE *);

/* Signal handling functions */
typedef	void	Sigfunc(int);	/* for signal handlers */

Sigfunc *signal(int, Sigfunc *);
Sigfunc *Signal(int, Sigfunc *);


/* Error reporting functions */
void	 err_dump(const char *, ...);
void	 err_msg(const char *, ...);
void	 err_quit(const char *, ...);
void	 err_ret(const char *, ...);
void	 err_sys(const char *, ...);

/************************************************************************/
/* ldf-implemented functions                                            */
/* these functions have been added by ldf to the original unp.c library */
/* disclaimer: do not trust them                                        */
/************************************************************************/

/* bread: buffered read */
typedef struct bread{
  int   fd;
  int	read_cnt;
  char	*read_ptr;
  char	read_buf[MAXLINE];
} bread_t;

int     bread_init(bread_t **bread, int connfd);
int     bread_free(bread_t **bread);
ssize_t breadn(bread_t *bread, void *vptr, size_t n);
ssize_t breadline(bread_t *bread, void *vptr, size_t maxlen);
ssize_t breadexp(bread_t *bread, void *vptr, size_t maxlen);
ssize_t breadlinebuf(bread_t *bread, void **vptrptr);

int     Bread_init(bread_t **bread, int connfd);
int     Bread_free(bread_t **bread);
ssize_t Breadn(bread_t *bread, void *ptr, size_t nbytes);
ssize_t Breadline(bread_t *bread, void *ptr, size_t maxlen);
ssize_t Breadexp(bread_t *bread, void *vptr, size_t maxlen);

#endif
