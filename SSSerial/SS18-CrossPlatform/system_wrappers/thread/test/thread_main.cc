#include "thread_wrapper.h"
#include <iostream>

using namespace::std;

bool Run(void* ptr)
{
	cout << "in thread...." << endl;
	return true;
}

int main()
{
	cp::ThreadWrapper* _thread;

	_thread = cp::ThreadWrapper::CreateThread(Run, NULL , cp::kNormalPriority,
		"ThreadTest thread");


	if (_thread)
	{
		unsigned int id;
		_thread->Start(id);
	}


	if (_thread)
	{
		_thread->SetNotAlive();
		if (_thread->Stop())
		{
			delete _thread;
			_thread = NULL;
		}
	}
}