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
//4.��ʱ�ļ�ʹ�ã���֤��ʱ�ļ����ļ����������ͻ����֤��ʱ�ļ������ݲ��������û����ߺڿͲ�����
//mkstemp�Ĳ�����XXXXXX��β������������������ַ����滻XXXXXX����֤���ļ�����Ψһ�ԣ�����Ȩ����0600
//unlink����ɾ���ļ���Ŀ¼��������ʱ�ļ�������ͨ��fd���ʣ�ֱ�����һ���򿪵Ľ��̹ر�fd�����߳����˳�ʱ��
//�ļ��Ż��Զ����ױ�ɾ��