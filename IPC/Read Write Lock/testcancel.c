/* include main */
//#include	"unpipc.h"
//#include	"pthread_rwlock.h"
#include <pthread.h>
pthread_rwlock_t	rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_t	tid1, tid2;
void	 *thread1(void *), *thread2(void *);

int
main(int argc, char **argv)
{
	void	*status;

//	Set_concurrency(2);
	pthread_create(&tid1, NULL, thread1, NULL);
	sleep(1);		/* let thread1() get the lock */
	pthread_create(&tid2, NULL, thread2, NULL);

	pthread_join(tid2, &status);
	if (status != PTHREAD_CANCELED)
		printf("thread2 status = %p\n", status);
	pthread_join(tid1, &status);
	if (status != NULL)
		printf("thread1 status = %p\n", status);

//	printf("rw_refcount = %d, rw_nwaitreaders = %d, rw_nwaitwriters = %d\n",
//		   rwlock.rw_refcount, rwlock.rw_nwaitreaders,
//		   rwlock.rw_nwaitwriters);
	pthread_rwlock_destroy(&rwlock);
printf("main over\n");
sleep(5);
	exit(0);
}

void *
thread1(void *arg)
{
	pthread_rwlock_rdlock(&rwlock);
	printf("thread1() got a read lock\n");
	sleep(3);		/* let thread2 block in pthread_rwlock_wrlock() */
	printf("cancel thread2\n");
   pthread_cancel(tid2);
	sleep(3);
   printf("thread1 unlock rdlock\n");
	pthread_rwlock_unlock(&rwlock);
	return(NULL);
}

void *
thread2(void *arg)
{
	printf("thread2() trying to obtain a write lock\n");
	pthread_rwlock_wrlock(&rwlock);
	printf("thread2() got2 a write lock\n");	/* should not get here */
	sleep(1);
   printf("thread2 unlock wrlock\n");
	pthread_rwlock_unlock(&rwlock);
	return(NULL);
}
/* end main */
