#include <iostream>
#include "child.h"
#include "parent.h"

void test_static_upstream_cast()
{
	CHILD *cld = new CHILD();
	PARENT* par = static_cast <PARENT*> (cld);
	
	if(NULL == par )
		cout<<"static up is NULL"<<endl;
	else
		cout<<par->get_company()<<endl;
}

void test_static_downstream_cast()
{
	PARENT* par;
	CHILD* cld;

	par = new PARENT();
	cld = static_cast<CHILD*> (par);
	
	if(NULL==cld)
		cout<<"static down is NULL"<<endl;
	else
		cout<<cld->get_age()<<endl;
}

void test_dynamic_upstream_cast()
{
	PARENT* par;
	CHILD* cld;

	cld = new CHILD();
	par = dynamic_cast<PARENT*> (cld);
	
	if(NULL==par)
		cout<<"dynamic up is NULL"<<endl;
	else
		cout<<par->get_company()<<endl;
}

void test_dynamic_downstream_cast()
{
	PARENT* par=new PARENT();
	CHILD* cld=NULL;

	cld = dynamic_cast <CHILD*> (par);
	
	if(NULL==cld)
		cout<<"dynamic down is NULL"<<endl;
	else
		;//cout<<cld->get_age()<<endl;
}

int main()
{
	cout<<"test_static_upstream_cast"<<endl;
	test_static_upstream_cast();

	cout<<"test_static_downstream_cast"<<endl;
	test_static_downstream_cast();

	cout<<"test_dynamic_upstream_cast"<<endl;
	test_dynamic_upstream_cast();

	cout<<"test_dynamic_downstream_cast"<<endl;
	test_dynamic_downstream_cast();

}
