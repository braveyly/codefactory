#include "product.h"
#include "builder.h"
#include "director.h"
#include "concrete_builder_a.h"
#include "concrete_builder_b.h"

int main()
{
    CONCRETE_BUILDER_B bld_a;
    DIRECTOR dtct ( &bld_a );
    dtct.construct();
    PRODUCT* ppcrt = bld_a.get_result();
    cout << ppcrt->part_a << ppcrt->part_b << ppcrt->part_c << endl;
}
