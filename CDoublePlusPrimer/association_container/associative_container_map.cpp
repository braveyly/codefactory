#include <iostream>
#include <string>
#include <map>

using namespace::std;

map<string,int> mp;

void traversal()
{
	cout<<"=====start to traversal====="<<endl;
	map<string,int>::iterator iter;
	for(iter = mp.begin();iter!=mp.end();iter++)
	{
		cout<<"first = "<<iter->first<<" second = "<<iter->second<<endl;
	}
}

int main()
{
	mp["liwei"] = 1;
	traversal();

	map<string,int>::value_type e("lijing",2);
	mp.insert(e);
	traversal();

	map<string, int>::size_type cnt = mp.count("liwei");
	cout<<"the number is "<<cnt<<" for liwei"<<endl;

	map<string, int>::iterator iter = mp.find("liwei");
	if(mp.end() != iter )
		cout<<"Find it first is "<<iter->first<<" second is "<<iter->second<<endl;

	cnt = mp.erase("liwei");
	cout<<"erase liwei return number is "<<cnt<<endl;
	traversal();
}
