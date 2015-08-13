#include <iostream>
#include <string>
#include <set>

using namespace::std;

multiset<string> mst;

void traversal()
{
    cout << "=====start to traversal=====" << endl;
    multiset<string>::iterator iter;
    for ( iter = mst.begin();iter != mst.end();iter++ )
    {
        cout << "*iter = " << *iter << endl;
    }
}

void search_method_one()
{
    cout << "first method to search" << endl;
    multiset<string, int>::size_type cnt = mst.count ( "lijing" );
    multiset<string, int>::iterator iter = mst.find ( "lijing" );
    for ( int i = 0; i < cnt; i++, iter++  )
        cout << "*iter is " << *iter << endl;
}

void search_method_two()
{
    cout << "second method to search" << endl;
    multiset<string, int>::iterator beg = mst.lower_bound ( "liwei" );
    multiset<string, int>::iterator end = mst.upper_bound ( "liwei" );
    while ( beg != end )
    {
        cout << "*beg is " << *beg << endl;
        beg++;
    }
}

void search_method_three()
{
    cout << "three method to search" << endl;
    pair<multiset<string, int>::iterator, multiset<string, int>::iterator> pos = mst.equal_range ( "liwei" );
    while ( pos.first != pos.second )
    {
        cout << "*(pos.first) " << * ( pos.first ) << endl;
        pos.first++;
    }
}

int main()
{
    multiset<string>::value_type e1 ( "lijing" );
    mst.insert ( e1 );
    e1 = "lijing";
    mst.insert ( e1 );

    multiset<string>::value_type e2 ( "liwei" );
    mst.insert ( e2 );
    e2 = "liwei";
    mst.insert ( e2 );
    traversal();

    search_method_one();
    search_method_two();
    search_method_three();

}




