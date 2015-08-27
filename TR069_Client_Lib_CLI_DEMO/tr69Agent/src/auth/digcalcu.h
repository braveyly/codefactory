#ifndef _DIGCALCU_H_
#define _DIGCALCU_H_

#define HASHLEN 16
typedef char HASH[HASHLEN];

#define HASHHEXLEN 32
typedef char HashHex[HASHHEXLEN+1];

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

/* calculate H(A1) as per HTTP Digest spec */                   
void digest_calc_ha1(char*pszAlg, char*pszUserName, char*pszRealm
                    , char *pasPassword, char *nonce
                    , char *cNonce, HashHex SessionKey);
                    
/* calculate request-digest/response-digest as per HTTP Digest spec */
void digest_calc_response(HashHex ha1, char *nonce, char *nonceCount
                         , char *cNonce, char *pszQop, char *pszMethod
                         , char *pszDigetUri, HashHex HEntity
                         , HashHex Response);

/* convert 16 bytes md5 hash code to hex */
void string2hex(HASH bin, HashHex hex);                         

#endif /* _DIGCALCU_H_ */                       