#include <stdio.h>

void test_mkstemp()
{
    int fd;
    char temp_file[] = "tmp_XXXXXX";
    /*Creat a temp file.*/
    if ( ( fd = mkstemp ( temp_file ) ) == -1 )
    {
        printf ( "Creat temp file faile.\n" );
        exit ( 1 );
    }
    /*Unlink the temp file.*/
    printf ( "temp_file is %s\n", temp_file );
    unlink ( temp_file );
    /*Then you can read or write the temp file.*/
    //ADD YOUR CODE;
    /*Close temp file, when exit this program, the temp file will be removed.*/
    close ( fd );
}

int main()
{
    printf ( "temp name is %s\n", tempnam ( "./", "set" ) );
    test_mkstemp();
}
