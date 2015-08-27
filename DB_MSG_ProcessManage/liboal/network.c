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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
//#include <net/if.h>

#include "tcm.h"
#include "tcm_util.h"

static SINT32 tcmNet_getLeftMostOneBits(SINT32 num);

tcmRet tcmNet_getLanInfo(const char *lan_ifname, struct in_addr *lan_ip, struct in_addr *lan_subnetmask)
{
   int socketfd;
   struct ifreq lan;

   if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      return TCMRET_INTERNAL_ERROR;
   }

   strcpy(lan.ifr_name,lan_ifname);
   if (ioctl(socketfd,SIOCGIFADDR,&lan) < 0) {
      close(socketfd);
      return TCMRET_INTERNAL_ERROR;
   }
   *lan_ip = ((struct sockaddr_in *)&(lan.ifr_addr))->sin_addr;

   if (ioctl(socketfd,SIOCGIFNETMASK,&lan) < 0) {
      close(socketfd);
      return TCMRET_INTERNAL_ERROR;
   }

   *lan_subnetmask = ((struct sockaddr_in *)&(lan.ifr_netmask))->sin_addr;

   close(socketfd);
   return TCMRET_SUCCESS;
}


UBOOL8 cmsNet_isInterfaceUp(const char *ifname)
{
   int  skfd;
   struct ifreq intf;
   UBOOL8 isUp = FALSE;

   if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      return FALSE;
   }

   strcpy(intf.ifr_name, ifname);

   // if interface flag is down then return down
   if (ioctl(skfd, SIOCGIFFLAGS, &intf) == -1) {
      isUp = 0;
   } else {
      isUp = (intf.ifr_flags & IFF_UP) ? TRUE : FALSE;
   }

   close(skfd);

   return isUp;
}


UBOOL8 cmsNet_isAddressOnLanSide(const char *ipAddr)
{
   UBOOL8 onLanSide = FALSE;

   return onLanSide;

}  /* End of cmsNet_isAddressOnLanSide() */


SINT32 tcmNet_getLeftMostOneBits(SINT32 num)
{
   int pos = 0;
   int numArr[8] = {128, 64, 32, 16, 8, 4, 2, 1};

   // find the left most zero bit position
   for ( pos = 0; pos < 8; pos++ )
   {
      if ( (num & numArr[pos]) == 0 )
         break;
   }

   return pos;
}

SINT32 tcmNet_getLeftMostOneBitsInMask(char *mask)
{
   char *pToken = NULL;
   char *pLast = NULL;
   char buf[BUFLEN_16];
   int num = 0, total = 0;

   if ( mask == NULL )
      return total;

   // need to copy since strtok_r updates string
   strcpy(buf, mask);

   // mask has the following format
   //   xxx.xxx.xxx.xxx where x is decimal number
   pToken = strtok_r(buf, ".", &pLast);
   if ( pToken == NULL )
      return total;

   num = tcmNet_getLeftMostOneBits(atoi(pToken));
   total += num;
   while ( num == 8 )
   {
      pToken = strtok_r(NULL, ".", &pLast);
      if ( pToken == NULL )
      {
         break;
      }
      num = tcmNet_getLeftMostOneBits(atoi(pToken));
      total += num;
   }

   return total;
}

SINT32 tcmNet_getIfindexByIfname(char *ifname)
{
   int sockfd;
   int idx;
   int ifindex = -1;
   struct ifreq ifr;

   /* open socket to get INET info */
   if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) <= 0)
   {
      //tcmLog_error("socket returns error. sockfd=%d", sockfd);
      return -1;
   }

   for (idx = 2; idx < 32; idx++)
   {
      memset(&ifr, 0, sizeof(struct ifreq));
      ifr.ifr_ifindex = idx;

      if (ioctl(sockfd, SIOCGIFNAME, &ifr) >= 0)
      {
         if (strcmp(ifname, ifr.ifr_name) == 0)
         {
            ifindex = idx;
            break;
         }
      }
   }

   close(sockfd);

   return ifindex;

}  /* End of cmsNet_getIfindexByIfname() */


#ifdef SUPPORT_IPV6_1 /* aka SUPPORT_IPV6 */

tcmRet tcmNet_getIfAddr6(const char *ifname, UINT32 addrIdx,
                         char *ipAddr, UINT32 *ifIndex, UINT32 *prefixLen, UINT32 *scope, UINT32 *ifaFlags)
{
   tcmRet   ret = TCMRET_NO_MORE_INSTANCES;
   FILE     *fp;
   SINT32   count = 0;
   char     line[BUFLEN_64];

   *ipAddr = '\0';

   if ((fp = fopen("/proc/net/if_inet6", "r")) == NULL)
   {
      return TCMRET_INTERNAL_ERROR;
   }

   while (fgets(line, sizeof(line), fp) != NULL)
   {
      /* remove the carriage return char */
      line[strlen(line)-1] = '\0';

      if (strstr(line, ifname) != NULL)
      {
         char *addr, *ifidx, *plen, *scp, *flags, *devname;
         char *nextToken = NULL;

         /* the first token in the line is the ip address */
         addr = strtok_r(line, " ", &nextToken);

         /* the second token is the Netlink device number (interface index) in hexadecimal */
         ifidx = strtok_r(NULL, " ", &nextToken);
         if (ifidx == NULL)
         {
            ret = TCMRET_INTERNAL_ERROR;
            break;
         }

         /* the third token is the Prefix length in hexadecimal */
         plen = strtok_r(NULL, " ", &nextToken);
         if (plen == NULL)
         {
            ret = TCMRET_INTERNAL_ERROR;
            break;
         }

         /* the forth token is the Scope value */
         scp = strtok_r(NULL, " ", &nextToken);
         if (scp == NULL)
         {
            ret = TCMRET_INTERNAL_ERROR;
            break;
         }

         /* the fifth token is the ifa flags */
         flags = strtok_r(NULL, " ", &nextToken);
         if (flags == NULL)
         {
            ret = TCMRET_INTERNAL_ERROR;
            break;
         }

         /* the sixth token is the device name */
         devname = strtok_r(NULL, " ", &nextToken);
         if (devname == NULL)
         {
            ret = TCMRET_INTERNAL_ERROR;
            break;
         }
         else
         {
            if (strcmp(devname, ifname) != 0)
            {
               continue;
            }
            else if (count == addrIdx)
            {
               SINT32   i;
               char     *p1, *p2;

               *ifIndex   = strtoul(ifidx, NULL, 16);
               *prefixLen = strtoul(plen, NULL, 16);
               *scope     = strtoul(scp, NULL, 16);
               *ifaFlags  = strtoul(flags, NULL, 16);

               /* insert a colon every 4 digits in the address string */
               p2 = ipAddr;
               for (i = 0, p1 = addr; *p1 != '\0'; i++)
               {
                  if (i == 4)
                  {
                     i = 0;
                     *p2++ = ':';
                  }
                  *p2++ = *p1++;
               }
               *p2 = '\0';

               ret = TCMRET_SUCCESS;
               break;   /* done */
            }
            else
            {
               count++;
            }
         }
      }
   }  /* while */

   fclose(fp);

   return ret;
}

#endif

