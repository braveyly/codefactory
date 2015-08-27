#include "pal_debug.h"
#include "pal_log.h"
#include "stdio.h"
#include "string.h"
#include "sysLib.h"





typedef struct _tagTCDBGMsg
{
	PAL_INT32_T iTaskID;
//	int iGroupID;
	PAL_INT32_T iType;
	int iLength;
	PAL_INT8_T szModule[13];
	PAL_INT8_T * sDbgString;
	PAL_INT32_T iEvent;
}TCDBGMsg;

#define TCDEBUG_MAX_MSG				64
#define TCDEBUG_MAX_STRING_LENGTH	192
#define TCDEBUG_MAX_QUEUE_LENGTH	TCDEBUG_MAX_MSG
//Control flags
MSG_Q_ID _tcdbgMsgQueueIDTR069;
extern MSG_Q_ID _tcdbgMsgQueueID;



void tcdebugPrintfExTR069(PAL_INT8_T * pModule, PAL_INT8_T * sendbuf, PAL_INT32_T iType)
{
	PAL_INT8_T * msgBuf;
	va_list arg_ptr;
	PAL_INT32_T iLen, ret;
//	int iTaskID;
	TCDBGMsg msgSend;


	msgBuf = (char *)malloc(TCDEBUG_MAX_STRING_LENGTH);
	if(msgBuf==NULL)
		return;
	/*va_start(arg_ptr,format);
	vsprintf(msgBuf,format,arg_ptr);
	va_end(arg_ptr);
        printf("msgBuf1 = %s, %d\n", msgBuf, strlen(msgBuf));*/
        if(sendbuf != NULL)
        {
        strcpy(msgBuf, sendbuf);
        }
	if(strlen(msgBuf)>=(TCDEBUG_MAX_STRING_LENGTH-1))
	{
		free(msgBuf);
		return;
	}

	iLen=strlen(msgBuf);
	msgSend.iType = iType;
	msgSend.iLength = iLen;
	msgSend.sDbgString = msgBuf;
	if(strlen(pModule)>13)
	{
		msgSend.szModule[0]=0;
	}
	else
	{
		strcpy(msgSend.szModule,pModule);
	}
    taskDelay(200);
    if ( ( ret = msgQSend ( _tcdbgMsgQueueID, ( char * ) & msgSend, sizeof ( TCDBGMsg ), NO_WAIT, MSG_PRI_NORMAL ) ) != 0 )
	{
		free(msgBuf);
		return;
	}

}

void InitTcDebugTR069()
{
	//_tcdbgMsgQueueIDTR069 = msgQCreate( TCDEBUG_MAX_QUEUE_LENGTH, sizeof(TCDBGMsg), MSG_Q_FIFO );
	_tcdbgMsgQueueIDTR069 = pal_create_msg(0);

	if( _tcdbgMsgQueueIDTR069==NULL )//something wrong
	{
		printf("Fail to create message queue for tcdebug task\n");
		return;
	}

}




