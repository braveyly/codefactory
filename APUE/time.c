#include <stdio.h>
#include <time.h>  //time()
#include <sys/timeb.h> //ftime()
#include <sys/time.h> //gettimeofday()
//#include <sys/time_impl.h> //gethrestime()
int main()
{
  time_t seconds;
  seconds = time((time_t *)NULL);
  printf("seconds is %d\n",seconds);

  char* tmstr = ctime(&seconds);
  printf("tmstr is %s\n", tmstr);

  char *wday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
  struct tm *p_tm = NULL;
  p_tm = gmtime(&seconds);
  printf("%d-%d-%d ", (p_tm->tm_year+1900),(p_tm->tm_mon+1),p_tm->tm_mday);
  printf("%s %d:%d:%d\n",wday[p_tm->tm_wday],p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);

  struct tm *p_tm2 = NULL;
  p_tm2 = localtime(&seconds);
  printf("%d-%d-%d ", (p_tm2->tm_year+1900),(p_tm2->tm_mon+1),p_tm2->tm_mday);
  printf("%s %d:%d:%d\n",wday[p_tm2->tm_wday],p_tm2->tm_hour,p_tm2->tm_min,p_tm2->tm_sec);
  
  struct timeb t_ms;
  int ret = ftime(&t_ms);
  if(-1 != ret)
  {
    printf("t_ms.seconds %d t_ms.microseconds %d\n",t_ms.time,t_ms.millitm);
  }
  
  struct timeval tm_us;
  ret = gettimeofday(&tm_us,NULL);
  if(-1 != ret)
  {
    printf("tm_us.tv_sec %d tm_us.tv_usec %d\n",tm_us.tv_sec,tm_us.tv_usec);
  }
  
  /*
  struc timespec tm_ns;
  ret = gethrestime(&tm_ns);
  if(-1 != ret)
  {
    printf("tm_ns.tv_sec %d tm_ns.tv_nsec %d\n",tm_ns.tv_sec,tm_ns.tv_nsec);
  }
  */

  time_t timep1,timep2;
  timep1 = time(NULL);
  sleep(2);
  timep2 = time(NULL);
  time_t diff = difftime(timep2,timep1);
  printf("the difference is %d seconds\n", diff) ;
  //printf("the difference is %d seconds\n", difftime(timep2,timep1));//this is error
  return 0;
}

/*
./time
seconds is 1398758096
tmstr is Tue Apr 29 15:54:56 2014

2014-4-29 Tue 7:54:56
2014-4-29 Tue 15:54:56
t_ms.seconds 1398758096 t_ms.microseconds 854
tm_us.tv_sec 1398758096 tm_us.tv_usec 854331
*/