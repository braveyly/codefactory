#include "pal_file.h"
#if 0
PAL_INT32_T pal_scan_dir ( PAL_INT8_T *dir, PAL_INT8_T *fileName )
{
    FAST DIR *pDir;
    FAST struct dirent *pDirEnt; /* ptr to dirent */
    struct stat fStat;
    PAL_INT8_T  tfileName[100] ;

    if(dir == NULL)
    {
        return -1;
    }
    pDir = opendir ( ( char * ) dir ) ;
    if ( pDir == NULL )
    {
        printf ( "opendir  %s failed\n" , dir);
        return -1;
    }
    else
    {
        do
        {

            pDirEnt = readdir ( pDir );

            if ( pDirEnt != NULL )
            {
                /* Construct path/filename for stat */
                sprintf ( tfileName, "%s/%s", dir, pDirEnt->d_name );
                stat ( ( char * ) tfileName, &fStat );

                if ( S_ISDIR ( fStat.st_mode ) )
                {
                    continue;
                }
                else
                {
                    strcpy(fileName, tfileName);
                    return 0;
                }
            }
            else
            {

                    printf ( "read dir error, errno: %x\n", errno );
                    closedir ( pDir );
                    return -1;
            }
        }
        while ( pDirEnt != NULL );  /* until end of dir */
    }

    closedir ( pDir );


}
#endif
PAL_INT32_T pal_scan_dir (char *dir, char *fileName)
{
 
    static  DIR *dp = NULL;
    static char dirName[MAX_FILENAME_LEN + 1]="";//gavin added here for initialization.
    struct  dirent *entry;
 scanstart:
    //printf("\n===,dirname[%s]==[%s]=\n",dirName,dp==NULL?"NULL":"NONNULL");
	
    if (0 == strlen (dirName) || 0 != strcmp (dirName, dir)) {
	
	 if (NULL != dp) 
	 {
	     closedir (dp);
	     dp = NULL;
	 }
	 
        if (NULL == (dp = opendir (dir)))
        {
             printf("\n===opendir [%s]==error=\n",dir);
	     return -1;
        }
	 strcpy (dirName, dir);
    }

    if (NULL == (entry = readdir (dp)))
    {
        if (NULL != dp)
        {
            closedir (dp);
	     dp = NULL;
        }

	 memset (dirName, 0, sizeof (dirName));
	 //printf("\n====read dir fail==\n");
	 return -1;
    } else {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
       {
            goto scanstart;
        }
        else
        {
            strcpy (fileName, entry->d_name);
        }
	//printf("\n===filename[%s] read==\n",fileName);
	 return 0;
    }
}

