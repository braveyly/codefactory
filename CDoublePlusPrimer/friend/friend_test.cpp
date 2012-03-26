#include <iostream>
#include "child.h"
#include "parent.h"
void show_parent(PARENT_1& par)
{
	cout<<par.company_1<<par.salary_1<<endl;
}

int main()
{
CHILD cld1;
PARENT_1 par1;
PARENT_2 par2;
cld1.display_parent_2_info(par2);
cout<<cld1.get_parent_1_company(par1)<<endl;
show_parent(par1);
}
