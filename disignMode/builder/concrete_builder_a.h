#ifndef _CONCRETE_BUILDER_A_H_
#define _CONCRETE_BUILDER_A_A_
#include <iostream>
#include <string>
#include "builder.h"
using namespace::std;

class CONCRETE_BUILDER_A : public BUILDER
{
public:
    CONCRETE_BUILDER_A();
    void build_part_a();
    void build_part_b();
    void build_part_c();
    PRODUCT* get_result();
private:
    PRODUCT _prct;
};
#endif
