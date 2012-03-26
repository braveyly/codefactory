#include "child.h"
#include "parent.h"
#include <iostream>
#include <string>

CHILD::CHILD ( string new_name, int new_age )
{
    name = new_name;
    age = new_age;
}

string CHILD::get_parent_1_company ( PARENT_1& par )
{
    return par.company_1;
}

void CHILD::display_parent_2_info ( PARENT_2& par )
{
    cout << par.company_2 << par.salary_2 << endl;
}
