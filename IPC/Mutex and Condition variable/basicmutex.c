/* include main */
#include <pthread.h>
#include <errno.h>
#include	<stdio.h>
#include <stdlib.h>
#define	MAXLINE		4096	/* max text line length */
#define	MAXNITEMS 		1000000
#define	MAXNTHREADS			100

int		nitems;			/* read-only by producer and consumer */
pthread_mutex_t	mutex= PTHREAD_MUTEX_INITIALIZER;
int	buff[MAXNITEMS];
int	nput=0;
int	nval=0;


void	*produce(void *), *consume(void *);

int
main(int argc, char **argv)
{
	int			i, nthreads, count[MAXNTHREADS];
	pthread_t	tid_produce[MAXNTHREADS], tid_consume;

	
	nitems = 100000;
	nthreads = 3;

//	Set_concurrency(nthreads);
		/* 4start all the producer threads */
	for (i = 0; i < nthreads; i++) {
		count[i] = 0;
		pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}

		/* 4wait for all the producer threads */
	for (i = 0; i < nthreads; i++) {
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);	
	}

		/* 4start, then wait for the consumer thread */
	pthread_create(&tid_consume, NULL, consume, NULL);
	pthread_join(tid_consume, NULL);

	exit(0);
}
/* end main */

/* include producer */
void *
produce(void *arg)
{
	for ( ; ; ) {
		pthread_mutex_lock(&mutex);
		if (nput >= nitems) {
			pthread_mutex_unlock(&mutex);
			return(NULL);		/* array is full, we're done */
		}
		buff[nput] = nval;
		nput++;
		nval++;
		pthread_mutex_unlock(&mutex);
		*((int *) arg) += 1;
	}
}

void *
consume(void *arg)
{
	int		i;

	for (i = 0; i < nitems; i++) {
		if (buff[i] != i)
			printf("buff[%d] = %d\n", i, buff[i]);
	}
	return(NULL);
}
/* end producer */
