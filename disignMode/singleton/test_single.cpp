#include <iostream>
#include "single.h"

int main()
{
    //SINGLE* p = new SINGLE();/*single.h:7: error: ‘SINGLE::SINGLE()’ is private*/
    SINGLE* p = SINGLE::get_instance();
    p->set_test_data ( 99 );
    p->print_test_data();
    SINGLE* q = SINGLE::get_instance();
    q->set_test_data ( 101 );
    p->print_test_data();
}
/*
[nick@d02 singleton]$ ./single
single
test_data = 99
test_data = 101
*/
