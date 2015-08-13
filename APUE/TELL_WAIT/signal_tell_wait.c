#include <signal.h>
#include <stdio.h>
#include "tell_wait.h"

static volatile sig_atomic_t sigflag;
static sigset_t newmask,oldmask,zeromask;

static void sig_usr(int signo)
{
  sigflag = 1;
}

void TELL_WAIT(void)
{
  if( SIG_ERR == signal(SIGUSR1, sig_usr) )
  {
    printf("signal(SIGUSR1) error!\n");
  }
  if( SIG_ERR == signal(SIGUSR2, sig_usr) )
  {
    printf("signal(SIGUSR2) error!\n");
  }
  sigemptyset(&zeromask);
  sigemptyset(&newmask);
  sigaddset(&newmask,SIGUSR1);
  sigaddset(&newmask,SIGUSR2);
  if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
  {
    printf("SIG_BLOCK error!\n");
  }
}

void TELL_PARENT(pid_t pid)
{
  kill(pid,SIGUSR2);
}


void WAIT_PARENT(void)
{
  while(0 == sigflag)
  {
    sigsuspend(&zeromask);
  }
  sigflag = 0;
  
  if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
  {
    printf("SIG_SETMASK error!\n");
  }
}


void WAIT_CHILD(void)
{
  while(0 == sigflag)
  {
    sigsuspend(&zeromask);
  }
  sigflag = 0;
  
  if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
  {
    printf("SIG_SETMASK error!\n");
  }
}

void TELL_CHILD(pid_t pid)
{
  kill(pid,SIGUSR1);
}