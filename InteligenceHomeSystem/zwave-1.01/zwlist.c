#include "zwlist.h"
uint32_t gzwSeqNum = 0;
ZwCommand* createZwCmd ( uint32_t ctNodeID, uint8_t ctCmdAct, uint8_t ctCmdStatus,
                         uint8_t* ctCmdReserve )
{
    char* buf =  tcmMem_alloc ( sizeof ( ZwCommand ), ALLOC_ZEROIZE );
    ZwCommand *cmdTmp = ( ZwCommand * ) buf;
    UPDATE_ZW_SEQ_NUMBER ( gzwSeqNum );
    cmdTmp->sequenceID = gzwSeqNum;
    gettimeofday ( & ( cmdTmp->tmPoint ), NULL );
    cmdTmp->nodeID = ctNodeID;
    cmdTmp->act = ctCmdAct;
    cmdTmp->status = ctCmdStatus;

    if ( NULL != ctCmdReserve )
    {
        strcpy ( cmdTmp->reserve, ctCmdReserve );
    }

    return cmdTmp;
}

tcmRet putZwCmdToList ( ZwCommand** cmdList, ZwCommand* cmd )
{
    if ( cmd == NULL )
        return TCMRET_INVALID_PARAMETER;

    if ( *cmdList == NULL )
    {
        *cmdList = cmd;
    }
    else
    {
        ZwCommand *cmdListTmp = *cmdList;

        while ( NULL != cmdListTmp->next )
            cmdListTmp = cmdListTmp->next;

        cmdListTmp->next = cmd;
    }
    return TCMRET_SUCCESS;
}

tcmRet freeZwCmd ( ZwCommand* cmd )
{
    if ( NULL == cmd )
    {
        //tcmLog_notice ( "cmd is NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    tcmMem_free ( cmd );
    return TCMRET_SUCCESS;

}

tcmRet freeZwCmdList ( ZwCommand* cmdList )
{
    if ( NULL == cmdList )
    {
        tcmLog_debug ( "Cmd list is NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    while ( NULL != cmdList )
    {
        if ( TCMRET_SUCCESS != freeZwCmd ( cmdList ) )
        {
            return TCMRET_FAILURE;
        }
        cmdList = cmdList->next;
    }
    return TCMRET_SUCCESS;
}


tcmRet deleteZwCmdFromListBySeqID ( ZwCommand** cmdList, uint32_t cmdSeqID )
{
    tcmLog_debug ( "Start to delete cmd which sequence ID is %d\n", cmdSeqID );
    if ( cmdList == NULL )
    {
        tcmLog_debug ( "Cmd List is NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    if ( cmdSeqID > UINT_SEQ_MAX )
    {
        tcmLog_notice ( "Cmd sequence ID invalid" );
        return TCMRET_INVALID_PARAMETER;
    }

    ZwCommand *cmdListQ, *cmdListP;
    cmdListQ = *cmdList;
    cmdListP = cmdListQ;
    if ( cmdListQ->sequenceID == cmdSeqID )
    {
        tcmLog_notice ( "Find the sequence ID %d at the first position in "
                        "the queue", cmdSeqID );
        *cmdList = cmdListQ ->next;
        if ( TCMRET_SUCCESS != freeZwCmd ( cmdListQ ) )
        {
            tcmLog_error ( "Delete cmd error which sequence ID is %d",
                           cmdListQ->sequenceID );
            return TCMRET_FAILURE;
        }
        return TCMRET_SUCCESS;
    }


    while ( cmdListQ != NULL )
    {
        if ( cmdListQ->sequenceID == cmdSeqID )
        {
            cmdListP-> next = cmdListQ -> next;
            if ( TCMRET_SUCCESS != freeZwCmd ( cmdListQ ) )
            {
                tcmLog_error ( "Delete msg wrong" );
                return TCMRET_FAILURE;
            }
            tcmLog_notice ( "Find the sequence ID msg at the non-first"
                            " position" );
            return TCMRET_SUCCESS;
            break;
        }
        cmdListP = cmdListQ;
        cmdListQ = cmdListQ -> next;
    }

    tcmLog_notice ( "Donot find the sequence ID %d message", cmdSeqID );
    return TCMRET_SUCCESS;
}

ZwCommand* cloneZwCmd ( ZwCommand* zwCmd )
{
    if ( zwCmd == NULL )
        return NULL;

    char* buf =  tcmMem_alloc ( sizeof ( ZwCommand ), ALLOC_ZEROIZE );
    ZwCommand *cmdTmp = ( ZwCommand * ) buf;
    cmdTmp->sequenceID = zwCmd->sequenceID;
    //gettimeofday ( & ( cmdTmp->tmPoint ), NULL );
    cmdTmp->nodeID = zwCmd->nodeID;
    cmdTmp->act = zwCmd->act;
    cmdTmp->status = zwCmd->status;

    if ( NULL != zwCmd->reserve )
    {
        strcpy ( cmdTmp->reserve, zwCmd->reserve );
    }

    return cmdTmp;
}


