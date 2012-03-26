#include <iostream>
#include <cassert>
using namespace::std;

int main()
{
#ifndef NDEBUG
    cerr << "this is debug info" << endl;
#endif

    cout << "File name:" << __FILE__ << endl;
    cout << "Line number:" << __LINE__ << endl;
    cout << "Compile time:" << __TIME__ << endl;
    cout << "Compile date:" << __DATE__ << endl;

    assert ( 1 > 2 );

}
/*
g++ -DNDEBUG test_debug_macro.cpp -o debug
*/
/*
File name:test_debug_macro.cpp
Line number:12
Compile time:16:33:20
Compile date:Mar 21 2012
debug: test_debug_macro.cpp:16: int main(): Assertion `1>2' failed.
Aborted
*/