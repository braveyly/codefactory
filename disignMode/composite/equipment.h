#ifndef _EQUIPMENT_H_
#define _EQUIPMENT_H_
#include "common_include.h"

class EQUIPMENT{
public:
	//virtual ~EQUIPMENT(){};
	const char* name(){return _name;};
	virtual int power(){return 0;};
	virtual int net_price(){return 0;};
	virtual int discount_price(){return 0;};
	virtual void add(EQUIPMENT*){};
	virtual void remove(EQUIPMENT*){};
	virtual list<EQUIPMENT*>::iterator CreateIterator(){};
protected:
	EQUIPMENT(const char*);
private:
	const char* _name;
};
#endif
