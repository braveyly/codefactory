#include <stdio.h>
#include "critical_section_wrapper.h"

cp::CriticalSectionWrapper* _csNetQueue;

void test_cs_none_raii()
{
	_csNetQueue->Enter();
	//do something without raii
	printf("test cs with none raii\n");
	_csNetQueue->Leave();
}

void test_cs_raii()
{
	cp::CriticalSectionScoped csScoped(_csNetQueue);
	//do something with raii
	printf("test cs raii\n");
}

int main()
{
	_csNetQueue = cp::CriticalSectionWrapper::CreateCriticalSection();
	test_cs_none_raii();
	test_cs_raii();
	if (0 != _csNetQueue)
	{
		delete _csNetQueue;
	}
	while (1)
	{
	}
}