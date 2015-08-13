#include <iostream>
#include <string>
#include <map>

using namespace::std;

multimap<string,int> mmp;

void traversal()
{
	cout<<"=====start to traversal====="<<endl;
	multimap<string,int>::iterator iter;
	for(iter = mmp.begin();iter!= mmp.end();iter++)
	{
		cout<<"first = "<<iter->first<<" second = "<<iter->second<<endl;
	}
}

void search_method_one()
{
	cout<<"first method to search"<<endl;
	multimap<string, int>::size_type cnt = mmp.count("lijing");
	multimap<string, int>::iterator iter = mmp.find("lijing");
	for( int i = 0; i < cnt; i++, iter++  )
		cout<<"first is "<<iter->first<<" second is "<<iter->second<<endl;
}

void search_method_two()
{
	cout<<"second method to search"<<endl;
	multimap<string,int>::iterator beg = mmp.lower_bound("liwei");
	multimap<string,int>::iterator end = mmp.upper_bound("liwei");
	while(beg!=end)
	{
		cout<<"beg->first "<<beg->first<<" beg->second "<<beg->second<<endl;
		beg++;
	}
}

void search_method_three()
{
	cout<<"three method to search"<<endl;
	pair<multimap<string,int>::iterator,multimap<string,int>::iterator> pos = mmp.equal_range("liwei");
	while(pos.first!=pos.second)
	{
		cout<<"pos.first->first "<<pos.first->first<<" pos.first->second "<<pos.first->second<<endl;
		pos.first++;
	}
}

int main()
{
	multimap<string,int>::value_type e1("lijing",2);
	mmp.insert(e1);

	//e1.first="lijing"//error because first is const
	e1.second=3;
	mmp.insert(e1);

	multimap<string,int>::value_type e2("liwei",20);
	mmp.insert(e2);
	e2.second=5;
	mmp.insert(e2);
	e2.second=8;
	mmp.insert(e2);
	traversal();
	
	search_method_one();
	search_method_two();
	search_method_three();
}



