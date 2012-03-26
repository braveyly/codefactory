#include <stdio.h>
#include <event2/util.h>
#include <event2/buffer.h>

struct evbuffer* pevb = NULL;

void printAlertInfo()
{
    printf ( "1:Get length of one evbuffer\n" );
    printf ( "2:Get length of the first contiguous space\n" );
    printf ( "3:Get data with evbuffer_copyout\n" );
    printf ( "4:Get data for the first ? bytes with evbuffer_peek\n" );
    printf ( "5:Get data for the first ? chunks with evbuffer_peek\n" );
    printf ( "6:Search data by key in evbuffer with evbuffer_search\n" );
    printf ( "7:Search data by offset in evbuffer with evbuffer_search\n" );
    printf ( "8:Add data to the end of evbuffer with evbuffer_add\n" );
    printf ( "9:Add data to the end of evbuffer with evbuffer_add_printf\n" );
    printf ( "10:Add data to the front of evbuffer with evbuffer_prepend\n" );
    printf ( "11:Remove data from evbuffer with evbuffer_drain\n" );
    printf ( "12:Remove data from evbuffer with evbuffer_remove\n" );
    printf ( "13:Advance to get all chunks info\n" );

}

int doNewEvbuffer()
{
    pevb = evbuffer_new();
    if ( NULL == pevb )
        return -1;
    else
        return 0;
}

void cb_add_remove_data ( struct evbuffer *buffer,
                          const struct evbuffer_cb_info *info, void *arg )
{
    int *tp = arg;
    printf ( "orig_size[%d] n_added[%d] n_deleted[%d] arg[%d] length[%d] contiguous[%d]\n",
             info->orig_size, info->n_added, info->n_deleted, *tp,
             evbuffer_get_length ( buffer ),
             evbuffer_get_contiguous_space ( buffer ) );

}

int doInstallCB()
{
    int tp = 1;
    evbuffer_add_cb ( pevb, cb_add_remove_data, &tp );
}

int test_evbuffer_get_length ( struct evbuffer *buffer )
{
    printf ( "Evbuffer length:%d\n", evbuffer_get_length ( buffer ) );
}

int test_evbuffer_get_contiguous_space ( struct evbuffer *buffer )
{
    printf ( "Evbuffer contiguous space length:%d\n", evbuffer_get_contiguous_space ( buffer ) );
}

int test_evbuffer_add ( struct evbuffer *buffer )
{
    char inputBuf[100] = {0};
    int ret;
    printf ( "Please input your end data:\n" );
    scanf ( "%s", inputBuf );
    ret = evbuffer_add ( buffer, inputBuf, strlen ( inputBuf ) );
    if ( 0 == ret )
    {
        printf ( "Succeed to evbuffer_add\n" );
        return 0;
    }
    else if ( -1 == ret )
    {
        printf ( "Failed to evbuffer_add\n" );
        return -1;
    }
}

int test_evbuffer_add_printf ( struct evbuffer *buffer )
{
    int ret;
    ret = evbuffer_add_printf ( buffer, "Hello %s %d.%d.%d", "world", 2, 0, 1 );
    if ( 0 == ret )
    {
        printf ( "Succeed to evbuffer_add_printf\n" );
        return 0;
    }
    else if ( -1 == ret )
    {
        printf ( "Failed to evbuffer_add_printf\n" );
        return -1;
    }
}

int test_evbuffer_prepend ( struct evbuffer *buffer )
{
    char inputBuf[100] = {0};
    int ret;
    printf ( "Please input your front data:\n" );
    scanf ( "%s", inputBuf );
    ret = evbuffer_prepend ( buffer, inputBuf, strlen ( inputBuf ) );
    if ( 0 == ret )
    {
        printf ( "Succeed to evbuffer_prepend\n" );
        return 0;
    }
    else
    {
        printf ( "Failed to evbuffer_prepend\n" );
        return -1;
    }
}

