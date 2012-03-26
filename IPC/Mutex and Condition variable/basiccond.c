
#include <pthread.h>
#include <errno.h>
#include	<stdio.h>
#include <stdlib.h>
#define	MAXNITEMS 		1000000
#define	MAXNTHREADS			100

		/* globals shared by threads */
int		nitems;				/* read-only by producer and consumer */
int		buff[MAXNITEMS];

  pthread_mutex_t	mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t	cond=PTHREAD_COND_INITIALIZER;
  int				nput=0;
  int				nval=0;
  int				nready=0;


void	*produce(void *), *consume(void *);

/* include main */
int
main(int argc, char **argv)
{
	int			i, nthreads, count[MAXNTHREADS];
	pthread_t	tid_produce[MAXNTHREADS], tid_consume;


	nitems = 10000;
	nthreads = 4;

//	Set_concurrency(nthreads + 1);
		/* 4create all producers and one consumer */
	for (i = 0; i < nthreads; i++) {
		count[i] = 0;
		pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}
	pthread_create(&tid_consume, NULL, consume, NULL);

		/* wait for all producers and the consumer */
	for (i = 0; i < nthreads; i++) {
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);	
	}
	pthread_join(tid_consume, NULL);

	exit(0);
}
/* end main */

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
		nready++;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		*((int *) arg) += 1;
	}
}

/* include consume */
void *
consume(void *arg)
{
	int		i;

	for (i = 0; i < nitems; i++) {
		pthread_mutex_lock(&mutex);
		while (nready == 0)
			pthread_cond_wait(&cond, &mutex);
		nready--;
		pthread_mutex_unlock(&mutex);

		if (buff[i] != i)
			printf("buff[%d] = %d\n", i, buff[i]);
	}
	return(NULL);
}
/* end consume */
