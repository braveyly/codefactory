#ifndef _PARENT_H_
#define _PARENT_H_
#include <iostream>
#include <string>
using namespace::std;

class PARENT
{
private:
    string company;
    int salary;
public:
    PARENT() : company ( "hust" ), salary ( 100 ) {};
	string get_company();
	virtual int get_nation();
};

#endif
