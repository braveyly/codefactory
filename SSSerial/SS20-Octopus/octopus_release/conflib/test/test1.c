#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "conflib.h"
#include "configapi.h"
#define MSEC_FLAG  1000
//#define GLOBAL_MUTEX
pthread_mutex_t globalmysqlmutex;
void *write_para ( void* arg )
{
   
    int ret = 0;
    int app_index = 1;
    int para_index = 2;
    char *value = "para_value_new";
	 if ( ( ret = config_api_init_library() ) < 0 )
        {
            printf ( "Error, config_api_init_library failed. \n" );
			printf("=============================================================================\n");
		//	sleep(2);
            return NULL;
        }

    while ( 1 )
    {
         #ifdef GLOBAL_MUTEX
         pthread_mutex_lock(&globalmysqlmutex);
         #endif

 /*       if ( ( ret = config_api_init_library() ) < 0 )
        {
            printf ( "Error, config_api_init_library failed. \n" );
            #ifdef GLOBAL_MUTEX
            pthread_mutex_unlock(&globalmysqlmutex);
            #endif
			printf("=============================================================================\n");
			sleep(2);
            return NULL;
        }
*/
        ret = config_api_write_parameter ( app_index, para_index, value );
        if ( ret == -1 )
        {
            printf ( "Error, config_api_write_parameter return -1.\n" );
            config_api_uninit_library ();
            #ifdef GLOBAL_MUTEX
            pthread_mutex_unlock(&globalmysqlmutex);
            #endif
			printf("==============================================================================\n");
			sleep(2);
            return NULL;
        }
        printf ( "write_para ---> 0\n" );
        config_api_write_to_flash ( );
//        config_api_uninit_library ();
        #ifdef GLOBAL_MUTEX
        pthread_mutex_unlock(&globalmysqlmutex);
        #endif
        usleep ( 100 * MSEC_FLAG );

    }
	config_api_uninit_library ();
}

void *write_para1 ( void* arg )
{
    
    int ret = 0;
    int app_index = 2;
    int para_index = 2;
    char *value = "para_value_new";

 if ( ( ret = config_api_init_library() ) < 0 )
        {
            printf ( "Error, config_api_init_library failed. \n" );
           
			printf("=================================================================\n");
		//	sleep(2);
            return NULL;
        }
 
    while ( 1 )
    {
        #ifdef GLOBAL_MUTEX
        pthread_mutex_lock(&globalmysqlmutex);
        #endif
 /*       if ( ( ret = config_api_init_library() ) < 0 )
        {
            printf ( "Error, config_api_init_library failed. \n" );
            #ifdef GLOBAL_MUTEX
            pthread_mutex_unlock(&globalmysqlmutex);
            #endif
			printf("=================================================================\n");
			sleep(2);
            return NULL;
        }
*/
        ret = config_api_write_parameter ( app_index, para_index, value );
        if ( ret == -1 )
        {
            printf ( "Error, config_api_write_parameter return -1.\n" );
            config_api_uninit_library ();
            #ifdef GLOBAL_MUTEX
            pthread_mutex_unlock(&globalmysqlmutex);
            #endif
			printf("========================================================================\n");
			sleep(2);
            return NULL;
        }
        printf ( "write_para ---> 1\n" );
        config_api_write_to_flash ( );
  //      config_api_uninit_library ();
        #ifdef GLOBAL_MUTEX
        pthread_mutex_unlock(&globalmysqlmutex);
        #endif
        usleep ( 80 * MSEC_FLAG );
    }
	 config_api_uninit_library ();
}

void *write_table_para ( void* arg )
{
    
    int ret = 0;
    int app_index = 3;
    int row_index = 6;
    int para_index = 1;

    char *value = "table_value_new";

   if ( ( ret = config_api_init_library() ) < 0 )
        {
            printf ( "Error, config_api_init_library failed. \n" );
            
			printf("============================================================================\n");
		//	sleep(2);
            return NULL;
        }
    while ( 1 )
    {
        #ifdef GLOBAL_MUTEX
        pthread_mutex_lock(&globalmysqlmutex);
        #endif
 /*       if ( ( ret = config_api_init_library() ) < 0 )
        {
            printf ( "Error, config_api_init_library failed. \n" );
            #ifdef GLOBAL_MUTEX
            pthread_mutex_unlock(&globalmysqlmutex);
            #endif
			printf("============================================================================\n");
			sleep(2);
            return NULL;
        } 
*/
        ret = config_api_write_table_parameter ( app_index, row_index, para_index, value );
        if ( ret == -1 )
        {
            printf ( "Error, config_api_write_parameter return -1.\n" );
            config_api_uninit_library ();
            #ifdef GLOBAL_MUTEX
            pthread_mutex_unlock(&globalmysqlmutex);
            #endif
			printf("===============================================================================\n");
			sleep(2);
            return NULL;
        }
        printf ( "write_table_para ---> 2\n" );

        config_api_write_to_flash ( );
  //      config_api_uninit_library ();
        #ifdef GLOBAL_MUTEX
        pthread_mutex_unlock(&globalmysqlmutex);
        #endif
        usleep ( 90 * MSEC_FLAG );
    }
 config_api_uninit_library ();
}



int main ( int argc, char *argv[] )
{
    pthread_t   tid1, tid2, tid3;
    struct sched_param sched;
    pthread_attr_t  attr;
//system("./test release");

int a=1;
int b=2;
int c=3;
int t=0;

t=GET_ROW_ID(1,2);
printf("a<<16=%d\t2<<8=%d\n",a<<16,b<<8);
printf("a=%d\tb=%d\t\n");
printf("t1==%d\n",t);
t=GET_PARA_ID(1,2,3);
printf("t2==%d\n",t);
    memset ( &sched, 0, sizeof ( sched ) );
pthread_mutex_init(&globalmysqlmutex,NULL);
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
   if ( ( pthread_create ( &tid1, &attr, write_para, 0 ) != 0 ) )
    {
        printf ( "Error, pthread_create \n" );
        return -1;
    }

#ifdef MUL_THREADS
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
   if ( ( pthread_create ( &tid3, &attr, write_para1, 0 ) != 0 ) )
    {
        printf ( "Error, pthread_create \n" );
        return -1;
    }

    memset ( &sched, 0, sizeof ( sched ) );

    /* create the acl_RTCPrxJob thread */
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

    sched.sched_priority = 70;
    if ( ( pthread_attr_setschedparam ( &attr, &sched ) != 0 ) )
    {
        printf ( "Error, pthread_attr_setschedparam \n" );
        return -1;
    }

    if ( ( pthread_create ( &tid2, &attr, write_table_para, 0 ) != 0 ) )
    {
        printf ( "Error, pthread_create \n" );
        return -1;
    	}
    	
#endif
    while ( 1 )
    {
        sleep ( 5 );
    }
    return 0;
}

