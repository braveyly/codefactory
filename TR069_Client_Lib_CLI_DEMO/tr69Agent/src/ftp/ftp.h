#ifndef _FTP_H_
#define _FTP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "pal.h"
#include "ftp_public.h"
#include "tr69_agent_public.h"

#define FTP_CMD_NONE ""
#define FTP_CMD_USER "USER"
#define FTP_CMD_PASS "PASS"
#define FTP_CMD_PASV "PASV"
#define FTP_CMD_TYPE "TYPE"
#define FTP_CMD_RETR "RETR"
#define FTP_CMD_CWD "CWD"
#define FTP_CMD_PWD "PWD"
#define FTP_CMD_STOR "STOR"
#define FTP_CMD_PORT "PORT"
#define FTP_CMD_MKD "MKD"
#define FTP_CMD_SIZE "SIZE"

#define FTP_RCV_RESP_TIMEOUT 3000

#define FTP_REQ_BUF_LEN 128
#define FTP_RESP_BUF_LEN 128
#define FTP_URL_PARAM_LEN 256

#define FTP_RCV_BUF_SIZE 1460

#endif /* _FTP_H_ */
