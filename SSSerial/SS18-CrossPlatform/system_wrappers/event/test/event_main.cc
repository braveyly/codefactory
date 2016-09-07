#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "event_wrapper.h"
using namespace::std;
cp::EventWrapper *eventWrapper;
DWORD ThreadProc(LPVOID threadNo)
{
	cout << "this is threadProc working thread" << endl;
	eventWrapper->Wait(5000);
	cout << "after wait" << endl;
	eventWrapper->Wait(5000);
	cout << "after wait again" << endl;
	return 1;
}
int main()
{
	DWORD targetThreadID = 0;
	eventWrapper = cp::EventWrapper::Create();
	HANDLE hThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadProc,
		NULL,
		0,
		&targetThreadID
		);
	Sleep(2000);
	eventWrapper->Set();
	while (1)
	{
		
	}
	return (DWORD)1;
}