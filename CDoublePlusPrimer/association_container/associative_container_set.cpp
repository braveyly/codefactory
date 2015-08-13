#include <iostream>
#include <string>
#include <set>

using namespace::std;

set<string> st;

void traversal()
{
	cout<<"=====start to traversal====="<<endl;
	set<string>::iterator iter;
	for(iter = st.begin();iter!=st.end();iter++)
	{
		cout<<"*iter = "<<*iter<<endl;
	}
}

int main()
{
	set<string>::value_type e("lijing");
	st.insert(e);
	
	e="liwei";
	st.insert(e);

	e="liwei2";
	st.insert(e);
	
	traversal();

	set<string>::size_type cnt = st.count("liwei");
	cout<<"the number is "<<cnt<<" for liwei"<<endl;

	set<string>::iterator iter = st.find("lijing");
	if(st.end() != iter )
		cout<<"Find it *iter is "<<*iter<<endl;

	cnt = st.erase("liwei");
	cout<<"erase liwei return number is "<<cnt<<endl;
	traversal();
}

