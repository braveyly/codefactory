#include <iostream>
#include <vector>

using namespace::std;

int main()
{
    /*Basic test*/
    vector<string> vec ( 4, "china" );
    int i;
    cout << "size=" << vec.size() << "empty=" << vec.empty() << endl;
    vec.push_back ( "vs" );
    vec.push_back ( "Japan" );

    for ( i = 0;i != vec.size();i++ )
        cout << "vec" << "[" << i << "]=" << vec[i] << endl;

    /*Iterator test*/
    vector<string>::iterator iter;
    for ( iter = vec.begin();iter != vec.end();iter++ )
        cout << "iter1=" << *iter << endl;

    iter = vec.begin();
    *iter = "good";

    * ( iter + 2 ) = "bad";

    for ( iter = vec.begin();iter != vec.end();iter++ )
        cout << "iter2=" << *iter << endl;
}
