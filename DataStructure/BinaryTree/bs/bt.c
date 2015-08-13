/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:
 * Abstract:
 *
 * Current version:
 * Author:
 * Date:
 * Modification:

 * Revise version:
 * Author:
 * Date:
 * Modification:
 */
#include "bt.h"


/*
 * Function: Bt_Entry* create_entry( void* data )
 * Description: create entry by item
 * Input:  data: item 
 * Output: none
 * Return: Bt_Entry*: the new created entry
 * Others: none
 */
Bt_Entry* create_entry( void* data )
{
	Bt_Entry* entry = ( Bt_Entry* )malloc( sizeof( Bt_Entry ) );
	entry->item	   = data;
	entry->right   = NULL;
	entry->left	   = NULL;
	_(printf( "create entry %d item %s\n", entry, data ));
	return entry;
}

/*
 * Function: void destroy_entry( Bt_Entry* entry )
 * Description: destroy entry
 * Input:  entry:the entry to be destroyed
 * Output: none
 * Return: void
 * Others: none
 */
void destroy_entry( Bt_Entry* entry )
{
	if( entry )
	{
		_(printf("destroy non-null entry\n"));
		free( entry );
	}
}

/*
 * Function: void show_entry( Bt_Entry * entry, show_item show )
 * Description: show one entry
 * Input:  entry: the entry to be showed
 *            show: the item field showing function 
 * Output: none
 * Return: void
 * Others: none
 */
void show_entry( Bt_Entry * entry, show_item show )
{
	if( !entry )
	{
		printf( "none for entry\n" );
		return;
	}

	if( !show )
	{
		printf( "none for item show function\n" );
		return;
	}

	( *show )( entry->item );
}

/*
 * Function: Bt* create_bt( )
 * Description: create one bt
 * Input:
 * Output: none
 * Return: Bt*: the created bt
 * Others: none
 */
Bt* create_bt( )
{
	Bt* bt = ( Bt* )malloc( sizeof( Bt ) );
	bt->cnt		   = 0;
	bt->root	   = NULL;
	bt->lock_type  = 0;
	bt->lock	   = NULL;
	return bt;
}

/*
 * Function: Bt_Entry* insert_entry( Bt_Entry* root, Bt_Entry* entry, compare_item cmp )
 * Description: insert entry at the left of the root which is less than root, or right if more
 * Input:  root: the root entry
 *          entry:  the entry to be inserted
 *           cmp: the item field comparing function
 * Output: none
 * Return: Bt_Entry*: the root entry
 * Others: none
 */
Bt_Entry* insert_entry( Bt_Entry* root, Bt_Entry* entry, compare_item cmp )
{
	if( NULL == root )
	{
		root = entry;
		return root;
	}

	if( cmp( entry->item, root->item ) > 0 )
	{
		root->right = insert_entry( root->right, entry, cmp );
	}else
	{
		root->left = insert_entry( root->left, entry, cmp );
	}
	return root;
}

/*
 * Function: Bt* init_bt( Bt* bt, void** array, int size, compare_item cmp )
 * Description: init one bt with the array of two dimensions
 * Input:    bt: the bt to be inited
 *         array: the initial array for bt
 *           size: array size
 *          cmp: item field comparing function
 * Output: none
 * Return: the bt that has been inited
 * Others: none
 */
Bt* init_bt( Bt* bt, void** array, int size, compare_item cmp )
{
	int		i = 0;
	Bt_Entry* entry;

	for( i = 0; i < size; i++ )
	{
		entry	   = create_entry( array[i] );
		bt->root   = insert_entry( bt->root, entry, cmp );
	}
	return bt;
}

/*
 * Function: void pre_order( Bt_Entry* root, show_item show )
 * Description: previous order
 * Input:  root: the binary tree root
 *         show:  item showing function
 * Output: none
 * Return: void
 * Others: none
 */
void pre_order( Bt_Entry* root, show_item show )
{
	if( !root )
	{
		printf( "none for root\n" );
		return;
	}

	( *show )( root->item );

	if( root->left )
	{
		pre_order( root->left, show );
	}
	if( root->right )
	{
		pre_order( root->right, show );
	}
}

/*
 * Function: void in_order( Bt_Entry* root, show_item show )
 * Description: inorder traversal
 * Input:  root: binary tree root entry
 *           show: item showing function
 * Output: none
 * Return: void
 * Others: none
 */
