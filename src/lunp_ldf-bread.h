#ifndef	__lunp_ldf_bread_h
#define	__lunp_ldf_bread_h

/* bread: buffered read */
typedef struct bread{
  int   fd;
  int	read_cnt;
  char	*read_ptr;
  char	read_buf[MAXLINE];
} bread_t;

int     Bread_init(bread_t **bread, int connfd);
int     Bread_free(bread_t **bread);
ssize_t Breadn(bread_t *bread, void *ptr, size_t nbytes);
ssize_t Breadline(bread_t *bread, void *ptr, size_t maxlen);
ssize_t Breadexp(bread_t *bread, void *vptr, size_t maxlen);

#endif