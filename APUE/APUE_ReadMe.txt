1、atexit.c
在函数自然或者异常退出时，确保能执行一段特定的清理代码
（1）kill pid，杀掉进程，产生SIGTERM信号，导致进程异常退出
（2）uninit_flag变量是确保清理代码只执行一次。便于写移植性程序，建议将信号处理函数中int类型设置成sig_atomic_t，理由如下：
“在Linux系统中，sig_atomic_t就是基本的int类型；事实上，对int或者更小的整型变量以及指针赋值的操作都是原子操作。不过，如果你希望所写的程序可以向任何标准UNIX系统移植，则应将所有全局变量设为sig_atomic_t类型。”

2、dir.c
打开某个目录（opendir），然后遍历（readdir）里面的文件。
涉及到的结构体是DIR和struct dirent。

3、errno.c
当程序出错时，查看errno信息。
（1）注意对应的头文件信息，否则程序会出现段错误
（2）strerror(errno)，打印errno对应的错误信息
（3）perror(str)，先打印str，然后在打印errno对应的错误信息

4、time.c
测试linux环境下各个时钟函数。
（1）获取不同精度的时间，包括秒、毫秒、微妙、纳秒。API分别是time()、ftime()、gettimeofday()和gethrestime()。其中gethrestime()是posix标准，没有实现。
其中结构体分别是：time_t、struct timeb、struc timeval和struct timespec
（2）在获取了从1970-01-01 00:00:00以来的秒数后，则可以进行各种转换。包括ctime：Tue Apr 29 15:54:56 2014；gmtime：2014-4-29 Tue 7:54:56；localtime：2014-4-29 Tue 15:54:56，本地时间相差8h；
（3）difftime是宏，而不是函数，注意在表达式中会展开。

5、mkstemp.c
临时文件生成和删除。
（1）tmpfile = mkstemp + unlink
（2）seek operation
（3）mktemp, tmpnam, tempnam are not safe.
（4）临时文件使用：保证临时文件的文件名不互相冲突；保证临时文件中内容不被其他用户或者黑客操作。
mkstemp的参数以XXXXXX结尾，函数会随机产生的字符串替换XXXXXX，保证了文件名得唯一性，访问权限是0600
unlink函数删除文件的目录，所以临时文件还可以通过fd访问，直到最后一个打开的进程关闭fd，或者程序退出时，
文件才会自动彻底被删除

6、syslog.c
关于syslog的测试程序

7、get_file_size.c
获取文件大小的方法

8、fcntl_record_lock
（1）记录锁分为读锁、写锁。
（2）读锁是共享锁，可以有多把，多个进程可以同时对一个文件读。
     写锁是互斥锁，同一时间只能有一个进程写文件。
struct flock
{
    short l_type;
    off_t l_start;
    short l_whence;
    off_t l_len;
    pid_t l_pid;
};
l_type有三种取值：F_RDLCK(读锁）,F_WRLCK（写锁）,F_UNLCK（解锁）；（也即0.1.2三个值）；
l_whence也有三种取值：SEEK_SET（文件开头）,SEEK_END（文件结尾）,SEEK_CUR（当前位置）；
l_len 是加锁区域的长度；
l_pid 是加锁进程的pid号；
（3）技巧：为加锁整个文件，通常的方法是将l_start说明为0，l_whence说明为SEEK_SET，l_len说明为0。

9、lockf
posix文件锁，比较于flock文件锁和fcntl记录锁