void in_order( Bt_Entry* root, show_item show )
{
	if( !root )
	{
		printf( "none for root\n" );
		return;
	}

	if( root->left )
	{
		in_order( root->left, show );
	}

	( *show )( root->item );

	if( root->right )
	{
		in_order( root->right, show );
	}
}

/*
 * Function: void post_order( Bt_Entry* root, show_item show )
 * Description: post order traversal 
 * Input:  root: binary tree root entry
 *         show: item field showing function
 * Output: none
 * Return: void
 * Others: none
 */
void post_order( Bt_Entry* root, show_item show )
{
	if( !root )
	{
		printf( "none for root\n" );
		return;
	}

	if( root->left )
	{
		post_order( root->left, show );
	}

	if( root->right )
	{
		post_order( root->right, show );
	}

	( *show )( root->item );
}

/*
 * Function: void destroy_root( Bt_Entry* root, destroy_item del_item )
 * Description: destroy binary tree
 * Input:  root: binary tree root entry
 *     del_item: item deleting function
 * Output: none
 * Return: void
 * Others: none
 */
void destroy_root( Bt_Entry* root, destroy_item del_item )
{
	if( !root )
	{
		return;
	}

	if( root->right )
	{
		destroy_root( root->right, del_item );
	}

	if( root->left )
	{
		destroy_root( root->left, del_item );
	}

	if( root )
	{
		if( del_item )
		{
			( *del_item )( root->item );
		}
		_( printf( "destroy entry with item[%s]\n", ( char * )root->item ) );
		destroy_entry( root );
	}
}

/*
 * Function: void* destroy_bt( Bt * bt, destroy_item del_item )
 * Description: bt deleting function
 * Input:  bt: the bt to be deleted 
 *   del_item: item deleting function
 * Output: none
 * Return: void
 * Others: none
 */
void* destroy_bt( Bt * bt, destroy_item del_item )
{
	destroy_root( bt->root, del_item );
	free( bt );
}


/*
 * Function: int is_empty( Bt* bt )
 * Description: check whether the bt is empty
 * Input:  bt: the bt
 * Output: none
 * Return: int:1:bt is empty
 *		      0:bt is not empty
 * Others: none
 */
int is_empty( Bt* bt )
{
	return bt->root ? 0 : 1;
}

/*
 * Function: Bt_Entry* get_root( Bt* bt )
 * Description: get the root entry of bt
 * Input:  bt: the bt
 * Output: none
 * Return: Bt_Entry*: the root entry of bt
 * Others: none
 */
Bt_Entry* get_root( Bt* bt )
{
	return bt->root;
}

/*
 * Function: int calc_tree_depth( Bt_Entry* root )
 * Description: calculate the depth of the binary tree
 * Input:  root:the binary tree root entry
 * Output: none
 * Return: int: the length of the tree
 * Others: none
 */
int calc_tree_depth( Bt_Entry* root )
{
	int ldepth = 0, rdepth = 0;
	if( NULL == root )
	{
		return 0;
	}

	ldepth = calc_tree_depth( root->left );
	rdepth = calc_tree_depth( root->right );

	if( ldepth > rdepth )
	{
		return ldepth + 1;
	} else
	{
		return rdepth + 1;
	}
}

/*
 * Function: Bt_Entry* get_entry( Bt_Entry* root, void* item, compare_item cmp )
 * Description: get entry by item value
 * Input:  root: bt root entry
 *           item: the item value
 *		cmp: item comparing function
 * Output: none
 * Return: Bt_Entry*: the entry which item  equals to item
 * Others: none
 */
Bt_Entry* get_entry( Bt_Entry* root, void* item, compare_item cmp )
{
	Bt_Entry* entry;
	if( !root )
	{
		printf( "none for root\n" );
		return NULL;
	}

	if( 0 == cmp( root->item, item ) )
	{
		return root;
	}

	if( root->left )
	{
		entry = get_entry( root->left, item, cmp );
		if( entry )
		{
			return entry;
		}
	}

	if( root->right )
	{
		entry = get_entry( root->right, item, cmp );
		if( entry )
		{
			return entry;
		}
	}
	return NULL;
}

/*
 * int set_entry( Bt_Entry* root, void* old_item, void* new_item, compare_item cmp )
 * Description: set the item value of the entry with old_item to new_item
 * Input:  root: bt root entry
 *     old_item: the previous item value that the entry has
 *   new_item: the new item value
 *           cmp: item value comparing function
 * Output: none
 * Return: int: 1: succceed
 *			0: failed
 * Others: none
 */
