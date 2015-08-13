#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned long get_file_size ( const char *filename )
{
    unsigned long size;
    FILE* fp = fopen ( filename, "rb" );
    if ( fp == NULL )
    {
        printf ( "ERROR: Open file %s failed.\n", filename );
        return 0;
    }
    fseek ( fp, SEEK_SET, SEEK_END );
    size = ftell ( fp );
    fclose ( fp );
    return size;
}

unsigned long get_file_size2 ( const char *filename )
{
    struct stat buf;
    if ( stat ( filename, &buf ) < 0 )
    {
        return 0;
    }
    return ( unsigned long ) buf.st_size;
}

int main()
{
    int a = get_file_size2 ( "./1.txt" );
    printf ( "a is %d\n", a );
}
