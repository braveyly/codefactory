#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "atomic32.h"

using namespace::std;
cp::Atomic32 ref_count = 0;
DWORD ThreadProc(LPVOID threadNo)
{
	cout << "Start ThreadProc...." << endl;
	while (1)
	{	
		ref_count++;
		cout << "ref_count is " << ref_count.Value() << endl;
	}
	return 1;
}

int main()
{
	DWORD targetThreadID = 0;
	HANDLE hThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadProc,
		NULL,
		0,
		&targetThreadID
		);

	while (1)
	{
		ref_count--;
	}
	return 0;
}