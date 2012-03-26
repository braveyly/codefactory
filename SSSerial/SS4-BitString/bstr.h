#ifndef _CP_BSTR_H
#define _CP_BSTR_H

#ifdef CP_HAS___BUILTIN_CLZ
#define CP_CLZ_CHAR(x) (__builtin_clz(x) - 24)
#define CP_CLZ_LONG_LONG(x) __builtin_clzll(x)
#else
//#error NOT HAS CLZ
#ifdef _MSC_VER
#include <intrin.h>
unsigned int __inline msc_clz( unsigned int x );
#define CP_CLZ_CHAR(x) (msc_clz(x) - 24)
unsigned int __inline msc_clz64( unsigned long long x );
#define CP_CLZ_LONG_LONG(x) msc_clz64(x)
#else
static int cp_clz_char_slow(unsigned int x)
{
	int bit, lz;
	for (lz = 0, bit = 0x80; bit > 0 && ((bit & x) == 0); lz++, bit >>= 1);
	return lz;
}
#define CP_CLZ_CHAR(x) cp_clz_char_slow(x)
static int cp_clz_long_long_slow(unsigned long long x)
{
	int count;
	for (count = 0; x != 0; x >>= 1, count++);
	return sizeof(long long) * 8 - count;
}
#define CP_CLZ_LONG_LONG(x) cp_clz_long_long_slow(x)
#endif /* _MSC_VER */
#endif /* CP_HAS___BUILTIN_CLZ */


typedef struct _cp_bstr
{
	unsigned char *bits;
	int length;
} cp_bstr;

#define BYTECOUNT(b) (((b)->length + 7) >> 3)

cp_bstr *cp_bstr_create(int length, unsigned char *bits);

/**
 * convenience function for debugging - initialize from a string that looks 
 * like "1010101" (actually any non-null character but '1' is interpreted as '0')
 */
cp_bstr *cstr_to_bstr(char *str);

void cp_bstr_destroy(cp_bstr *seq);

cp_bstr *cp_bstr_dup(cp_bstr *seq);

cp_bstr *cp_bstr_cpy(cp_bstr *dst, cp_bstr *src);

cp_bstr *cp_bstr_cat(cp_bstr *head, cp_bstr *tail);

/**
 * shift bits left by the specified count, resulting in a shorter bit sequence
 */
int cp_bstr_shift_left(cp_bstr *seq, int count);

/**
 * compares bit sequences a and b, strcmp semantics. if pos is not null it is
 * set to the index of the first differing bit. if the sequences are identical
 * to the length they are defined, and pos is not null, it is set to the length
 * of the shorter sequence - e.g. 010 and 0101 are identical to the third bit, 
 * hence *pos is set to 3.
 cp_bstr_cmp returns > 0 if a > b , < 0 if a < b or 0 if a == b 
 */
int cp_bstr_cmp(cp_bstr *a, cp_bstr *b, int *pos);

#define cp_bstr_length(seq) (seq)->length

void cp_bstr_dump(cp_bstr *seq);

char *cp_bstr_to_string(cp_bstr *seq);

#endif /* _CP_BSTR_H */

