#ifndef _CONCRETE_FACTORY_H_
#define _CONCRETE_FACTORY_H_
#include "factory.h"
class PRODUCT;
class CONCRETE_FACTORY:public FACTORY{
	public:
    	PRODUCT* create_product ( int product_id );
};
#endif 
