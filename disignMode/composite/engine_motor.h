#ifndef _ENGINE_MOTOR_H_
#define _ENGINE_MOTOR_H_
#include "composite_equipment.h"
class ENGINE_MOTOR:public COMPOSITE_EQUIPMENT{
public:
	ENGINE_MOTOR(const char* name):COMPOSITE_EQUIPMENT(name){};
	virtual int power();
	virtual int net_price();
	virtual int discount_price();
};
#endif

