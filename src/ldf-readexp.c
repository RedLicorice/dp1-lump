#include	"lunp.h"
#include "readline.h"

static ssize_t
my_peep(int fd, char *ptr);

ssize_t
readexp(int fd, void *vptr, size_t maxlen);

static ssize_t
my_peep(int fd, char *ptr)
{

	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	*ptr = *read_ptr;
	return(1);
}

ssize_t
readexp(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\r'){
			  if ( (rc = my_peep(fd, &c)) == 1) {
			    *ptr++ = c;
			    if (c == '\n'){
			      *ptr++ = c;
			      break;	/* newline is stored, like fgets() */
			    }else{
			      break;
			    }
			  } 
			}
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

ssize_t
Readexp(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readexp(fd, ptr, maxlen)) < 0)
		err_sys("readline error");
	return(n);
}
