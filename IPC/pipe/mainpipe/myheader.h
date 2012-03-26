#include <sys/types.h> // pid_t
#include <stddef.h>   // NULL
#include <errno.h>    // noerror
//#include	<sys/stat.h> 
#include <fcntl.h> // O_CREATE
//#include <stdarg.h>
#include	<unistd.h> // STDOUT_FILENO
//#include	<syslog.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>

#include <stdio.h> //stdout
//#include	<sys/socket.h>	/* basic socket definitions */
//#include	<sys/time.h>	/* includes <time.h> unsafely */
//#include	<sys/stat.h>	/* for S_xxx file mode constants */
//#include	<sys/uio.h>		/* for iovec{} and readv/writev */

#ifdef	__bsdi__
#define	va_mode_t	int
#else
#define	va_mode_t	mode_t
#endif

#define	MAXLINE		4096	/* max text line length */
