/**********************************************************************
 Copyright (c) 2009 Wuhan Tecom Co., Ltd.
 All Rights Reserved
 No portions of this material may be reproduced in any form without the
 written permission of:
   Wuhan Tecom Co., Ltd.
   #18, Huaguang Road
   Wuhan, PR China 430074
 All information contained in this document is Wuhan Tecom Co., Ltd.
 company private, proprietary, and trade secret.
***********************************************************************
 Filename:    configapi.c
 Author:      Mark Yan
 Date:        2009-03-13
 Description:

 Histroy:
    1.  Modification: modify the row part, rewrite read, write, init funcations
                      add check_library, remove some funcations,
                      remove the ftok to get key instead of using the fixed key
        Author:   Ken
        Date:     2009-08-19

    2.
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h>

#include "configapi.h"
#include <tcm.h>
#include <tcm_eid.h>
#include <tcm_util.h>

/* If this marco is defined, the table group parameter is saved id sensitivity
 * otherwise the conflib will assign the table id automatically
 */
#define CONFLIB_TABLE_ID_SENSITIVITY

/* TODO: Add sync mechanism for cache_addr */
static void *cache_addr = NULL;

FILE *log_file = NULL;

/* Record the write app id */
char gRecordWriteID[ID_END - 1];

/* Record the max app number */
int gMaxAppNum = 0;

/* Record the max table number */
int gMaxTabNum = 0;

/* Record the max parameter number */
int gMaxParaNum = 0;

/* mmap size */
int gMMAPSIZE = sizeof(int) * 2;

/* glibc developer ask me to do this. detail please see bits/sem.h */
#if defined(_SEM_SEMUN_UNDEFINED)
union semun
{
    int                val;
    struct             semid_ds *buf;
    unsigned short int *array;
    struct             seminfo *__buf;
};
#endif

/********************** Functions ************************************/

int update_config_file ( void );


/***********************************************************************
Function: APP_START_NODE 
Description: return the pointer of node which is app beginner 
 
Return: app_node_t*, 
Input: void* addr, 
Output: 
Others: 
 
Histroy: 
    1.  Modification: Created. 
        Author: Ken 
        Date: 2010-09-06 
  
    2.  Modification: 
        Author: 
        Date: 
************************************************************************/
app_node_t* APP_START_NODE(void* addr)
{
    return (app_node_t *)(addr + sizeof(int));
}


/***********************************************************************
Function: ROW_START_NODE 
Description: return the pointer of node which is row beginner 
 
Return: row_node_t*, 
Input: void* addr, 
Output: 
Others: 
 
Histroy: 
    1.  Modification: Created. 
        Author: Ken 
        Date: 2010-09-06 
  
    2.  Modification: 
        Author: 
        Date: 
************************************************************************/
row_node_t* ROW_START_NODE(void* addr)
{
    return (row_node_t *)((void *)APP_START_NODE(addr) + sizeof(app_node_t) * gMaxAppNum);
}


/***********************************************************************
Function: PARA_START_NODE 
Description: return the pointer of node which is para beginner 
 
Return: para_node_t*, 
Input: void* addr, 
Output: 
Others: 
 
Histroy: 
    1.  Modification: Created. 
        Author: Ken 
        Date: 2010-09-06 
  
    2.  Modification: 
        Author: 
        Date: 
************************************************************************/
para_node_t* PARA_START_NODE(void* addr)
{
    return (para_node_t *)((void *)ROW_START_NODE(addr) + sizeof(row_node_t) * gMaxTabNum);
}


/***********************************************************************
Function: open_logger
Description:  open log file or open stdout

Return: void,
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
void open_logger ( void )
{
#ifdef  LOG_TO_FILE
    log_file = fopen ( LOG_FILE, "w" );
    if ( !log_file )
    {
        log_file = stdout;
    }
#else
    log_file = stdout;
#endif
}


/***********************************************************************
Function: close_logger
Description: close the log file

Return: void,
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
void close_logger ( void )
{
#ifdef  LOG_TO_FILE
    if ( log_file && log_file != stdout )
    {
        fflush ( log_file );
        /* BUG: Something has been written to log_file after this. By Peter
        fclose(log_file);
        */
    }
#endif
}


/***********************************************************************
Function: init_cache_lock
Description:  init the cache lock

Return: int, if init lock successfully, return 0, otherwise return -1
Input: void,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: remove the ftok, use the fix key KEY_LOCK_CACHE_FILE
        Author: Ken
        Date:   2009-08-19
************************************************************************/
int init_cache_lock ( void )
{
    int     sem;
    union   semun  un;

    sem = semget ( KEY_LOCK_CACHE_FILE, 1, IPC_CREAT | 0666 | IPC_EXCL);
    if ( sem == -1 )
    {
        CFG_DEBUG ( log_file, "semget:%s\n", strerror ( errno ) ) ;
        if( semget( KEY_LOCK_CACHE_FILE,1,0666) != -1)
        {
            CFG_DEBUG ( log_file, "semget:%s\n", strerror ( errno ) );
            return 0;
        }
        else
        {
            CFG_DEBUG ( log_file, "semget:%s\n", strerror ( errno )  );
            return -1;
        }
    }
 /*
    sem = semget ( KEY_LOCK_CACHE_FILE, 1, IPC_CREAT | 0666 );
    if ( sem == -1 )
    {
        CFG_DEBUG ( log_file, "semget:%s\n", strerror ( errno ) );
        return -1;
    }
*/
    un.val = 1;
    if ( semctl ( sem, 0, SETVAL, un ) < 0 )
    {
        CFG_DEBUG ( log_file, "semctl:%s\n", strerror ( errno ) );
        return -1;
    }

    return 0;
}


/***********************************************************************
Function: get_sem_id
Description: get the share memory lock

Return: int, if get the lock successfully, return the key, otherwise return -1
Input: void,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: remove the ftok, use the fix key KEY_LOCK_CACHE_FILE
        Author: Ken
        Date:   2009-08-19
************************************************************************/
int get_sem_id ( void )
{
    int     sem;

    sem = semget ( KEY_LOCK_CACHE_FILE, 1, 0666 );
    if ( sem == -1 )
    {
        CFG_DEBUG ( log_file, "semget:%s\n", strerror ( errno ) );
        return -1;
    }

    return sem;
}


