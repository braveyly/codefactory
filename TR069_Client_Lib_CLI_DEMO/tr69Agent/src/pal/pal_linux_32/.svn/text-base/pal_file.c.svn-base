#include "pal_file.h"

PAL_INT32_T pal_scan_dir (char *dir, char *fileName)
{
    static DIR *dp = NULL;
    static char dirName[MAX_FILENAME_LEN + 1]="";//gavin added here for initialization.
    struct dirent *entry;

    //printf("\n===,dirname[%s]==[%s]=\n",dirName,dp==NULL?"NULL":"NONNULL");
	
    if (0 == strlen (dirName) || 0 != strcmp (dirName, dir)) {
	
	 if (NULL != dp) 
	 {
	     closedir (dp);
	     dp = NULL;
	 }
	 
        if (NULL == (dp = opendir (dir)))
        {
             //printf("\n===opendir [%s]===\n",dir);
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
        strcpy (fileName, entry->d_name);
	//printf("\n===filename[%s] read==\n",fileName);
	 return 0;
    }
}

PAL_INT32_T pal_mkdir(const char *pathname)
{
    mode_t mask;

    if (pathname == NULL || strlen(pathname) == 0)
    {
        fprintf(stderr, "no directory name specified\n");
        return -1;
    }

    mask = S_IRUSR | S_IWUSR | S_IXUSR; /* set default mask to 0700 */
    return mkdir(pathname, mask);
}

PAL_DIR_T* pal_opendir(const char *name)
{
    if (name == NULL || strlen(name) == 0)
    {
        fprintf(stderr, "no directory name specified\n");
        return -1;
    }

    return opendir(name);
}

PAL_INT32_T pal_closedir(PAL_DIR_T *dir)
{
    return closedir(dir);
}
