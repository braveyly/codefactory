#include <stdio.h>
#include <string.h>
#include "sort.h"

int print_my_array ( char* ele, int* index )
{
    printf ( "[%d]=[%s]\n", *index, ele  );
}

void shell_sort()
{
 	 const int n = 15;
     int i, j, temp; 
     int gap = 0;
     int a[] = {8, 4, 31, 222, 10, 1,3,6,11,32,22,100,99,88,108}; 
     while (gap<=n)
     {
          gap = gap * 3 + 1;
     } 
	 printf( "gap is %d\n", gap );
     while (gap > 0) 
     {
         for ( i = gap; i < n; i++ )
         {
             j = i - gap;
             temp = a[i];             
             while (( j >= 0 ) && ( a[j] > temp ))
             {
                     a[j + gap] = a[j];
                     j = j - gap;
              }
             a[j + gap] = temp;
         }
         gap = ( gap - 1 ) / 3;
     } 
	 
for( i = 0; i< n; i++)
	 {
	 	printf( "a[%d]=%d\n", i, a[i] );
	 }
}
int main()
{
    char* a[5] = { "z", "a", "r", "y", "f" };
    //char** b = ( char** ) sort_bubble ( &a, 5, ( cp_compare_fn ) strcmp, ASCEND_MODE );
    //callback_print_array ( b, 5, print_my_array );

    //char** c = ( char** ) sort_selection ( &a, 5, ( cp_compare_fn ) strcmp, ASCEND_MODE );
    //callback_print_array ( c, 5, print_my_array );

    //char** d = ( char** ) sort_insertion ( &a, 5, ( cp_compare_fn ) strcmp, DESCEND_MODE );
    //callback_print_array ( d, 5, print_my_array );

    //char** e = ( char** ) sort_quick(&a, 0, 4, ( cp_compare_fn )strcmp, DESCEND_MODE);
    //callback_print_array ( e, 5, print_my_array );

    //char** f = ( char** ) sort_merge(&a, 0, 4, ( cp_compare_fn )strcmp, DESCEND_MODE);
    //sort_merge ( &a, 0, 4, ( cp_compare_fn ) strcmp, ASCEND_MODE );
    //callback_print_array ( a, 5, print_my_array );

	char* aa[10] = { "x", "j", "h", "a", "p","e","q","m","s","r" };

	//char** g = sort_shell(&aa,10,( cp_compare_fn ) strcmp, ASCEND_MODE );
	//callback_print_array ( g, 10, print_my_array );

	char** h = sort_comb(&aa,10,( cp_compare_fn ) strcmp, DESCEND_MODE );
	callback_print_array ( h, 10, print_my_array );

}

