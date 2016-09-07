#include "rw_lock_wrapper.h"
#include <stdio.h>
cp::RWLockWrapper *_queueRWLock;//for none_raii

void test_readlock_no_raii()
{
	_queueRWLock->AcquireLockShared();
	//do something,ie:check list whether is NULL
	printf("test readlock no raii\n");
	_queueRWLock->ReleaseLockShared();
}

void test_writelock_no_raii()
{
	_queueRWLock->AcquireLockExclusive();
	//do something, ie:push or pull list
	printf("test writelock no raii\n");
	_queueRWLock->ReleaseLockExclusive();
}

void test_readlock_raii()
{
	cp::ReadLockScoped lockNetEq(*_queueRWLock);
	//do something,ie:check list whether is NULL
	printf("test readlock raii\n");
}

void test_writelock_raii()
{
	cp::WriteLockScoped lockNetEq(*_queueRWLock);
	//do something,ie:check list whether is NULL
	printf("test writelock raii\n");
}

int main()
{
	_queueRWLock = cp::RWLockWrapper::CreateRWLock();
	//for none raii
	test_readlock_no_raii();
	test_writelock_no_raii();
	//for raii
	test_readlock_raii();
	test_writelock_raii();
	if (0 != _queueRWLock)
	{
		delete _queueRWLock;
	}
	while (1)
	{
	}
}