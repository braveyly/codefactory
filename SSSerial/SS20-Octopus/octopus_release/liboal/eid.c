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

#include "tcm.h"
//#include "tcm_util.h"
#include "tcm_eid.h"
//#include "conflib.h"

tcmParamInfo portforward_tab_paras[] =
{
    { PARA_PORTFORWARD_NAME,                  "name" },
    { PARA_PORTFORWARD_PROTOCOL,              "protocol" },
    { PARA_PORTFORWARD_START_INCOMING_PORT,   "startIncomingPort" },
    { PARA_PORTFORWARD_END_INCOMING_PORT,     "endIncomingPort" },
    { PARA_PORTFORWARD_START_FORWARD_PORT,    "startForwardPort" },
    { PARA_PORTFORWARD_END_FORWARD_PORT,      "endForwardPort" },
    { PARA_PORTFORWARD_SERVER_IP,             "serverIp" },
    { PARA_PORTFORWARD_END,                   NULL },
};

tcmParamInfo firewall_tab_paras[] = 
{
    { PARA_FIREWALL_SRCIP,                    "sourceIp" },
    { PARA_FIREWALL_SRCMASK,                  "sourceMask" },
    { PARA_FIREWALL_SRCPORT,                  "sourcePort" },
    { PARA_FIREWALL_DSTIP,                    "destinationIp" },
    { PARA_FIREWALL_DSTMASK,                  "destinationMask" },
    { PARA_FIREWALL_DSTPORT,                  "destinationPort" },
    { PARA_FIREWALL_PROTOCOL,                 "protocol" },
    { PARA_FIREWALL_END,                      NULL },
};

tcmParamInfo route_tab_paras[] = 
{
    { PARA_ROUTE_DSTIP,                       "destinationIp" },
    { PARA_ROUTE_MASK,                        "mask" },
    { PARA_ROUTE_NEXTHOPTYPE,                 "nexthoptype" },
    { PARA_ROUTE_NEXTHOPTYPEINTERFACE,        "nexthopintf" },
    { PARA_ROUTE_NEXTHOPTYPEIP,               "nexthopip" },
    { PARA_ROUTE_METRIC,                      "metric" },
    { PARA_ROUTE_DSTIP,                       NULL },
};

tcmEntityInfo entityInfoArray[] = {
   {ID_LAN,
    "lan",
    "/home/nick/lan",
    "",                 /* run args */
    APPCFG_LAUNCH_IN_STAGE_1,
    0,                  /* Back Blog */
    0,                  /* Port */
    TRUE,               /* isFeatureCompiledIn */
    PARA_LAN_END,
    NULL,
    0, 0},


   {ID_DHCPD,
    "dhcpd",
    "/home/nick/dhcpddd",
    "",                 /* run args */
    APPCFG_LAUNCH_IN_STAGE_1,
    0,                  /* Back Blog */
    0,                  /* Port */
    TRUE,               /* isFeatureCompiledIn */
    PARA_DHCPD_END,
    NULL,
    0, 0},
};

#define NUM_ENTITY_INFO_ENTRIES (sizeof(entityInfoArray)/sizeof(tcmEntityInfo))


UINT32 tcmEid_getNumberOfEntityInfoEntries(void)
{
   return NUM_ENTITY_INFO_ENTRIES;
}


const tcmEntityInfo *tcmEid_getFirstEntityInfo(void)
{
   return &(entityInfoArray[0]);
}


const tcmEntityInfo *tcmEid_getEntityInfo(tcmEntityId eid)
{
   UINT32 i=0;

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (entityInfoArray[i].eid == eid)
      {
         return (&entityInfoArray[i]);
      }

      i++;
   }

   return NULL;
}

const tcmEntityInfo *tcmEid_getEntityInfoByStringName(const char *name)
{
   UINT32 i=0;

   if (name == NULL)
   {
      return NULL;
   }

   while (i < NUM_ENTITY_INFO_ENTRIES)
   {
      if (!strcmp(entityInfoArray[i].name, name))
      {
         return (&entityInfoArray[i]);
      }

      i++;
   }

   return NULL;
}

SINT32 tcmEid_getPid(void)
{
   return (getpid());
}

