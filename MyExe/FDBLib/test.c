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
#include <stdio.h>
#include "FDBLib.h"


/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
int main( )
{
	char	gtDataTmp[100] = { 0 };
	int		gtDataLen;

	FDB		* fdb = fdb_init( 4, 8 );
	fdb_show( fdb );

	fdb_put( fdb, "abc", 3 );
	fdb_show( fdb );

	fdb_put( fdb, "def", 3 );
	fdb_show( fdb );

	fdb_put( fdb, "ghi", 3 );
	fdb_show( fdb );

	fdb_put( fdb, "jkl", 3 );
	fdb_show( fdb );

	fdb_get( fdb, gtDataTmp, 40, &gtDataLen );
	printf( "Get data length is %d, data is %s\n", gtDataLen, gtDataTmp );

	fdb_accept( fdb, 0 );
	fdb_show( fdb );
}

/*
 * End of file
 */