int set_entry( Bt_Entry* root, void* old_item, void* new_item, compare_item cmp )
{
	Bt_Entry* entry = get_entry( root, old_item, cmp );
	if( entry )
	{
		entry->item = new_item;
		return 1;
	}else
	{
		return 0;
	}
}

/*
 * Function: Bt_Entry* get_parent( Bt_Entry* root, void* item, compare_item cmp )
 * Description: get the parent entry of the entry with item value
 * Input:  root: binary tree root entry
 *            item: the item value
 *		cmp: item value comparing function
 * Output: none
 * Return: Bt_Entry*: the parent entry that is found
 * Others: none
 */
Bt_Entry* get_parent( Bt_Entry* root, void* item, compare_item cmp )
{
	Bt_Entry* entry;
	void	*item1, *item2;
	if( NULL == root )
	{
		return NULL;
	}

	if( NULL == root->right || NULL == root->left )
	{
		return NULL;
	}

	entry  = root->right;
	item1  = entry->item;
	entry  = root->left;
	item2  = entry->item;
	if( 0 == cmp( item1, item ) ||
	    0 == cmp( item2, item ) )
	{
		return root;
	}

	if( root->right )
	{
		entry = get_parent( root->right, item, cmp );
		if( entry )
		{
			return entry;
		}
	}

	if( root->left )
	{
		entry = get_parent( root->left, item, cmp );
		if( entry )
		{
			return entry;
		}
	}

	return NULL;
}

/*
 * Function: Bt_Entry* get_right( Bt_Entry* root, void* item, compare_item cmp )
 * Description: get the right entry of the entry with item value
 * Input:  root: bt root entry
 *           item: search the entry by the item value
 *		cmp: item value comparing function
 * Output: none
 * Return: Bt_Entry*: the right entry
 * Others: none
 */
Bt_Entry* get_right( Bt_Entry* root, void* item, compare_item cmp )
{
	Bt_Entry* entry;
	void	*left_item;
	if( NULL == root )
	{
		return NULL;
	}

	entry = root->left;

	if( NULL == entry )
	{
		entry = get_right( root->right, item, cmp );
		if( entry )
		{
			return entry;
		}
	}

	if( entry )
	{
		left_item = entry->item;
		if( 0 == cmp( left_item, item ) )
		{
			return root->right;
		} else
		{
			entry = get_right( root->right, item, cmp );
			if( entry )
			{
				return entry;
			}

			entry = get_right( root->left, item, cmp );
			if( entry )
			{
				return entry;
			}
		}
	}

	return NULL;
}


/*
 * Function: Bt_Entry* get_left( Bt_Entry* root, void* item, compare_item cmp )
 * Description: get the left entry of the entry with item value
 * Input:  root: bt root entry
 *           item: search the entry by the item value
 *		cmp: item value comparing function
 * Output: none
 * Return: Bt_Entry*: the left entry
 * Others: none
 */
Bt_Entry* get_left( Bt_Entry* root, void* item, compare_item cmp )
{
	Bt_Entry* entry;
	void	*right_item;
	if( NULL == root )
	{
		return NULL;
	}

	entry = root->right;

	if( NULL == entry )
	{
		entry = get_left( root->left, item, cmp );
		if( entry )
		{
			return entry;
		}
	}

	if( entry )
	{
		right_item = entry->item;
		if( 0 == cmp( right_item, item ) )
		{
			return root->left;
		} else
		{
			entry = get_left( root->right, item, cmp );
			if( entry )
			{
				return entry;
			}

			entry = get_left( root->left, item, cmp );
			if( entry )
			{
				return entry;
			}
		}
	}

	return NULL;
}

int special_level_order(Bt_Entry* root, int level, show_item show)
{
	if( NULL == root || level < 0 )
		return 0;

	if( 0 == level )
	{
		(*show)(root->item);
		return 1;
	}

	return special_level_order(root->left, level-1, show) +
		   special_level_order(root->right, level-1,show);
}

void level_order(Bt_Entry* root, show_item show)
{
	int depth = calc_tree_depth(root);
	int i;

	for( i = 0; i < depth; i++ )
	{
		special_level_order(root,i,show);
	}
}

/*
 * End of file
 */
