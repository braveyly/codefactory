#include <iostream>
#include <bitset>
#include <string>

using namespace::std;


int main()
{
    bitset<10> a;

    unsigned long u = 0;
    bitset<12> b ( u );

    string s ( "11010100011101" );
    bitset<5> c ( s );
    bitset<5> d ( s, 3, 5 );

    cout << "The count of bitset a/b/c/d is "
         << a.count() << "/" << b.count()
         << "/" << c.count() << "/" << d.count()
         << endl;

    cout << "The size of bitset a/b/c/d is "
         << a.size() << "/" << b.size()
         << "/" << c.size() << "/" << d.size()
         << endl;

    cout << "The content of bitset a/b/c/d is "
         << a << "/" << b << "/" << c << "/" << d << endl;
}
