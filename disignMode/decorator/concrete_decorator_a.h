#ifndef _CONCRETE_DECORATOR_A_H_
#define _CONCRETE_DECORATOR_A_H_
#include "decorator.h"
class CONCRETE_DECORATOR_A: public DECORATOR
{
public:
    CONCRETE_DECORATOR_A ( COMPONENT* cpt ) : DECORATOR ( cpt ) {};
    void operate();
protected:
    void add_behavior_a();
};
#endif
