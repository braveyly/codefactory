#ifndef _INTELLIGENT_VEHICLE_
#define _INTELLIGENT_VEHICLE_
#include "composite_equipment.h"
class INTELLIGENT_VEHICLE:public COMPOSITE_EQUIPMENT{
public:
	INTELLIGENT_VEHICLE(const char* name):COMPOSITE_EQUIPMENT(name){};
	virtual int power();
	virtual int net_price();
	virtual int discount_price();
};
#endif

