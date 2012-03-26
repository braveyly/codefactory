#ifndef _CONCRETE_PROTOTYPE_H_
#define _CONCRETE_PROTOTYPE_H_

#include "prototype.h"

class CONCRETE_PROTOTYPE: public PROTOTYPE{
	public:
		CONCRETE_PROTOTYPE():my_data(0){};
		CONCRETE_PROTOTYPE(CONCRETE_PROTOTYPE& new_one);
		PROTOTYPE* clone();
		void set_data(int data);
		void print_data();
	private:
		int my_data;
};

#endif
