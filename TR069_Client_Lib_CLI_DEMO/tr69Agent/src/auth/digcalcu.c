#include <string.h>
#include "global.h"
#include "md5.h"
#include "digcalcu.h"

void CvtHex(
    IN HASH Bin,
    OUT HashHex Hex
    )
{
    unsigned short i;
    unsigned char j;

    for (i = 0; i < HASHLEN; i++) {
        j = (Bin[i] >> 4) & 0xf;
        if (j <= 9)
            Hex[i*2] = (j + '0');
         else
            Hex[i*2] = (j + 'a' - 10);
        j = Bin[i] & 0xf;
        if (j <= 9)
            Hex[i*2+1] = (j + '0');
         else
            Hex[i*2+1] = (j + 'a' - 10);
    };
    Hex[HASHHEXLEN] = '\0';
};

void string2hex(HASH bin, HashHex hex)
{
    CvtHex(bin, hex);
}

/* calculate H(A1) as per spec */
void digest_calc_ha1(
    IN char * pszAlg,
    IN char * userNamePtr,
    IN char * realmPtr,
    IN char * passwordPtr,
    IN char * noncePtr,
    IN char * cNoncePtr,
    OUT HashHex SessionKey
    )
{
      MD5_CTX Md5Ctx;
      HASH hash1;

      md5_init(&Md5Ctx);
      md5_update(&Md5Ctx, userNamePtr, strlen(userNamePtr));
      md5_update(&Md5Ctx, ":", 1);
      md5_update(&Md5Ctx, realmPtr, strlen(realmPtr));
      md5_update(&Md5Ctx, ":", 1);
      md5_update(&Md5Ctx, passwordPtr, strlen(passwordPtr));
      md5_final(hash1, &Md5Ctx);
      if (strcasecmp(pszAlg, "md5-sess") == 0) {
	        md5_init(&Md5Ctx);
            md5_update(&Md5Ctx, hash1, HASHLEN);
            md5_update(&Md5Ctx, ":", 1);
            md5_update(&Md5Ctx, noncePtr, strlen(noncePtr));
            md5_update(&Md5Ctx, ":", 1);
            md5_update(&Md5Ctx, cNoncePtr, strlen(cNoncePtr));
            md5_final(hash1, &Md5Ctx);
      };
      CvtHex(hash1, SessionKey);
};

/* calculate request-digest/response-digest as per HTTP Digest spec */
void digest_calc_response(
    IN HashHex HA1,           /* H(A1) */
    IN char * nonce,       /* nonce from server */
    IN char * nonceCount,  /* 8 hex digits */
    IN char * cNonce,      /* client nonce */
    IN char * pszQop,         /* qop-value: "", "auth", "auth-int" */
    IN char * pszMethod,      /* method from the request */
    IN char * pszDigestUri,   /* requested URL */
    IN HashHex HEntity,       /* H(entity body) if qop="auth-int" */
    OUT HashHex Response      /* request-digest or response-digest */
    )
{
      MD5_CTX Md5Ctx;
      HASH HA2;
      HASH RespHash;
       HashHex HA2Hex;

      // calculate H(A2)
      md5_init(&Md5Ctx);
      md5_update(&Md5Ctx, pszMethod, strlen(pszMethod));
      md5_update(&Md5Ctx, ":", 1);
      md5_update(&Md5Ctx, pszDigestUri, strlen(pszDigestUri));
      if (strcasecmp(pszQop, "auth-int") == 0) {
            md5_update(&Md5Ctx, ":", 1);
            md5_update(&Md5Ctx, HEntity, HASHHEXLEN);
      };
      md5_final(HA2, &Md5Ctx);
       CvtHex(HA2, HA2Hex);

      // calculate response
      md5_init(&Md5Ctx);
      md5_update(&Md5Ctx, HA1, HASHHEXLEN);
      md5_update(&Md5Ctx, ":", 1);
      md5_update(&Md5Ctx, nonce, strlen(nonce));
      md5_update(&Md5Ctx, ":", 1);
      if (*pszQop) {
	  md5_update(&Md5Ctx, nonceCount, strlen(nonceCount));
          md5_update(&Md5Ctx, ":", 1);
          md5_update(&Md5Ctx, cNonce, strlen(cNonce));
          md5_update(&Md5Ctx, ":", 1);
          md5_update(&Md5Ctx, pszQop, strlen(pszQop));
          md5_update(&Md5Ctx, ":", 1);
      };
      md5_update(&Md5Ctx, HA2Hex, HASHHEXLEN);
      md5_final(RespHash, &Md5Ctx);
      CvtHex(RespHash, Response);
};