#include "concrete_builder_b.h"

CONCRETE_BUILDER_B::CONCRETE_BUILDER_B()
{
    _prct.part_a = "2_a_null";
    _prct.part_b = "2_b_null";
    _prct.part_c = "2_c_null";

}
void CONCRETE_BUILDER_B::build_part_a()
{
    cout << "CONCRETE_BUILDER_B::build_part_a()" << endl;

    _prct.part_a = "1_step+us_brand";
}

void CONCRETE_BUILDER_B::build_part_b()
{
    _prct.part_b = "1_step+us_brand";
}

void CONCRETE_BUILDER_B::build_part_c()
{
    _prct.part_c = "1_step+us_brand";
}

PRODUCT* CONCRETE_BUILDER_B::get_result()
{
    return &_prct;
}
