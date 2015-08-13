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
//初次读globalvar
read(fd, &num, sizeof(int));
printf("The globalvar is %d\n", num);
//写globalvar
printf("Please input the num written to globalvar\n");
scanf("%d", &num);
write(fd, &num, sizeof(int));

//再次读globalvar
read(fd, &num, sizeof(int));
printf("The globalvar is %d\n", num);

//关闭"/dev/globalvar"
close(fd);
}
else
{
printf("Device open failure\n");
}
}
