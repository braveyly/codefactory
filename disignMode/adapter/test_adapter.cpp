#include <iostream>
#include <string>
#include "adapter.h"

using namespace::std;

int main()
{
    ADAPTER adp;
    int a = 0, b = 0, c = 0, d = 0;
    adp.request ( &a, &b, &c, &d );//we use request interface instead of not_need_request
    cout << a << b << c << d << endl;
}
