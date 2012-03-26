#include <iostream>

using namespace::std;

class Order{
	public:
		Order();
		template<typename U> inline  const U min(const U& a,const U& b)
		{
			cout<<a<<endl;
			cout<<b<<endl;
			if(a>b)
				return b;
			else if(a==b)
				return a;
			else
				return a;
		};
};

Order::Order()
{

}

int main()
{
	Order temp;
	cout<<temp.min(3,6)<<endl;
}
