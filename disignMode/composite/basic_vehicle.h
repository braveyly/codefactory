#ifndef _BASIC_VEHICLE_
#define _BASIC_VEHICLE_
#include "composite_equipment.h"

class BASIC_VEHICLE:public COMPOSITE_EQUIPMENT{
public:
	BASIC_VEHICLE(const char* name):COMPOSITE_EQUIPMENT(name){};
	/*the power, net_price and discount_price will adopt the COMPOSITE_EQUIPMENT method
	because basic vehicle need not extra installing fee*/
	/*However we have to spend some charge for advertise*/
	int advertise_fee();
	
};
#endif
