#ifndef _CHILD_H_
#define _CHILD_H_
#include <iostream>
#include <string>
#include "parent.h"
using namespace::std;

class CHILD:public PARENT
{
private:
    string name;
    int age;
public:

    CHILD() : name ( "" ), age ( 0 ) {};
    CHILD ( string new_name, int new_age );

	int get_age();
};

#endif
