#include "Memento.h"
#include "Originator.h"


int main()
{
	Originator* porig = new Originator();
	porig->set_state(8);
	porig->print_state();

	Memento* pmm = porig->create_memento();
	porig->set_state(10);
	porig->print_state();

	porig->restore_to_memento(pmm);
	porig->print_state();
	
	
	
}
