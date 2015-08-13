#include <iostream>
#include <string>

using namespace::std;

class Sale
{
	public:
		Sale();
		Sale& operator = (const Sale&);
		Sale& operator += (const Sale&);
		void set_price(int);
		void print_price();
		friend Sale& operator+=(Sale& left, Sale& right);
	private:
		int price;
};

Sale::Sale():price(0)
{}

Sale& Sale::operator=(const Sale& right)
{
	price=right.price;
	return *this;
}

Sale& Sale::operator+=(const Sale& right)
{
	price+=right.price;
	return *this;
}

void Sale::set_price(int pr)
{
	price = pr;
}

void Sale::print_price()
{
	cout<<"price is "<<price<<endl;
}

Sale& operator+=(Sale& left, Sale& right)
{
	left.price += right.price;
	return left;
}

int main()
{
	Sale sa1;
	Sale sa2;
	Sale sa3;
	sa1.set_price(11);
	sa1.print_price();

	sa2.set_price(28);
	sa2.print_price();

	sa3.set_price(33);
	sa3.print_price();

#if 0
	//sa1.operator=(sa2);
	//sa1=sa2;
	sa1.operator=(sa2.operator=(sa3));
	//sa1=sa2=sa3;
	sa1.print_price();
#endif

	//sa1.operator+=(sa2);
	//sa1+=sa2;
	//sa1.operator+=(sa2.operator+=(sa3));	
	//sa1+=sa2+=sa3;
	//operator+=(sa1,sa2);
	//operator+=(operator+=(sa1,sa2),sa3);
	sa1+=sa2+=sa3;
	sa1.print_price();
}
