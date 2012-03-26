#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
#include <pthread.h>
#define MUTEX_SEM_ID 0x1d2f3e4a

union semun
{
    int                val;
    struct             semid_ds *buf;
    unsigned short int *array;
    struct             seminfo *__buf;
};

int init_mutex_sem()
{
  int sem_id  = 0;
  int ret     = 0;
  int value   = 0;
  union   semun  un;
  
  sem_id =semget( MUTEX_SEM_ID,1,IPC_CREAT | 0666 | IPC_EXCL );
  if( sem_id == -1)
  {
    if( ( sem_id = semget( MUTEX_SEM_ID,1,0666 ) )!= -1  )
    {
      printf( "Error semget: %s\n",strerror( errno ) );
      return 0;
    }
    else
    {
      printf( "Error semget: %s\n",strerror( errno ) );
      return -1;
    }
  }
  printf( "sem_id = %d\n",sem_id );
  un.val = 1;
  ret = semctl( sem_id,0,SETVAL,1 );
  if( ret == -1)
  {
    printf( "Error semctl SETVAL:%s\n",strerror( errno ) );
    return -1;
  }
  else
  {
    printf( "semctl SETVAL successfully\n");
  }

/*  
  value = semctl( sem_id,0,GETVAL,0);
  if( value == -1)
  {
    printf( "Error semctl GETVAL: %s\n",strerror( errno ) );
    return -1;
  }
  else
  {
    printf("semctl GETVALUE successfully,value = %d\n",value);
  }
 
  struct  sembuf ops[1];
  ops[0].sem_num = 0;
  ops[0].sem_op = -1;
  ops[0].sem_flg = IPC_NOWAIT; 

  ret = semop( sem_id,ops,1 );
  if( ret == -1)
  {
    printf( "Error semop: %s\n", strerror( errno ) );
    return -1;
  }
  else
  {
    printf( "Semop successfully!\n" );
  }
  
  
  value=semctl( sem_id,0,GETVAL,0);
  if( value == -1)
  {
    printf( "Error semctl GETVAL: %s\n",strerror( errno ) );
    return -1;
  }
  else
  {
    printf("semctl GETVALUE successfully,value = %d\n",value);
  }
*/

}

int lock_semaphore( int sem )
{
    struct  sembuf ops[1];

    ops[0].sem_num = 0;
    ops[0].sem_op = -1; //semophare's value add -1,it is -1+1=0,then the process which call semop(-1) will lock until shaophare's value becomes 1
    ops[0].sem_flg = 0;

    if ( semop ( sem, ops, 1 ) < 0 )
    {
        printf ( "Error sem_op:%s\n", strerror ( errno ) );
        return -1;
    }

    return 0;
}

int unlock_semaphore( int sem )
{
     struct  sembuf ops[1];

    ops[0].sem_num = 0;
    ops[0].sem_op = 1; 
    ops[0].sem_flg = 0;

    if ( semop ( sem, ops, 1 ) < 0 )
    {
        printf ( "Error sem_op:%s\n", strerror ( errno ) );
        return -1;
    }

    return 0;
}

int release_sem()
{
    int sem_id = 0;
    int ret    = 0;
    sem_id = semget ( MUTEX_SEM_ID, 0, IPC_EXCL );
    if( sem_id == -1)
    {
        printf( "Error release_sem semget: %s\n",strerror( errno ) );
        return -1;
    }
    if ( semctl ( sem_id, 0, IPC_RMID, NULL ) < 0 )
    {
        printf( "Error release_sem semctl:%s\n", strerror ( errno ) );
        return -1;
    }
    
    return 0;
}
void* fun1( void *arg )
{
    int sem = 0;
    
    #ifdef USE_SEM_LOCK
    if ( ( sem = semget( MUTEX_SEM_ID, 1, 0666) ) < 0 )
    {
        printf ( "Get sem id error\n" );
        return -1;
    }

    if ( lock_semaphore ( sem ) < 0 )
    {
        printf ( "lock cache error\n" );
        return -1;
    }
    #endif

    int n = 0;
    while( n < 100000 )
    {
        if( n%100 != 0 )
            printf( "Fun1 n = %d \t", n );
        else
            printf( "Fun1 n = %d \t \n ", n );
        n++; 
    }
   
    #ifdef USE_SEM_LOCK
    unlock_semaphore( sem );
    #endif

}



void* fun2( void *arg )
{
    int sem = 0;
    
    #ifdef USE_SEM_LOCK
    if ( ( sem = semget( MUTEX_SEM_ID, 1, 0666) ) < 0 )
    {
        printf ( "Get sem id error\n" );
        return -1;
    }

    if ( lock_semaphore ( sem ) < 0 )
    {
        printf ( "lock cache error\n" );
        return -1;
    }
    #endif
    
    int n = 0;
    while( n < 100000 )
    {
        if( n%100 != 0 )
            printf( "================================= = %d \t", n );
        else
            printf( "================================= = %d \t \n ", n );
        n++;
    }
    
    #ifdef USE_SEM_LOCK
    unlock_semaphore( sem );
    #endif
}

int main( int argc, char *argv[] )
{
    if( argc == 2 && strcmp( argv[1], "release") == 0 )
    {
        release_sem();
    }

    init_mutex_sem();

    pthread_t   tid1, tid2, tid3;
    struct sched_param sched;
    pthread_attr_t  attr;
    memset ( &sched, 0, sizeof ( sched ) );
    
    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        printf ( "Error, pthread_attr_init \n" );
        return -1;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        printf ( "Error, pthread_attr_setschedpolicy \n" );
        return -1;
    }

    sched.sched_priority = 76;
    if ( ( pthread_attr_setschedparam ( &attr, &sched ) != 0 ) )
    {
        printf ( "Error, pthread_attr_setschedparam \n" );
        return -1;
    }
    
    if ( ( pthread_create ( &tid1, &attr, fun1, 0 ) != 0 ) )
    {
        printf ( "Error, pthread_create \n" );
        return -1;
    }
    
    
    memset ( &sched, 0, sizeof ( sched ) );

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        printf ( "Error, pthread_attr_init \n" );
        return -1;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        printf ( "Error, pthread_attr_setschedpolicy \n" );
        return -1;
    }

    sched.sched_priority = 80;
    if ( ( pthread_attr_setschedparam ( &attr, &sched ) != 0 ) )
    {
        printf ( "Error, pthread_attr_setschedparam \n" );
        return -1;
    }
    
    if ( ( pthread_create ( &tid3, &attr, fun2, 0 ) != 0 ) )
    {
        printf ( "Error, pthread_create \n" );
        return -1;
    }
    
    while(1)
    {
        sleep(10);
    }
}

/*
when the value of a semaphore is zero,the thread or process go to sleep while calling semop.
*/
