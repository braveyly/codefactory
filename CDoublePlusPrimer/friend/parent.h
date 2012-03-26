#ifndef _PARENT_H_
#define _PARENT_H_
#include <iostream>
#include <string>
#include "child.h"
using namespace::std;

class PARENT_1
{
    friend class CHILD;
    friend void show_parent ( PARENT_1& par );
private:
    string company_1;
    int salary_1;
public:
    PARENT_1() : company_1 ( "hust" ), salary_1 ( 100 ) {};
};

class PARENT_2
{
    friend void CHILD::display_parent_2_info ( PARENT_2& par );
private:
    string company_2;
    int salary_2;
public:
    PARENT_2() : company_2 ( "ccnu" ), salary_2 ( 99 ) {};
};
#endif
