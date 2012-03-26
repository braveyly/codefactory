#ifndef _SORT_H_
typedef enum _sort_mode_
{
	ASCEND_MODE = 0,
	DESCEND_MODE,
}sort_mode;
typedef int ( *cp_compare_fn ) ( void *, void * );
typedef int ( *cp_callback_fn ) ( void *entry, void *client_prm );
int callback_print_array ( void** array, int size, cp_callback_fn print );
void swap(void** ele_a, void** ele_b);

void** sort_bubble ( void** array, int size, cp_compare_fn cmp, sort_mode mode );
void** sort_selection ( void** array, int size, cp_compare_fn cmp, sort_mode mode );
void** sort_insertion ( void** array, int size, cp_compare_fn cmp, sort_mode mode );
void** sort_quick ( void** array, int left, int right, cp_compare_fn cmp, sort_mode mode );
void** sort_shell ( void** array, int size, cp_compare_fn cmp, sort_mode mode );
void** sort_comb ( void** array, int size, cp_compare_fn cmp, sort_mode mode );

#endif
