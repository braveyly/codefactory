#include <pthread.h>
#include <configact.h>
#include <configapi.h>
#include <tcm_eid.h>
#include <tcm_msg.h>
#include <tcm_log.h>
#include "zwctrl.h"
#include "zwlist.h"
#include "zwave.h"
#include "zwdb.h"

void *   msgHandle = NULL;
const tcmEntityId myId = ID_ZWCTRL;
extern DONGLE_INFO device_info;
extern BYTE cmd_exe_status;

int tcmLib_init()
{
    tcmRet ret;

    /*Init the log library*/
    tcmLog_init ( myId );

    /* Init the message library */
    if ( ( ret = tcmMsg_init ( myId, &msgHandle ) ) != TCMRET_SUCCESS )
    {
        tcmLog_cleanup();
        tcmLog_error ( "Fail to init the msg handle: %d", ret );
        return -1;
    }

    /* Init the conflib */
    if ( ( ret = capi_init_library() ) < 0 )
    {
        tcmMsg_cleanup ( &msgHandle );
        tcmLog_cleanup();
        tcmLog_error ( "Fail to init the library: %d", ret );
        return -1;
    }

    /* Init the act library */
    if ( ( ret = actLib_init ( myId, msgHandle ) ) != TCMRET_SUCCESS )
    {
        capi_uninit_library();
        tcmMsg_cleanup ( &msgHandle );
        tcmLog_cleanup();
        tcmLog_error ( "Fail to init act library: %d", ret );
        return -1;
    }
    
    actUtil_setLogLevel();

    return 0;
}

tcmRet APSetSwitchLight ( int stNodeID, int stAct )
{
    tcmLog_debug ( "Control binary switch %d to act %d", stNodeID, stAct );
    API_SetBinarySwitch ( stNodeID, stAct );
    tcmLog_debug ( "End APSetSwitchLight" );
    return TCMRET_SUCCESS;
}

tcmRet APSetDimmerLight ( int stNodeID, int stAct )
{
    int dimmerLightValue = 0;

    if( 0 != stAct )
    {
       dimmerLightValue = stAct*10 -1;
    }
    tcmLog_debug ( "Control dimmerlight %d to act %d", stNodeID, dimmerLightValue );
    API_SetDimmer ( stNodeID, dimmerLightValue );
    tcmLog_debug ( "End APSetDimmerLight" );
    return TCMRET_SUCCESS;
}

tcmRet APSetCurtain ( int stNodeID, int stAct )
{
    tcmLog_debug ( "Control curtain %d to act %d", stNodeID, stAct );
    API_SetCurtain( stNodeID, stAct );
    tcmLog_debug ( "End APSetCurtain" );
    return TCMRET_SUCCESS;
}

tcmRet APSetIRC ( int stNodeID, int stAct )
{
    tcmLog_debug ( "Control TV/DVD/AC %d to act %d", stNodeID, stAct );
    API_SetAVDevice( stNodeID, stAct );
    tcmLog_debug ( "End APSetIRC" );
    return TCMRET_SUCCESS;
}

#define DONGLE_DEVICE_ID 0x01
tcmRet APAssociationIRSensor( uint8_t IRsensorID, uint8_t AssociationID )
{
   int ret;
   ret = API_AssociationSetandRemoveandGet( ASSOCIATIONSET, IRsensorID, AssociationID );
   if(ret == STATUS_OK)
   {
	  tcmLog_debug("Associate OK\n");
      return TCMRET_SUCCESS;
   }	
   else
   {
	  tcmLog_error("get error %d\n", ret);
      return TCMRET_FAILURE;
   }
}

tcmRet startLocalMsgSelectThread()
{
    pthread_t tid;
    struct sched_param sched;
    pthread_attr_t  attr;

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_init " );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_setschedpolicy " );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_create ( &tid, &attr, startRecvbySelect, NULL ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_create \n" );
        return TCMRET_FAILURE;
    }

    return TCMRET_SUCCESS;
}


