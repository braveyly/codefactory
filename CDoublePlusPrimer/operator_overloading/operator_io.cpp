#include <iostream>
#include <string>

using namespace::std;

class Sale
{
	public:
		Sale();
		void set_price(int);
		friend ostream& operator<<(ostream& out, const Sale& sale);
		friend istream& operator>>(istream& in, Sale& sale);
	private:
		int price;
};

Sale::Sale():price(0)
{}

void Sale::set_price(int pr)
{
	price = pr;
}


ostream& operator<<(ostream& out, const Sale& sale)
{
	out<<sale.price;
	return out;
}

istream& operator>>(istream& in, Sale& sale)
{
	in>>sale.price;
	return in;
}

int main()
{
	Sale sa1;
	sa1.set_price(22);
	cout<<sa1<<endl;
	operator<<(cout,sa1)<<endl;	
	cin>>sa1;
	cout<<sa1<<endl;
}