/***********************************************************************
Function: lock_cache
Description: lock the cache lock

Return: int, if lock successfully, return 0, otherwise return -1
Input: int sem ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
int lock_cache ( int sem )
{
    struct  sembuf ops[1];

    ops[0].sem_num = 0;
    ops[0].sem_op = -1;
    ops[0].sem_flg = 0;

    if ( semop ( sem, ops, 1 ) < 0 )
    {
        CFG_DEBUG ( log_file, "sem_op:%s\n", strerror ( errno ) );
        return -1;
    }

    return 0;
}


/***********************************************************************
Function: unlock_cache
Description: unlock the cache lock

Return: int, if unlock successfully, return 0, otherwise return -1
Input: int sem ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
int unlock_cache ( int sem )
{
    struct  sembuf ops[1];

    ops[0].sem_num = 0;
    ops[0].sem_op = 1;
    ops[0].sem_flg = 0;

    if ( semop ( sem, ops, 1 ) < 0 )
    {
        CFG_DEBUG ( log_file, "sem_op:%s\n", strerror ( errno ) );
        return -1;
    }

    return 0;
}


/***********************************************************************
Function: find_free_node
Description: find the free row or parameter node

Return: void* , if find the node, return the new node pointer, otherwise return NULL
Input: void *addr,
       NODE_TYPE type ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
void* find_free_node ( void *addr, NODE_TYPE type )
{
    row_node_t   *row;
    para_node_t  *para;
    int    i;

    if ( !addr )
        return NULL;

    switch ( type )
    {
        case APP_NODE_TYPE:
            /* if need  */
            break;

        case ROW_NODE_TYPE:
            row = ROW_START_NODE ( addr );
            for ( i = 0; i < gMaxTabNum; i++, row++ )
            {
                if ( row->row_id == -1 )
                {
                    row->next = 0;
                    row->para = 0;
                    return ( void * ) row;
                }
            }
            break;

        case PARA_NODE_TYPE:
            para = PARA_START_NODE ( addr );
            for ( i = 0; i < gMaxParaNum; i++, para++ )
            {
                if ( para->para_id == -1 )
                {
                    memset ( para->para_name, 0, MAX_NAME_LEN );
                    memset ( para->para_val, 0, MAX_VALUE_LEN );
                    para->next = 0;
                    return ( void * ) para;
                }

            }
        default:
            break;

    }

    CFG_DEBUG ( log_file, "No free node in the share memory\n" );
    return NULL;

}



/***********************************************************************
Function: config_api_get_app_id_by_name
Description: get app id by app name

Return: int, if get the app id, return it, otherwise return -1
Input: const char *app_name ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
extern tcmEntityInfo entityInfoArray[];
int config_api_get_app_id_by_name ( const char *app_name )
{
    tcmEntityInfo *tmp;

    /* search all the apps */
    for ( tmp = &(entityInfoArray[0]); tmp && tmp->eid < ID_END; tmp++ )
    {
        if ( tcmUtl_strcmp ( tmp->name, app_name ) == 0 )
        {
            return tmp->eid;
        }
    }

    CFG_DEBUG ( log_file, "Can not find APP ID by name.\n" );
    return -1;
}

/***********************************************************************
Function: config_api_get_para_name_by_id
Description:  get the table parameter name by id

Return: int, if get successfully, return 0, otherwise return -1
Input: int app_index,
       int para_index,
       char* name, if get name, the result will be stored in it
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
int config_api_get_para_name_by_id ( int app_index, int para_index, char* name )
{
    tcmEntityInfo *tmp_app;
    tcmParamInfo *tmp_para = NULL;

    for ( tmp_app = &entityInfoArray; tmp_app && tmp_app->eid < ID_END; tmp_app++ )
    {
        if ( tmp_app->eid == app_index )
        {
            if ( tmp_app->row_paras != NULL )
            {
                for ( tmp_para = tmp_app->row_paras; tmp_para && tmp_para->name != NULL; tmp_para++ )
                {
                    if ( tmp_para->id == para_index )
                    {
                        strncpy ( name, tmp_para->name, MAX_NAME_LEN - 1 );
                        return 0;
                    }
                }

                CFG_DEBUG ( log_file, "Find the app id but can not find the para name.\n" );
                return -1;
            }

            CFG_DEBUG ( log_file, "This APP does not have the row parameter.\n" );
            return -1;
        }
    }

    CFG_DEBUG ( log_file, "Can't find the app id.\n" );
    return -1;
}




/***********************************************************************
Function: config_api_app_exist_table
Description: check whether the app has the table

Return: int, if has table return 0, otherwise return -1
Input: int app_index,
       int* max_num, if has table, the table max num will be stored in it
       int* end_num, if has table, the table end num will be stored in it
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-19

    2.  Modification: add max_num to get the max table number
        Author: Ken
        Date: 2009-08-20
************************************************************************/
int config_api_app_exist_table ( int app_index, int* max_num, int* end_num )
{
    tcmEntityInfo *tmp_app;

    for ( tmp_app = &entityInfoArray; tmp_app && tmp_app->eid < ID_END; tmp_app++ )
    {
        if ( tmp_app->eid == app_index )
        {
            if ( tmp_app->row_paras != NULL )
            {
                *max_num = tmp_app->row_maxnum;
                *end_num = tmp_app->row_end;
                return 0;
            }
            else
            {
                CFG_DEBUG ( log_file, "The app id %d does not have the table parameter.\n", app_index );
                return -1;
            }
        }
    }

    CFG_DEBUG ( log_file, "Can't find the app id.\n" );
    return -1;
}


/***********************************************************************
Function: add_para_tail
Description:  add new parameter to the tail of the parameter part

Return: para_node_t , if add successfully, return the new parameter pointer,
                      otherwise return NULL
Input: int *next,
       int id,
       char *val ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
para_node_t *add_para_tail ( int *next, int id, char *val )
{
    para_node_t *new_para = NULL;
    int         app_index = id >> 16;
    int         para_index = id & 0x000000ff;
    char        tmp_name[MAX_NAME_LEN];

    new_para = ( para_node_t * ) find_free_node ( cache_addr, PARA_NODE_TYPE );
    if ( !new_para )
    {
        return NULL;
    }

    /* It is only used when adding the row, so we need to get the row parameter name */
    if ( !config_api_get_para_name_by_id ( app_index, para_index, tmp_name ) )
    {
        strncpy ( new_para->para_name, tmp_name, MAX_NAME_LEN - 1 );
    }
    else
    {
        CFG_DEBUG ( log_file, "Can not find the name, app_index: %d, para_index: %d\n", app_index, para_index );
        return NULL;
    }

    strncpy ( new_para->para_val, val, MAX_VALUE_LEN - 1 );

    new_para->para_id = id;
    *next = OFFSET_BASE ( new_para, cache_addr );

    new_para->next = 0;

    CFG_DEBUG ( log_file, "Add new parameter: %s\n", val );

    return new_para;

}