void CBAddNode ( BYTE state, ADDREMOVE_NODE_INFO* p )
{
    tcmLog_debug( "Enter CBAddNode" );
    if ( state == STATUS_ADDNODEDONE || state == STATUS_REMOVENODEDONE )
    {
        if ( p->dev_num == 1 )
        {
            tcmLog_debug ( "ap callback, state:%d, id:%d, type:%d\n",
                           state,
                           p->id[0],
                           p->type[0] );
            ADDREMOVE_NODE_INFO nodeinfo;
            int ret;
            nodeinfo.id[0] = p->id[0];
            nodeinfo.type[0] = p->type[0];
            ret = addZunitIntoDbByNode ( nodeinfo, gaddDeviceProfileID );
            if ( ret == TCMRET_SUCCESS )
            {
                updateZunitAddandDeleteFlag ( nodeinfo.id[0],
                                              NOT_UPDATE_ADD_DELETE_FLAG );
            }
            else
            {
                updateZunitAddandDeleteFlag ( 0, NOT_UPDATE_ADD_DELETE_FLAG );
            }

            if( p->type[0] == DEVICE_IR_SENSOR )
            {
                ret = APAssociationIRSensor( p->id[0], DONGLE_DEVICE_ID );

                if( ret == STATUS_OK )
                {
                   tcmLog_debug( "Association OK" );
                }
                else
                {
                   tcmLog_error( "Association error" );
                }
            }
            
            //sleep ( 10 );
            updateZunitStatus ( nodeinfo.id );
        }
        else
        {
            tcmLog_debug ( "Add multiple nodes, total number is %d", 
                            p->dev_num );
            int i;
            int ret;
            ADDREMOVE_NODE_INFO nodeinfo;
            for(i=0;i<(p->dev_num);i++)
            {
                memset( &nodeinfo, 0, sizeof(nodeinfo)/sizeof(char) );
				tcmLog_debug("AP_callback_removeadd state:%d, id:%x, type:%x\n", 
                              state, p->id[i], p->type[i]);

                nodeinfo.id[0] = p->id[i];
                nodeinfo.type[0] = p->type[i];

                ret = addZunitIntoDbByNode ( nodeinfo, gaddDeviceProfileID );
                if ( ret == TCMRET_SUCCESS )
                {
                    updateZunitAddandDeleteFlag ( nodeinfo.id[0],
                                                  NOT_UPDATE_ADD_DELETE_FLAG );
                }
                else
                {
                    updateZunitAddandDeleteFlag ( 0, NOT_UPDATE_ADD_DELETE_FLAG );
                }
                //sleep ( 10 );
                //updateZunitStatus ( nodeinfo.id );                           
            }
        }

    }
    else if ( state == STATUS_ADDNODEFAIL || state == STATUS_REMOVENODEFAIL )
    {
        tcmLog_error ( "Add node error ap callback, state:%d\n", state );
        updateZunitAddandDeleteFlag ( -1, NOT_UPDATE_ADD_DELETE_FLAG );
    }

}

void CBIRSensorStatus(WORD node, BYTE status)
{
    char IRSensorStatus[MAX_VALUE_LEN];
    memset( IRSensorStatus, 0, MAX_VALUE_LEN );
    
	if(status == SENSOR_DETECTED)
	{
		tcmLog_debug("Get sensor ID %d, sensor detected\n", node);
        strcpy( IRSensorStatus, "on" );
	}
	else if(status == SENSOR_NORMAL)
	{
		tcmLog_debug("Get sensor ID %d, sensor normal\n", node);
        strcpy( IRSensorStatus, "off" );
	}
    else
    {
        tcmLog_error( "Invalid IRSensor status Report" );
    }

    ZWUnit unitTmp;
    unitTmp.ID = node;
    unitTmp.type = NOT_CHANGE_TYPE;
    unitTmp.name = NOT_CHANGE_NAME;
    unitTmp.x = NOT_CHANGE_X;
    unitTmp.y = NOT_CHANGE_Y;
    unitTmp.value = IRSensorStatus;
    unitTmp.schedSun = NOT_CHANGE_SUN;
    unitTmp.schedMon = NOT_CHANGE_MON;
    unitTmp.schedTue = NOT_CHANGE_TUE;
    unitTmp.schedWed = NOT_CHANGE_WED;
    unitTmp.schedThu = NOT_CHANGE_THU;
    unitTmp.schedFri = NOT_CHANGE_FRI;
    unitTmp.schedSat = NOT_CHANGE_SAT;
    updateByZWUnit ( unitTmp );
}

