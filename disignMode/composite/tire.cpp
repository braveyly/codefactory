#include "tire.h"

TIRE::TIRE(const char* name):EQUIPMENT(name)
{}

int TIRE::power()
{
	return 1;
}

int TIRE::net_price()
{
	return 2;
}

int TIRE::discount_price()
{
	return 3;
}