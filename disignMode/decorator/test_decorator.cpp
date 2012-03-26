#include "concrete_component.h"
#include "concrete_decorator_a.h"
#include "concrete_decorator_b.h"

int main()
{
    CONCRETE_COMPONENT cpt;
    CONCRETE_DECORATOR_A dec_a ( &cpt );
    CONCRETE_DECORATOR_B dec_b ( &cpt );
    dec_a.operate();
    dec_b.operate();
}
