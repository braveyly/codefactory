#include "concrete_decorator_b.h"

void CONCRETE_DECORATOR_B::operate()
{
    cout << "concrete_decorator_b::operator()" << endl;
    _cpt->operate();
    add_behavior_b();
}

void CONCRETE_DECORATOR_B::add_behavior_b()
{
    cout << "concrete_decoratora_b::add_behavior_b" << endl;
}