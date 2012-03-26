#include <stdio.h>
#include <string.h>     /* for strdup */
#include <stdlib.h>     /* for free */
#include <errno.h>
#include "rb.h"

#ifdef DEBUG
int check_tree ( cp_rbtree *t );
int node_count ( cp_rbtree *t );
#endif /* DEBUG */

#if 1 //for multiple string values
void print_result ( cp_vector *v, char *title )
{
    int i;
    printf ( "%s:\n", title );
    for ( i = 0; i < cp_vector_size ( v ); i++ )
        printf ( "%s\n", ( char * ) cp_vector_element_at ( v, i ) );
}

int main()
{
    int opcount = 0;
    int insert_count = 0;
    int delete_count = 0;
    int ncount;
    cp_rbtree *t =
        cp_rbtree_create_by_option ( COLLECTION_MODE_COPY |
                                     COLLECTION_MODE_DEEP |
                                     COLLECTION_MODE_NOSYNC |
                                     COLLECTION_MODE_MULTIPLE_VALUES,
                                     ( cp_compare_fn ) strcmp,
                                     ( cp_copy_fn ) strdup, free,
                                     ( cp_copy_fn ) strdup, free );

    if ( t == NULL )
    {
        perror ( "create" );
        exit ( 1 );
    }

    while ( ++opcount )
    {
        char *x;
        char name[80];
        char number[80];

        printf ( "[== %d ==] enter name [next]: ", opcount );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "enter number [next]: " );
        number[0] = '\0';
        fgets ( number, 80, stdin );
        number[strlen ( number ) - 1] = '\0'; /* chop newline */
        if ( number[0] == '\0' ) break;

        cp_rbtree_insert ( t, strdup ( name ), strdup ( number ) );

        printf ( "\nsize: %d \n", cp_rbtree_count ( t ) );

#ifdef DEBUG
        if ( check_tree ( t ) ) exit ( -1 );
#endif
        cp_rbtree_dump ( t );
    }

    while ( 1 )
    {
        char name[80];
        char *number;

        printf ( "enter name [quit]: " );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "contains: %d\n", cp_rbtree_contains ( t, name ) );

        if ( ( number = cp_rbtree_get ( t, name ) ) != NULL )
            print_result ( number, name );
        else
            printf ( "no entry for %s\n", name );
    }

    while ( 1 )
    {
        char name[80];
        void *number;

        printf ( "enter name [delete]: " );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "contains: %d\n", cp_rbtree_contains ( t, name ) );

        if ( ( number = cp_rbtree_delete ( t, name ) ) != NULL )
            print_result ( number, name );
        else
            printf ( "no entry for %s\n", name );
    }

    cp_rbtree_destroy ( t );
    return 0;
}
#endif

#if 0 //for single string value
int main()
{
    int opcount = 0;
    int insert_count = 0;
    int delete_count = 0;
    int ncount;
    cp_rbtree *t =
        cp_rbtree_create_by_option ( COLLECTION_MODE_COPY |
                                     COLLECTION_MODE_DEEP |
                                     COLLECTION_MODE_NOSYNC,         ,
                                     ( cp_compare_fn ) strcmp,
                                     ( cp_copy_fn ) strdup, free,
                                     ( cp_copy_fn ) strdup, free );

    if ( t == NULL )
    {
        perror ( "create" );
        exit ( 1 );
    }

    while ( ++opcount )
    {
        char *x;
        char name[80];
        char number[80];

        printf ( "[== %d ==] enter name [next]: ", opcount );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "enter number [next]: " );
        number[0] = '\0';
        fgets ( number, 80, stdin );
        number[strlen ( number ) - 1] = '\0'; /* chop newline */
        if ( number[0] == '\0' ) break;

        cp_rbtree_insert ( t, strdup ( name ), strdup ( number ) );

        printf ( "\nsize: %d \n", cp_rbtree_count ( t ) );

#ifdef DEBUG
        if ( check_tree ( t ) ) exit ( -1 );
#endif
        cp_rbtree_dump ( t );
    }

    while ( 1 )
    {
        char name[80];
        char *number;

        printf ( "enter name [quit]: " );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "contains: %d\n", cp_rbtree_contains ( t, name ) );

        if ( ( number = cp_rbtree_get ( t, name ) ) != NULL )
            printf ( "the number is [%s] for name [%s]", number, name );
        else
            printf ( "no entry for %s\n", name );
    }

    cp_rbtree_destroy ( t );
    return 0;
}
#endif

#if 0 //for multiple struct values
typedef struct NODEINFO
{
    char nodeName[10];
    char nodeDes[10];
}NodeInfo;

