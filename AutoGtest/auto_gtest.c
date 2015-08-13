#include <stdio.h>
#include <gtest/gtest.h>
extern int get_cell_value(char* file_path, int row, int col, char* value);
extern int set_cell_value(char* file_path, int row, int col, char* value);
extern int dump_excel(char* file_path);

int Foo(int a, int b)
{
    if (a == 0 || b == 0)
    {
        throw "don't do that";
    }
    int c = a % b;
    if (c == 0)
        return b;
    return Foo(b, c);
}
int num1 = 0;
int num2 = 0;
int expert_result = 0;

TEST(FooTest, HandleNoneZeroInput)
{
    EXPECT_EQ(expert_result, Foo(num1, num2));
}

int main(int argc, char* argv[])
{
	char value1[100] = {0};
	char value2[100] = {0};
	char expect[100] = {0};
	char actual_str[100] = {0};
	int actual_num = 0;
	int ret = 0;
	ret = get_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 2, 1, value1);
	if( -1 == ret )
	{
		return -1;
	}
	ret = get_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 2, 2, value2);
	if( -1 == ret )
	{
		return -1;
	}
	ret = get_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 2, 3, expect);
	if( -1 == ret )
	{
		return -1;
	}
	num1 = atoi(value1);
	num2 = atoi(value2);
	expert_result = atoi(expect);
	actual_num = Foo(num1,num2);
	sprintf(actual_str,"%d",actual_num);
	ret = set_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 2, 4, actual_str);
	if( -1 == ret )
	{
		return -1;
	}
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}