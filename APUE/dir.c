#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

int main(int argc,char* argv[])
{
  DIR *dp;
  struct dirent *dirp;

  if(2 != argc)
  {
    printf("usage:ls directory name\n");
  }
  
  if(NULL == (dp = opendir(argv[1])))
  {
    printf("open directory error\n");
  }
  
  while( NULL != (dirp = readdir(dp)))
  {
    printf("%s\n",dirp->d_name);
  }

  closedir(dp);
  exit(0);
}