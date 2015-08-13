#include <stdio.h> // void perror(const char* msg);
#include <errno.h> //errno
#include <string.h> //char *strerror(int errnum);

int main(int argc, char* argv[])
{
  //fprintf(stderr, "EACCESS:%s\n", strerror(EACCES));
  printf("This is a main test!\n");
  printf("EACCESS:%s\n",(char*)strerror(EACCES));
  errno = ENOENT;
  perror("Failed:");
  exit(0);
}

/*
./errno 
This is a main test!
EACCESS:Permission denied
Failed:: No such file or directory
*/

/*
1、头文件对应关系见上
2、strerror打印参数errno对应的错误信息
3、perror先打印参数字符串，然后在打印该线程的errno对应的错误信息
*/