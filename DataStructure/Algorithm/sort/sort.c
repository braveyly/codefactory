#include "sort.h"

int callback_print_array ( void** array, int size, cp_callback_fn print_fn )
{
    int i = 0;
    for ( i = 0; i < size; i++ )
    {
        ( *print_fn ) ( array[i], &i );
    }
    return 1;
}

void swap ( void** ele_a, void** ele_b )
{
    printf ( "%s<->%s\n", *ele_a, *ele_b );
    void* tmp;
    tmp = *ele_a;
    *ele_a = *ele_b;
    *ele_b = tmp;
}

void** sort_bubble ( void** array, int size, cp_compare_fn cmp_fn, sort_mode mode )
{
    int i, begin_index = 0, end_index = size, repeat_flag;
    void * temp;

    if ( ASCEND_MODE != mode && DESCEND_MODE != mode )
        mode = ASCEND_MODE;

    do
    {
        repeat_flag = 0;
        for ( i = begin_index; i < end_index - 1; i++ )
        {
            /*If ASCEND_MODE, Move MAX to end; Else Move MIN to end*/
            if ( ( *cmp_fn ) ( array[i], array[i+1] ) > 0 &&
                    ASCEND_MODE == mode ||
                    ( *cmp_fn ) ( array[i], array[i+1] ) < 0 &&
                    DESCEND_MODE == mode )
            {
                swap ( &array[i], &array[i+1] );
                repeat_flag = 1;
            }
        }
        end_index--;
    }
    while ( repeat_flag );

    return array;
}

void** sort_selection ( void** array, int size, cp_compare_fn cmp_fn, sort_mode mode )
{
    int i, j, tmp_index, begin_index = 0, end_index = size;
    void* tmp;
    for ( i = 0; i < size; i++ )
    {
        tmp_index = begin_index;
        for ( j = begin_index; j < end_index; j++ )
        {
            /*tmp_index store the index of MAX*/
            if ( ( *cmp_fn ) ( array[j], array[tmp_index] ) > 0 )
            {
                tmp_index = j;
            }
        }
        /*Move MAX to the end*/
        if ( ASCEND_MODE == mode )
        {
            swap ( &array[tmp_index], &array[end_index-1] );
            end_index--;
        }
        /*Move MAX to the begin*/
        if ( DESCEND_MODE == mode )
        {
            swap ( &array[tmp_index], &array[begin_index] );
            begin_index++;
        }
    }
    return array;
}

void** sort_insertion ( void** array, int size, cp_compare_fn cmp, sort_mode mode )
{
    int i, j, move_flag = 0;
    void* tmp;
    /*The ordered array is from 0 to i-1*/
    for ( i = 1; i < size; i++ )
    {
        printf ( "0-%d\n", i );
        move_flag = 0;
        /*Index i is the element that is going to be inserted*/
        tmp = array[i];
        for ( j = i - 1; j >= 0; j-- )
        {
            if ( ASCEND_MODE == mode && ( *cmp ) ( tmp, array[j] ) < 0 ||
                    DESCEND_MODE == mode && ( *cmp ) ( tmp, array[j] ) > 0 )
            {
                /*Move sub array to find the proper position for i*/
                array[j+1] = array[j];
                /*If this flag is set, INSERT operation must be done*/
                move_flag = 1;
            }
            else
                break;
        }
        if ( move_flag )
        {
            array[j+1] = tmp;/*Insert i into j+1*/
        }
    }
    return array;
}

int quick_partition ( void** array, int left, int right, int pivot, cp_compare_fn cmp, sort_mode mode )
{
    void* pivot_value;
    int i, store_index;

    pivot_value = array[pivot];
    /*Put this reference element at the end*/
    swap ( &array[pivot], &array[right] );
    store_index = left;
    /*The elements from 0 to store_index-1 are less than pivot_value which is equal to array[right] */
    for ( i = left;i < right;i++ )
    {
        if ( ( *cmp ) ( array[i], pivot_value ) < 0 && ASCEND_MODE == mode ||
                ( *cmp ) ( array[i], pivot_value ) > 0 && DESCEND_MODE == mode )
        {
            swap ( &array[i], &array[store_index] );
            store_index++;
        }
    }
    /*Put back the end element in store_index*/
    /*store_index is greater than elements from 0 to store_index-1*/
    swap ( &array[store_index], &array[right] );
    return store_index;
}

