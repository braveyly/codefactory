#include <iostream>
#include "factory.h"
#include "product.h"

using namespace::std;

PRODUCT* FACTORY::create_product ( int product_id )
{
	return new PRODUCT();
}