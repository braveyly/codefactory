#include <iostream>
#include <string>

using namespace::std;

class Sale
{
	public:
		Sale();
		void set_price(int);
		void print_price();
		friend Sale operator+(Sale& left, Sale& right);
		friend Sale operator-(Sale& left, Sale& right);
		friend Sale operator*(Sale& left, Sale& right);
		friend Sale operator/(Sale& left, Sale& right);
		friend inline bool operator==(Sale& left, Sale& right);
		friend inline bool operator!=(Sale& left, Sale& right);
		friend inline bool operator>(Sale& left, Sale& right);
		friend inline bool operator<(Sale& left, Sale& right);
	private:
		int price;
};

Sale::Sale():price(0)
{}

void Sale::set_price(int pr)
{
	price = pr;
}

void Sale::print_price()
{
	cout<<"price is "<<price<<endl;
}

Sale operator+(Sale& left, Sale& right)
{
	Sale ret(left);
	ret.price+=right.price;
	return ret;
}

Sale operator-(Sale& left, Sale& right)
{
	Sale ret(left);
	ret.price-=right.price;
	return ret;
}

Sale operator*(Sale& left, Sale& right)
{
	Sale ret(left);
	ret.price*=right.price;
	return ret;
}

Sale operator/(Sale& left, Sale& right)
{
	Sale ret(left);
	ret.price/=right.price;
	return ret;
}

inline bool operator==(Sale& left, Sale& right)
{
	return (left.price == right.price);
}

inline bool operator!=(Sale& left, Sale& right)
{
	return !(left == right);
}

inline bool operator>(Sale& left, Sale& right)
{
	return (left.price > right.price);
}

inline bool operator<(Sale& left, Sale& right)
{
	return (left.price < right.price);
}

int main()
{
	Sale sa1;
	Sale sa2;
	sa1.set_price(8);
	sa2.set_price(4);

	Sale tmp = sa1+sa2;
	tmp.print_price();

	tmp = sa1-sa2;
	tmp.print_price();

	tmp=sa1*sa2;
	tmp.print_price();

	tmp=sa1/sa2;
	tmp.print_price();

	if(sa1==sa2)
		cout<<"sa1==sa2"<<endl;
	
	if(sa1!=sa2)
		cout<<"sa1!=sa2"<<endl;

	if(sa1>sa2)
		cout<<"sa1>sa2"<<endl;
	
	if(sa1<sa2)
		cout<<"sa1<sa2"<<endl;
}

