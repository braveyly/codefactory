#include "singleList.h"
long int gzwSeqNum = 0;
ZwCommand* createZwCmd ( int ctNodeID, char ctCmdAct, char ctCmdStatus,
                         char* ctCmdReserve )
{
    char* buf =  (char *)malloc ( sizeof ( ZwCommand )/sizeof(char) );
    ZwCommand *cmdTmp = ( ZwCommand * ) buf;
    UPDATE_ZW_SEQ_NUMBER ( gzwSeqNum );
    cmdTmp->sequenceID = gzwSeqNum;
    //gettimeofday ( & ( cmdTmp->tmPoint ), NULL );
    cmdTmp->nodeID = ctNodeID;
    cmdTmp->act = ctCmdAct;
    cmdTmp->status = ctCmdStatus;

    if ( NULL != ctCmdReserve )
    {
        strcpy ( cmdTmp->reserve, ctCmdReserve );
    }

    return cmdTmp;
}

int putZwCmdToList ( ZwCommand** cmdList, ZwCommand* cmd )
{
    if ( cmd == NULL )
        return -1;

    if ( *cmdList == NULL )
    {
        *cmdList = cmd;
		(*cmdList)->next = NULL;
    }
    else
    {
        ZwCommand *cmdListTmp = *cmdList;

        while ( NULL != cmdListTmp->next )
            cmdListTmp = cmdListTmp->next;

        cmdListTmp->next = cmd;
		cmd->next = NULL;
    }
    return 0;
}

int freeZwCmd ( ZwCommand* cmd )
{
    if ( NULL == cmd )
    {
        printf ( "cmd is NULL" );
        return -1;
    }

    free ( cmd );
    return 0;

}

int freeZwCmdList ( ZwCommand* cmdList )
{
    if ( NULL == cmdList )
    {
        printf ( "Cmd list is NULL" );
        return -1;
    }

    while ( NULL != cmdList )
    {
        if ( 0 != freeZwCmd ( cmdList ) )
        {
            return -1;
        }
        cmdList = cmdList->next;
    }
    return 0;
}


int deleteZwCmdFromListBySeqID ( ZwCommand** cmdList, long int cmdSeqID )
{
	ZwCommand *cmdListQ = NULL;
	ZwCommand *cmdListP = NULL;
    printf ( "Start to delete cmd which sequence ID is %d\n", cmdSeqID );
    if ( cmdList == NULL )
    {
        printf ( "Cmd List is NULL" );
        return -1;
    }

    if ( cmdSeqID > UINT_SEQ_MAX )
    {
        printf ( "Cmd sequence ID invalid" );
        return -1;
    }


    cmdListQ = *cmdList;
    cmdListP = cmdListQ;
    if ( cmdListQ->sequenceID == cmdSeqID )
    {
        printf ( "Find the sequence ID %d at the first position in "
                        "the queue", cmdSeqID );
        *cmdList = cmdListQ ->next;
        if ( 0 != freeZwCmd ( cmdListQ ) )
        {
            printf ( "Delete cmd error which sequence ID is %d",
                           cmdListQ->sequenceID );
            return -1;
        }
        return 0;
    }


    while ( cmdListQ != NULL )
    {
        if ( cmdListQ->sequenceID == cmdSeqID )
        {
            cmdListP-> next = cmdListQ -> next;
            if ( 0 != freeZwCmd ( cmdListQ ) )
            {
                printf ( "Delete msg wrong" );
                return -1;
            }
            printf ( "Find the sequence ID msg at the non-first"
                            " position" );
            return 0;
            break;
        }
        cmdListP = cmdListQ;
        cmdListQ = cmdListQ -> next;
    }

    printf ( "Donot find the sequence ID %d message", cmdSeqID );
    return 0;
}

ZwCommand* cloneZwCmd ( ZwCommand* zwCmd )
{
	char* buf = NULL;
	ZwCommand *cmdTmp = NULL;

    if ( zwCmd == NULL )
        return NULL;
    
	buf = (char *)malloc ( sizeof ( ZwCommand )/sizeof( char ) );
    cmdTmp = ( ZwCommand * ) buf;
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


int showZwCmd ( ZwCommand* cmd )
{
	printf( "cmd sequence ID %d, node id %d, act %d, status %d\n", cmd->sequenceID, 
		    cmd->nodeID, cmd->act, cmd->status );
	return 1;
}

int showZwCmdList ( ZwCommand* cmdList )
{
	ZwCommand* cmdTmp = cmdList;

	while( cmdTmp != NULL )
	{
		showZwCmd( cmdTmp );
		cmdTmp = cmdTmp->next;
	}
	return 1;
}