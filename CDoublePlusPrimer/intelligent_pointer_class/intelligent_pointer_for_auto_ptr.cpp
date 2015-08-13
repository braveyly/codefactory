#include <iostream>
#include <stdexcept>
#include <vector>

using namespace::std;

class Base_Item
{
public:
    Base_Item() {};
    virtual void net_price() const {}
    virtual Base_Item* clone() const {}
    virtual ~Base_Item()
    {
        cout << "~Base_Item()" << endl;
    };
};

class Compat_Disc: public Base_Item
{
public:
    Compat_Disc ( int pc ) : price ( pc ), Base_Item() {};
    void net_price() const;
    Compat_Disc* clone() const
    {
        return new Compat_Disc ( *this );
    }
    ~Compat_Disc()
    {
        cout << "~Compat_Disc()" << endl;
    };
private:
    int price;
};

void Compat_Disc::net_price() const
{
    cout << "CD discount price is " << price*4 / 5 << endl;
}

class Computer_Book: public Base_Item
{
public:
    Computer_Book ( int pc ) : price ( pc ), Base_Item() {};
    void net_price() const;
    Computer_Book* clone() const
    {
        return new Computer_Book ( *this );
    }
    ~Computer_Book()
    {
        cout << "~Computer_Book()" << endl;
    };
private:
    int price;
};

void Computer_Book::net_price() const
{
    cout << "Computer Book discount price is " << price / 2 << endl;
}

void test_auto_ptr()
{
 	auto_ptr<Base_Item> pBI1(new Compat_Disc(9));
	pBI1->net_price();

	auto_ptr<Base_Item> pBI2(pBI1); //pBI1 is null
	pBI1->net_price();  //seg error
	pBI2->net_price();
}

#if 0
void test_shared_ptr()
{
	trl::shared_ptr<Base_Item> pSP1(new Compat_Disc(9));
	pSP1->net_price();

	trl::shared_ptr<Base_Item> pSP2(pSP1); //pSP1 and pSP2 are the same
	pSP1->net_price();  
	pSP2->net_price();	
}
#endif

int main()
{
	test_auto_ptr();
}

