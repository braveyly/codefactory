#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "critical_section_wrapper.h"
#include "condition_variable_wrapper.h"

using namespace::std;

cp::CriticalSectionWrapper* _cs;
cp::ConditionVariableWrapper* _cv;
bool cvFlag = false;



DWORD ThreadProc(LPVOID threadNo)
{
	cout << "Start ThreadProc...." << endl;
	while (1)
	{
	    _cs->Enter();
	    cout << "1-Enter critical section...." << endl;

		cout << "1-before cv wait...." << endl;
		while (false == cvFlag)
		_cv->SleepCS(*_cs, 5000);
		//////////consume one
		cvFlag = false;
		cout << "1-after cv wait...." << endl;

	    _cs->Leave();
	    cout << "1-Leave critical section...." << endl;
	}
	return 1;
}

int main()
{
	_cs = cp::CriticalSectionWrapper::CreateCriticalSection();
	_cv = cp::ConditionVariableWrapper::CreateConditionVariable();
	DWORD targetThreadID = 0;
	HANDLE hThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadProc,
		NULL,
		0,
		&targetThreadID
		);
	Sleep(2000);
	while (1)
	{
		cout << "2-before cs enter" << endl;
		_cs->Enter();
		cout << "2-Enter cs" << endl;

		cout << "2-before cv wait" << endl;
		_cv->Wake();
		cout << "2-after cv wait" << endl;
		cvFlag = true; //produce one

		_cs->Leave();
		cout << "2-Leave cs" << endl;
	}
	return (DWORD)1;
}