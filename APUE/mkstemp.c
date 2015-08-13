#include <stdio.h>
#include <unistd.h>

int main1()
{
  char filename_template[] = "/home/liwei/test/tmpfile/temp_file.XXXXXX";
  int fd = mkstemp(filename_template);
  char* write_buf = "Hello World!123";
  char read_buf[100] = "123";
  unlink(filename_template);
  printf("filename_template %s fd %d\n",filename_template, fd);
  int ret = write(fd,write_buf,strlen(write_buf));//fd is at the end of the file
  printf("write buffer is %s ret is %d\n",write_buf,ret);
  int len =0;
  lseek(fd,0,SEEK_SET);//fd is at the start of the file
  ret = read(fd,(void *) read_buf, sizeof(read_buf)/sizeof(char));
  printf("buf is %s ret is %d\n",read_buf, ret);
  sleep(1000);
  close(fd);
}

int main()
{
  FILE* file = tmpfile();
  char *write_buf = "Hello world!";
  char read_buf[100] = {0};
  fwrite(write_buf,sizeof(char),strlen(write_buf),file);
  fseek(file,0,SEEK_SET);
  fread(read_buf,sizeof(char),100,file);
  printf("read_buf %s\n",read_buf);
}


//1.tmpfile = mkstemp + unlink
//2.seek operation
//3.mktemp, tmpnam, tempnam are not safe.
//4.临时文件使用：保证临时文件的文件名不互相冲突；保证临时文件中内容不被其他用户或者黑客操作。
//mkstemp的参数以XXXXXX结尾，函数会随机产生的字符串替换XXXXXX，保证了文件名得唯一性，访问权限是0600
//unlink函数删除文件的目录，所以临时文件还可以通过fd访问，直到最后一个打开的进程关闭fd，或者程序退出时，
//文件才会自动彻底被删除