#include <stdio.h>
#include <gtest/gtest.h>


extern int get_cell_value(char* file_path, int row, int col, char* value);
extern int set_cell_value(char* file_path, int row, int col, char* value);
extern int dump_excel(char* file_path);
char value[100] = {0};
TEST(GET_CELL_VALUE,NOTZERO)
{
	ASSERT_EQ(-1,get_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 2, 4, value));
}

TEST(GET_CELL_VALUE,ZERO)
{
	ASSERT_EQ(0,get_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 1, 2, value));
}

TEST(GET_CELL_VALUE,VALUE)
{
	ASSERT_EQ(0,set_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 1, 2, "abc"));
	ASSERT_EQ(0,get_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 1, 2, value));
	EXPECT_STREQ("abc",value);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}