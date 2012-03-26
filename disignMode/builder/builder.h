#ifndef _BUILDER_H_
#define _BUILDER_H_
#include <iostream>
#include <string>
#include "product.h"
using namespace::std;
class BUILDER
{
public:
    virtual void build_part_a() = 0;
    virtual void build_part_b() = 0;
    virtual void build_part_c() = 0;
    virtual PRODUCT* get_result() = 0;
protected:
    PRODUCT _prct;
};
#endif