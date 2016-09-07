#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int footprint = 0;
char* storage;

void freerc(void* s)
{
    free(s);
    puts("the free called!");
}

static void checkResults(char* string, int rc)
{
    if(rc)
    {
        printf("Error on : %s, rc = %d", string, rc);
        exit(EXIT_FAILURE);
    }
    return;
}

void *thread(void* arg)
{
    puts("sub thread started");
    int rc = 0, oldState = 0;
    rc = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldState);
    checkResults("pthread_setcancelstate()\n", rc);
    storage = (char*)malloc(80*sizeof(char));
    if(NULL == storage)
    {
        perror("malloc() failed!\n");
        exit(6);
    }
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);
    checkResults("pthread_setcancelstate(2)\n", rc);
    //Plan to release storage, even if thread doesnot exit normally
    pthread_cleanup_push(freerc, storage);
    footprint++;
    while(1)
    {
        puts("before testcancel");
        pthread_testcancel();
        puts("after testcancel");
        sleep(1);
    }
    puts("after testcancel2");
    pthread_cleanup_pop(1);
    puts("after testcancel3");
}

int main()
{
    pthread_t tid;
    void* status = NULL;

    if(pthread_create(&tid, NULL, thread, NULL) != 0)
    {
        perror("Failed to pthread_create!");
        exit(1);
    }

    while(0 == footprint)
        sleep(1);
    
    puts("before pthread cancel");
    if(pthread_cancel(tid) != 0)
    {
        perror("pthread_cancel error!");
        sleep(2);
        exit(3);
    }
    puts("after pthread cancel");

    if(pthread_join(tid, &status) != 0)
    {
        if(PTHREAD_CANCELED != status)
        {
            perror("pthread_join() error!");
            exit(4);
        }
    }

    if(PTHREAD_CANCELED == status)
    {
        puts("PTHREAD CANCELED!");
    }
    puts("main exit");
}
