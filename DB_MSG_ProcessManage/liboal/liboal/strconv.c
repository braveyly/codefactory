/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>     /* for isDigit, really should be in oal_strconv.c */
#include <sys/stat.h>  /* this really should be in oal_strconv.c */
#include <arpa/inet.h> /* for inet_aton */
#include <errno.h>

#include "tcm.h"
#include "tcm_mem.h"
//#include "tcm_util.h"

#if 0
tcmRet tcmUtl_macStrToNum(const char *macStr, UINT8 *macNum)
{
   char *pToken = NULL;
   char *pLast = NULL;
   char *buf;
   SINT32 i;

   if (macNum == NULL || macStr == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   if (tcmUtl_isValidMacAddress(macStr) == FALSE)
   {
      return TCMRET_INVALID_PARAM_VALUE;
   }

   if ((buf = (char *) tcmMem_alloc(MAC_STR_LEN+1, ALLOC_ZEROIZE)) == NULL)
   {
      return TCMRET_RESOURCE_EXCEEDED;
   }

   /* need to copy since strtok_r updates string */
   strcpy(buf, macStr);

   /* Mac address has the following format
    * xx:xx:xx:xx:xx:xx where x is hex number
    */
   pToken = strtok_r(buf, ":", &pLast);
   macNum[0] = (UINT8) strtol(pToken, (char **)NULL, 16);
   for (i = 1; i < MAC_ADDR_LEN; i++)
   {
      pToken = strtok_r(NULL, ":", &pLast);
      macNum[i] = (UINT8) strtol(pToken, (char **)NULL, 16);
   }

   tcmMem_free(buf);

   return TCMRET_SUCCESS;

}
#endif

tcmRet tcmUtl_macNumToStr(const UINT8 *macNum, char *macStr)
{
   if (macNum == NULL || macStr == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   sprintf(macStr, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
           (UINT8) macNum[0], (UINT8) macNum[1], (UINT8) macNum[2],
           (UINT8) macNum[3], (UINT8) macNum[4], (UINT8) macNum[5]);

   return TCMRET_SUCCESS;
}

UINT32 tcmUtl_ipMaskToCidrLen(const char *ipMask)
{
   struct in_addr inetAddr;
   UINT32 cidrLen = 0;
   SINT32 i;

   if (inet_aton(ipMask, &inetAddr) != 0)
   {
      for (i = 31; i >= 0; i--)
      {
         if (inetAddr.s_addr & (UINT32)(1<<i))
         {
            cidrLen++;
         }
         else
         {
            break;
         }
      }
   }
   return cidrLen;

}

tcmRet tcmUtl_mergeIpWithMask(const char *ipStr, const char *maskStr, char *resultStr)
{
   struct in_addr ipAddr, maskAddr, tmpAddr;

   if( ipStr == NULL || maskStr == NULL || resultStr == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   if( inet_aton(ipStr, &ipAddr) != 0 && inet_aton(maskStr, &maskAddr) != 0)
   {
      tmpAddr.s_addr = ipAddr.s_addr & maskAddr.s_addr;
      strcpy(resultStr, inet_ntoa(tmpAddr));
   }
   else
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   return TCMRET_SUCCESS;
}

tcmRet tcmUtl_strtol(const char *str, char **endptr, SINT32 base, SINT32 *val)
{
   tcmRet ret=TCMRET_SUCCESS;
   char *localEndPtr=NULL;

   errno = 0;  /* set to 0 so we can detect ERANGE */

   *val = strtol(str, &localEndPtr, base);

   if ((errno != 0) || (*localEndPtr != '\0'))
   {
      *val = 0;
      ret = TCMRET_INVALID_ARGUMENTS;
   }

   if (endptr != NULL)
   {
      *endptr = localEndPtr;
   }

   return ret;
}


tcmRet tcmUtl_strtoul(const char *str, char **endptr, SINT32 base, UINT32 *val)
{
   tcmRet ret=TCMRET_SUCCESS;
   char *localEndPtr=NULL;

   /*
    * Linux strtoul allows a minus sign in front of the number.
    * This seems wrong to me.  Specifically check for this and reject
    * such strings.
    */
   while (isspace(*str))
   {
      str++;
   }
   if (*str == '-')
   {
      if (endptr)
      {
         *endptr = (char *) str;
      }
      *val = 0;
      return TCMRET_INVALID_ARGUMENTS;
   }

   errno = 0;  /* set to 0 so we can detect ERANGE */

   *val = strtoul(str, &localEndPtr, base);

   if ((errno != 0) || (*localEndPtr != '\0'))
   {
      *val = 0;
      ret = TCMRET_INVALID_ARGUMENTS;
   }

   if (endptr != NULL)
   {
      *endptr = localEndPtr;
   }

   return ret;
}

void tcmUtl_strToLower(char *string)
{
   char *ptr = string;
   for (ptr = string; *ptr; ptr++)
   {
       *ptr = tolower(*ptr);
   }
}
#if 0
tcmRet tcmUtl_parseUrl(const char *url, UrlProto *proto, char **addr, UINT16 *port, char **path)
{
   int n = 0;
   char *p = NULL;
   char protocol[BUFLEN_16];
   char host[BUFLEN_1024];
   char uri[BUFLEN_1024];

   if (url == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

  *port = 0;
   protocol[0] = host[0]  = uri[0] = '\0';

   /* proto */
   p = (char *) url;
   if ((p = strchr(url, ':')) == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }
   n = p - url;
   strncpy(protocol, url, n);
   protocol[n] = '\0';

   if (!strcmp(protocol, "http"))
   {
      *proto = URL_PROTO_HTTP;
   }
   else if (!strcmp(protocol, "https"))
   {
      *proto = URL_PROTO_HTTPS;
   }
   else if (!strcmp(protocol, "ftp"))
   {
      *proto = URL_PROTO_FTP;
   }
   else if (!strcmp(protocol, "tftp"))
   {
      *proto = URL_PROTO_TFTP;
   }
   else
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   /* skip "://" */
   if (*p++ != ':') return TCMRET_INVALID_ARGUMENTS;
   if (*p++ != '/') return TCMRET_INVALID_ARGUMENTS;
   if (*p++ != '/') return TCMRET_INVALID_ARGUMENTS;

   /* host */
   {
      char *pHost = host;

      while (*p && *p != ':' && *p != '/')
      {
         *pHost++ = *p++;
      }
      *pHost = '\0';
   }
   if (strlen(host) != 0)
   {
      *addr = tcmMem_strdup(host);
   }
   else
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   /* end */
   if (*p == '\0')
   {
      *path = tcmMem_strdup("/");
       return TCMRET_SUCCESS;
   }

   /* port */
   if (*p == ':')
   {
      char buf[BUFLEN_16];
      char *pBuf = buf;

      p++;
      while (isdigit(*p))
      {
         *pBuf++ = *p++;
      }
      *pBuf = '\0';
      if (strlen(buf) == 0)
      {
         TCMMEM_FREE_BUF_AND_NULL_PTR(*addr);
         return TCMRET_INVALID_ARGUMENTS;
      }
      *port = atoi(buf);
   }

   /* path */
   if (*p == '/')
   {
      char *pUri = uri;

      while ((*pUri++ = *p++));
      *path = tcmMem_strdup(uri);
   }
   else
   {
      *path = tcmMem_strdup("/");
   }

   return TCMRET_SUCCESS;
}
#endif
tcmRet tcmUtl_getBaseDir(char *pathBuf, UINT32 pathBufLen)
{
   UINT32 rc;

   rc = snprintf(pathBuf, pathBufLen, "/var");

   if (rc >= pathBufLen)
   {
      return TCMRET_RESOURCE_EXCEEDED;
   }

   return TCMRET_SUCCESS;
}

tcmRet tcmUtl_parseDNS(const char *inDsnServers, char *outDnsPrimary, char *outDnsSecondary)
{
   tcmRet ret = TCMRET_SUCCESS;
   char *tmpBuf;
   char *separator;
   char *discard;
   UINT32 len;

   if (inDsnServers == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   len = strlen(inDsnServers);

   if ((tmpBuf = tcmMem_alloc(len+1, 0)) == NULL)
   {
      ret = TCMRET_INTERNAL_ERROR;
   }
   else
   {
      sprintf(tmpBuf, "%s", inDsnServers);
      separator = strstr(tmpBuf, ",");
      if (separator != NULL)
      {
         /* break the string into two strings */
         *separator = 0;
         separator++;
         while ((isspace(*separator)) && (*separator != 0))
         {
            /* skip white space after comma */
            separator++;
         }

         /* Only record two DNS */
         if ( (discard = strstr(separator, ",")) != NULL )
         {
            *discard = 0;
         }

         if (outDnsSecondary != NULL)
         {
            strcpy(outDnsSecondary, separator);
         }
      }

      if (outDnsPrimary != NULL)
      {
         strcpy(outDnsPrimary, tmpBuf);
      }

      tcmMem_free(tmpBuf);
   }

   return ret;

}


UBOOL8 tcmUtl_isValidSyslogMode(const char * modeStr)
{
   UINT32 mode;

   if (tcmUtl_strtoul(modeStr, NULL, 10, &mode) != TCMRET_SUCCESS)
   {
      return FALSE;
   }

   return ((mode >= 1) && (mode <= 3));
}

UBOOL8 tcmUtl_isValidSyslogLevel(const char *levelStr)
{
   UINT32 level;

   if (tcmUtl_strtoul(levelStr, NULL, 10, &level) != TCMRET_SUCCESS)
   {
      return FALSE;
   }

   return (level <= 7);
}


UBOOL8 tcmUtl_isValidIpAddress(const char* input)
{
   UBOOL8 ret = TRUE;
   char *pToken = NULL;
   char *pLast = NULL;
   char buf[BUFLEN_16];
   UINT32 i, num;

   if (input == NULL || strlen(input) < 7 || strlen(input) > 15)
   {
      return FALSE;
   }

   /* need to copy since strtok_r updates string */
   strcpy(buf, input);

   /* IP address has the following format
      xxx.xxx.xxx.xxx where x is decimal number */
   pToken = strtok_r(buf, ".", &pLast);
   if ((tcmUtl_strtoul(pToken, NULL, 10, &num) != TCMRET_SUCCESS) ||
       (num > 255))
   {
      ret = FALSE;
   }
   else
   {
      for ( i = 0; i < 3; i++ )
      {
         pToken = strtok_r(NULL, ".", &pLast);

         if ((tcmUtl_strtoul(pToken, NULL, 10, &num) != TCMRET_SUCCESS) ||
             (num > 255))
         {
            ret = FALSE;
            break;
         }
      }
   }

   return ret;
}


#if 0
UBOOL8 tcmUtl_isValidMacAddress(const char* input)
{
   UBOOL8 ret =  TRUE;
   char *pToken = NULL;
   char *pLast = NULL;
   char buf[BUFLEN_32];
   UINT32 i, num;

   if (input == NULL || strlen(input) != MAC_STR_LEN)
   {
      return FALSE;
   }

   /* need to copy since strtok_r updates string */
   strcpy(buf, input);

   /* Mac address has the following format
       xx:xx:xx:xx:xx:xx where x is hex number */
   pToken = strtok_r(buf, ":", &pLast);
   if ((strlen(pToken) != 2) ||
       (tcmUtl_strtoul(pToken, NULL, 16, &num) != TCMRET_SUCCESS))
   {
      ret = FALSE;
   }
   else
   {
      for ( i = 0; i < 5; i++ )
      {
         pToken = strtok_r(NULL, ":", &pLast);
         if ((strlen(pToken) != 2) ||
             (tcmUtl_strtoul(pToken, NULL, 16, &num) != TCMRET_SUCCESS))
         {
            ret = FALSE;
            break;
         }
      }
   }

   return ret;
}
#endif
UBOOL8 isProtocolCover( const char* inputProtocol, const char* saveProtocol )
{
   UINT16 ipputBit = 0;
   UINT16 saveBit = 0;
   if ( !tcmUtl_strcmp(inputProtocol, "1"))
   {
      ipputBit = 0x01;
   }
   else if ( !tcmUtl_strcmp(inputProtocol, "2"))
   {
      ipputBit = 0x10;
   }
   else
   {
      ipputBit = 0x11;
   }

   if ( !tcmUtl_strcmp(saveProtocol, "TCP"))
   {
      saveBit = 0x01;
   }
   else if ( !tcmUtl_strcmp(saveProtocol, "UDP"))
   {
      saveBit = 0x10;
   }
   else
   {
      saveBit = 0x11;
   }

   if ( ipputBit & saveBit )
      return TRUE;
   else
      return FALSE;
}

UBOOL8 tcmUtl_isValidPortNumber(const char * portNumberStr)
{
   UINT32 portNum;

   if (tcmUtl_strtoul(portNumberStr, NULL, 10, &portNum) != TCMRET_SUCCESS)
   {
      return FALSE;
   }

   return (portNum < (64 * 1024));
}


SINT32 tcmUtl_strcmp(const char *s1, const char *s2)
{
   char emptyStr = '\0';
   char *str1 = (char *) s1;
   char *str2 = (char *) s2;

   if (str1 == NULL)
   {
      str1 = &emptyStr;
   }
   if (str2 == NULL)
   {
      str2 = &emptyStr;
   }

   return strcmp(str1, str2);
}

char* tcmUtl_strcpy(char *s1, const char *s2)
{
   if( s1 == NULL || s2 == NULL)
   {
      return NULL;
   }

   return strcpy(s1, s2);
}

SINT32 tcmUtl_strcasecmp(const char *s1, const char *s2)
{
   char emptyStr = '\0';
   char *str1 = (char *) s1;
   char *str2 = (char *) s2;

   if (str1 == NULL)
   {
      str1 = &emptyStr;
   }
   if (str2 == NULL)
   {
      str2 = &emptyStr;
   }

   return strcasecmp(str1, str2);
}


SINT32 tcmUtl_strncmp(const char *s1, const char *s2, SINT32 n)
{
   char emptyStr = '\0';
   char *str1 = (char *) s1;
   char *str2 = (char *) s2;

   if (str1 == NULL)
   {
      str1 = &emptyStr;
   }
   if (str2 == NULL)
   {
      str2 = &emptyStr;
   }

   return strncmp(str1, str2, n);
}


SINT32 tcmUtl_strncasecmp(const char *s1, const char *s2, SINT32 n)
{
   char emptyStr = '\0';
   char *str1 = (char *) s1;
   char *str2 = (char *) s2;

   if (str1 == NULL)
   {
      str1 = &emptyStr;
   }
   if (str2 == NULL)
   {
      str2 = &emptyStr;
   }

   return strncasecmp(str1, str2, n);
}


char *tcmUtl_strstr(const char *s1, const char *s2)
{
   char emptyStr = '\0';
   char *str1 = (char *)s1;
   char *str2 = (char *)s2;

   if (str1 == NULL)
   {
      str1 = &emptyStr;
   }
   if (str2 == NULL)
   {
      str2 = &emptyStr;
   }

   return strstr(str1, str2);
}


UBOOL8 tcmUtl_isSubOptionPresent(const char *fullOptionString, const char *subOption)
{
   const char *startChar, *currChar;
   UINT32 len=0;
   UBOOL8 found=FALSE;

   if (fullOptionString == NULL || subOption == NULL)
   {
      return FALSE;
   }

   startChar = fullOptionString;
   currChar = startChar;

   while (!found && *currChar != '\0')
   {
      /* get to the end of the current subOption */
      while (*currChar != ' ' && *currChar != ',' && *currChar != '\0')
      {
         currChar++;
         len++;
      }

      /* compare the current subOption with the subOption that was specified */
      if ((len == strlen(subOption)) &&
          (0 == strncmp(subOption, startChar, len)))
      {
         found = TRUE;
      }

      /* advance to the start of the next subOption */
      if (*currChar != '\0')
      {
         while (*currChar == ' ' || *currChar == ',')
         {
            currChar++;
         }

         len = 0;
         startChar = currChar;
      }
   }

   return found;
}

#if 0
char *tcmUtl_getAggregateStringFromDhcpVendorIds(const char *vendorIds)
{
   char *aggregateString;
   const char *vendorId;
   UINT32 i, count=0;

   if (vendorIds == NULL)
   {
      return NULL;
   }

   aggregateString = tcmMem_alloc(MAX_PORTMAPPING_DHCP_VENDOR_IDS * (DHCP_VENDOR_ID_LEN + 1), ALLOC_ZEROIZE);
   if (aggregateString == NULL)
   {
      return NULL;
   }

   for (i=0; i < MAX_PORTMAPPING_DHCP_VENDOR_IDS; i++)
   {
      vendorId = &(vendorIds[i * (DHCP_VENDOR_ID_LEN + 1)]);
      if (*vendorId != '\0')
      {
         if (count > 0)
         {
            strcat(aggregateString, ",");
         }
         /* strncat writes at most DHCP_VENDOR_ID_LEN+1 bytes, which includes the trailing NULL */
         strncat(aggregateString, vendorId, DHCP_VENDOR_ID_LEN);

         count++;
      }
   }

   return aggregateString;
}


char *tcmUtl_getDhcpVendorIdsFromAggregateString(const char *aggregateString)
{
   char *vendorIds, *vendorId, *ptr, *savePtr=NULL;
   char *copy;
   UINT32 count=0;

   if (aggregateString == NULL)
   {
      return NULL;
   }

   vendorIds = tcmMem_alloc(MAX_PORTMAPPING_DHCP_VENDOR_IDS * (DHCP_VENDOR_ID_LEN + 1), ALLOC_ZEROIZE);
   if (vendorIds == NULL)
   {
      return NULL;
   }

   copy = tcmMem_strdup(aggregateString);
   ptr = strtok_r(copy, ",", &savePtr);
   while ((ptr != NULL) && (count < MAX_PORTMAPPING_DHCP_VENDOR_IDS))
   {
      vendorId = &(vendorIds[count * (DHCP_VENDOR_ID_LEN + 1)]);
      /*
       * copy at most DHCP_VENDOR_ID_LEN bytes.  Since each chunk in the linear
       * buffer is DHCP_VENDOR_ID_LEN+1 bytes long and initialized to 0,
       * we are guaranteed that each vendor id is null terminated.
       */
      strncpy(vendorId, ptr, DHCP_VENDOR_ID_LEN);
      count++;

      ptr = strtok_r(NULL, ",", &savePtr);
   }

   tcmMem_free(copy);

   return vendorIds;
}
#endif

#ifdef SUPPORT_IPV6_1 /* aka SUPPORT_IPV6 */
tcmRet tcmUtl_stdIp6Address(const char *address, char *stdAddr)
{
   struct in6_addr in6Addr;

   if (inet_pton(AF_INET6, address, &in6Addr) <= 0)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   if (in6Addr.s6_addr32[0] == 0 && in6Addr.s6_addr32[1] == 0 && in6Addr.s6_addr32[2] == 0 &&
       (in6Addr.s6_addr32[3] == 0 || in6Addr.s6_addr32[3] == 1))
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   inet_ntop(AF_INET6, &in6Addr, stdAddr, BUFLEN_40);

   return TCMRET_SUCCESS;

}  /* End of cmsUtl_stdIp6Address() */

UBOOL8 tcmUtl_isGlobalUnicastAddress(const char *address)
{
   struct in6_addr in6Addr;

   if (inet_pton(AF_INET6, address, &in6Addr) <= 0)
   {
      return FALSE;
   }

   /* currently IANA assigned global unicast address prefix is 001..... */
   if ((in6Addr.s6_addr[0] & 0x20) && !(in6Addr.s6_addr[0] & 0xc0))
   {
      return TRUE;
   }

   return FALSE;

}  /* End of cmsUtl_isGlobalUnicastAddress() */

tcmRet tcmUtl_getAddrPrefix(const char *address, UINT32 plen, char *prefix)
{
   struct in6_addr   in6Addr;
   UINT16 i, k, mask;

   if (plen > 128)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }
   if (inet_pton(AF_INET6, address, &in6Addr) <= 0)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   k = plen / 16;
   mask = 0;
   if (plen % 16)
   {
      mask = ~(UINT16)(((1 << (16 - (plen % 16))) - 1) & 0xFFFF);
   }

   in6Addr.s6_addr16[k] &= mask;

   for (i = k+1; i < 8; i++)
   {
      in6Addr.s6_addr16[i] = 0;
   }

   if (inet_ntop(AF_INET6, &in6Addr, prefix, BUFLEN_40) == NULL)
   {
      return TCMRET_INTERNAL_ERROR;
   }

   return TCMRET_SUCCESS;

}  /* End of cmsUtl_getAddrPrefix() */

tcmRet tcmUtl_replaceEui64(const char *address1, char *address2)
{
   struct in6_addr   in6Addr1, in6Addr2;

   if (inet_pton(AF_INET6, address1, &in6Addr1) <= 0)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }
   if (inet_pton(AF_INET6, address2, &in6Addr2) <= 0)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   in6Addr2.s6_addr32[2] = in6Addr1.s6_addr32[2];
   in6Addr2.s6_addr32[3] = in6Addr1.s6_addr32[3];

   if (inet_ntop(AF_INET6, &in6Addr2, address2, BUFLEN_40) == NULL)
   {
      tcmLog_error("inet_ntop returns NULL");
      return TCMRET_INTERNAL_ERROR;
   }

   return TCMRET_SUCCESS;

}  /* End of cmsUtl_replaceEui64() */

tcmRet tcmUtl_parsePrefixAddress(const char *prefixAddr, char *address, UINT32 *plen)
{
   tcmRet ret = TCMRET_SUCCESS;
   char *tmpBuf;
   char *separator;
   UINT32 len;

   if (prefixAddr == NULL || address == NULL || plen == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   *address = '\0';
   *plen    = 128;

   len = strlen(prefixAddr);

   if ((tmpBuf = tcmMem_alloc(len+1, 0)) == NULL)
   {
      ret = TCMRET_INTERNAL_ERROR;
   }
   else
   {
      sprintf(tmpBuf, "%s", prefixAddr);
      separator = strstr(tmpBuf, "/");
      if (separator != NULL)
      {
         /* break the string into two strings */
         *separator = 0;
         separator++;
         while ((isspace(*separator)) && (*separator != 0))
         {
            /* skip white space after comma */
            separator++;
         }

         *plen = atoi(separator);
      }

      if (strlen(tmpBuf) < BUFLEN_40 && *plen <= 128)
      {
         strcpy(address, tmpBuf);
      }
      else
      {
         ret = TCMRET_INVALID_ARGUMENTS;
      }
      tcmMem_free(tmpBuf);
   }

   return ret;

}  /* End of cmsUtl_parsePrefixAddress() */

#endif


UBOOL8 tcmUtl_ipStrToOctets(const char *input, char *output)
{
   UBOOL8 ret = TRUE;
   char *pToken = NULL;
   char *pLast = NULL;
   char buf[BUFLEN_16];
   UINT32 i, num;

   if (input == NULL || strlen(input) < 7 || strlen(input) > 15)
   {
      return FALSE;
   }

   /* need to copy since strtok_r updates string */
   strcpy(buf, input);

   /* IP address has the following format
      xxx.xxx.xxx.xxx where x is decimal number */
   pToken = strtok_r(buf, ".", &pLast);
   if ((tcmUtl_strtoul(pToken, NULL, 10, &num) != TCMRET_SUCCESS) ||
       (num > 255))
   {
      ret = FALSE;
   }
   else
   {
      output[0] = num;

      for ( i = 0; i < 3; i++ )
      {
         pToken = strtok_r(NULL, ".", &pLast);

         if ((tcmUtl_strtoul(pToken, NULL, 10, &num) != TCMRET_SUCCESS) ||
             (num > 255))
         {
            ret = FALSE;
            break;
         }
         else
         {
            output[i+1] = num;
         }
      }
   }
   return ret;
}