void print_struct_result ( cp_vector *v )
{
    int i;
    NodeInfo* pNode = NULL;
    for ( i = 0; i < cp_vector_size ( v ); i++ )
    {
        printf ( "vector[%d]\n", i );
        pNode = ( NodeInfo* ) cp_vector_element_at ( v, i );
        printf ( "NodeName %s NodeDes %s\n", pNode->nodeName, pNode->nodeDes );
    }
}

void* dump_struct ( NodeInfo* node ) /*the format has to be the same with strdup*//*By LIWEI 2012-02-23*/
{
    NodeInfo* p = ( NodeInfo* ) malloc ( sizeof ( NodeInfo ) );
    strcpy ( p->nodeDes, node->nodeDes );
    strcpy ( p->nodeName, node->nodeName );
    return p;
}


int main()
{
    int opcount = 0;
    int insert_count = 0;
    int delete_count = 0;
    int ncount;
    cp_rbtree *t =
        cp_rbtree_create_by_option ( COLLECTION_MODE_COPY |
                                     COLLECTION_MODE_DEEP |
                                     COLLECTION_MODE_NOSYNC |
                                     COLLECTION_MODE_MULTIPLE_VALUES,
                                     ( cp_compare_fn ) strcmp,
                                     ( cp_copy_fn ) strdup, free,
                                     ( cp_copy_fn ) dump_struct, free );//The val_copy parameter is optional for dump_struct or NULL/*By LIWEI 2012-02-23*/

    if ( t == NULL )
    {
        perror ( "create" );
        exit ( 1 );
    }

    while ( ++opcount )
    {
        NodeInfo node;
        memset ( &node, 0, sizeof ( node ) );

        printf ( "enter Name [next]:" );
        fgets ( node.nodeName, 10, stdin );
        node.nodeName[strlen ( node.nodeName ) - 1] = '\0'; /* chop newline */
        if ( node.nodeName[0] == '\0' ) break;

        printf ( "enter Description [next]: " );
        fgets ( node.nodeDes, 10, stdin );
        node.nodeDes[strlen ( node.nodeDes ) - 1] = '\0'; /* chop newline */
        if ( node.nodeDes[0] == '\0' ) break;
        cp_rbtree_insert ( t, strdup ( node.nodeName ), dump_struct ( &node ) );

        printf ( "\nsize: %d \n", cp_rbtree_count ( t ) );

#ifdef DEBUG
        if ( check_tree ( t ) ) exit ( -1 );
#endif
        cp_rbtree_dump ( t );
    }

    while ( 1 )
    {
        char name[80];
        void *node;
        fflush ( stdout );
        printf ( "enter name [search]: " );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "contains: %d\n", cp_rbtree_contains ( t, name ) );

        if ( ( node = cp_rbtree_get ( t, name ) ) != NULL )
            print_struct_result ( node );
        else
            printf ( "no entry for %s\n", name );
    }

    while ( 1 )
    {
        char name[80];
        void *node2;
        fflush ( stdout );
        printf ( "enter name [delete]: " );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "contains: %d\n", cp_rbtree_contains ( t, name ) );
        cp_rbtree_dump ( t );
        /*Segment error for delete struct elements*/
        if ( ( node2 = ( cp_vector* ) cp_rbtree_delete ( t, name ) ) != NULL )
            //print_struct_result ( node2 );
            cp_rbtree_dump ( t );
        else
            printf ( "no entry for %s\n", name );
    }

    cp_rbtree_destroy ( t );
    return 0;
}
#endif

#if 0 //for single struct values
typedef struct NODEINFO
{
    char nodeName[10];
    char nodeDes[10];
}NodeInfo;

void print_struct_result ( cp_vector *v )
{
    int i;
    NodeInfo* pNode = NULL;
    for ( i = 0; i < cp_vector_size ( v ); i++ )
    {
        pNode = ( NodeInfo* ) cp_vector_element_at ( v, i );
        printf ( "NodeName %s NodeDes %s\n", pNode->nodeName, pNode->nodeDes );
    }
}

void* dump_struct ( NodeInfo* node ) /*the format has to be the same with strdup*//*By LIWEI 2012-02-23*/
{
    NodeInfo* p = ( NodeInfo* ) malloc ( sizeof ( NodeInfo ) );
    memset ( p, 0, sizeof ( NodeInfo ) );
    strcpy ( p->nodeDes, node->nodeDes );
    strcpy ( p->nodeName, node->nodeName );
    return p;
}

