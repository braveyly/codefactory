#include "ColleageA.h"
#include "ColleageB.h"
#include "ConcreteMediator.h"

int main()
{
    ConcreteMediator* pmdt = new ConcreteMediator();
    ColleageA* pa = new ColleageA ( pmdt );
    ColleageB* pb = new ColleageB ( pmdt );
    pmdt->connect_colleage ( pa, pb );

    pa->set_state ( "lw-a" );
    pb->set_state ( "lw-b" );
    cout << "a state" << pa->get_state() << endl;
    cout << "b state" << pb->get_state() << endl;

    pa->action();


}
