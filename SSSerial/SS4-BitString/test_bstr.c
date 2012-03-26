#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bstr.h"

int main(int argc, char *argv[])
{
	unsigned long p = 0xAA0000AA;
	char q = 0x10;
	char r = 'a';

	char buf[0x1000];
	char *pre1, *pre2, *pre3, *pre4;
	int cpos;

/*Test 1:convert other type variable to bitstring*/
    printf( "/*Test 1:convert other type variable to bitstring*/\n" );
	cp_bstr *a = cp_bstr_create(7, (unsigned char *) &p);
	cp_bstr *b = cp_bstr_create(8, (unsigned char *) &q);
	cp_bstr *c = cp_bstr_create(8, (unsigned char *) &r);

	pre1 = cp_bstr_to_string(a);
	pre2 = cp_bstr_to_string(b);
	pre3 = cp_bstr_to_string(c);

	cp_bstr_dump(a);
	cp_bstr_dump(b);
	cp_bstr_dump(c);
	printf( "pre1 is %s\n", pre1 );
	printf( "pre2 is %s\n", pre2 );
	printf( "pre3 is %s\n", pre3 );

/*Test 2:convert "010100"like string to cp_bstr bitstring*/
    printf( "/*Test 2:convert 010100like string to cp_bstr bitstring*/\n" );
	cp_bstr *d = cstr_to_bstr("100");
	pre4 = cp_bstr_to_string(d);
    cp_bstr_dump(d);
	printf( "pre4 is %s\n", pre4 );

/*Test 3: test concatenate function*/
    printf( "/*Test 3: test concatenate function*/\n" );
	cp_bstr_cat(d, d);
	cp_bstr_dump(d);

/*Test 4: test compare function*/
    printf( "/*Test 4: test compare function*/\n" );
	int res2 = cp_bstr_cmp( b, c, &cpos );
	printf( "res is %d cpos is %d\n", res2, cpos );
	res2 = cp_bstr_cmp( b, b, &cpos );
	printf( "res is %d cpos is %d\n", res2, cpos );

/*Test 5: test bit shift left function*/
    printf( "/*Test 5: test bit shift left function*/\n" );
    cp_bstr_dump(a);
	cp_bstr_shift_left( a, 1 );
	cp_bstr_dump(a);

    free(pre1);
	free(pre2);
	free(pre3);
	free(pre4);

	cp_bstr_destroy(a);
	cp_bstr_destroy(b);
	cp_bstr_destroy(c);
	cp_bstr_destroy(d);
	
	return 0;
}

#if 0
/*Test 1:convert other type variable to bitstring*/
bit sequence: 7 bits
1010101
bit sequence: 8 bits
00010000
bit sequence: 8 bits
01100001
pre1 is 1010101
pre2 is 00010000
pre3 is 01100001
/*Test 2:convert 010100like string to cp_bstr bitstring*/
bit sequence: 3 bits
100
pre4 is 100
/*Test 3: test concatenate function*/
bit sequence: 6 bits
100100
/*Test 4: test compare function*/
res is -81 cpos is 1
res is 0 cpos is 8
/*Test 5: test bit shift left function*/
bit sequence: 7 bits
1010101
bit sequence: 6 bits
010101
#endif