int test_evbuffer_copyout ( struct evbuffer *buffer )
{
    char outputBuf[100] = {0};
    int outputLen = 0;
    int ret;
    printf ( "Please input your output data length:\n" );
    scanf ( "%d", &outputLen );
    ret = evbuffer_copyout ( buffer, outputBuf, outputLen );
    if ( -1 == ret )
    {
        printf ( "Failed to evbuffer_copyout\n" );
        return -1;
    }
    else
    {
        printf ( "copy out data bytes [%d]\n", ret );
        printf ( "output data [%s]\n", outputBuf );
        return 0;
    }

}

int test_evbuffer_drain ( struct evbuffer *buffer )
{
    int removeLen = 0;
    int ret = 0;
    printf ( "Please input remove data length:\n" );
    scanf ( "%d", &removeLen );
    ret = evbuffer_drain ( buffer, removeLen );
    if ( 0 == ret )
    {
        printf ( "Succeed to evbuffer_drain\n" );
        return 0;
    }
    else
    {
        printf ( "Failed to evbuffer_drain\n" );
        return -1;
    }
}

int test_evbuffer_remove ( struct evbuffer *buffer )
{
    int removeLen = 0;
    char removeBuf[100] = {0};
    int ret;
    printf ( "Please input remove data length:\n" );
    scanf ( "%d", &removeLen );
    ret = evbuffer_remove ( buffer, removeBuf, removeLen );
    if ( -1 == ret )
    {
        printf ( "Failed to evbuffer_remove\n" );
        return -1;
    }
    else
    {
        printf ( "Succeed to evbuffer_remove %d bytes\n", ret );
        return 0;
    }
}

int test_evbuffer_peek_by_chunks ( struct evbuffer *buffer )
{
    int chunksNum = 0;
    int i;
    struct evbuffer_iovec *v;
    printf ( "Please input the number of chunk that you want to access:\n" );
    scanf ( "%d", &chunksNum );
    v = malloc ( sizeof ( struct evbuffer_iovec ) * chunksNum );
    chunksNum = evbuffer_peek ( buffer, -1, NULL, v, chunksNum );
    for ( i = 0; i < chunksNum; ++i )
    {
        printf ( "v[%d].iov_base=[%s],v[%d].iov_len=[%d]\n",
                 i, v[i].iov_base, i, v[i].iov_len );
    }
    free ( v );
}

int test_evbuffer_peek_by_bytes ( struct evbuffer *buffer )
{
    int bytesNum = 0;
    int chunksNum = 0;
    int i;
    struct evbuffer_iovec *v;
    printf ( "Please input the number of bytes that you want to access:\n" );
    scanf ( "%d", &bytesNum );
    /* Theorilly:determine how many chunks we need. */
    chunksNum = evbuffer_peek ( buffer, bytesNum, NULL, NULL, 0 );
    printf ( "In theory:Need %d chunks for %d bytes\n", chunksNum, bytesNum );
    /* Allocate space for the chunks.*/
    v = malloc ( sizeof ( struct evbuffer_iovec ) * chunksNum );
    /* Actually:fill up v. */
    /*The total bytes in chunksNum chunks is more than bytesNum*/
    chunksNum = evbuffer_peek ( buffer, bytesNum, NULL, v, chunksNum );
    printf ( "Actually:Need %d chunks\n", chunksNum );
    for ( i = 0; i < chunksNum; ++i )
    {
        printf ( "v[%d].iov_base=[%s],v[%d].iov_len=[%d]\n",
                 i, v[i].iov_base, i, v[i].iov_len );
    }
    free ( v );
}

