#include <stdio.h>
#include <string.h>

#define LINE_MAX_LENGTH 1024
#define MAX_COL 100
#define MAX_ROW 1000
#define FILE_BUFFER_LEN 1024*100

//--------------------------------------------------------
//Name:get_cell_value
//Input:
//     file_path 文件路径
//     row 单位格行
//     col 单位各列
//Output：
//     int 成功0 失败-1
//     value 单位格值
//Discription:
//     获取第row行第col列单位格的值，通过value返回，成功返回0，失败返回-1
//Notice:
//     通过fgets来取各行，通过strtok来取特定行的各列
//----------------------------------------------------------
int get_cell_value(char* file_path, int row, int col, char* value)
{
    FILE* file = NULL;
    char* cell_value = 0;
    int row_count = 0;
    int col_count = 0;
	char line[LINE_MAX_LENGTH];
	printf("get_cell_value:row=%d,col=%d\n",row,col);
	file = fopen(file_path, "r+");
    if ( NULL == file )
    {
        printf("Failed to open file %s\n", file_path);
		fclose(file);
        return -1;
    }
    
    while (fgets(line, sizeof(line), file))
    {
        row_count++;
        if ( row != row_count )
        {
            continue;
        }

        cell_value =  strtok(line, ",");
        col_count++;
        if ( col == col_count )
        {
            strcpy( value, cell_value );
            printf("Find cell: row=%d col=%d value=%s\n", row_count, col_count, value);
			break;
        }

        while ( cell_value = strtok(NULL, ",") )
        {
            col_count++;
            if ( col == col_count )
            {
                strcpy( value, cell_value );
                printf("Find cell: row=%d col=%d value=%s\n", row_count, col_count, value);
				break;
            }
        }
    }
	
	if( row != row_count )
	{
		printf( "Input row %d is bigger than file max row %d\n", row, row_count );
		fclose(file);
		return -1;
	}

	if( col != col_count )
	{
		printf( "Input col %d is bigger than file max col %d\n", col, col_count );
		fclose(file);
		return -1;
	}
    return 0;
}

//--------------------------------------------------------
//Name:set_cell_value
//Input:
//     file_path 文件路径
//     row 单位格行
//     col 单位各列
//     value 单位各值
//Output：
//     int 成功0 失败-1
//Discription:
//     设置第row行第col列单位格的值为value，成功返回0，失败返回-1
//Notice:
//     整个文件的内容由三部分构成，第row行之前的内容、第row行、
//     第row行之后的内容，最后的文件依次写入第row行的内容和
//     第row行之后的内容,第row行由三部分构成，第col列之前的内容，
//     新的col列内容value，第col列之后的内容
//----------------------------------------------------------
int set_cell_value(char* file_path, int row, int col, char* value)
{
    FILE* file = NULL;
    char* comma = 0;
    char* pre_comma = 0;
    int row_count = 0;
    int col_count = 0;
    int cur = 0;
    int pre_cur = 0;
    int last_col_flag = 0;
    char line[LINE_MAX_LENGTH] = {0};
    char line_left[LINE_MAX_LENGTH] = {0};
    char buffer_file_left[FILE_BUFFER_LEN] = {0};
	printf("set_cell_value:row=%d,col=%d,value=%s\n",row,col,value);
    file = fopen(file_path, "r+");
    if ( NULL == file )
    {
        printf("Failed to open file %s\n", file_path);
		fclose(file);
        return -1;
    }

    while (fgets(line, sizeof(line), file)) //get every line from file
    {
        row_count++;
        cur = ftell(file);
        if ( row == row_count )
        {
            break;
        }
        pre_cur = ftell(file);
    }

    if ( row != row_count )
    {
        printf( "Input row %d is bigger than file max row %d\n", row, row_count );
		fclose(file);
        return -1;
    }
	
	//pre_cur is the begining of the row line,cur is the end of the row line
    printf("Find row:pre_cur=%d, cur=%d, line=%s\n", pre_cur, cur, line); 

    fseek( file , 0 , SEEK_END );
    int end = ftell(file);//calculate the file length
    if (end > cur) //make sure that the current pointer of file doesnot reach the end of file
    {
        fread( buffer_file_left, 1, end - cur, file);
    }

    fseek( file, cur, SEEK_SET ); //reset the file pointer to the source position
    pre_comma = line;
    while ( comma = strchr(pre_comma, ',') )
    {
        col_count++;
        if ( col_count == col )
        {
            break;
        }
        pre_comma = comma + 1;
    }
	
    //pre_comma points the col cell,comma points the next comma
    printf("Find col:pre_comma=%s comma=%s\n", pre_comma, comma ); 
    if ( col_count + 1 == col )
    {
        last_col_flag = 1;
    }
    else if ( col_count  == col )
    {
        last_col_flag = 0;
    }
    else
    {
        printf( "Input col %d is bigger than file max col %d\n", col, col_count );
    }

    if ( 0 == last_col_flag )
    {
        strcpy(line_left, comma); //save the left string of line in line_left
    }

    strcpy(pre_comma, value); //replace the target cell
    pre_comma += strlen(value);

    if ( 0 == last_col_flag )
    {
        strcpy(pre_comma, line_left);
        pre_comma += strlen(line_left);
		//set the other charactors to 0. Now, finish constructing the row line
        memset(pre_comma, 0, strlen(pre_comma)); 
    }
    printf("Finish construct line=%s\n", line);

    fseek(file, pre_cur, SEEK_SET); //set the file pointer to the begining of row line
    fwrite(line, 1, strlen(line), file); //write the reconstructing line
    if (strlen(buffer_file_left))
    {
        fwrite(buffer_file_left, 1, strlen(buffer_file_left) + 2, file); //write the left file buffer
    }
    fclose(file);
    return 0;
}

//--------------------------------------------------------
//Name:dump_excel
//Input:
//     file_path 文件路径
//Output：
//     int 成功0 失败-1
//Discription:
//     输出file_path指向的文件的内容
//Notice:
//----------------------------------------------------------
int dump_excel(char* file_path)
{
    FILE* file = fopen(file_path, "r+");
    if ( NULL == file )
    {
        printf("Failed to open file %s\n", file_path);
        return -1;
    }
	
    char line[LINE_MAX_LENGTH];
    printf("\n\nDump start:\n");
    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }
	printf("\n\n");
    fclose(file);
	return 0;
}
/*
int main()
{
    char value[100] = {0};
    dump_excel("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv");
    get_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 2, 4, value);
    char value2[100] = "ghjkl";
    set_cell_value("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv", 1, 3, value2);
    dump_excel("C:\\Users\\l00219108\\Documents\\Visual Studio 2010\\Projects\\csvapi\\Debug\\test.csv");
    while (1)
    {}
    return 0;
}
*/