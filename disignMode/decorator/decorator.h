#ifndef _DECORATOR_H_
#define _DECORATOR_H_
#include "component.h"
class DECORATOR:public COMPONENT{
	public:
		DECORATOR(COMPONENT* cpt){_cpt = cpt;};
		virtual void operate(){};
	protected:
		COMPONENT* _cpt;
};
#endif
