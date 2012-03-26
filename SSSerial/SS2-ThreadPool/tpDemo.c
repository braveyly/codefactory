#include "thread-pool.h"

void workFun(tp_work *work, tp_work_desc *job)
{
	printf( "this work\n" );
	printf( "instr %s oustr %s chnum %d", job->inum, job->onum, job->chnum );
	return;
}

int main()
{
	tp_work_desc job;
	tp_work worker;
	char inStr[] = "111";
	char ouStr[] = "222";
	
	memset( &job, 0 ,sizeof(job) );
	memset( &worker, 0 ,sizeof(worker) );
   
	job.inum = inStr;
	job.onum = ouStr;
	job.chnum = 3;
	worker.process_job = workFun;

	tp_thread_pool *myThreadPool = creat_thread_pool( 3, 10 );
	myThreadPool->init( myThreadPool );
	sleep(1);
	myThreadPool->process_job( myThreadPool, &worker, &job );

    while(1)
    {
		sleep(1);
	}
	
}
