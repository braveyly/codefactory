#include "concrete_decorator_a.h"

void CONCRETE_DECORATOR_A::operate()
{
    cout << "concrete_decorator_a::operator()" << endl;
    _cpt->operate();
    add_behavior_a();
}

void CONCRETE_DECORATOR_A::add_behavior_a()
{
    cout << "concrete_decoratora_a::add_behavior_a" << endl;
}