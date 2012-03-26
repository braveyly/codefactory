#include "ConcreteVisitor1.h"
#include "ConcreteVisitor2.h"
#include "ElementA.h"
#include "ElementB.h"


int main()
{
	ConcreteVisitor1* pvis1 = new ConcreteVisitor1();
	ElementA* pelea = new ElementA();
	pelea->accept(pvis1);
}
