#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <string>

using namespace::std;

int main()
{
    queue<int> qu;
    qu.push ( 1 );
    cout << "qu.size=" << qu.size() << endl;
    qu.push ( 2 );
    qu.push ( 3 );
    cout << "qu.front=" << qu.front() << endl;//get the begin of queue
    cout << "qu.back=" << qu.back() << endl;//get the end of queue
    qu.pop();//delete the begin of queue
    cout << "qu.front=" << qu.front() << endl;
    cout << "qu.back=" << qu.back() << endl;
}