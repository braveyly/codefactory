#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "appBeMonitored.h"

int main ( int argc, char* argv[] )
{
    keepFileLock ( argv[0] + 2 );
    sleep ( 100 );
}
