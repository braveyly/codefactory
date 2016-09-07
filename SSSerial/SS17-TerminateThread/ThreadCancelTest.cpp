#include <windows.h>
#include <iostream>

using namespace::std;
DWORD saveImageFunc(LPVOID threadNo)
{
	int i = 0;
	int result = 0;
	while(1)
	{
		puts("saveImageFunc is doing");
		Sleep(1000);
	}
	return 0;
}

int main()
{
    HANDLE saveimage_thread_handle;
    DWORD saveimageThreadID = 0;
    DWORD dwExitCode = 0;
	 
	saveimage_thread_handle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)saveImageFunc,
		NULL,
		0,
		&saveimageThreadID
		);
	int n = 10;
	while(n)
	{
		n--;
		Sleep(1000);
	}
	puts("before terminate thread");
    TerminateThread(saveimage_thread_handle, dwExitCode);
	puts("after terminate thread");
	DWORD rtType = WaitForSingleObject(saveimage_thread_handle, 5000);
	puts("after WaitForSingleObject");
	if (WAIT_TIMEOUT == rtType)
	{
		puts("bufferEvent wait timeout");
		return -1;
	}
	while(1)
	{
		Sleep(1000);
	}
}