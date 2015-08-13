#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <list>
using namespace::std;

void test_find_1()
{
	vector<int> vec;
	vec.push_back(8);
	vec.push_back(21);
	vec.push_back(81);
	vec.push_back(32);

	vector<int>::const_iterator result;
	result = find(vec.begin(),vec.end(),81);
	if( result == vec.end() )
		cout<<"donot find"<<endl;
	else
		cout<<"find"<<endl;
}

void test_find_2()
{
	int a[5] = { 8,2,32,11,65};
	int* p = find(a,a+3,2);
	if( p == a+3 )
		cout<<"donot find"<<endl;
	else
		cout<<"find"<<endl;
}

void test_accumulate()
{
	vector<int> vec;
	vec.push_back(8);
	vec.push_back(21);
	vec.push_back(81);
	vec.push_back(32);

	int total = accumulate(vec.begin(),vec.end(),4);
	cout<<"total is "<<total<<endl;
}

void test_find_first_of()
{
	vector<int> vec1;
	vec1.push_back(8);
	vec1.push_back(21);
	vec1.push_back(81);
	vec1.push_back(32);	

	vector<int> vec2;
	vec2.push_back(1);
	vec2.push_back(8);
	vec2.push_back(2);
	vec2.push_back(81);

	vector<int>::iterator iter = vec1.begin();
	while( ( iter = find_first_of(iter,vec1.end(),vec2.begin(),vec2.end()) ) != vec1.end() )
	{
		cout<<"Find the same element " << *iter << " in vec1 with vec2" <<endl;
		iter++;
	}
}

void test_fill()
{
	vector<int> vec;
	vec.push_back(8);
	vec.push_back(21);
	vec.push_back(81);
	vec.push_back(32);	

	fill(vec.begin(),vec.end(),10);
	vector<int>::iterator iter;
	for( iter = vec.begin(); iter != vec.end(); iter++ )
		cout<<*iter<<endl;
}

void test_fill_n()
{
	vector<int> vec;
	//fill_n(vec.begin(),10,2);//segment error
	fill_n(back_inserter(vec),10,2);
	vector<int>::iterator iter;
	for( iter = vec.begin(); iter != vec.end(); iter++ )
		cout<<*iter<<endl;	
}

void test_copy()
{
	vector<int> vec1;
	vec1.push_back(8);
	vec1.push_back(21);
	vec1.push_back(81);
	vec1.push_back(32);	
	vector<int> vec2;

	copy(vec1.begin(),vec1.end(), back_inserter(vec2));

	vector<int>::iterator iter;
	for( iter = vec2.begin(); iter != vec2.end(); iter++ )
		cout<<*iter<<endl;	
}

void test_replace()
{
	vector<int> vec;
	vec.push_back(8);
	vec.push_back(21);
	vec.push_back(81);
	vec.push_back(32);

	replace(vec.begin(),vec.end(),8,9);
	vector<int>::iterator iter;
	for( iter = vec.begin(); iter != vec.end(); iter++ )
		cout<<*iter<<endl;	
}

void test_replace_copy()
{
	vector<int> vec;
	vec.push_back(8);
	vec.push_back(21);
	vec.push_back(81);
	vec.push_back(32);

	list<int> ls;

	replace_copy(vec.begin(),vec.end(),back_inserter(ls), 81,101 );

	vector<int>::iterator iter1;
	for( iter1 = vec.begin(); iter1 != vec.end(); iter1++ )
		cout<<*iter1<<endl;

	list<int>::iterator iter2;
	for( iter2 = ls.begin(); iter2 != ls.end(); iter2++ )
		cout<<*iter2<<endl;	
}


void test_sort()
{
	vector<string> vec;
	vec.push_back("apple");
	vec.push_back("banana");
	vec.push_back("orange");
	vec.push_back("apple");

	vector<string>::iterator iter;
	cout<<"before sort"<<endl;
	for( iter = vec.begin(); iter!=vec.end();iter++)
		cout<<*iter<<endl;

	sort(vec.begin(),vec.end());
	cout<<"after sort"<<endl;
	for( iter = vec.begin(); iter!=vec.end();iter++)
		cout<<*iter<<endl;	

	vector<string>::iterator end_unique = unique(vec.begin(), vec.end());
	cout<<"after unique"<<endl;
	for( iter = vec.begin(); iter!=vec.end();iter++)
		cout<<*iter<<endl;	

	vec.erase(end_unique, vec.end());
	cout<<"after unique"<<endl;
	for( iter = vec.begin(); iter!=vec.end();iter++)
		cout<<*iter<<endl;	
}

bool is_shorter(const string& str1, const string& str2)
{
	return str1.size() < str2.size();
}

void test_stable_sort()
{
	vector<string> vec;
	vec.push_back("apple");
	vec.push_back("banana");
	vec.push_back("man");
	vec.push_back("appriciation");

	stable_sort(vec.begin(),vec.end(),is_shorter);
	vector<string>::iterator iter;
	for( iter = vec.begin(); iter!=vec.end();iter++)
		cout<<*iter<<endl;
}

bool isGT6(const string& str)
{
	return str.size()>=6;
}

void test_count_if()
{
	vector<string> vec;
	vec.push_back("apple");
	vec.push_back("banana");
	vec.push_back("man");
	vec.push_back("appriciation");

	int total = count_if(vec.begin(),vec.end(),isGT6);
	cout<<"total is "<<total<<endl;
}

int main()
{
	cout<<"\n\n====test_find_1======"<<endl;
	test_find_1();
	
	cout<<"\n\n====test_find_2======"<<endl;
	test_find_2();

	cout<<"\n\n====test_accumulate======"<<endl;
	test_accumulate();

	cout<<"\n\n====test_find_first_of======"<<endl;
	test_find_first_of();

	cout<<"\n\n====test_fill======"<<endl;
	test_fill();

	cout<<"\n\n====test_fill_n======"<<endl;
	test_fill_n();

	cout<<"\n\n====test_copy======"<<endl;
	test_copy();

	cout<<"\n\n====test_replace======"<<endl;
	test_replace();

	cout<<"\n\n====test_replace_copy======"<<endl;
	test_replace_copy();

	cout<<"\n\n====test_sort======"<<endl;
	test_sort();

	cout<<"\n\n====test_stable_sort======"<<endl;
	test_stable_sort();

	cout<<"\n\n====test_count_if======"<<endl;
	test_count_if();
}
