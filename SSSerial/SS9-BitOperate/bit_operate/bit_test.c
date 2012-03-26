#include <stdio.h>

/*Set the special number bit to 0*/
void set_to_zero_by_bit_index ( unsigned char* A, int offset_bit )
{
    ( *A ) &= ~ ( 1 << offset_bit );
}

/*Set the special number bit to 1*/
void set_to_one_by_bit_index ( unsigned char* A, int offset_bit )
{
    ( *A ) |= ( 1 << offset_bit );
}

/*Set the special number bit to the opposite*/
void set_to_opposite_by_bit_index ( unsigned char* A, int offset_bit )
{
    ( *A ) ^= ( 1 << offset_bit );
}

/*Swap two numbers*/
void swap ( unsigned char* A, unsigned char* B )
{
    *A ^= *B;
    *B ^= *A;
    *A ^= *B;
}

/*Test the special number bit
If 0:return 0
If 1:return 1*/
int test_by_bit_index ( unsigned char*A, int offset_bit )
{
    if ( 0 == ( ( *A ) & ( 1 << offset_bit ) ) )
        return 0;
    else
        return 1;
}

int main()
{
    int a = 0x42;
    set_to_zero_by_bit_index ( &a, 6 );
    printf ( "hex:0x%x,decimal:%d,octal:0%o\n", a, a, a );

    int b = 0x42;
    set_to_one_by_bit_index ( &b, 0 );
    printf ( "hex:0x%x,decimal:%d,octal:0%o\n", b, b, b );

    set_to_opposite_by_bit_index ( &b, 1 );
    printf ( "hex:0x%x,decimal:%d,octal:0%o\n", b, b, b );

    set_to_opposite_by_bit_index ( &b, 2 );
    printf ( "hex:0x%x,decimal:%d,octal:0%o\n", b, b, b );

    int c = 2, d = 21;

    swap ( &c, &d );
    printf ( "c = %d d = %d\n", c, d );

    int e = 2;
    printf ( "return value for 2 is %d\n", test_by_bit_index ( &e, 2 ) );
    printf ( "return value for 1 is %d\n", test_by_bit_index ( &e, 1 ) );

}
