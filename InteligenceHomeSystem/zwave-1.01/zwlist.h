#ifndef _ZW_LIST_H_
#define _ZW_LIST_H_
#include <conflib.h>
#include <tcm_mem.h>
#include <tcm_log.h>
#include "zwlist.h"

#define LOCAL_MSG_VALUE_LEN 16
#define NODE_ID_OFFSET 0
#define ACT_OFFSET     4
#define STATUS_OFFSET  5
#define RESERVE_OFFSET 6
#define START_ADD_NODE 100
#define STOP_ADD_NODE  101
#define START_DEL_NODE 102
#define STOP_DEL_NODE  103
#define CONTROL_BINARY_SWITCH 104
#define CONTROL_DIMMER 105
#define CONTROL_METER 106
#define CONTROL_CURTAIN 107

typedef struct ZWAVECOMMAND
{
    uint32_t sequenceID;
    struct timeval tmPoint;
    uint32_t nodeID;
    uint8_t act;
    uint8_t status;
    uint8_t reserve[10];
    struct ZWAVECOMMAND* next;
}ZwCommand;
#define UINT_SEQ_MAX 2147483647
extern uint32_t gzwSeqNum;
#define UPDATE_ZW_SEQ_NUMBER(num) {if(num == UINT_SEQ_MAX) num=0;else ++num;}

ZwCommand* gzwCmdList;
ZwCommand* createZwCmd ( uint32_t ctNodeID, uint8_t ctCmdAct, uint8_t ctCmdStatus,
                         uint8_t* ctCmdReserve );
tcmRet freeZwCmd ( ZwCommand* cmd );
tcmRet freeZwCmdList ( ZwCommand* cmdList );
tcmRet putZwCmdToList ( ZwCommand** cmdList, ZwCommand* cmd );
tcmRet deleteZwCmdFromListBySeqID ( ZwCommand** cmdList, uint32_t cmdSeqID );
tcmRet showZwCmd ( ZwCommand* cmd );
tcmRet showZwCmdList ( ZwCommand* cmdList );
tcmRet checkZwCmdList ( ZwCommand* cmdList );



#endif
