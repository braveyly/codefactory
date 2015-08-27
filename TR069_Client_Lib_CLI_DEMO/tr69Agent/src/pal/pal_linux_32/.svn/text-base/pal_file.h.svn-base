#ifndef _PAL_FILE_H_
#define _PAL_FILE_H_

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "pal_types.h"

#define MAX_FILENAME_LEN 256

typedef DIR PAL_DIR_T;

/* if return 0, then fileName is valid, other it's invalid. */
extern PAL_INT32_T pal_scan_dir (char *dir,  char *fileName);
extern PAL_INT32_T pal_mkdir(const char *);
extern PAL_DIR_T* pal_opendir(const char *);
extern PAL_INT32_T pal_closedir(PAL_DIR_T *);


#endif /* _PAL_FILE_H_ */
