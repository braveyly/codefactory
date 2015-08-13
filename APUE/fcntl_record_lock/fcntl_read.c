/*
 * �����ļ���ȡ��
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void lock_set(int, int);

void die(char *msg)
{
        perror(msg);
        exit(1);
}

int open_file(void)
{
        int fd;

        if ((fd = open("/tmp/hello.c", O_CREAT | O_TRUNC | O_RDWR, 0666)) < 0) {
                die("open error");
        }
        else {
                printf("Open file: hello.c %d\n", fd);
        }

        return fd;
}

void close_file(int fd)
{
        if (close(fd) < 0) {
                die("close error");
        }
        else {
                printf("Close file: hello.c\n");
        }
}

int main(void)
{
        int fd;

        fd = open_file();

        /*���ļ���д����*/
        lock_set(fd, F_RDLCK);
        /*�ȴ��������������*/
        getchar();
        /*���ļ�����*/
        lock_set(fd, F_UNLCK);
        getchar();
        close_file(fd);

        return 0;
}