/***********************************************************************
Function: add_row_tail
Description: add row node to the row tail

Return: row_node_t ,if add successfully, return the new row pointer,
                     otherwise return NULL
Input: int *next,
       int row_id ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
row_node_t *add_row_tail ( int *next, int row_id )
{
    row_node_t   *new_row;

    new_row = ( row_node_t * ) find_free_node ( cache_addr, ROW_NODE_TYPE );

    if ( !new_row )
    {
        return NULL;
    }

    new_row->row_id = row_id;
    new_row->para = 0;
    new_row->next = 0;
    *next = OFFSET_BASE ( new_row, cache_addr );

    return  new_row;
}


/***********************************************************************
Function: build_cache
Description:  Create the share memory.

Return: void ,
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: use KEY_SHARE_MEMORY instead of the ftok
        Author: Ken
        Date: 2009-08-17
************************************************************************/
void *build_cache ( void )
{
    int      shm_id;
    void     *buf = NULL;
    int      *num = NULL;

    /* Creat gMMAPSIZE share memory */
    shm_id = shmget ( KEY_SHARE_MEMORY, gMMAPSIZE, IPC_CREAT | 0666 );
    if ( shm_id == -1 )
    {
        CFG_DEBUG ( log_file, "shmget error.\n" );
        return NULL;
    }

    /* add it to application */
    buf = shmat ( shm_id, NULL, 0 );
    if ( buf == ( void * ) ( -1 ) )
    {
        CFG_DEBUG ( log_file, "shmat error.\n" );
        return NULL;
    }

    /* init buf */
    memset ( buf, 0, gMMAPSIZE );

    /* put magic number for validation */
    num = ( int * ) buf;
    *num = MAGIC_NUM_1;

    num = ( int * ) GET_MAGIC_2 ( buf, gMMAPSIZE );
    *num = MAGIC_NUM_2;

    CFG_DEBUG ( log_file, "Build memory cache successfully\n" );
    return buf;
}


/***********************************************************************
Function: open_para_file
Description:  Open the file "os.conf".

Return: FILE *,  the fd after open successfully, otherwise return NULL
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
FILE *open_para_file ( void )
{
    FILE   *fp = NULL;

    fp = fopen ( PARAMETER_FILE, "r" );
    if ( !fp )
    {
        CFG_DEBUG ( log_file, "Open parameter file error, please check %s\n", PARAMETER_FILE );

        fp = fopen ( DEFAULT_FILE, "r" );
        if ( !fp )
        {
            CFG_DEBUG ( log_file, "Open default file error, please check %s\n", DEFAULT_FILE );
            return NULL;
        }
    }

    return fp;
}



/***********************************************************************
Function: init_node
Description:  Format the share memory.

Return: int, init all nodes, return 0
Input: void *addr ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: Init row of the app
        Author: Ken
        Date: 2009-08-17
************************************************************************/
int init_node ( void *addr )
{
    int         i;
    app_node_t  *app, *app_next;
    para_node_t *para;
    row_node_t *row;

    /* initial the app node */
    app = APP_START_NODE ( addr );
    for ( i = 0; i < gMaxAppNum; i++, app++ )
    {
        app->app_id = -1;
        app->app_name[0] = '\0';
        app->para = 0;
        app->row = 0;
        if ( i == gMaxAppNum - 1 )
        {
            app->next = 0;
        }
        else
        {
            app_next = app;
            app_next++;
            app->next = OFFSET_BASE ( app_next, addr );
        }
    }

    /* initial the row/table node */
    row = ROW_START_NODE ( addr );
    for ( i = 0; i < gMaxTabNum; i++, row++ )
    {
        row->row_id = -1;
        row->next = 0;
        row->para = 0;
    }

    /* initial the para node */
    para = PARA_START_NODE ( addr );
    for ( i = 0; i < gMaxParaNum; i++, para++ )
    {
        para->para_id = -1;
        para->para_name[0] = '\0';
        para->para_val[0] = '\0';
        para->next = 0;
    }

    CFG_DEBUG ( log_file, "Init all the parmeter node successfully\n" );
    return 0;
}



/***********************************************************************
Function: build_para_table
Description:  According the "os.conf" to initialize the share memory.

Return: int, if build successfully, return 0, otherwise return -1
Input: FILE *fp,
       void *addr ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: format it and use GET_ROW_ID and GET_PARA_ID
        Author: Ken
        Date: 2009-08-18
************************************************************************/
int build_para_table ( FILE *fp, void *addr )
{
    char         *str_start, *str_end;
    char         *var, *val, *mark;
    int          index, len, parameter = 1, row_num = 0;
    app_node_t   *app, *pre_app;
    para_node_t  *para, *pre_para = NULL;
    row_node_t   *row, *pre_row = NULL;
    char         configline[MAX_CONF_LINE+1];

    app = APP_START_NODE ( addr );
    row = ROW_START_NODE ( addr );
    para = PARA_START_NODE ( addr );

    while ( fgets ( configline, MAX_CONF_LINE, fp ) )
    {
        if ( configline[0] == '[' )
        {
            str_start = configline;
            if ( ( str_end = strchr ( configline, ']' ) ) == NULL )
            {
                CFG_DEBUG ( log_file, "[] not matched, line: %s\n", configline );
                /* replace with continue */
                fclose ( fp );
                return -1;
            }

            /* strip the space/tab charactor here? */
            str_start++;
            *str_end = '\0';
            strncpy ( app->app_name, str_start, MAX_NAME_LEN - 1 );

            index =  config_api_get_app_id_by_name ( app->app_name );
            if ( index != -1 ) /*if fail to get the ID*/
            {
                app->app_id = index;
            }
            else
            {
                CFG_DEBUG ( log_file, "Can not get application id\n" );
                fclose ( fp );
                return -1;
            }

            CFG_DEBUG ( log_file, "[%s] id:%d, offset:%d\n", app->app_name, app->app_id, OFFSET_BASE ( app, addr ) );

            pre_app = app++;    /* record the pre app, and put the pointer app to the next one */
            row_num = 0;        /* clear the row number */
            parameter = 1;      /* each app's parameter id start with 1 */
            pre_para = NULL;    /* clear the pre parameter */
            pre_row = NULL;     /* clear the pre row */
        }
        else if ( ( mark = strchr ( configline, '<' ) ) && !strchr ( configline, '=' ) )
        {
            str_start = mark + 1;
            if ( ( str_end = strchr ( str_start, '>' ) ) == NULL )
            {
                CFG_DEBUG ( log_file, "<> must be pair, line: %s\n", configline );
                fclose ( fp );
                return -1;
            }
            *str_end = '\0';

#ifdef CONFLIB_TABLE_ID_SENSITIVITY
            /* the number in <> is our row id */
            row_num = atoi ( str_start );
            if ( row_num <= 0 )
            {
                CFG_DEBUG ( log_file, "The string %s in <> is not a number or too big\n", str_start );
                fclose ( fp );
                return -1;
            }

            row->row_id = GET_ROW_ID ( pre_app->app_id, row_num );

#else
            row_num++;              /* row number +1 */
            row->row_id = GET_ROW_ID ( pre_app->app_id, row_num );

#endif

            if ( pre_row )
            {
                pre_row->next = OFFSET_BASE ( row, addr );
            }
            else
            {
                pre_app->row = OFFSET_BASE ( row, addr );
            }

            CFG_DEBUG ( log_file, "  <%d> offset:%d\n", row_num, OFFSET_BASE ( row, addr ) );

            if ( pre_para )
            {
                pre_para->next = 0; /* clear the next of the pre parameter */
            }
            pre_para = NULL;        /* clear the pre parameter */
            pre_row = row++;        /* record the pre row and put the row to the next one*/
            parameter = 1;          /* clear the parameter number */
        }
        else
        {
            if ( ( mark = strchr ( configline, '=' ) ) == NULL )
            {
                CFG_DEBUG ( log_file, "Can not find '=' in parameter line: %s\n", configline );
                fclose ( fp );
                return -1;
            }

            var = configline;
            val = mark + 1;
            *mark = '\0';
            len = strlen ( val );
            if ( val[len-1] == '\n' )
                val[len-1] = '\0';

            para->para_id = GET_PARA_ID ( pre_app->app_id, row_num, parameter );

            strncpy ( para->para_name, var, MAX_NAME_LEN - 1 );
            strncpy ( para->para_val, val, MAX_VALUE_LEN - 1 );

            if ( pre_para )
                pre_para->next = OFFSET_BASE ( para, addr );
            else if ( pre_row )
                pre_row->para = OFFSET_BASE ( para, addr );
            else
                pre_app->para = OFFSET_BASE ( para, addr );

            CFG_DEBUG ( log_file, "    %s=%s offset:%d \n", para->para_name, para->para_val, OFFSET_BASE ( para, addr ) );
            pre_para = para++;   /* record the pre parameter and put the para to the next one */
            parameter++;         /* parameter number +1 */
        }
    }

    fclose ( fp );
    return 0;

}