void CBRemoveNode ( BYTE state, ADDREMOVE_NODE_INFO* p )
{
    if ( state == STATUS_ADDNODEDONE || state == STATUS_REMOVENODEDONE )
    {
        if ( p->dev_num == 1 )
        {
            tcmLog_debug ( "Remove single node, state:%d, id:%d, type:%d\n",
                           state, p->id[0],p->type[0] );
            EACH_NODE_INFO nodeinfo;
            nodeinfo.id = p->id[0];
            nodeinfo.type = p->type[0];
            deleteZunitFromDb ( nodeinfo );
        }
        else
        {
            tcmLog_debug( "Remove multiple nodes, number is %d", p->dev_num );
            int i;
            int ret;
            ADDREMOVE_NODE_INFO nodeinfo;
            for(i=0;i<(p->dev_num);i++)
            {
                memset( &nodeinfo, 0, sizeof(nodeinfo)/sizeof(char) );
				tcmLog_debug("AP_callback_removeadd state:%d, id:%x, type:%x\n", 
                              state, p->id[i], p->type[i]);

                nodeinfo.id[0] = p->id[i];
                nodeinfo.type[0] = p->type[i];

                ret = deleteZunitFromDbByNode ( nodeinfo );
                if ( ret == TCMRET_SUCCESS )
                {
                    updateZunitAddandDeleteFlag ( NOT_UPDATE_ADD_DELETE_FLAG,
                                                  nodeinfo.id[0]
                                                   );
                }
                else
                {
                    updateZunitAddandDeleteFlag ( NOT_UPDATE_ADD_DELETE_FLAG, 
                                                  0 );
                }
                //sleep ( 10 );
                //updateZunitStatus ( nodeinfo.id );                           
            }
        }
    }
    else if ( state == STATUS_ADDNODEFAIL || state == STATUS_REMOVENODEFAIL )
    {
        tcmLog_error ( "Add node error ap callback, state:%d\n", state );
    }

}

tcmRet handleCmd ( ZwCommand* hdCmd )
{
    tcmLog_debug ( "Begin handle command" );
    if ( hdCmd->nodeID == START_ADD_NODE )
    {
        gaddDeviceProfileID = hdCmd->act;
        return APStartAddNodeToNetwork();
    }

    if ( hdCmd->nodeID == START_DEL_NODE )
    {
        return APStartRemoveNodeFromNetwork();
    }

    if( hdCmd->nodeID == TEST_RESET_TO_DEFAULT )
    {
        deleteZwaveApp();
        return APResetToDefault();
    }
    
    int type = getTypeByID ( hdCmd->nodeID );
    if ( type == DEVICE_BINARY_SWITCH )
    {
        APSetSwitchLight ( hdCmd->nodeID, hdCmd->status );
        updateZunitStatus ( hdCmd->nodeID );
        return TCMRET_SUCCESS;
    }
    else if ( type == DEVICE_DIMMER )
    {
        APSetDimmerLight ( hdCmd->nodeID, hdCmd->status );
        updateZunitStatus ( hdCmd->nodeID );
        return TCMRET_SUCCESS;
    }
    else if ( type == DEVICE_POWER_METER )
    {
        updateZunitStatus ( hdCmd->nodeID );
        return TCMRET_SUCCESS;
    }
    else if( type == DEVICE_CURTAIN_CONTROLLER )
    {
       APSetCurtain( hdCmd->nodeID, hdCmd->act );
       return TCMRET_SUCCESS;
    }
    else if( type == DEVICE_TV 
             || type == DEVICE_ACCONDITION 
             || type == DEVICE_DVD )
    {
       APSetIRC( hdCmd->nodeID, hdCmd->act );
       return TCMRET_SUCCESS;
    }
    else
    {
        tcmLog_error ( "Wrong type %d", type );
    }

    return TCMRET_FAILURE;
}

int zwCmdProcessCenter()
{
    while ( 1 )
    {
        sleep ( 1 );
        ZwCommand* cmdTmp = cloneZwCmd ( gzwCmdList );

        if ( cmdTmp != NULL )
            if ( TCMRET_SUCCESS != deleteZwCmdFromListBySeqID ( &gzwCmdList,
                    cmdTmp->sequenceID ) )
            {
                tcmLog_error ( "CPC Delete eventtype %d from queue failed",
                               cmdTmp->sequenceID );
            }
            else
            {
                tcmLog_debug ( "CPC Delete eventtype %d message from queue ok",
                               cmdTmp->sequenceID );
            }

        if ( cmdTmp != NULL )
        {
            if ( TCMRET_SUCCESS != handleCmd ( cmdTmp ) )
            {
                tcmLog_error ( "CPC Handle CMD %d failed", cmdTmp->sequenceID );
            }
            else
            {
                tcmLog_notice ( "CPC Handle CMD %d ok", cmdTmp->sequenceID );
            }

        }
        freeZwCmd ( cmdTmp );
    }
}