int test_evbuffer_search_by_key ( struct evbuffer *buffer )
{
    char searchStr[100] = {0};
    struct evbuffer_ptr ptr;
    struct evbuffer_iovec v[1];

    printf ( "Please input the string that you want to search:\n" );
    scanf ( "%s", searchStr );

    evbuffer_ptr_set ( buffer, &ptr, 0, EVBUFFER_PTR_SET );//IMPORTANT
    while ( 1 )
    {
        ptr = evbuffer_search ( buffer, searchStr, strlen ( searchStr ), &ptr );
        if ( ptr.pos < 0 )
            break;

        printf ( "Find %s at position %d\n", searchStr, ptr.pos );
        ////GET part of chunk by start arg
        if ( evbuffer_peek ( buffer, strlen ( searchStr ), &ptr, v, 1 ) < 0 )
        {
            printf ( "Cannot peek it out\n" );
            break;
        }
        printf ( "Peek [%s][%d] out\n", v[0].iov_base, v[0].iov_len );
        evbuffer_ptr_set ( buffer, &ptr, 1, EVBUFFER_PTR_ADD );
    }
}

int test_evbuffer_ptr_set_and_peek_by_offet ( struct evbuffer *buffer )
{
    int offset;
    struct evbuffer_ptr ptr;
    struct evbuffer_iovec v[1];

    while ( 1 )
    {
        printf ( "Please input the offset from the start that you want to view:\n" );
        scanf ( "%d", &offset );

        evbuffer_ptr_set ( buffer, &ptr, 0, EVBUFFER_PTR_SET );//IMPORTANT
        evbuffer_ptr_set ( buffer, &ptr, offset, EVBUFFER_PTR_ADD );

        ////GET part of chunk by start arg
        if ( evbuffer_peek ( buffer, 0, &ptr, v, 1 ) < 0 )
        {
            printf ( "Cannot peek it out\n" );
            break;
        }
        printf ( "Peek [%s][%d] out\n", v[0].iov_base, v[0].iov_len );

    }
}

int advance_get_chunks_info ( struct evbuffer *buffer )
{
    int totalBytes = 0, i;
    int totalChunksTheory = 0, totalChunksActual = 0;//totalChunksActual <= totalChunksTheory
    struct evbuffer_iovec *v = NULL;

    totalBytes = evbuffer_get_length ( buffer );

    totalChunksTheory = evbuffer_peek ( buffer, totalBytes, NULL, NULL, 0 );
    v = malloc ( sizeof ( struct evbuffer_iovec ) * totalChunksTheory );
    totalChunksActual = evbuffer_peek ( buffer, totalBytes, NULL, v, totalChunksTheory );
    printf ( "totalBytes[%d] totalChunksTheory[%d] totalChunksActual[%d]\n",
             totalBytes, totalChunksTheory, totalChunksActual );
    for ( i = 0; i < totalChunksActual; ++i )
    {
        printf ( "v[%d].iov_base=[%s],v[%d].iov_len=[%d]\n",
                 i, v[i].iov_base, i, v[i].iov_len );
    }

    if ( v )
        free ( v );
}

int main()
{
    int select;
    doNewEvbuffer();
    doInstallCB();
    while ( 1 )
    {
        printAlertInfo();
        scanf ( "%d", &select );
        printf ( "Your input is %d\n", select );
        switch ( select )
        {
            case 1:
                test_evbuffer_get_length ( pevb );
                break;
            case 2:
                test_evbuffer_get_contiguous_space ( pevb );
                break;
            case 3:
                test_evbuffer_copyout ( pevb );
                break;
            case 4:
                test_evbuffer_peek_by_bytes ( pevb );
                break;
            case 5:
                test_evbuffer_peek_by_chunks ( pevb );
                break;
            case 6:
                test_evbuffer_search_by_key ( pevb );
                break;
            case 7:
                test_evbuffer_ptr_set_and_peek_by_offet ( pevb );
                break;
            case 8:
                test_evbuffer_add ( pevb );
                break;
            case 9:
                test_evbuffer_add_printf ( pevb );
                break;
            case 10:
                test_evbuffer_prepend ( pevb );
                break;
            case 11:
                test_evbuffer_drain ( pevb );
                break;
            case 12:
                test_evbuffer_remove ( pevb );
                break;
            case 13:
                advance_get_chunks_info ( pevb );
                break;
            default:
                printf ( "Input is invalid\n" );
                break;
        }
    }
}
