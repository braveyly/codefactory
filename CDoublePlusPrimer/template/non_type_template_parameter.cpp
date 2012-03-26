#include <iostream>
#include <string>

using namespace::std;

template<typename T, size_t N> void init_array(T (&parm)[N])
{
   for(size_t i = 0; i!=N; i++)
   	{
   		parm[i] = 1;
   	}
}

int main()
{
   int a[100];
   double b[100];
   init_array(a);
   init_array(b);
}
