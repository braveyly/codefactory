#include <iostream>
#include <string>
#include "concrete_prototype.h"
using namespace::std;

CONCRETE_PROTOTYPE::CONCRETE_PROTOTYPE(CONCRETE_PROTOTYPE& new_one)
{
	my_data = new_one.my_data;
}

PROTOTYPE* CONCRETE_PROTOTYPE::clone()
{
	return &CONCRETE_PROTOTYPE(*this);
}

void CONCRETE_PROTOTYPE::set_data(int data)
{
	my_data = data;
}

void CONCRETE_PROTOTYPE::print_data()
{
	cout<<my_data<<endl;
}