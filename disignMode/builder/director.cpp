#include "builder.h"
#include "director.h"

DIRECTOR::DIRECTOR ( BUILDER* bld )
{
    cout << "DIRECTOR()" << endl;
    _bld = bld;
}

void DIRECTOR::construct()
{
    cout << "construct()" << endl;
    _bld->build_part_a();
    _bld->build_part_b();
    _bld->build_part_c();
}

