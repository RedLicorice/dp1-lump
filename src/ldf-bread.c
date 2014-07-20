#include	"lunp.h"

int     bread_init(bread_t **bread, int connfd);
int     bread_free(bread_t **bread);
ssize_t breadn(bread_t *bread, void *vptr, size_t n);
ssize_t breadline(bread_t *bread, void *vptr, size_t maxlen);
ssize_t breadexp(bread_t *bread, void *vptr, size_t maxlen);
ssize_t breadlinebuf(bread_t *bread, void **vptrptr);

int
bread_init(bread_t **bread, int connfd){
  if( ((*bread) = (bread_t *)malloc(sizeof(bread_t))) ){
    (*bread)->fd = connfd;
  }
  return ((*bread)==NULL) ? 0 : 1;
}

int
bread_free(bread_t **bread){
  // Does not deallocate the internal fd !
  free(*bread);
  return 0;
}

static ssize_t
my_bread(bread_t *bread, char *ptr)
{

	if (bread->read_cnt <= 0) {
again:
		if ( (bread->read_cnt = read(bread->fd, bread->read_buf, sizeof(bread->read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (bread->read_cnt == 0)
			return(0);
		bread->read_ptr = bread->read_buf;
	}

	bread->read_cnt--;
	*(ptr) = *(bread->read_ptr)++;
	return(1);
}

ssize_t
breadline(bread_t *bread, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_bread(bread, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

static ssize_t
my_bpeep(bread_t *bread, char *ptr)
{
	if (bread->read_cnt <= 0) {
again:
		if ( (bread->read_cnt = read(bread->fd, bread->read_buf, sizeof(bread->read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (bread->read_cnt == 0)
			return(0);
		bread->read_ptr = bread->read_buf;
	}

        *(ptr) = *(bread->read_ptr);
	return(1);
}

ssize_t
breadexp(bread_t *bread, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_bread(bread, &c)) == 1) {
			*ptr++ = c;
			if (c == '\r'){
			  if ( (rc = my_bpeep(bread, &c)) == 1) {
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
breadlinebuf(bread_t *bread, void **vptrptr)
{
	if (bread->read_cnt)
		*vptrptr = bread->read_ptr;
	return(bread->read_cnt);
}
/* end readline */

ssize_t
Breadline(bread_t *bread, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = breadline(bread, ptr, maxlen)) < 0)
		err_sys("readline error");
	return(n);
}
ssize_t
Breadexp(bread_t *bread, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = breadline(bread, ptr, maxlen)) < 0)
		err_sys("readline error");
	return(n);
}
int 
Bread_init(bread_t **bread, int connfd){
  if( !bread_init(bread, connfd) )
    err_sys("Bread_init error");
  return 0;
}

int
Bread_free(bread_t **bread){
  bread_free(bread);
  return 0;
}

/* Read "n" bytes from a descriptor. */
ssize_t
breadn(bread_t *bread, void *vptr, size_t n)
{
  size_t	nleft;
  //ssize_t	nread;
  char	*ptr;
  char	c;
  int rc;
    
  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ((rc = my_bread(bread, &c)) == 1) {
            
      *ptr++ =c;
      nleft -= 1;
            
    } else if (rc == 0) {
      break;
    } else{
      //nread = 0;
      return(-1);
    }
    
  }
  return(n - nleft);		/* return >= 0 */
}


ssize_t
Breadn(bread_t *bread, void *ptr, size_t nbytes)
{
  ssize_t		n;
    
  if ( (n = breadn(bread, ptr, nbytes)) < 0)
    err_sys("readn error");
  return(n);
}
