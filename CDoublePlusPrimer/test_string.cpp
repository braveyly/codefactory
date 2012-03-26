#include <iostream>
#include <string>

using namespace::std;

int main()
{
    string s1 ( "Hello world!" );
    string s2 ( 4, 'C' );
	string s3 = s1;
	string s4="bad";
	//s4 = "good";
	cout<<"s4="<<s4<<endl;
    bool b;
    string::size_type l = s1.size();
    b = s1.empty();
    cout << "s1=" << s1 << "s2=" << s2 << "s3=" << s3 << "s1[1]" << s1[1] << endl;
    cout << "length=" << l << "bool=" << b << endl;
	if(s1==s3)
		cout << s1 << "==" << s3<<endl;
}
/*
[nick@d02 c++primer]$ ./test_string
s4=bad
s1=Hello world!s2=CCCCs3=Hello world!s1[1]e
length=12bool=0
Hello world!==Hello world!
*/