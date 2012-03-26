#include <iostream>
#include "single.h"

using namespace::std;
SINGLE* SINGLE::_instance = 0;//The 5ed highlight

SINGLE::SINGLE() : test_data ( 0 )
{
    cout << "single" << endl;
}

SINGLE* SINGLE::get_instance()
{
    if ( 0 == _instance )
    {
        _instance = new SINGLE();
    }

    return _instance;
}

void SINGLE::set_test_data ( int new_data )
{
    test_data = new_data;
}

void SINGLE::print_test_data()
{
    cout << "test_data = " << test_data << endl;
}
