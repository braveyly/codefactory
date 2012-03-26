#include <stdio.h>
#include <string.h>

//No.1
void test_strstr()
{
    char *str="Borland   International";
    char *str1="B",*ptr1;
    char *str2="na",*ptr2;
    char *str3="",*ptr3;

    ptr1=strstr(str,str1);
    printf("The   substring1   is:   %s\n",   ptr1);//Borland   International

    ptr2=strstr(str,str2);
    printf("The   substring2   is:   %s\n",   ptr2);//national

    ptr3=strstr(str,str3);
    printf("The   substring3   is:   %s\n",   ptr3); //Borland   International
}
/*****************************************************
Function:strstr
Description:test whether the substring is in the parent string
  ******************************************************/

//No.2
void char2int_1()
{
    char* temp1="123";
    int a1=atoi(temp1);
    printf("char2int_1()\ta1+1=%d\n",a1+1);//124
}

void int2char_1()
{
    char temp[20];
    char* retp;
    int a2=65;

    retp=_itoa(a2,temp,16);
    printf("int2char_1\tretp=%s\n",retp);//41
    printf("int2char_1\ttemp=%s\n",temp);//41

    retp=_itoa(a2,temp,10);
    printf("int2char_1\tretp=%s\n",retp);//65
    printf("int2char_1\ttemp=%s\n",temp);//65
}

void int2char_2()
{
    char temp[20];
    int a2=65;
    sprintf(temp,"%d",65);
    printf("int2char_2\ttemp=%s\n",temp);//65
}

void test_intchar()
{
    char2int_1();
    int2char_1();
    int2char_2();
}
/*********************************************************************
Function:atoi itoa sprintf
Description:Convertion between char and int
**********************************************************************/

//No.3
char* noleftspace(char* str)
{
    char* temp;
    printf("before noleftspace=[%s]\n",str);
    while(*str!='\0')
    {
        if(*str==' ')
        {
            str=str+1;
            temp=str;
        }
        else
        {
            temp=str;
            break;
        }
    }
    printf("after noleftspace=[%s]\n",temp);
    return temp;
}

char* norightspace(char* str)
{
    int length=strlen(str);
    int a=length-1;
    char* temp;

    *str='1';
    printf("before noleftspace=[%s]\n",str);

    while(a>0)
    {
        if(*(str+a)!=' ')
        {
            (char*)str;
            *(str+a+1)='\0';
            break;
        }
        else
            a--;
    }
    printf("after noleftspace=[%s]\n",str);
    return str;
}

char* trace(char* str)
{
    return norightspace(noleftspace(str));
}

void test_trace()
{
    char a[100];
    strcpy(a, " abcdefgh  ");
    printf(" a after trace is [%s]", trace(a));
}
/*******************************************************************************
Function: No
Description: Cut the space charactors in both ends
********************************************************************************/

//No.4
int test_strchr(void)
{
    char string[20];
    char *ptr, c = 'r';

    strcpy(string, "This is a string");
    ptr = strchr(string, c);

    if(ptr)
        printf("The character %c is at position: %d\n", c, ptr-string); //12
    else
        printf("The character was not found\n");
    printf("ptr=%s\n",ptr);//ring
    return 0;
}
/*********************************************************************************
Function: strchr
Description: Test whether the charactor is in the char string
**********************************************************************************/

//No.5
int test_strcspn(void)
{
    char *string = "D23557890"; //string1中的D出现在string的第0个位置
    char *string1 = "7DC8";
    int length1;
    char *string2 = "45"; //string2中的5出现在string的第3个位置
    int length2;

    length1 = strcspn(string, string1);
    printf("Character where strings intersect is at position %d\n", length1); //0
    length2 = strcspn(string, string2);
    printf("Character where strings intersect is at position %d\n", length2); //3
    return 0;
}
/*******************************************************
size_t strcspn( const char *string, const char *strCharSet );
在string中，第一次出现strCharSet中任意一个字符的位置
*********************************************************/

//No.6
int test_strpbrk(void) //查找string2中字符在string1中出现的位置
{
    char *string1 = "abcdefghi3jklmnopqrstuvwxyz";
    char *string2 = "9876543210";
    char *ptr;


    ptr = strpbrk(string1, string2);
    if(ptr)
    {
        printf("strpbrk found first character: %c\n", *ptr); //3
        printf("the position of ptr is %d\n",(ptr-string1));  //9
    }
    else
        printf("strpbrk didn't find character in set\n");
    return 0;
}
/**************************************************************
char *strpbrk( const char *string, const char *strCharSet );
查找在string中出现的来自strCharSet第一个字符，并返回指向它的指针
Each of these functions returns a pointer to the first occurrence of any character
from strCharSet in string, or a NULL pointer if the two string arguments have no characters in common.
****************************************************************/

//No.7
int test_strspn(void)
{
    char *string1 = "1234567890"; //5不在string2中
    char *string2 = "000000213985fDC84";
    int length;

    length = strspn(string1, string2);
    printf("Character where strings differ is at position %d\n", length); //5
    return 0;
}
/***********************************************************
size_t strspn( const char *string, const char *strCharSet );
string中第一个不在strCharSet中的字符的位置
  ***********************************************************/

//No.8
char string[] = "10.8.'2.173--xense rver3";
char seps[]   = "'- '";
char *token;
void test_strtok(void)
{
    printf("%s\n\nTokens:\n", string);
    token = strtok(string, seps);
    while(token != NULL)
    {
        printf(" %s\n", token);
        token = strtok(NULL, seps);
    }
}
/***********************************************
strtok
10.8.
2.173
xense
rver3
************************************************/


int main()
{
	
    test_strstr();
    test_intchar();
    test_trace();
    test_strchr();
    test_strcspn();
    test_strpbrk();
    test_strspn();
    test_strtok();
	return 0;
}
