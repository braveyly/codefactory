#ifndef _COMPOSITE_EQUIPMENT_H_
#define _COMPOSITE_EQUIPMENT_H_
#include "equipment.h"
class COMPOSITE_EQUIPMENT:public EQUIPMENT{
public:
	virtual int power();
	virtual int net_price();
	virtual int discount_price();
	virtual void add(EQUIPMENT*);
	virtual void remove(EQUIPMENT*);
	virtual list<EQUIPMENT*>::iterator CreateIterator(list<EQUIPMENT*>::iterator *);
protected:
	COMPOSITE_EQUIPMENT(const char* name):EQUIPMENT(name){};
private:
	list<EQUIPMENT*> _equipment;
};
#endif
