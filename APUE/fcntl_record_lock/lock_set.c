/*
 * ���ü�¼���Ӻ���lock_set.c
 *
 * ��¼����Ϊ��ȡ����д���������ж�ȡ���ֳ�Ϊ������������ʹ���
 * ���̶��ܹ����ļ���ͬһ���ֽ�����ȡ������д�����ֳ�Ϊ��������
 * ���κ�ʱ��ֻ����һ���������ļ���ĳ�����ֽ���д��������Ȼ����
 * �ļ���ͬһ���ֲ���ͬʱ������ȡ����д������
 *
 * fcntl��lock�ṹ������ʾ��
 * struct flock {
 * short l_type;
 * off_t l_start;
 * short l_whence;
 * off_t l_len;
 * pid_t l_pid;
 * }
 *
 * ���ɣ�Ϊ���������ļ���ͨ���ķ����ǽ�l_start˵��Ϊ0��l_whence
 * ˵��ΪSEEK_SET��l_len˵��Ϊ0��
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void print_lock(struct flock lock)
{
        printf(" -----------------------------\n");

        if (lock.l_type == F_RDLCK) {
                printf("\tl_type: F_RDLCK\n");
        }
        else if (lock.l_type == F_WRLCK) {
                printf("\tl_type: F_WRLCK\n");
        }
        else if (lock.l_type == F_UNLCK) {
                printf("\tl_type: F_UNLCK\n");
        }

        printf("\tl_start: %d\n", (int)lock.l_start);

        if (lock.l_whence == SEEK_SET) {
                printf("\tl_whence: SEEK_SET\n");
        }
        else if (lock.l_whence == SEEK_CUR) {
                printf("\tl_whence: SEEK_CUR\n");
        }
        else if (lock.l_whence == SEEK_END) {
                printf("\tl_whence: SEEK_END\n");
        }

        printf("\tl_len: %d\n", (int)lock.l_len);

        printf(" -----------------------------\n");
}

void lock_set(int fd, int type)
{
        struct flock lock;

        /*��ֵlock�ṹ�壬���������ļ�*/
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 10;

        while (1) {
                lock.l_type = type;

                /*
                 * ���ݲ�ͬ��type�����ļ������������
                 * ����ɹ����򷵻�0��ʧ���򷵻�1��
                 * ���������һ���ļ�ԭ���Ѿ������˻���������ô�ٵ���fcntl
                 * �������ͻ�ʧ�ܣ�����-1��
                 */
                if ((fcntl(fd, F_SETLK, &lock)) == 0) {
                        /*����ǹ�����*/
                        if (lock.l_type == F_RDLCK) {
                                printf("read only set by %d\n", getpid());
                        }
                        /*����ǻ�����*/
                        else if (lock.l_type == F_WRLCK) {
                                printf("write lock set by %d\n", getpid());
                        }
                        else if (lock.l_type == F_UNLCK) {
                                printf("release lock by %d\n", getpid());
                        }
                        print_lock(lock);
                        return;
                }
                else {
                        /*
                         * ���lock��������Ҳ���ǽ��ļ�fd�ļ�����Ϣ���뵽lock�ṹ��
                         * ����ɹ��򷵻�0
                         * ���ʧ���򷵻�-1
                         */
                        if ((fcntl(fd, F_GETLK, &lock)) == 0) {
                                if (lock.l_type != F_UNLCK) {
                                        if (lock.l_type == F_RDLCK) {
                                                printf("read lock already set by %d\n", lock.l_pid);
                                        }
                                        else if (lock.l_type == F_WRLCK) {
                                                printf("write lock already set by %d\n", lock.l_pid);
                                        }
                                        getchar();
                                }
                        }
                        else {
                                printf("cannot get the description of struck flock.\n");
                        }
                }
        }
}