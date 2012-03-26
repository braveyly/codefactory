#include	<sys/msg.h>
#include <fcntl.h> // O_CREATE
#include <sys/types.h> // pid_t
#include	<sys/stat.h> 
#include <limits.h>

#define	MAXMESGDATA	(PIPE_BUF - 2*sizeof(long))
#define	MESGHDRSIZE	(sizeof(struct mymesg) - MAXMESGDATA)
struct mymesg {
  long	mesg_len;	/* #bytes in mesg_data, can be 0 */
  long	mesg_type;	/* message type, must be > 0 */
  char	mesg_data[MAXMESGDATA];
};
