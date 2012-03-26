#ifndef _TIRE_H_
#define _TIRE_H_
#include "equipment.h"

class TIRE:public EQUIPMENT{
public:
	TIRE(const char*);
	virtual int power();
	virtual int net_price();
	virtual int discount_price();
};
#endif
