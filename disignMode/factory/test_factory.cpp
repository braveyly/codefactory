#include "product.h"
#include "factory.h"
#include "concrete_factory.h"

int main()
{
    CONCRETE_FACTORY fac;
    PRODUCT* pproc;
    pproc = fac.create_product ( 1 );
    pproc->operate();
	delete pproc;
	pproc =0;
}
