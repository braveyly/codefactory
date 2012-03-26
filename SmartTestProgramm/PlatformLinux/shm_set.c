#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

#define FILE_SHM_ID 0x1f234d5a
#define FILE_SHM_LEN (1 << 18) /*256K*/

int main()
{
  int sem_id = 0;
  int ret    = 0;
  void *buf  = NULL;

  sem_id = shmget( FILE_SHM_ID,FILE_SHM_LEN,IPC_CREAT | 0666 | IPC_EXCL);

  if( sem_id == -1 )
  {
    if( (sem_id = shmget( FILE_SHM_ID,FILE_SHM_LEN,0666 ) ) != -1 )
    { 
       printf( "Error shmget: %s\n",strerror( errno ) ); 
    }
    else
    {
      printf( "Error shmget: %s\n",strerror( errno ) );
      return -1;
    }  
  }
  else
  {
    printf( "Create or Get shm successfully!\n" );
  }

  buf = shmat( sem_id,NULL,0 );
  if( buf == ( void * ) -1 )
  {
    printf("Error shmat: %s\n",strerror(errno));
    return -1;
  }
  else
  {
    printf("Shmat successfully!\n");
  }

  memset(buf,0,FILE_SHM_LEN);
  strncpy(buf,"begin",5);
  strncpy(buf+100,"end",3);
  
  ret = shmdt(buf);
  if( ret == -1 )
  {
    printf( "Error shmdt: %s\n",strerror( errno ) );
    return -1;
  }
  else
  {
    printf("Shmdt successfully!\n");
  }
  
  return 0;
}
