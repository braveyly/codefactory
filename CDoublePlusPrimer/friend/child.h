#ifndef _CHILD_H_
#define _CHILD_H_
#include <iostream>
#include <string>
using namespace::std;

class PARENT_1;
class PARENT_2;
class CHILD
{
private:
    string name;
    int age;
public:

    CHILD() : name ( "" ), age ( 0 ) {};
    CHILD ( string new_name, int new_age );

    string get_parent_1_company ( PARENT_1& par );
    void display_parent_2_info ( PARENT_2& par );
};

#endif
