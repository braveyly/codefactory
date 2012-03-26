#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

#define FILE_SHM_ID 0x1f234d5a
#define FILE_SHM_LEN (1 << 18) /*256K*/

int main()
{
  int sem_id = 0;
  void *buf  = NULL;
  int ret    = 0;
  char * temp=( char * )malloc( 100*sizeof(char)) ;
  sem_id = shmget( FILE_SHM_ID,FILE_SHM_LEN,IPC_CREAT | 0666 | IPC_EXCL);//Create share memory
  if( sem_id == -1 )
  {
    if( (sem_id = shmget( FILE_SHM_ID,FILE_SHM_LEN,0666 ) ) != -1 )
    { 
       printf( "Error shmget: %s\n",strerror( errno ) ); //Share memory already exists 
    }
    else
    {
      printf( "Error shmget: %s\n",strerror( errno ) );
      return -1;
    }  
  }
  else
  {
    printf( "Create or Get shm successfully!");
  }

  buf = shmat( sem_id,NULL,0 );//Buf represent the share memory in this process
  if( buf == ( void * ) -1 )
  {
    printf("Error shmat: %s\n",strerror(errno));
    return -1;
  }
  else
  {
    printf("Shmat successfully!\n");
  } 
  
  memcpy( temp,buf,6 );
  printf( "*buf = %s \n",temp );

  memcpy( temp,buf+100,4 );
  printf( "buf = %s \n", temp );
  
  ret = shmdt(buf);//Detach the share memory from this process
  if( ret == -1 )
  {
    printf( "Error shmdt: %s\n",strerror( errno ) );
    return -1;
  }
  else
  {
    printf("Shmdt successfully!\n");
  }

  ret = shmctl( sem_id,IPC_RMID,NULL );//Remove/Delete the share memory
  if( ret == -1)
  {
    printf( "Error shmctl:%s\n", strerror(errno) );
    return -1;
  }
  else
  {
    printf( "Shmctl successfully!\n" );
  }
  return 0;
}
