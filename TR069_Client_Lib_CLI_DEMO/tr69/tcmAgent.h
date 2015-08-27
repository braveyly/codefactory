/**********************************************************************
Copyright (c) 2009 Wuhan Tecom Co., Ltd.
All Rights Reserved
No portions of this material may be reproduced in any form without the
written permission of:
Wuhan Tecom Co., Ltd.
#18, Huaguang Road
Wuhan, PR China 430074
All information contained in this document is Wuhan Tecom Co., Ltd.
company private, proprietary, and trade secret.
************************************************************************/

#ifndef _DEMO_H_
#define _DEMO_H_


#include <sys/param.h> 
#include <netinet/in.h> 
#include <net/if_arp.h> 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>      /* open */
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include<sys/stat.h>
#include "tcm_log.h"
#include "tr69_agent.h"

#define TCMAGENT_LOG

void tcmAgent_printf(const char *func, UINT32 lineNum, char *fmt, ...);

#ifdef TCMAGENT_LOG
#define  TR069_DEBUG(args...) tcmLog_debug(args); tcmAgent_printf(__FUNCTION__, __LINE__,args)
#define  TR069_NOTICE(args...) tcmLog_notice(args); tcmAgent_printf(__FUNCTION__, __LINE__,args)
#define  TR069_ERROR(args...) tcmLog_error(args); tcmAgent_printf(__FUNCTION__, __LINE__,args)
#else
#define  TR069_DEBUG(args...) tcmLog_debug(args)
#define  TR069_NOTICE(args...) tcmLog_notice(args)
#define  TR069_ERROR(args...) tcmLog_error(args)
#endif


/*For parsing name*/
#define TEMPLATE_STR ".0."
#define TEMPLATE_LEN 3

/*assume that row ID max number is 99*/
#define MAX_INSTANCE_ID_LEN 2

/*assume that row max parameter number is 8*/
#define MAX_NUM_OF_ROW_PARA 8

/* For tcmCB_get_wan_ipaddress*/
#define MAXINTERFACES 16

/* For configuration.xml in main*/
#define MIN_TEMPLATE_FILE_SIZE (1024 + 1)

/* For download and upload */
#define LOG_DIR "./conf/"
#define CONFIG_DIR "/var/update/"
#define FIRMWARE_DIR   "/var/update/"
#define WEBCONTENT_DIR "/var/update/"
#define MAX_FILENAME_LEN 256
#define SOFTWARE_VERSION_LEN 128

#define MAX_FILE_SIZE 64*1000 /* it's enough */
#define configName "/home/httpd/os.conf"
#define logName    "/home/httpd/messages"
#define FIRMWARE_NAME "GCT-FSSU.img"

/*****for wimax certs download*****/
#define CA_CERT_SUFFIX ".ca.pem"
#define CLIENT_CERT_SUFFIX ".client.pem"
#define KEY_CERT_SUFFIX ".key.pem"
#define CA_CERT_FLAG 1
#define CLIENT_CERT_FLAG 2
#define KEY_CERT_FLAG 3
#define AUTH_DIR "/etc/sysconfig/os/certs"
/**********************************/
typedef ApplyStatus ( *get_status_func ) ( char *, char* );
typedef ApplyStatus ( *get_multi_object_status_func ) ( char *, char *, int );


#endif /* _DEMO_H_ */
