#include <string>
#include <iostream>

using namespace::std;

void test_substr_exist()
{
    string str="email:sjliwei2002@126.com";
    int pos1=-1;
    string str0("liwei");

    pos1=str.find(str0,0);

    if(pos1==-1)
        cout<<"\'liwei\' is not in "<<str<<endl ;
    else
        cout<<"\'liwei\' is in "<<str<<endl ;
}
/********************************************************
Function:
	string.find
Description:
	search one string in another string
*********************************************************/


void get_substr()
{

    string str="email:sjliwei2002@126.com";
    int pos1=-1,pos2=-1;
    string str0("email:");
    string str1("@");
    string str2;

    pos1=str.find(str0,0);
    pos1+=str0.size();
    pos2=str.find(str1,0);
    str2=str.substr(pos1,pos2-pos1);
    cout<<"the substr is "<<str2<<endl;
}
/**********************************************************
Function:
	string.substr
Description:
	get one sub string from another string
*************************************************************/


int string2int(string str)
{
    int b=atoi(str.c_str());
    return b;
}

string int2string(int a)
{
    char temp[64];
    sprintf(temp,"%d",a);
    string strtemp(temp);
    return strtemp;
}

void test_stringint()
{
    string a("123");
    int a1 = string2int(a);

    cout<<"string "<<a<<" to int "<<a1<<endl;

    int b = 234;
    string b1 = int2string(b);
    cout<<"int "<<b<<" to string "<<b1<<endl;
}
/***************************************************************
Function:
	atoi
	string.c_str
Description:
	convertation between int and string
******************************************************************/


int main()
{
    test_substr_exist();
    get_substr();
    test_stringint();
	return 0;
}