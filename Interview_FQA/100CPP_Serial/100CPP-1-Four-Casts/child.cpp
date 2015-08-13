#include "child.h"
#include "parent.h"
#include <iostream>
#include <string>

CHILD::CHILD ( string new_name, int new_age )
{
    name = new_name;
    age = new_age;
}


int CHILD::get_age()
{
	return age;
}