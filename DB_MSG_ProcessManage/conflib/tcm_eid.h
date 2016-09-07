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

#ifndef __TCM_EID_H__
#define __TCM_EID_H__

#define MAXTABLE_PORTFORWARD  32
#define MAXTABLE_FIREWALL     32
#define MAXTABLE_ROUTE        32


/*!\file tcm_eid.h
 * \brief Header file containing entity id assignments in
 * the CPE Management System (CMS).
 */

/* Entity List below */
typedef enum
{
    ID_LAN = 1,
    ID_DHCPD,
    ID_END,
    /* Please keep it at the end */
    ID_ALLEND,
} tcmEntityId;

/** Entity is a server, which opens a socket and listens for requests.
 *
 * This flag is used in CmsEntityInfo.flags below.
 */
#define APPCFG_SERVER                    0x01


/** If entity is a server, server socket uses TCP (otherwise, uses UDP)
 *
 * This flag is used in CmsEntityInfo.flags below.
 */
#define APPCFG_SERVER_TCP                0x02


/** Entity should be launched during stage 1 of CMS startup..
 *
 * This flag is used in CmsEntityInfo.flags below.
 * So far, the only apps we launch during stage 1 are pvc2684d and ssk.
 */
#define APPCFG_LAUNCH_IN_STAGE_1         0x04


/** Entity should be launched on system boot (during stage 2).
 *
 * This flag is used in CmsEntityInfo.flags below.
 */
#define APPCFG_LAUNCH_ON_BOOT            0x08


/** Entity will need access to the MDM.
 *
 * This flag is used in CmsEntityInfo.flags below.
 * This flag will cause smd to pass the MDM shared memory id to the
 * app in the -m option.
 * If the entity is going to access the MDM, it must have a NDA_ACCESS_xxx
 * bit defined for it.
 */
#define APPCFG_MDM                       0x10


/** Entity can run on the Desktop Linux.
 *
 * Some entities, such as httpd, can run on the Desktop Linux as
 * well as modems.  Others, such as dhcpd, can only run on the modem.
 */
#define APPCFG_DESKTOP_LINUX_CAPABLE     0x20


/** Entity will create a cms_msg communications link to smd. */
#define APPCFG_MESSAGING_CAPABLE         0x40


/** Entity can have multiple instances, currently only ppp and dhcpc */
#define APPCFG_MULTIPLE_INSTANCES        0x80


/** Entity supports IPv6. */
#define APPCFG_IPV6                      0x100

#define APPCFG_AUTO_RELAUNCH             0x200



/** A structure that contains info about an CmsEntityId.
 *
 * For each new CmsEntityId defined, add a corresponding entry to the
 * EntityInfoArray in cms_util/eid.c
 *
 */
typedef struct parm_entry
{
   int id;
   char *name;
} tcmParamInfo;

typedef struct apps_info
{
    tcmEntityId         eid;
    char                *name;
    char                *path;
    char                *runArgs;
    UINT16              flags;
    UINT8               backLog;    /* As Linux defined, max number of connection allowed */
    UINT16              port;
    UBOOL8              featureCompiled;
    UINT32              app_maxnum;
    tcmParamInfo        *row_paras;
    UINT32              row_maxnum;
    UINT32              row_end;
} tcmEntityInfo;

const tcmEntityInfo *tcmEid_getEntityInfo(tcmEntityId eid);

const tcmEntityInfo *tcmEid_getEntityInfoByStringName(const char *stringName);

SINT32 tcmEid_getPid(void);

UINT32 tcmEid_getNumberOfEntityInfoEntries(void);

const tcmEntityInfo *tcmEid_getFirstEntityInfo(void);

/** Mask to get the generic eid out of a UINT32 eid field. */
#define EID_EID_MASK  0x0000ffff

/** Mask to get the pid out of a UINT32 eid field */
#define EID_PID_MASK  0xffff0000

/** Get the generic eid out of a UINT32 eid field */
#define GENERIC_EID(e) (((UINT32) e) & EID_EID_MASK)

/** Get the pid out of a UINT32 eid field */
#define PID_IN_EID(e) ((((UINT32) e) & EID_EID_MASK) >> 16)

/** Make a specific UINT32 eid field out of a pid and a generic eid */
#define MAKE_SPECIFIC_EID(p, e) ((p << 16) | (((UINT32) e) & EID_EID_MASK))

#endif  /* __TCM_EID_H__ */
