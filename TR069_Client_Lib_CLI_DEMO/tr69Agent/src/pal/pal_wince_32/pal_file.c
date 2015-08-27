#include "pal_file.h"

PAL_INT32_T pal_scan_dir (char *dir, char *filename)
{
    static HANDLE h = NULL;
    WIN32_FIND_DATA fdata;
    TCHAR _dirW[MAX_FILENAME_LEN];

    MultiByteToWideChar(CP_ACP, MB_COMPOSITE, dir, strlen(filename)
                       , _dirW, MAX_FILENAME_LEN);

    if (h == NULL)
    {
        /* should go here only in the first time this routine is called */
        if ((h = FindFirstFile(_dirW, &fdata)) == INVALID_HANDLE_VALUE)
        {
            return -1;
        }
    }
    else
    {
        if (FindNextFile(h, &fdata) == 0)
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                /* no more file to read */
                return -2;
            }
            else
            {
                /* error */
                return -1;
            }
        }
    }
    
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, fdata.cFileName
                       , _tcslen(fdata.cFileName), filename, strlen(filename)
                       , NULL, NULL);

    return 0;
}
