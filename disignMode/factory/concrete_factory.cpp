#include <iostream>
#include "product.h"
#include "factory.h"
#include "concrete_factory.h"
#include "concrete_product.h"
PRODUCT* CONCRETE_FACTORY::create_product ( int product_id )
{
    PRODUCT* pprdct = NULL;
    if ( 1 == product_id )
    {
        pprdct = new PRODUCT_1();
    }
    else if( 2 == product_id )
    {
        pprdct = new PRODUCT_2();
    }
	else
	{
		pprdct = FACTORY::create_product(product_id);
	}
	
    return pprdct;
}