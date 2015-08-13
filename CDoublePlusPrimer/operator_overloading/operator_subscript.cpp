#include <iostream>
#include <vector>

using namespace::std;


class Foo
{
	public:
		Foo();
		int& operator[](const size_t);
		const int& operator[](const size_t)const;
	private:
		vector<int> data;
};

Foo::Foo()
{
	data.push_back(8);
	data.push_back(11);
	data.push_back(20);
	
}

int& Foo::operator[](const size_t index)
{
	cout<<"no const []"<<endl;
	return data[index];
}

const int& Foo::operator[](const size_t index)const
{
	cout<<"const []"<<endl;
	return data[index];
}

int main()
{
	const Foo fo1;
	cout<<fo1[0]<<endl;

	Foo fo2;
	fo2[0]=fo1[1];
	cout<<fo2[0]<<endl;
}