/***********************************************************************
Function: init_library
Description:  When "config_api_init_library()" is called for the first
     time, using this function to initialization.
     Alloc the share memory, and read the os.conf into memory

Return: int, if init library successfully, return 0, otherwise return -1
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: check os.conf first, then build cache
        Author: Ken
        Date: 2009-08-17
************************************************************************/
int init_library ( void )
{
    int      ret;
    void     *addr;
    FILE     *fp;

    //check the os.conf first, if exit, build cache
    if ( ( fp = open_para_file() ) == NULL )
    {
        CFG_DEBUG ( log_file, "Can not open any parameter file\n" );
        return -1;
    }

    addr = build_cache();
    if ( !addr )
    {
        CFG_DEBUG ( log_file, "build cache error\n" );
        return -1;
    }
    init_node ( addr );

    ret =  build_para_table ( fp, addr );
    if ( ret < 0 )
    {
        /* TODO: use default.conf again */
        CFG_DEBUG ( log_file, "build parameter table error\n" );
        return -1;
    }

    cache_addr = addr;
    CFG_DEBUG ( log_file, "Init parameter table successfully\n" );

    return 0;

}




/***********************************************************************
Function: check_library
Description:  check whether the library is safe

Return: int, 0 means safe, otherwise destoried
Input: void* baseaddr,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
int check_library ( void* baseaddr )
{
    if ( !baseaddr )
    {
        return -1;
    }

    int  *magic1, *magic2;

    magic1 = ( int * ) ( baseaddr );
    magic2 = GET_MAGIC_2 ( baseaddr, gMMAPSIZE );
    if ( *magic1 != MAGIC_NUM_1 || *magic2 != MAGIC_NUM_2 )
    {
        CFG_DEBUG ( log_file, "Magic number error, cache destory\n" );
        return -1;
    }

    return 0;
}


/***********************************************************************
Function: init_Param 
Description: init the parameter for library
 
Return: void, 
Input: void, 
Output: 
Others: 
 
Histroy: 
    1.  Modification: Created. 
        Author: Ken 
        Date: 2010-09-06 
  
    2.  Modification: 
        Author: 
        Date: 
************************************************************************/
void init_Param()
{
    int i = 0;
    int iTabMaxNum, iTabParaEnd;
    tcmEntityInfo *tmp_app;
    
    gMaxAppNum = ID_END - 1;
    gMaxTabNum = 0;
    gMaxParaNum = 0;

    for ( tmp_app = &entityInfoArray; tmp_app && tmp_app->eid < ID_END; tmp_app++ )
    {
        /* The parameter number that this application has */
        if ( tmp_app->app_maxnum > 1 )
        {
            gMaxParaNum += tmp_app->app_maxnum - 1;
        }

        /* The table and parameter number that this application has */
        if ( tmp_app->row_paras != NULL )
        {
            gMaxTabNum += tmp_app->row_maxnum;
            if ( tmp_app->row_end > 1 )
            {
                gMaxParaNum += (tmp_app->row_end - 1)*(tmp_app->row_maxnum);
            }
        }
    }

    gMMAPSIZE = gMaxAppNum*sizeof(app_node_t) + gMaxTabNum*sizeof(row_node_t) + gMaxParaNum*sizeof(para_node_t) + 2*sizeof(int);

//    CFG_DEBUG ( log_file, "app size %d, table size %d, para size %d\n", sizeof(app_node_t), sizeof(row_node_t), sizeof(para_node_t));
    CFG_DEBUG ( log_file, "Max app number %d, max table number %d, max parameter number %d, mmap size %d\n", 
       gMaxAppNum, gMaxTabNum, gMaxParaNum, gMMAPSIZE );

    /* Init the recorded writen ID */
    memset(gRecordWriteID, 0, sizeof(gRecordWriteID));
}