tcmRet startZwCmdHandleThread()
{
    pthread_t tid;
    struct sched_param sched;
    pthread_attr_t  attr;

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_init " );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_setschedpolicy " );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_create ( &tid, &attr, zwCmdProcessCenter, NULL ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_create \n" );
        return TCMRET_FAILURE;
    }

    return TCMRET_SUCCESS;
}

tcmRet initZwave ( int usbComNum )
{
    if ( -1 == openCom ( usbComNum ) )
    {
        return TCMRET_FAILURE;
    }
    pthread_create ( &gzwave_rx_thread, NULL, process_zwave_rx, NULL );
    pthread_create ( &gzwave_apipoll_thread, NULL, process_zwave_apipoll, NULL );
    pthread_create ( &gzwave_timer_thread, NULL, process_timer, NULL );
    pthread_create ( &gzwave_rxdata_thread, NULL, process_exe_callback, NULL);

    DONGLE_INFO nodeinfo;

    int a;
    for ( a = 0; a < ZW_MAX_NODES; a++ )
    {
        device_info.node_info_table[a].id = 0;
    }

    if ( API_Initial ( &device_info ) == INIT_FAIL )
        if ( API_Initial ( &device_info ) == INIT_FAIL )
            if ( API_Initial ( &device_info ) == INIT_FAIL )
            {
                tcmLog_error ( "Initial fail\n" );
                return TCMRET_FAILURE;
            }

    tcmLog_debug ( "There are %d nodes in dongle totally",
                   device_info.total_device_number );

    for ( a = 0; a < ZW_MAX_NODES; a++ )
    {
        if ( device_info.node_info_table[a].id != 0 )
        {
            tcmLog_debug ( "NodeID  %d, NodeType %d",
                           device_info.node_info_table[a].id,
                           device_info.node_info_table[a].type );
        }
    }
    return TCMRET_SUCCESS;
}

tcmRet APStartAddNodeToNetwork()
{
    tcmLog_debug ( "Start to add device " );
    int ret = API_StartAddNodeIntoNetwork_cb ( CBAddNode );
    if ( ret == STATUS_ADDNODEREADY )
    {
        tcmLog_debug ( "Ready for adding node" );
        return TCMRET_SUCCESS;
    }
    else
    {
        tcmLog_error ( "Not ready to add node" );
        return TCMRET_FAILURE;
    }
}

tcmRet APStopAddNodeToNetwork()
{
    unsigned int* pairing_status_local = NULL;
    unsigned int* add_remove_type_local = NULL;
    int ret = API_StopAddNodeIntoNetwork ( &pairing_status_local,
                                           &add_remove_type_local );

    if ( ret == STATUS_ADDNODESTOP )
    {
        tcmLog_debug ( "stop add node\n" );
        return TCMRET_SUCCESS;
    }
    else
    {
        tcmLog_error ( "return value:%d\n", ret );
        return TCMRET_FAILURE;
    }
}

tcmRet APStartRemoveNodeFromNetwork()
{
    tcmLog_debug ( "Start to delete device" );
    int ret = API_StartRemoveNodeFromNetwork_cb ( CBRemoveNode );
    if ( ret == STATUS_REMOVENODEREADY )
    {
        tcmLog_debug ( "ready pairing\n" );
        return TCMRET_SUCCESS;
    }
    else
    {
        tcmLog_error ( "return value:%d\n", ret );
        return TCMRET_FAILURE;
    }
}

tcmRet APStopRemoveNodeFromNetwork()
{
    unsigned int* pairing_status_tmp = NULL;
    unsigned int* add_remove_type_tmp = NULL;
    int ret = API_StopRemoveNodeFromNetwork ( &pairing_status_tmp,
              &add_remove_type_tmp );

    if ( ret == STATUS_REMOVENODESTOP )
    {
        tcmLog_debug ( "stop remove node\n" );
        return TCMRET_SUCCESS;
    }
    else
    {
        tcmLog_error ( "return value:%d\n", ret );
        return TCMRET_FAILURE;
    }
}

