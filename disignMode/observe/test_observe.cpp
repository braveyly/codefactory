#include "DataSubject.h"
#include "TableObserve.h"
#include "ColumnObserve.h"
#include "CobObserve.h"



int main()
{
	Subject *psub = new DataSubject();
	TableObserve *ptable = new TableObserve();
	ptable->set_subject(psub);

	ColumnObserve* pclm = new ColumnObserve();
	pclm->set_subject(psub);

	CobObserve* pcob = new CobObserve();
	pcob->set_subject(psub);

	cout<<"Now modify data info in table view"<<endl;
	ptable->operate(3);
	
}
