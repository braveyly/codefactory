#include "intelligent_vehicle.h"

int INTELLIGENT_VEHICLE::power()
{
	return COMPOSITE_EQUIPMENT::power();
}

int INTELLIGENT_VEHICLE::net_price()
{
	int net_price = COMPOSITE_EQUIPMENT::net_price();
	net_price+=1;
	return net_price;
}

int INTELLIGENT_VEHICLE::discount_price()
{
	int discount_price = COMPOSITE_EQUIPMENT::discount_price();
	discount_price+=1;
	return discount_price;
}