tcmRet APResetToDefault()
{
    API_ResetToDefault();
    return TCMRET_SUCCESS;
}

void deleteZwaveApp()
{
    int i;
    char IDTmp[MAX_VALUE_LEN];
    for ( i = 0; i < MAXTABLE_ZUNIT + 1; i++ )
    {
        if ( 0 == capi_read_table_parameter ( ID_ZWAVE_UNIT, i,
                                              TAB_PARA_ZWAVE_UNIT_ID,
                                              IDTmp ) )
        {
            capi_delete_table_row ( ID_ZWAVE_UNIT, i );
            tcmLog_notice ( "Delete the %dth unit", i );
        }
    }

    capi_write_to_flash();
}


tcmRet updateZunitStatus ( int upID )
{
    int i, j;
    char IDTmp[MAX_VALUE_LEN];
    int idNum;
    char TypeTmp[MAX_VALUE_LEN];
    int typeNum;

    for ( i = 1; i < MAXTABLE_ZUNIT + 1; i++ )
    {
        memset ( IDTmp, 0, MAX_VALUE_LEN );
        memset ( TypeTmp, 0, MAX_VALUE_LEN );

        if ( 0 == capi_read_table_parameter ( ID_ZWAVE_UNIT, i,
                                              TAB_PARA_ZWAVE_UNIT_ID,
                                              IDTmp ) )
            if ( 0 == capi_read_table_parameter ( ID_ZWAVE_UNIT, i,
                                                  TAB_PARA_ZWAVE_UNIT_TYPE,
                                                  TypeTmp ) )
            {
                idNum = atoi ( IDTmp );

                if ( strcasecmp ( TypeTmp, UNIT_LIGHT ) == 0 )
                {
                    typeNum = DEVICE_BINARY_SWITCH;
                }
                else if ( strcasecmp ( TypeTmp, UNIT_DIMMER_LIGHT ) == 0 )
                {
                    typeNum = DEVICE_DIMMER;
                }
                else if ( strcasecmp ( TypeTmp, UNIT_IR_SENSOR ) == 0 )
                {
                    typeNum = DEVICE_IR_SENSOR;
                }
                else if ( strcasecmp ( TypeTmp, UNIT_CURTAIN ) == 0 )
                {
                    typeNum = DEVICE_CURTAIN_CONTROLLER;
                }
                else if ( strcasecmp ( TypeTmp, UNIT_METER ) == 0 )
                {
                    typeNum = DEVICE_POWER_METER;
                }

                if ( ( typeNum == DEVICE_BINARY_SWITCH &&
                        upID == UPDATE_ALL_NODE_ID )
                        || ( typeNum == DEVICE_BINARY_SWITCH && upID == idNum )
                        || ( typeNum == DEVICE_IR_SENSOR &&
                             upID == UPDATE_ALL_NODE_ID ) 
                        || ( typeNum == DEVICE_IR_SENSOR && upID == idNum ) )
                {
                    char switchLightStatusStr[10];
                    EACH_NODE_INFO node_info;
                    int ret;
                    ret = API_GetBinarySwitchStatus ( idNum, &node_info );

                    if ( ret == STATUS_OK )
                    {
                        if ( node_info.status == STATUS_BINARY_SWITCH_OFF )
                        {
                            strcpy ( switchLightStatusStr, "off" );
                        }
                        else if ( node_info.status == STATUS_BINARY_SWITCH_ON )
                        {
                            strcpy ( switchLightStatusStr, "on" );
                        }
                    
                        tcmLog_debug ( "Update binary swtich %d status to %s  "
                                       "ret %d",
                                       idNum, switchLightStatusStr, ret );
    
                        ZWUnit unitTmp;
                        unitTmp.ID = idNum;
                        unitTmp.type = NOT_CHANGE_TYPE;
                        unitTmp.name = NOT_CHANGE_NAME;
                        unitTmp.x = NOT_CHANGE_X;
                        unitTmp.y = NOT_CHANGE_Y;
                        unitTmp.value = switchLightStatusStr;
                        unitTmp.schedSun = NOT_CHANGE_SUN;
                        unitTmp.schedMon = NOT_CHANGE_MON;
                        unitTmp.schedTue = NOT_CHANGE_TUE;
                        unitTmp.schedWed = NOT_CHANGE_WED;
                        unitTmp.schedThu = NOT_CHANGE_THU;
                        unitTmp.schedFri = NOT_CHANGE_FRI;
                        unitTmp.schedSat = NOT_CHANGE_SAT;
                        updateByZWUnit ( unitTmp );
                    }
                }

                if ( ( typeNum == DEVICE_DIMMER && upID == UPDATE_ALL_NODE_ID )
                        || ( typeNum == DEVICE_DIMMER && upID == idNum ) )
                {
                    char dimmerLightValue[MAX_VALUE_LEN];
                    EACH_NODE_INFO node_info;
                    int ret;
                    ret = API_GetDimmerStatus ( idNum, &node_info );

                    if ( ret == STATUS_OK )
                    {
                        sprintf ( dimmerLightValue, "%d", node_info.status/10 );
                        tcmLog_debug ( "Update dimmer light value to %s",
                                       dimmerLightValue );

                        ZWUnit unitTmp;
                        unitTmp.ID = idNum;
                        unitTmp.type = NOT_CHANGE_TYPE;
                        unitTmp.name = NOT_CHANGE_NAME;
                        unitTmp.x = NOT_CHANGE_X;
                        unitTmp.y = NOT_CHANGE_Y;
                        unitTmp.value = dimmerLightValue;
                        unitTmp.schedSun = NOT_CHANGE_SUN;
                        unitTmp.schedMon = NOT_CHANGE_MON;
                        unitTmp.schedTue = NOT_CHANGE_TUE;
                        unitTmp.schedWed = NOT_CHANGE_WED;
                        unitTmp.schedThu = NOT_CHANGE_THU;
                        unitTmp.schedFri = NOT_CHANGE_FRI;
                        unitTmp.schedSat = NOT_CHANGE_SAT;

                        updateByZWUnit ( unitTmp );
                    }
                }

                if ( ( typeNum == DEVICE_POWER_METER
                        && upID == UPDATE_ALL_NODE_ID )
                        || ( typeNum == DEVICE_POWER_METER && upID == idNum ) )
                {
                    char meterInfo[MAX_VALUE_LEN];
                    EACH_NODE_INFO node_info;
                    int ret;
                    ret = API_GetMeter ( idNum, &node_info );

                    if ( ret == STATUS_OK )
                    {
                        tcmLog_debug ( "Meter NodeID  %d, NodeType %d,"
                                       "NodeStatus %d, vatt %d, current %d,"
                                       " power %d, sec %d",
                                       node_info.id,
                                       node_info.type,
                                       node_info.status,
                                       node_info.voltage,
                                       node_info.current,
                                       node_info.power,
                                       node_info.sec );

                        sprintf ( meterInfo, "%d, %d, %d",
                                  node_info.voltage,
                                  node_info.current,
                                  node_info.power
                                );

                        ZWUnit unitTmp;
                        unitTmp.ID = idNum;
                        unitTmp.type = NOT_CHANGE_TYPE;
                        unitTmp.name = NOT_CHANGE_NAME;
                        unitTmp.x = NOT_CHANGE_X;
                        unitTmp.y = NOT_CHANGE_Y;
                        unitTmp.value = meterInfo;
                        unitTmp.schedSun = NOT_CHANGE_SUN;
                        unitTmp.schedMon = NOT_CHANGE_MON;
                        unitTmp.schedTue = NOT_CHANGE_TUE;
                        unitTmp.schedWed = NOT_CHANGE_WED;
                        unitTmp.schedThu = NOT_CHANGE_THU;
                        unitTmp.schedFri = NOT_CHANGE_FRI;
                        unitTmp.schedSat = NOT_CHANGE_SAT;

                        updateByZWUnit ( unitTmp );
                    }
                }
            }
    }

    return TCMRET_SUCCESS;
}


