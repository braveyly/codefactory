#include <sys/mman.h>  /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h>  /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */
#include <stdio.h>

#define PAGE_SIZE 4096

int main ( int argc, char **argv )
{
    int fd, flenth, i = 0, readSize = 0, readOffset = 0;
    char *mapped_mem;
    void * start_addr = 0;

    fd = open ( argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
    flenth = lseek ( fd, 1, SEEK_END );
    printf ( "File %s length is %d\n", argv[1], flenth );

    while ( ( readSize + readOffset ) < flenth )
    {
        sleep ( 4 );
        readSize = PAGE_SIZE * 10 * 10;
        readOffset = PAGE_SIZE * 10 *10 * i;
        mapped_mem = mmap ( NULL, readSize, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE,
                            fd, readOffset );
        * ( mapped_mem + readSize ) = '\0';
        printf ( "Map memory [%d==%s==%dst=read%d=left%d]\n", i, mapped_mem, i, readSize, flenth - readSize );
        i++;
        munmap ( mapped_mem, readSize );
    }
    /*Read last block shared memory*/
    readSize = flenth - readOffset;
    readOffset = readOffset;
    mapped_mem = mmap ( NULL, readSize, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE,
                        fd, readOffset );
    * ( mapped_mem + readSize ) = '\0';
    printf ( "Last Map Memory [%s] readsize [%d]\n", mapped_mem, readSize );
    munmap ( mapped_mem, readSize );
    close ( fd );

    sleep ( 100 );

    return 0;
}
