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
1��ͷ�ļ���Ӧ��ϵ����
2��strerror��ӡ����errno��Ӧ�Ĵ�����Ϣ
3��perror�ȴ�ӡ�����ַ�����Ȼ���ڴ�ӡ���̵߳�errno��Ӧ�Ĵ�����Ϣ
*/