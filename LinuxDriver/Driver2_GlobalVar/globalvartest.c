#include <sys/types.h>           
#include <sys/stat.h> 
#include <stdio.h>
#include <fcntl.h> 
main()
{
int fd, num;
fd = open("/dev/globalvar", O_RDWR, S_IRUSR | S_IWUSR); 
if (fd != -1 )
{
//���ζ�globalvar
read(fd, &num, sizeof(int));
printf("The globalvar is %d\n", num);
//дglobalvar
printf("Please input the num written to globalvar\n");
scanf("%d", &num);
write(fd, &num, sizeof(int));

//�ٴζ�globalvar
read(fd, &num, sizeof(int));
printf("The globalvar is %d\n", num);

//�ر�"/dev/globalvar"
close(fd);
}
else
{
printf("Device open failure\n");
}
}