int main()
{
    int opcount = 0;
    int insert_count = 0;
    int delete_count = 0;
    int ncount;
    cp_rbtree *t =
        cp_rbtree_create_by_option ( COLLECTION_MODE_COPY |
                                     COLLECTION_MODE_DEEP |
                                     COLLECTION_MODE_NOSYNC,
                                     ( cp_compare_fn ) strcmp,
                                     ( cp_copy_fn ) strdup, free,
                                     ( cp_copy_fn ) dump_struct, free );  // The val_copy parameter is optional for dump_struct or NULL/*By LIWEI 2012-02-23*/

    if ( t == NULL )
    {
        perror ( "create" );
        exit ( 1 );
    }

    while ( ++opcount )
    {
        NodeInfo node;
        memset ( &node, 0, sizeof ( node ) );

        printf ( "enter Name [next]:" );
        fgets ( node.nodeName, 10, stdin );
        node.nodeName[strlen ( node.nodeName ) - 1] = '\0'; /* chop newline */
        if ( node.nodeName[0] == '\0' ) break;

        printf ( "enter Description [next]: " );
        fgets ( node.nodeDes, 10, stdin );
        node.nodeDes[strlen ( node.nodeDes ) - 1] = '\0'; /* chop newline */
        if ( node.nodeDes[0] == '\0' ) break;

        cp_rbtree_insert ( t, strdup ( node.nodeName ), dump_struct ( &node ) );

        printf ( "\nsize: %d \n", cp_rbtree_count ( t ) );

#ifdef DEBUG
        if ( check_tree ( t ) ) exit ( -1 );
#endif
        cp_rbtree_dump ( t );
    }

    while ( 1 )
    {
        char name[80];
        NodeInfo *node;
        fflush ( stdout );
        printf ( "enter name [quit]: " );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "contains: %d\n", cp_rbtree_contains ( t, name ) );

        /*if COLLECTION_MODE_MULTIPLE_VALUES is not set,
          the retrival of cp_rbtree_get is the value but not the value vector*//*By LIWEI 2012-02-23*/
        if ( ( node = ( NodeInfo* ) cp_rbtree_get ( t, name ) ) != NULL )
            //print_struct_result(node);
            printf ( "node name %s node description %s\n", node->nodeName, node->nodeDes );
        else
            printf ( "no entry for %s\n", name );
    }

    while ( 1 )
    {
        char name[80];
        NodeInfo *node;
        fflush ( stdout );
        printf ( "enter name [delete]: " );
        name[0] = '\0';
        fgets ( name, 80, stdin );
        name[strlen ( name ) - 1] = '\0'; /* chop newline */
        if ( name[0] == '\0' ) break;

        printf ( "contains: %d\n", cp_rbtree_contains ( t, name ) );

        /*Unrecognizable code occurs when Deleting the second element *//*By LIWEI 2012-02-23*/
        if ( ( node = ( NodeInfo* ) cp_rbtree_delete ( t, name ) ) != NULL )
            //print_struct_result(node);
            printf ( "node name %s node description %s\n", node->nodeName, node->nodeDes );
        else
            printf ( "no entry for %s\n", name );
    }

    cp_rbtree_destroy ( t );
    return 0;
}
#endif


#ifdef DEBUG

int scan_rbtree ( cp_rbnode *node, int black_count, int *max_count )
{
    int rc;
    if ( node == NULL )
    {
        if ( *max_count == 0 )
        {
            *max_count = black_count;
            return 1;
        }
        else
        {
            if ( *max_count != black_count )
            {
                printf ( "IMBALANCE: max_count %d, black_count %d\n",
                         *max_count, black_count );
                return -1;
            }

            return 1;
        }
    }

    if ( node->color == RB_BLACK ) black_count++;
    else
    {
        if ( ( node->right && node->right->color == RB_RED ) ||
                ( node->left && node->left->color == RB_RED ) )
        {
            printf ( "IMBALANCE: in node [%s]: red node with red child/ren\n",
                     ( char * ) node->key );
            return 0;
        }
    }

    rc = scan_rbtree ( node->left, black_count, max_count );
    if ( rc == -1 )
    {
        printf ( "offending node: %s\n", node->key );
        return 0;
    }
    else if ( rc == 0 ) return 0;

    rc = scan_rbtree ( node->right, black_count, max_count );
    if ( rc == -1 )
    {
        printf ( "offending node: %s\n", node->key );
        return 0;
    }
    else if ( rc == 0 ) return 0;
    return 1;
}

int check_tree ( cp_rbtree *t )
{
    int valid;
    int black_count = 0;
    int max_count = 0;

    if ( t->root && t->root->color == RB_RED )
    {
        printf ( "IMBALANCE: root is RED\n" );
        cp_rbtree_dump ( t );
        return -1;
    }

    valid = scan_rbtree ( t->root, black_count, &max_count );
    if ( !valid )
        cp_rbtree_dump ( t );
    if ( valid ) return 0;
    return -1;
}

void count_node ( cp_rbnode *node, int *count )
{
    if ( node )
    {
        ( *count ) ++;
        if ( node->right ) count_node ( node->right, count );
        if ( node->left ) count_node ( node->left, count );
    }
}

int node_count ( cp_rbtree *t )
{
    int res = 0;
    count_node ( t->root, &res );
    return res;
}

#endif /* DEBUG */
