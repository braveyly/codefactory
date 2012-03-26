# include <semaphore.h>	
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h>
int main(int argc, char **argv)
{
int a;
sem_t *sem;
int c=0;
c=sem_init(sem,0,11);
sem_getvalue(sem,&a);
printf("a=%d\n",a);

sem_post(sem);
sem_getvalue(sem,&a);
printf("a=%d\n",a);

sem_wait(sem);
sem_getvalue(sem,&a);
printf("a=%d\n",a);
if(c==-1)
printf("sem_init failed\n");
else
printf("sem_init success\n");
sem_destroy(sem);
exit(0);
}
