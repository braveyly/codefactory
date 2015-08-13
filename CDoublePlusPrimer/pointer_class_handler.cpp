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

class Sales_item
{
public:
    Sales_item() : p ( 0 ), use ( new size_t ( 1 ) ) {}
    explicit Sales_item ( Base_Item& b ) : p ( b.clone() ), use ( new size_t ( 1 ) ) {}
    explicit Sales_item ( Base_Item* b ) : p ( b ), use ( new size_t ( 1 ) ) {}
    Sales_item ( const Sales_item &i ) : p ( i.p ), use ( i.use )
    {
        ++*use;
    }
    ~Sales_item()
    {
        decr_use();
    }
    Sales_item& operator= ( const Sales_item& );
    const Base_Item *operator->() const
    {
        if ( p ) return p;
        else throw logic_error ( "unbound Sales_item" );
    }
    const Base_Item &operator*() const
    {
        if ( p ) return *p;
        else throw logic_error ( "unbound Sales_item" );
    }

    void print_use()
    {
        cout << "*use is " << *use << endl;
    }
private:
    Base_Item* p;
    size_t *use;
    inline void decr_use()
    {
        if ( --*use == 0 )
        {
            cout << "use == 0" << endl;
            delete p;
            delete use;
        }
    }
};

Sales_item& Sales_item::operator= ( const Sales_item & rhs )
{
    ++*rhs.use;
    decr_use();
    p = rhs.p;
    use = rhs.use;
    return *this;
}

int main()
{
    Compat_Disc item ( 9 );
    Sales_item handler1 ( item );
    Sales_item handler2 ( handler1 );
    Sales_item handler3;
    Sales_item handler4 ( new Computer_Book ( 4 ) );
    handler3 = handler4;
    handler1.print_use();
    handler2.print_use();
    handler3.print_use();
    handler4->net_price();

    vector<Sales_item> vec;
    vec.push_back ( handler1 );
    vec.push_back ( handler4 );
    vector<Sales_item>::iterator iter = vec.begin();
    ( *iter )->net_price();
    iter++;
    ( *iter )->net_price();

}
