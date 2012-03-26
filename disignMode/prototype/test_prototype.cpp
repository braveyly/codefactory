#include <iostream>
#include <string>
#include "concrete_prototype.h"
using namespace::std;

int main()
{
    CONCRETE_PROTOTYPE instance;
    instance.set_data ( 8 );
    instance.print_data();
    PROTOTYPE* p = instance.clone();
    p->set_data ( 9 );
    p->print_data();
    instance.print_data();
}
