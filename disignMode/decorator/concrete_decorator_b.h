#ifndef _CONCRETE_DECORATOR_B_H_
#define _CONCRETE_DECORATOR_B_H_
#include "decorator.h"

class CONCRETE_DECORATOR_B: public DECORATOR
{
public:
    CONCRETE_DECORATOR_B ( COMPONENT* cpt ) : DECORATOR ( cpt ) {};
    void operate();
protected:
    void add_behavior_b();
};
#endif

