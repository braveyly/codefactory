#include "ColleageA.h"
#include "ColleageB.h"

int main()
{
	ColleageA* pca = new ColleageA();
	ColleageB* pcb = new ColleageB();
	pca->set_state("lw-a");
	pcb->set_state("lw-b");
	
	pca->set_colleage(pcb);
	pcb->set_colleage(pca);

	pca->action();
}
