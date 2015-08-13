#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <stack>
#include <string>

using namespace::std;

int main()
{
    stack<int> sk1;

    deque<int> de;
    de.push_back ( 3 );
    de.push_back ( 6 );
    stack<int> sk2 ( de );

    vector<string> vec;
    vec.push_back ( "abc" );
    vec.push_back ( "def" );

    stack< string, vector<string> > sk3 ( vec );//make sure that blanks seperate <, string, vector<string>  and >

    if ( true == sk1.empty() )
        cout << "sk1 is null" << endl;
    else
        cout << "sk1 size is " << sk1.size() << endl;

    if ( true == sk2.empty() )
        cout << "sk2 is null" << endl;
    else
        cout << "sk2 size is " << sk2.size() << endl;

    if ( true == sk3.empty() )
        cout << "sk3 is null" << endl;
    else
        cout << "sk3 size is " << sk3.size() << endl;

    sk1.push ( 1 );
    sk1.push ( 2 );
    sk1.push ( 3 );
    cout << "sk1.top=" << sk1.top() << endl;//return stack top
    cout << "sk1.size=" << sk1.size() << endl;
    sk1.pop();//delete the stack top and return void
    cout << "sk1.size=" << sk1.size() << endl;
}