tcmRet startRecvbySelect()
{
    tcmRet ret;
    tcmMsgHeader *msg = NULL;
    SINT32 commFd;
    SINT32 n, maxFd;
    fd_set readFdsMaster, readFds;

    struct timeval tv;

    /* set up all the fd stuff for select */
    FD_ZERO ( &readFdsMaster );

    tcmMsg_getEventHandle ( msgHandle, &commFd );
    FD_SET ( commFd, &readFdsMaster );

    maxFd = commFd;

    while ( 1 )
    {
        readFds = readFdsMaster;

        /* Set the default polling time is 1s for checking all the messages
        or event */
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        n = select ( maxFd + 1, &readFds, NULL, NULL, &tv );

        if ( n < 0 )
        {
            /* interrupted by signal or something, continue */
            continue;
        }

        if ( 0 == n )
        {
            /*receives data timeout*/
            continue;
        }

        if ( FD_ISSET ( commFd, &readFds ) )
        {
            //tcmLog_error( "There is data coming" );
            if ( ( ret = tcmMsg_receive ( msgHandle, &msg ) ) != TCMRET_SUCCESS )
            {
                if ( ret == TCMRET_DISCONNECTED )
                {
                    tcmLog_debug ( "detected exit of sched, zwctrl will "
                                   "also exit" );
                    return TCMRET_FAILURE;
                }
                else
                {
                    struct timespec sleep_ts = {0, 100*NSECS_IN_MSEC};

                    /* try to keep running after a 100ms pause */
                    tcmLog_error ( "error during tcmMsg_receive, ret=%d", ret );
                    nanosleep ( &sleep_ts, NULL );
                    continue;
                }
            }

            if( msg->type == TCM_MSG_REFRESH_LOG_LEVEL )
            {
                actUtil_setLogLevel();
            }  
            else if ( msg->type == TCM_MSG_ZWAVE_ADD_DEVICE )
            {
                tcmLog_debug ( "Receive one adding node into message queue" );
                ZwCommand* cmdTmp = createZwCmd ( START_ADD_NODE , 
                                                  msg->wordData, 0, 0 );
                putZwCmdToList ( &gzwCmdList , cmdTmp );
            }
            else if ( msg->type == TCM_MSG_ZWAVE_DEL_DEVICE )
            {
                tcmLog_debug ( "Receive one deleting node into message queue" );
                ZwCommand* cmdTmp = createZwCmd ( START_DEL_NODE , 0, 0, 0 );
                putZwCmdToList ( &gzwCmdList , cmdTmp );
            }
            else if ( msg->type == TCM_MSG_ZWAVE_SEND_CMD )
            {
                char valueBuf[LOCAL_MSG_VALUE_LEN];
                memcpy ( valueBuf, ( uint8_t * ) ( msg + 1 ),
                         LOCAL_MSG_VALUE_LEN );
                tcmLog_debug ( "Receive one normal message type %d"
                               "into message queue",
                               msg->type );
                uint32_t nodeID;
                memcpy( &nodeID, valueBuf, 4 );
                ZwCommand* cmdTmp = createZwCmd ( nodeID,
                                                  valueBuf[ACT_OFFSET],
                                                  valueBuf[STATUS_OFFSET],
                                                  ( uint8_t * ) ( valueBuf + RESERVE_OFFSET ) );
                putZwCmdToList ( &gzwCmdList , cmdTmp );
            }
            else
            {
                tcmLog_notice ( "Ignore this type %d message", msg->type );
                TCMMEM_FREE_BUF_AND_NULL_PTR ( msg );
            }
        }
    }
    return TCMRET_SUCCESS;
}

int demo_main()
{
    int i;
    HCMD hcmd;

    if ( -1 == tcmLib_init() )
    {
        tcmLog_error ( "Init tcm library wrong" );
        return 1;
    }

    if ( TCMRET_SUCCESS != startZwCmdHandleThread() )
    {
        tcmLog_error ( "Start zwave command handle thread wrong" );
        capi_uninit_library();
        tcmMsg_cleanup ( &msgHandle );
        tcmLog_cleanup();
        return 1;
    }


    /*First step init zwave open usb com*/
    if ( TCMRET_SUCCESS != initZwave ( 0 ) )
    {
        tcmLog_error ( "Init zwave Error" );
        capi_uninit_library();
        tcmMsg_cleanup ( &msgHandle );
        tcmLog_cleanup();
        return 1;
    }

    updateZunitAddandDeleteFlag ( 0, 0 );

    API_SerialAPI_InitCBFunction( NULL, CBIRSensorStatus, NULL );
    
    if ( TCMRET_SUCCESS != startRecvbySelect() )
    {
        tcmLog_error ( "Start select thread wrong" );
        capi_uninit_library();
        tcmMsg_cleanup ( &msgHandle );
        tcmLog_cleanup();
        return 1;
    }

    return 0;
}
