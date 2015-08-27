#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "sha1.h"

#define B_LEN 64
#define SHA1_LEN 20
int  hmac_sha1(char * key,  char * context, int context_len, char *result);

