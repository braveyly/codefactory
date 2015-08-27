#include "hmac.h"

char encrypt_xor(char x, char y)
{
    return x ^ y;
}

int  hmac_sha1(char * key,  char * context, int context_len, char *result)
{
    SHA1Context sha;
    char keypad[B_LEN+1];
    char first_xor[B_LEN+1];
    char second_xor[B_LEN+1];
    char contextpad1[B_LEN*5];
    char contextpad2[B_LEN*2];
    int    contextpad1_len = 0;
    int    contextpad2_len = 0;
    int i;

    char ipad[B_LEN+1] =
    {
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
        0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36
    };
    char opad[B_LEN+1] =
    {
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
        0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c
    };

    memset(keypad, 0, sizeof(keypad));
    memset(first_xor, 0, sizeof(first_xor));
    memset(second_xor, 0, sizeof(second_xor));
    memset(contextpad1, 0, sizeof(contextpad1));
    memset(contextpad2, 0, sizeof(contextpad2));


    strcpy(keypad, key);

    for (i = 0; i < B_LEN; i++)
    {
        first_xor[i] = encrypt_xor(keypad[i], ipad[i]);
    }

    for (i = 0; i < B_LEN; i++)
    {
        second_xor[i] = encrypt_xor(keypad[i], opad[i]);
    }



    memcpy(contextpad1, first_xor, B_LEN);
    memcpy(&contextpad1[B_LEN], context, context_len);
    contextpad1_len = B_LEN + context_len;

    SHA1Reset(&sha);
    SHA1Input(&sha, (const unsigned char *)contextpad1, contextpad1_len);
    if (!SHA1Result(&sha))
    {
        printf("hmac_sha1:ERROR-- could not compute message digest\n");
        return 0;
    }
    else
    {
        for (i = 0; i < 5 ; i++)
        {
            printf("%X ", sha.Message_Digest[i]);
        }
    }


    memcpy(contextpad2, second_xor, B_LEN);
    memcpy(&contextpad2[B_LEN], &sha.Message_Digest[0], SHA1_LEN);
    contextpad2_len = B_LEN + SHA1_LEN;

    SHA1Reset(&sha);
    SHA1Input(&sha, (const unsigned char *)contextpad2, contextpad2_len);
    if (!SHA1Result(&sha))
    {
        printf("hmac_sha1:ERROR-- could not compute message digest\n");
        return 0;
    }
    else
    {
        for (i = 0; i < 5 ; i++)
        {
            printf("%X ", sha.Message_Digest[i]);
        }
    }


    memcpy(result, &sha.Message_Digest[0], SHA1_LEN);
    return 1;

}

