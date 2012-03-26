#ifndef _STEER_WHEEL_H_
#define _STEER_WHEEL_H_
#include "equipment.h"

class STEER_WHEEL:public EQUIPMENT{
public:
	STEER_WHEEL(const char* name):EQUIPMENT(name){};
	virtual int power();
	virtual int net_price();
	virtual int discount_price();
};
#endif

