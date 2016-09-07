#ifndef _COM_DEF_H_
#define _COM_DEF_H_


#include <Afxmt.h>
#include <iostream>
#include <vector>
using namespace::std;

typedef struct _onlinehost
{
    char ip[18];
	unsigned long icmp_seq;
	unsigned long time;
}OnLineHost;

extern vector<OnLineHost> hostList;
extern CEvent eventUpdateView;
extern int finishFlag;
#define SCAN_LAN_HOST_START 1
#define SCAN_LAN_HOST_FINISH 2

int demo_main(int argc, char **argv);
#endif