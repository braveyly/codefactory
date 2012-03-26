#ifndef _FACADE_H_
#define _FACADE_H_
#include <iostream>
#include "subsystem_a.h"
#include "subsystem_b.h"
using namespace::std;

class FACADE{
	public:
		FACADE();
		~FACADE();
		void operate(int a);
	private:
		SUBSYSTEM_A* _subsystem_a;
		SUBSYSTEM_B* _subsystem_b;
};
#endif
