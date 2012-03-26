#ifndef _NAVIGATOR_H_
#define _NAVIGATOR_H_
#include "composite_equipment.h"
class NAVIGATOR:public COMPOSITE_EQUIPMENT{
public:
	NAVIGATOR(const char* name):COMPOSITE_EQUIPMENT(name){};
	virtual int power();
	virtual int net_price();
	virtual int discount_price();
};
#endif
