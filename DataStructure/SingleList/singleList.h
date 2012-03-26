#ifndef _ZW_LIST_H_
#define _ZW_LIST_H_
#include <stdio.h>

typedef struct ZWAVECOMMAND
{
    long int sequenceID;
    long int tmPoint;
    int nodeID;
    char act;
    char status;
    char reserve[10];
    struct ZWAVECOMMAND* next;
}ZwCommand;

#define UINT_SEQ_MAX 2147483647
extern long int gzwSeqNum;
#define UPDATE_ZW_SEQ_NUMBER(num) {if(num == UINT_SEQ_MAX) num=0;else ++num;}

ZwCommand* gzwCmdList;
ZwCommand* createZwCmd ( int ctNodeID, char ctCmdAct, char ctCmdStatus,
                         char* ctCmdReserve );
int freeZwCmd ( ZwCommand* cmd );
int freeZwCmdList ( ZwCommand* cmdList );
int putZwCmdToList ( ZwCommand** cmdList, ZwCommand* cmd );
int deleteZwCmdFromListBySeqID ( ZwCommand** cmdList, long int cmdSeqID );
int showZwCmd ( ZwCommand* cmd );
int showZwCmdList ( ZwCommand* cmdList );
int checkZwCmdList ( ZwCommand* cmdList );



#endif
