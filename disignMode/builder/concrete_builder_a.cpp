#include "concrete_builder_a.h"

CONCRETE_BUILDER_A::CONCRETE_BUILDER_A()
{
    _prct.part_a = "1_a_null";
    _prct.part_b = "1_b_null";
    _prct.part_c = "1_c_null";

}

void CONCRETE_BUILDER_A::build_part_a()
{
    cout << "CONCRETE_BUILDER_A::build_part_a()" << endl;
    _prct.part_a = "1_step+china_brand";
}

void CONCRETE_BUILDER_A::build_part_b()
{
    _prct.part_b = "1_step+china_brand";
}

void CONCRETE_BUILDER_A::build_part_c()
{
    _prct.part_c = "1_step+china_brand";
}

PRODUCT* CONCRETE_BUILDER_A::get_result()
{
    return &_prct;
}