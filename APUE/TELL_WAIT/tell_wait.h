#ifndef _TELL_WAIT_H_
#define _TELL_WAIT_H_

#include <signal.h>

void TELL_WAIT(void);

void TELL_PARENT(pid_t pid);

void WAIT_PARENT(void);

void WAIT_CHILD(void);

void TELL_CHILD(pid_t pid);


#endif