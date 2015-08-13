/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:bst.c
 * Abstract:binary search tree
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
#include "bst.h"

static Bst_Entry* create_entry( void* data )
{
	Bst_Entry* entry = ( Bst_Entry* )malloc( sizeof( Bst_Entry ) );
	entry->item	   = data;
	entry->right   = NULL;
	entry->left	   = NULL;
	_(printf( "create entry %d item %s\n", entry, data ));
	return entry;
}

static void destroy_entry( Bst_Entry* entry, item_destroy_fn dtr)
{
	if(NULL == entry)
		return;
	if(entry->item && dtr)
	{
		(*dtr)(entry->item);
	}
	_(printf("destroy entry item %s\n", (char*)entry->item));
	free(entry);
}

Bst* create_bst(item_show_fn show, item_destroy_fn dtr, item_compare_fn comp)
{
	Bst* bst = ( Bst* )malloc( sizeof( Bst ) );
	bst->cnt		   = 0;
	bst->root	   = NULL;
	bst->show = show;
	bst->dtr = dtr;
	bst->comp = comp;
	return bst;
}

static void destroy_root( Bst_Entry* root, item_destroy_fn del_item )
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
		free( root );
	}
}

void destroy_bst( Bst ** bst )
{
	if(NULL == *bst)
		return;
	if(NULL == (*bst)->root)
		return;

	destroy_root((*bst)->root,(*bst)->dtr);
	free((*bst));
	*bst = NULL;
}

static Bst_Entry* insert_entry( Bst_Entry* root, Bst_Entry* entry, item_compare_fn cmp )
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

int bst_insert(Bst** ppbst, void* item)
{
	Bst_Entry* entry;
	
	if(NULL==ppbst || NULL == *ppbst )
		return -1;

	entry = create_entry(item);
	
	(*ppbst)->root = insert_entry( (*ppbst)->root, entry, (*ppbst)->comp );

	return 0;
}

static void delete_entry(Bst_Entry* result, Bst_Entry* father, item_destroy_fn dtr )
{
	Bst_Entry* bottom_right,*tmp, *tmp2, *rmv;

	rmv = result;
	if(	NULL==result->left && NULL==result->right )//case 2:left and right are null
	{
		_(printf("right and left is NULL\n" ));
		if(result == father->left )
			father->left = NULL;
		if(result == father->right )
			father->right = NULL;	
	}

	if(	NULL==result->left && NULL!=result->right )//case 3: either left or right is null
	{
		tmp = result->right;
		_(printf("right is %s\n", tmp->item ));
		father->right = result->right;	
	}

	if(	NULL==result->right && NULL!=result->left )//case 3
	{
		tmp = result->left;
		_(printf("left is %s\n", tmp->item ));
		father->left = result->left;	
	}

	if(	NULL!=result->right && NULL!=result->left )//case 4:both right and left are not null
	{
		tmp = result->right;
		tmp2 = result->left;
		_(printf("right is %s left is %s\n", tmp->item, tmp2->item ));
		bottom_right = result->left;
		while(bottom_right->right)
		{
			bottom_right = bottom_right->right;
		}
		bottom_right->right = result->right;
		if( result == father->left)
		{
			father->left = result->left;
		}
		if( result == father->right )
		{
			father->right = result->left;
		}
	}
	destroy_entry(rmv,dtr);
	
}

int bst_delete(Bst** ppbst, void* item)
{
	Bst_Entry* r,*f, *bottom_right, *rmv;
	int res;
	
	if(NULL==ppbst || NULL == *ppbst )
		return -1;
	r=f=NULL;
	res = search_entry( (*ppbst)->root, item, (*ppbst)->comp, &r, &f );
	if( -1 == res )
	{
		printf("unable to delete item for it doesnot exist in bst\n");
		return res;
	}

	if(NULL == f)//case 1:father is null
	{
		_(printf("f is NULL\n"));
		bottom_right = (*ppbst)->root->left;
		rmv = (*ppbst)->root;
		if(bottom_right)
		{
		   while(bottom_right->right)
		   {
			 bottom_right = bottom_right->right;
		   }
		   bottom_right->right = (*ppbst)->root->right;
		   (*ppbst)->root = (*ppbst)->root->left;
		}
		else
		{
			(*ppbst)->root = (*ppbst)->root->right;
		}
		destroy_entry(rmv,(*ppbst)->dtr);
	}
	
	if(f)
	{
		printf("f item is %s r item is %s\n", (char*)f->item, (char*)r->item);
		delete_entry(r, f, (*ppbst)->dtr);
	}
	return 0;
}

static int search_entry(Bst_Entry* root, void* item, item_compare_fn comp, Bst_Entry** result, Bst_Entry** father)
{
	if( NULL == root || NULL == item )
		return -1;

	
	if( 0 == comp(item, root->item))
	{
		_(printf("find item in root\n"));
		*result = root;
		return 0;		
	}
	else if( comp(item, root->item) < 0 )
	{
		*father = root;
		_(printf("goto left child tree to search\n"));
		return search_entry(root->left,item,comp,result,father);
	}
	else
	{
		*father = root;
		_(printf("goto right child tree to search\n"));
		return search_entry(root->right,item,comp,result,father);		
	}
}

int bst_search(Bst* bst, void* item)
{
	Bst_Entry *p, *f;
	int ret;
	if( NULL == bst || NULL == bst->root )
		return -1;
	p=f=NULL;
	ret = search_entry( bst->root, item, bst->comp, &p, &f );
	_(printf("ret is %d\n", ret));
	if(p)
		_(printf("result is %s\n", (char*)p->item));
	if(f)
		_(printf("father is %s\n", (char*)f->item));
	return ret;
	
}


static void pre_order_entry( Bst_Entry* root, item_show_fn show )
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

void pre_order(Bst* bst)
{
	if(NULL==bst)
	{
		printf("none for bst\n");
		return;
	}
	
	pre_order_entry(bst->root,bst->show);
}


static void in_order_entry( Bst_Entry* root, item_show_fn show )
{
	if( !root )
	{
		printf( "none for root\n" );
		return;
	}

	if( root->left )
	{
		in_order_entry( root->left, show );
	}

	( *show )( root->item );

	if( root->right )
	{
		in_order_entry( root->right, show );
	}
}

void in_order(Bst* bst)
{
	if(NULL==bst)
	{
		printf("none for bst\n");
		return;
	}
	
	in_order_entry(bst->root,bst->show);	
}

static void post_order_entry( Bst_Entry* root, item_show_fn show )
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

void post_order(Bst* bst)
{
	if(NULL==bst)
	{
		printf("none for bst\n");
		return;
	}
	
	post_order_entry(bst->root,bst->show);	
}


/*
 * End of file
 */