/***********************************************************************
Function: config_api_read_parameter
Description:  Read the value of parameter

Return: int, if read successfully, return 0, otherwise return -1
Input: int app_index,
       int para_index,
       char *result ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: add check_library
        Author: Ken
        Date: 2009-08-18
************************************************************************/
int config_api_read_parameter ( int app_index, int para_index, char *result )
{
    app_node_t    *tmp_app;
    para_node_t   *tmp_para;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }

    if ( app_index >= ID_END || app_index <= 0 || !result )
    {
        CFG_DEBUG ( log_file, "APP index error\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    /* TODO: need lock it when read */
    for ( ; tmp_app && tmp_app->app_id != -1; tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        if ( tmp_app->app_id == app_index )
        {
            for ( tmp_para = NEXT ( para_node_t, tmp_app->para, cache_addr );
                    tmp_para && tmp_para->para_id != -1;
                    tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
            {
                if ( tmp_para->para_id == GET_PARA_ID ( app_index, 0, para_index ) )
                {
                    strncpy ( result, tmp_para->para_val, ((strlen(tmp_para->para_val)+1) < (MAX_VALUE_LEN - 1))?(strlen(tmp_para->para_val) + 1):(MAX_VALUE_LEN - 1) );
                    CFG_DEBUG ( log_file, "Read %s=%s success, result %s\n", tmp_para->para_name, tmp_para->para_val, result );
                    return 0;
                }
            }

            CFG_DEBUG ( log_file, "[READ] Find the app but can not find the parameter\n" );
            return -1;
        }
    }

    CFG_DEBUG ( log_file, "[READ] Can not find the app index\n" );
    return -1;
}


/***********************************************************************
Function: config_api_write_parameter
Description:  Write the value of parameter

Return: int, if write successfully, return 0, otherwise return -1
Input: int app_index,
       int para_index,
       char *val ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: add check library
        Author: Ken
        Date:   2009-08-19
************************************************************************/
int config_api_write_parameter ( int app_index, int para_index, char *val )
{
    app_node_t   *tmp_app;
    para_node_t  *tmp_para;
    int          sem;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }

    if ( app_index >= ID_END || app_index <= 0 || !val )
    {
        CFG_DEBUG ( log_file, "APP index error\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    if ( ( sem = get_sem_id() ) < 0 )
    {
        CFG_DEBUG ( log_file, "Get sem id error\n" );
        return -1;
    }

    if ( lock_cache ( sem ) < 0 )
    {
        CFG_DEBUG ( log_file, "lock cache error\n" );
        return -1;
    }


    for ( ; tmp_app && tmp_app->app_id != -1; tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        if ( tmp_app->app_id == app_index )
        {
            for ( tmp_para = NEXT ( para_node_t, tmp_app->para, cache_addr );
                    tmp_para && tmp_para->para_id != -1;
                    tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
            {
                if ( tmp_para->para_id == GET_PARA_ID ( app_index, 0, para_index ) )
                {
                    strncpy ( tmp_para->para_val, val, MAX_VALUE_LEN - 1 );
                    /* Record the write ID */
                    gRecordWriteID[app_index - 1] = '1';
                    unlock_cache ( sem );
                    return 0;
                }
            }

            CFG_DEBUG ( log_file, "[Write] Find the app but can not find the parameter\n" );
            unlock_cache ( sem );
            return -1;   /* find the app but can not find the parameter */
        }
    }

    CFG_DEBUG ( log_file, "[Write] Can not find the app index\n" );

    unlock_cache ( sem );
    return -1;
}

/***********************************************************************
Function: config_api_read_table_parameter
Description:  Read the value of table parameter

Return: int, if read successfully, return 0, otherwise return -1
Input: int app_index,
       int row_index,
       int para_index,
       char *result ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: format it and add debug information
        Author: Ken
        Date: 2009-08-18
************************************************************************/
int config_api_read_table_parameter ( int app_index, int row_index, int para_index, char *result )
{
    app_node_t   *tmp_app;
    para_node_t  *tmp_para;
    row_node_t   *tmp_row;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }

    if ( app_index >= ID_END || app_index <= 0 || !result )
    {
        CFG_DEBUG ( log_file, "APP index error\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    for ( ; tmp_app && tmp_app->app_id != -1; tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        if ( tmp_app->app_id == app_index )
        {
            for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                    tmp_row && tmp_row->row_id != -1;
                    tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
            {
                if ( tmp_row->row_id  == GET_ROW_ID ( app_index, row_index ) )
                {
                    for ( tmp_para = NEXT ( para_node_t, tmp_row->para, cache_addr );
                            tmp_para && tmp_para->para_id != -1;
                            tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
                    {
                        if ( tmp_para->para_id == GET_PARA_ID ( app_index, row_index, para_index ) )
                        {
                            strncpy ( result, tmp_para->para_val, ((strlen(tmp_para->para_val) + 1) < (MAX_VALUE_LEN - 1))?(strlen(tmp_para->para_val) + 1):(MAX_VALUE_LEN - 1) );
                            CFG_DEBUG ( log_file, "Read %s=%s success, result %s\n", tmp_para->para_name, tmp_para->para_val, result );
                            return 0;
                        }
                    }

                    CFG_DEBUG ( log_file, "[READ TABLE PARA] Find the app and row but can not find the paremeter\n" );
                    return -1;    /* Find the app and row but can not find the paremeter */
                }
            }

            CFG_DEBUG ( log_file, "[READ TABLE PARA] Find the app but can not find the row\n" );
            return -1;    /* Find the app but can not find the row */
        }
    }

    CFG_DEBUG ( log_file, "[READ TABLE PARA] Can not find the app index\n" );
    return -1;

}



/***********************************************************************
Function: config_api_write_table_parameter
Description:  Write the value of table parameter

Return: int, if write successfully, return 0, otherwise return -1
Input: int app_index,
       int row_index,
       int para_index,
       char *val ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: remove write the dirty flag, does not write the parameter if it does not exist
        Author: Ken
        Date: 2009-08-17
************************************************************************/
int config_api_write_table_parameter ( int app_index, int row_index, int para_index, char *val )
{
    int    sem;
    app_node_t   *tmp_app;
    para_node_t  *tmp_para;
    row_node_t   *tmp_row;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }

    if ( app_index >= ID_END || app_index <= 0 || !val )
    {
        CFG_DEBUG ( log_file, "APP index error\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    if ( ( sem = get_sem_id() ) < 0 )
    {
        CFG_DEBUG ( log_file, "Get sem id error\n" );
        return -1;
    }

    if ( lock_cache ( sem ) < 0 )
    {
        CFG_DEBUG ( log_file, "lock cache error\n" );
        return -1;
    }

    for ( ; tmp_app && tmp_app->app_id != -1;
            tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        if ( tmp_app->app_id == app_index )
        {
            for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                    tmp_row && tmp_row->row_id != -1;
                    tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
            {
                if ( tmp_row->row_id == GET_ROW_ID ( app_index, row_index ) )
                {
                    for ( tmp_para = NEXT ( para_node_t, tmp_row->para, cache_addr );
                            tmp_para && tmp_para->para_id != -1;
                            tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
                    {
                        if ( tmp_para->para_id == GET_PARA_ID ( app_index, row_index, para_index ) )
                        {
                            strncpy ( tmp_para->para_val, val, MAX_VALUE_LEN - 1 );
                            /* Record the write ID */
                            gRecordWriteID[app_index - 1] = '1';
                            CFG_DEBUG ( log_file, "Write %s=%s success\n",
                                        tmp_para->para_name, tmp_para->para_val );
                            unlock_cache ( sem );
                            return 0;
                        }

                    }

                    CFG_DEBUG ( log_file, "[WRITE TABLE PARA] Find the app and row but can not find the paremeter\n" );
                    unlock_cache ( sem );
                    return -1;    /* Find the app and row but can not find the paremeter */
                }
            }

            CFG_DEBUG ( log_file, "[WRITE TABLE PARA] Find the app but can not find the row\n" );
            unlock_cache ( sem );
            return -1;    /* Find the app but can not find the row */
        }
    }

    CFG_DEBUG ( log_file, "[WRITE TABLE PARA] Can not find the app index\n" );
    unlock_cache ( sem );
    return -1;
}



/***********************************************************************
Function: config_api_read_table_row
Description:  read one group row parameter

Return: int, if read successfully, return 0, otherwise return -1
Input: int app_index,
       int row_index,
       char **val,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
int config_api_read_table_row ( int app_index, int row_index, char **val )
{
    app_node_t   *tmp_app;
    para_node_t  *tmp_para;
    row_node_t   *tmp_row;
    int           ipara_index = 0;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }

    if ( app_index >= ID_END || app_index <= 0 || *val == NULL )
    {
        CFG_DEBUG ( log_file, "APP index error\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    for ( ; tmp_app && tmp_app->app_id != -1;
            tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        if ( tmp_app->app_id == app_index )
        {
            for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                    tmp_row && tmp_row->row_id != -1;
                    tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
            {
                if ( tmp_row->row_id  == GET_ROW_ID ( app_index, row_index ) )
                {
                    for ( ipara_index = 0, tmp_para = NEXT ( para_node_t, tmp_row->para, cache_addr );
                            tmp_para && tmp_para->para_id != -1;
                            ipara_index++, tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
                    {
                        strncpy ( val[ ipara_index ], tmp_para->para_val, ((strlen(tmp_para->para_val) + 1) < (MAX_VALUE_LEN - 1))?(strlen(tmp_para->para_val) + 1):(MAX_VALUE_LEN - 1) );
                        CFG_DEBUG ( log_file, "Read %s=%s success, result %s\n",
                                    tmp_para->para_name, tmp_para->para_val, val[ ipara_index ] );

                    }
                    return 0;
                }
            }

            CFG_DEBUG ( log_file, "[READ TABLE GROUP] Find the app but can not find the row\n" );
            return -1;    /* Find the app but can not find the row */
        }
    }
    CFG_DEBUG ( log_file, "[READ TABLE GROUP] Can not find the app index\n" );
    return -1;
}



/***********************************************************************
Function: config_api_add_table_row
Description:  add one table row to the app

Return: int, if add successfully, return 0, otherwise return -1
Input: int app_index,
       int row_index,
       char **val ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: add the row exist check, and get the end number of the row,
                      make sure do not add more parameters, or add less parameters
        Author: Ken
        Date:   2009-08-19
************************************************************************/
int config_api_add_table_row ( int app_index, int row_index, char **val )
{
    app_node_t   *tmp_app;
    para_node_t  *tmp_para;
    row_node_t   *tmp_row, *last_row = NULL, *pre_row = NULL;
    int          *tmp_int = NULL;
    int           irow_endnum = 0, irow_maxnum = 0;
    char          str_tmp[MAX_VALUE_LEN];

    int     sem, i;
    int     tmp_index_row = 0, tmp_index_para = 0;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }

    if ( app_index >= ID_END || app_index <= 0 || *val == NULL )
    {
        CFG_DEBUG ( log_file, "APP index error\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    if ( ( sem = get_sem_id() ) < 0 )
    {
        CFG_DEBUG ( log_file, "Get sem id error\n" );
        return -1;
    }

    if ( lock_cache ( sem ) < 0 )
    {
        CFG_DEBUG ( log_file, "lock cache error\n" );
        return -1;
    }

    for ( ; tmp_app && tmp_app->app_id != -1;
            tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        if ( tmp_app->app_id == app_index )
        {
            /* check whether this app has the table*/
            if ( config_api_app_exist_table ( app_index, &irow_maxnum, &irow_endnum ) )
            {
                unlock_cache ( sem );
                return -1;
            }

            /* Check the row_index */
            if ( row_index > irow_maxnum || row_index < 0 )
            {
                CFG_DEBUG ( log_file, "Table index error, the max number is %d\n", irow_maxnum );
                unlock_cache ( sem );
                return -1;
            }

            pre_row  = NULL;
            last_row = NULL;

#ifdef CONFLIB_TABLE_ID_SENSITIVITY
            if ( row_index > 0 )
            {
                /* Make sure we can find the position easily, the table id must be stored from small to big*/
                for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                        tmp_row && tmp_row->row_id != -1;
                        tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
                {
                    if ( tmp_row->row_id < GET_ROW_ID ( app_index, row_index ) )
                    {
                        pre_row = tmp_row; /* Find the pre row whose id is smaller than row_index */
                        continue;
                    }
                    else if ( tmp_row->row_id == GET_ROW_ID ( app_index, row_index ) )
                    {
                        CFG_DEBUG ( log_file, "The table already exist, id = %d\n", row_index );
                        unlock_cache ( sem );
                        return -1;
                    }
                    else if ( tmp_row->row_id > GET_ROW_ID ( app_index, row_index ) )
                    {
                        last_row = tmp_row; /* Find the last row whose id is bigger than row_index*/
                        break;
                    }
                }
                tmp_index_row = row_index;
            }
            else // row_index == 0, find the small index to add it
            {
                /* Make sure we can find the position easily, the table id must be stored from small to big*/
                for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr ), i = 1;
                        tmp_row && tmp_row->row_id != -1;
                        tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ), i++ )
                {
                    if ( tmp_row->row_id == GET_ROW_ID ( app_index, i ) )
                    {
                        pre_row = tmp_row;
                        continue;
                    }
                    else
                    {
                        last_row = tmp_row;
                        break;
                    }
                }

                tmp_index_row = i;
            }

            if ( pre_row )
            {
                tmp_int = & ( pre_row->next );
            }
            else
            {
                tmp_int = & ( tmp_app->row );
            }
#else
            for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                    tmp_row && tmp_row->row_id != -1;
                    tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
            {
                /* record the last row of current app */
                last_row = tmp_row;
            }

            if ( last_row )
            {
                tmp_index_row = ( ( last_row->row_id >> 8 ) & 0x000000ff ) + 1;
                tmp_int = & ( last_row->next );
            }
            else
            {
                tmp_index_row = 1;
                tmp_int = & ( tmp_app->row );
            }
#endif

            if ( val[tmp_index_para] != NULL )
            {
                tmp_row = add_row_tail ( tmp_int, GET_ROW_ID ( app_index, tmp_index_row ) );
                if ( !tmp_row )
                {
                    CFG_DEBUG ( log_file, "Add new row and para to row tail failed.\n" );
                    unlock_cache ( sem );
                    return -1;
                }
            }
            else
            {
                CFG_DEBUG ( log_file, "Value[%d] == NULL.\n", tmp_index_para );
                unlock_cache ( sem );
                return -1;
            }

#ifdef CONFLIB_TABLE_ID_SENSITIVITY
            /* if the last_row is not NULL, add last_row to new row
               if the last_row is NULL, the new row is already inited to 0
             */
            if ( last_row )
            {
                tmp_row->next = OFFSET_BASE ( last_row, cache_addr );
            }
#endif

            for ( tmp_index_para = 0; tmp_index_para < irow_endnum - 1; tmp_index_para++ )
            {
                /* no matter how many val it is, we always add the "irow_endnum" parameters */
                memset ( str_tmp, 0, MAX_VALUE_LEN );
                if ( val[tmp_index_para] != NULL )
                {
                    strncpy ( str_tmp, val[tmp_index_para], MAX_VALUE_LEN - 1 );
                }

                if ( tmp_index_para == 0 ) // the first parameter
                {
                    tmp_para = add_para_tail ( & ( tmp_row->para ), GET_PARA_ID ( app_index, tmp_index_row, tmp_index_para + 1 ), str_tmp );
                }
                else
                {
                    tmp_para = add_para_tail ( & ( tmp_para->next ), GET_PARA_ID ( app_index, tmp_index_row, tmp_index_para + 1 ), str_tmp );
                }

                if ( !tmp_para )
                {
                    CFG_DEBUG ( log_file, "Add new para failed.\n" );
                    unlock_cache ( sem );
                    return -1;
                }

                CFG_DEBUG ( log_file, "Add parameter successfully\n" );

            }

            /* Record the write ID */
            gRecordWriteID[app_index - 1] = '1';
            
            /* come here is that add row sucess. */
            unlock_cache ( sem );
            return 0;
        }
    }

    CFG_DEBUG ( log_file, "[ADD TABLE] Can not find the app index\n" );
    unlock_cache ( sem );
    return -1;
}



/***********************************************************************
Function: config_api_delete_table_row
Description:  delete the table row

Return: int, if delete successfully, return 0, otherwise return -1
Input: int app_index,
       int row_index ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: Add library check
        Author: Ken
        Date:   2009-08-19
************************************************************************/
int config_api_delete_table_row ( int app_index, int row_index )
{
    app_node_t    *tmp_app;
    para_node_t   *tmp_para;
    row_node_t    *tmp_row, *prev_row = NULL, *next_row = NULL;
    int      sem;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }
    if ( app_index >= ID_END || app_index <= 0 )
    {
        CFG_DEBUG ( log_file, "APP index error\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    if ( ( sem = get_sem_id() ) < 0 )
    {
        CFG_DEBUG ( log_file, "Get sem id error\n" );
        return -1;
    }
    if ( lock_cache ( sem ) < 0 )
    {
        CFG_DEBUG ( log_file, "lock cache error\n" );
        return -1;
    }

    for ( ; tmp_app && tmp_app->app_id != -1;
            tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        if ( tmp_app->app_id == app_index )
        {
            prev_row = next_row = NULL;

            for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                    tmp_row && tmp_row->row_id != -1;
                    tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
            {
                if ( tmp_row->row_id < GET_ROW_ID ( app_index, row_index ) )
                {
                    /* record the PREVIOUS row of current row */
                    prev_row = tmp_row;
                }
                else if ( tmp_row->row_id > GET_ROW_ID ( app_index, row_index ) )
                {
                    CFG_DEBUG ( log_file, "[DELETE TABLE] Find the app but row id does not exist \n" );
                    unlock_cache ( sem );
                    return -1;
                }
                else /* find row == */
                {
                    /* record the NEXT row of current row */
                    next_row = NEXT ( row_node_t, tmp_row->next, cache_addr );

                    for ( tmp_para = NEXT ( para_node_t, tmp_row->para, cache_addr );
                            tmp_para && tmp_para->para_id != -1;
                            tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
                    {  /* clean up current table */
                        tmp_para->para_id = -1;
                        tmp_para->para_name[0] = '\0';
                        tmp_para->para_val[0] = '\0';
                        /* here we do not clean the "next", it doesn't matter, find_free_node just check para_id == -1*/
                    }

                    /* cleanup current row node */
                    tmp_row->row_id = -1;
                    tmp_row->para = 0;
                    tmp_row->next = 0;

                    if ( next_row != NULL )
                    {
                        if ( prev_row != NULL )
                        {
                            prev_row->next = OFFSET_BASE ( next_row, cache_addr );

                        }
                        else /* prev_row == NULL */
                        {
                            tmp_app->row = OFFSET_BASE ( next_row, cache_addr );
                        }
#ifndef CONFLIB_TABLE_ID_SENSITIVITY
                        /* modify the row id */
                        for ( tmp_row = next_row;
                                tmp_row && tmp_row->row_id != -1;
                                tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
                        {
                            tmp_row->row_id -= 0x100;

                            tmp_para = NEXT ( para_node_t, tmp_row->para, cache_addr );
                            for ( ; tmp_para && tmp_para->para_id != -1;
                                    tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
                            {
                                tmp_para->para_id -= 0x100;
                            }

                        }
#endif
                    }
                    else /* next_row == NULL */
                    {
                        if ( prev_row != NULL )
                        {
                            prev_row->next = 0;
                        }
                        else
                        {
                            /* the deleted row is pointing to app */
                            tmp_app->row = 0;
                        }
                    }

                    /* Record the write ID */
                    gRecordWriteID[app_index - 1] = '1';
                    
                    /* come here is that delete table row success. */
                    CFG_DEBUG ( log_file, "[DELETE TABLE] Delete table sucessfully \n" );
                    unlock_cache ( sem );
                    return 0;
                }
            }

            CFG_DEBUG ( log_file, "[DELETE TABLE] Find the app but can not find the row \n" );
            unlock_cache ( sem );
            return -1;
        }
    }

    CFG_DEBUG ( log_file, "[DELETE TABLE] Can not find app index \n" );
    unlock_cache ( sem );
    return -1;
}


/***********************************************************************
Function: config_api_init_library
Description:  Initialize the library conflib.

Return: int, if init successfully, return 0, otherwise return -1
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: use the fixed key instead of ftok
        Author: Ken
        Date:   2009-08-19
************************************************************************/
int config_api_init_library ( void )
{
    int         sem;
    int         ret;
    int         shm_id;
    void        *buf;

#ifdef   config_debug
    open_logger ();
#endif

    //init lock,add by nick
    ret = init_cache_lock();
    if ( ret < 0 )
    {
        CFG_DEBUG ( log_file, "init cache lock error\n" );
        return -1;
    }
    
    //lock this function to make function calling safe under mutiple threads,add by nick
    if ( ( sem = get_sem_id() ) < 0 )
    {
        CFG_DEBUG ( log_file, "Get sem id error\n" );
        return -1;
    }

    if ( lock_cache ( sem ) < 0 )
    {
        CFG_DEBUG ( log_file, "lock cache error\n" );
        return -1;
    } 
    
    //add by ken, if recalled, return
    if ( cache_addr != NULL )
    {
        CFG_DEBUG ( log_file, "mmap is already inited, no need to call me\n" );
        //unlock befor return ,or blocked, add by nick
        unlock_cache ( sem );
        return 0;
    }

    /* Init the parameter */
    init_Param();
    
    shm_id = shmget ( KEY_SHARE_MEMORY, 0, IPC_EXCL );

    /* The share memory is not exist. */
    if ( shm_id == -1 )
    {
        CFG_DEBUG ( log_file, "The first time to init paramter table\n" );
        int a = init_library();
        unlock_cache ( sem );
        return a;
    }

    shm_id = shmget ( KEY_SHARE_MEMORY, gMMAPSIZE, IPC_CREAT | 0666 );
    if ( shm_id == -1 )
    {
        CFG_DEBUG ( log_file, "shmget error.\n" );
        unlock_cache ( sem );
        return -1;
    }

    buf = shmat ( shm_id, NULL, 0 );
    if ( buf == ( void * ) ( -1 ) )
    {
        CFG_DEBUG ( log_file, "shmat error.\n" );
        unlock_cache ( sem );
        return -1;
    }

    if ( check_library ( buf ) )
    {
        unlock_cache ( sem );
        return -1;
    }

    CFG_DEBUG ( log_file, "config_api_init_library success\n" );
    cache_addr = buf;
    unlock_cache ( sem );
    return  0;
}


/***********************************************************************
Function: config_api_uninit_library
Description:  Uninit the library conflib.

Return: void,
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
void config_api_uninit_library ( void )
{
    if ( cache_addr )
    {
        if ( shmdt ( cache_addr ) == -1 )
        {
            CFG_DEBUG ( log_file, "shmdt error.\n" );
        }
        CFG_DEBUG ( log_file, "config_api_uninit_library\n" );
        cache_addr = NULL;
    }

#ifdef   config_debug
    close_logger();
#endif
}


/***********************************************************************
Function: config_api_release_library
Description:  This function will release all the resource, contain share
        memory and lock.

Return: int, if release successfully, return 0, otherwise return -1
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: use the fixed key instead of ftok
        Author: Ken
        Date:   2009-08-19
************************************************************************/
int config_api_release_library ( void )
{
    int sem_id, shm_id;
    shm_id = shmget ( KEY_SHARE_MEMORY, 0, IPC_EXCL );

    /* The share memory is not exist. */
    if ( shm_id == -1 )
    {
        CFG_DEBUG ( log_file, "The first time to init paramter table\n" );
        return -1;
    }

    shm_id = shmctl ( shm_id, IPC_RMID, NULL );
    if ( shm_id == -1 )
    {
        CFG_DEBUG ( log_file, "shmget error.\n" );
        return -1;
    }

    sem_id = semget ( KEY_LOCK_CACHE_FILE, 0, IPC_EXCL );
    if ( sem_id == -1 )
    {
        CFG_DEBUG ( log_file, "semget:%s\n", strerror ( errno ) );
        return -1;
    }

    if ( semctl ( sem_id, 0, IPC_RMID, NULL ) < 0 )
    {
        CFG_DEBUG ( log_file, "semctl:%s\n", strerror ( errno ) );
        return -1;
    }
    return 0;

}



/***********************************************************************
Function: config_api_write_to_flash
Description:  Write the data which from share memory to os.conf/flash.

Return: int, if write to flash successfully, return 0, otherwise return -1
Input: 
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification: when writing the file, check the ID, if necessary, write the config file
        Author: Ken
        Date: 2009-08-20
************************************************************************/
int config_api_write_to_flash ()
{
    FILE         *fp;
    int          sem_id;
    app_node_t   *tmp_app;
    para_node_t  *tmp_para;
    row_node_t   *tmp_row;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return -1;
    }

    if ( check_library ( cache_addr ) )
    {  
        return -1;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Parameter table destoryed\n" );
        return -1;
    }

    if ( ( sem_id = get_sem_id() ) < 0 )
    {
        CFG_DEBUG ( log_file, "Get sem id error\n" );
        return -1;
    }

    if ( lock_cache ( sem_id ) < 0 )
    {
        CFG_DEBUG ( log_file, "lock cache error\n" );
        return -1;
    }

    fp = fopen ( PARAMETER_FILE, "w" );
    if ( !fp )
    {
        CFG_DEBUG ( log_file, "Open parameter file error\n" );
        unlock_cache ( sem_id );
        return -1;
    }

    /* loop all the app node */
    for ( ; tmp_app && tmp_app->app_id != -1; tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        fprintf ( fp, "[%s]\n", tmp_app->app_name );

        /* loop current app's parameter node */
        for ( tmp_para = NEXT ( para_node_t, tmp_app->para, cache_addr );
                tmp_para && tmp_para->para_id != -1;
                tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
        {
            fprintf ( fp, "%s=%s\n", tmp_para->para_name, tmp_para->para_val );
        }

        /* if current app has rows, loop all the rows.
           else start checking next app node */
        for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                tmp_row && tmp_row->row_id != -1;
                tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
        {
            fprintf ( fp, "<%d>\n", ( tmp_row->row_id >> 8 ) & 0x000000ff );

            /* loop current row's parameter node */
            for ( tmp_para = NEXT ( para_node_t, tmp_row->para, cache_addr );
                    tmp_para && tmp_para->para_id != -1;
                    tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
            {
                fprintf ( fp, "%s=%s\n", tmp_para->para_name, tmp_para->para_val );
            }

        }

    }

    fclose ( fp );

    unlock_cache ( sem_id );
    CFG_DEBUG ( log_file, "Write parameters to flash success.\n" );

    /* update some application's config file */
    update_config_file ( );
    return 0;

}


/***********************************************************************
Function: dump_para_table
Description:  dump all the parameters in share memory

Return: void,
Input: void ,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Mark Yan
        Date: 2009-03-18

    2.  Modification:
        Author:
        Date:
************************************************************************/
void dump_para_table ( void )
{
    app_node_t   *tmp_app;
    para_node_t  *tmp_para;
    row_node_t   *tmp_row;

    if ( !cache_addr )
    {
        CFG_DEBUG ( log_file, "Config library not initialize\n" );
        return;
    }

    if ( check_library ( cache_addr ) )
    {
        return;
    }

    tmp_app = APP_START_NODE ( cache_addr );
    if ( !tmp_app )
    {
        CFG_DEBUG ( log_file, "Memory has been destroied\n" );
        return;
    }

    /* loop all the app node */
    for ( ; tmp_app && tmp_app->app_id != -1;
            tmp_app = NEXT ( app_node_t, tmp_app->next, cache_addr ) )
    {
        printf ( "[%s]\n", tmp_app->app_name );

        /* loop current app's parameter node */
        for ( tmp_para = NEXT ( para_node_t, tmp_app->para, cache_addr );
                tmp_para && tmp_para->para_id != -1;
                tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
        {
            printf ( "%s=%s\n", tmp_para->para_name, tmp_para->para_val );

        }


        /* if current app has rows, loop all the rows.
           else start checking next app node */
        for ( tmp_row = NEXT ( row_node_t, tmp_app->row, cache_addr );
                tmp_row && tmp_row->row_id != -1;
                tmp_row = NEXT ( row_node_t, tmp_row->next, cache_addr ) )
        {
            printf ( "<%d>\n", ( tmp_row->row_id >> 8 ) & 0x000000ff );

            /* loop current row's parameter node */
            for ( tmp_para = NEXT ( para_node_t, tmp_row->para, cache_addr );
                    tmp_para && tmp_para->para_id != -1;
                    tmp_para = NEXT ( para_node_t, tmp_para->next, cache_addr ) )
            {
                printf ( "%s=%s\n", tmp_para->para_name, tmp_para->para_val );
            }
        }

    }

    return;
}