void** sort_quick ( void** array, int left, int right, cp_compare_fn cmp, sort_mode mode )
{
    int pivot_index;
    if ( left >= right )
        return 0;
    pivot_index = left + ( right - left ) / 2;
    pivot_index = quick_partition ( array, left, right, pivot_index, cmp, mode );
    /*Recur for the left partition*/
    sort_quick ( array, left, pivot_index - 1, cmp, mode );
    /*Recur for the right partition*/
    sort_quick ( array, pivot_index + 1, right, cmp, mode );
    return array;
}

void** merge ( void** array, int first, int mid, int last, cp_compare_fn cmp, sort_mode mode )
{
    int i, k;
    int begin1, end1, begin2, end2;
    void** temp = ( void** ) malloc ( ( last - first + 1 ) * sizeof ( void* ) );
    begin1 = first;
    end1 = mid;
    begin2 = mid + 1;
    end2 = last;
    k = 0;
    while ( ( begin1 <= end1 ) && ( begin2 <= end2 ) )
    {
        if ( ( *cmp ) ( array[begin1], array[begin2] ) < 0 && ASCEND_MODE == mode ||
                ( *cmp ) ( array[begin1], array[begin2] ) > 0 && DESCEND_MODE == mode )
        {
            temp[k] = array[begin1];
            begin1++;
        }
        else
        {
            temp[k] = array[begin2];
            begin2++;
        }
        k++;
    }

    while ( begin1 <= end1 || begin2 <= end2 )
    {
        if ( begin1 <= end1 )
        {
            temp[k++] = array[begin1++];
        }

        if ( begin2 <= end2 )
        {
            temp[k++] = array[begin2++];
        }
    }

    for ( i = 0;i <= ( last - first );i++ )
        array[first+i] = temp[i];
    free ( temp );
    return array;
}

void sort_merge ( void** array, int first, int last, cp_compare_fn cmp, sort_mode mode )
{
    printf ( "first[%d]last[%d]\n", first, last );
    int mid;
    if ( last <= first )
        return 0;

    mid = ( first + last ) / 2;
    sort_merge ( array, first, mid, cmp, mode );
    sort_merge ( array, mid + 1, last, cmp, mode );
    merge ( array, first, mid, last, cmp, mode );
    printf ( "-%s-%s-%s-%s-%s\n", array[0], array[1], array[2], array[3], array[4] );
    return 0;
}

void** sort_shell ( void** array, int size, cp_compare_fn cmp, sort_mode mode )
{
    int gaps[] = {701, 301, 132, 57, 23, 10, 4, 1};
    int i, j, k, gap;
    void *tmp;

    for ( i = 0; i < sizeof ( gaps ) / sizeof ( int ); i++ )
    {
        gap = gaps[i];
        /*The elements from 0 to tmp_index-1 are the corresponding first elements in different ordered array */
        for ( j = gap;j < size;j++ )
        {
            /*tmp is the element that is waiting to be inserted for specific subarray*/
            tmp = array[j];
            /*Compare the insert element with the ones before it, and move them backforward*/
            for ( k = j - gap;k >= 0;k -= gap )
            {
                if ( ASCEND_MODE == mode && ( *cmp ) ( tmp, array[k] ) < 0 ||
                        DESCEND_MODE == mode && ( *cmp ) ( tmp, array[k] ) > 0 )
                    array[k+gap] = array[k];
                else break;/*Make sure that insert the element at the proper position*/
            }
            array[k+gap] = tmp;/*Insert it*/
        }
    }
    return array;
}

void** sort_comb ( void** array, int size, cp_compare_fn cmp, sort_mode mode )
{
    int i, j, k, repeat_flag, gap, gaps[] = {11, 8, 6, 4, 3, 2, 1};
    for ( i = 0; i < sizeof ( gaps ) / sizeof ( int ); i++ )
    {
        gap = gaps[i];
        for ( j = 0;j < gap;j++ )
        {
            printf ( "aa\t j[%d] in gap[%d]\n", j, gap );
            /*single bubble sort start*/
            do
            {
                repeat_flag = 0;
                for ( k = j;k + gap < size;k += gap )
                {
                    printf ( "bb\tk [%d]\n", k );
                    if ( ASCEND_MODE == mode && ( *cmp ) ( array[k], array[k+gap] ) > 0 ||
                            DESCEND_MODE == mode && ( *cmp ) ( array[k], array[k+gap] ) < 0 )
                    {
                        swap ( &array[k], &array[k+gap] );
                        repeat_flag = 1;
                    }
                }
            }
            while ( repeat_flag );
            /*Single bubble sort end*/
        }
    }
    return array;
}
