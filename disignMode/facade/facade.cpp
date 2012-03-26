#include "facade.h"

FACADE::FACADE()
{
	_subsystem_a = new SUBSYSTEM_A();
	_subsystem_b = new SUBSYSTEM_B();
}

FACADE::~FACADE()
{
	delete _subsystem_a;
	delete _subsystem_b;
}
		
void FACADE::operate(int a)
{
   _subsystem_a->operate_a(a,0,0);
   _subsystem_b->operate_b(a,0,0,0,0);
}