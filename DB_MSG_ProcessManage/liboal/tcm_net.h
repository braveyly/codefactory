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

#ifndef __TCM_NET_H__
#define __TCM_NET_H__

#include "tcm.h"
#include <arpa/inet.h>  /* mwang_todo: should not include OS dependent file */

tcmRet tcmNet_getLanInfo(const char *ifname, struct in_addr *lan_ip, struct in_addr *lan_subnetmask);

UBOOL8 tcmNet_isInterfaceUp(const char *ifname);

UBOOL8 tcmNet_isAddressOnLanSide(const char *ipAddr);

SINT32 tcmNet_getLeftMostOneBitsInMask(char *mask);

SINT32 tcmNet_getIfindexByIfname(char *ifname);


#ifdef SUPPORT_IPV6_1
tcmRet tcmNet_getIfAddr6(const char *ifname, UINT32 addrIdx,
                         char *ipAddr, UINT32 *ifIndex, UINT32 *prefixLen, UINT32 *scope, UINT32 *ifaFlags);
UBOOL8 tcmNet_areIp6AddrEqual(const char *ip6Addr1, const char *ip6Addr2);
UBOOL8 tcmNet_areIp6DnsEqual(const char *dnsServers1, const char *dnsServers2);
UBOOL8 tcmNet_areIp6SubnetPrefixEqual(const char *sp1, const char *sp2);
CmsRet tcmNet_subnetIp6SitePrefix(const char *sp, UINT8 subnetId, UINT32 snPlen, char *snPrefix);
#endif

/** first sub interface number for virtual ports, e.g. eth1.2, eth1.3 */
#define START_PMAP_ID           2

/** Max vendor id string len */
#define DHCP_VENDOR_ID_LEN      64

/** Maximum number of vendor id strings we can support in WebUI for portmapping.
 *
 * This is an arbitrary limit from the WebUI, but it propagates through to
 * utility functions dealing with DHCP Vendor Id for port mapping.
 */
#define MAX_PORTMAPPING_DHCP_VENDOR_IDS     5


#endif /* __TCM_NET_H__ */
