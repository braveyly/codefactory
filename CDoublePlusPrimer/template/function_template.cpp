#include <iostream>
using namespace::std;
/*const parameter allow const variable*/
template <typename T> int compare( const T& a, const T&b)
{
	if(a>b)
		return 1;
	else if(a==b)
		return 0;
	else
		return -1;
}

int main()
{
   int a = 3;
   int b =11;
   cout<<compare(1,2)<<endl;/*If no const, it failed to compile*/
   cout<<compare(3.2,1.1)<<endl;
   cout<<compare(a,b)<<endl;

}
