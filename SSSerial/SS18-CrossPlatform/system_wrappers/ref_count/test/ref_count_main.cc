#include "ref_count.h"
#include <iostream>
using namespace::std;

class Printer
{
public:
	static int create(int idx);

	Printer(int idx);
	~Printer();
	int midx;

};

int Printer::create(int idx)
{
	

	return 1;
}

Printer::Printer(int idx)
{
	midx = idx;
	cout << "Printer is created, idx = " << midx << endl;
}

Printer::~Printer()
{
	cout << "Printer is destroyed, idx = " << midx << endl;
}


int main()
{
	cp::RefCountImpl<Printer>* printerDevice = new RefCountImpl<Printer>(idx);
	//Printer* printer = Printer::create(1);
	return 1;
}