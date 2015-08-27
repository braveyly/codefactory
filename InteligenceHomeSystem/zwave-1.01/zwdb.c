#include "zwdb.h"

int updateZunitAddandDeleteFlag ( int upAddFlag, int upDeleteFlag )
{
    if ( upAddFlag != NOT_UPDATE_ADD_DELETE_FLAG )
    {
        char addFlagStrTmp[MAX_VALUE_LEN];
        sprintf ( addFlagStrTmp, "%d", upAddFlag );
        capi_write_parameter ( ID_ZWAVE_UNIT, PARA_ZWAVE_UNIT_ADD_FLAG,
                               addFlagStrTmp );
        tcmLog_notice ( "Update zunit add flag to %s", addFlagStrTmp );
    }

    if ( upDeleteFlag != NOT_UPDATE_ADD_DELETE_FLAG )
    {
        char deleteFlagStrTmp[MAX_VALUE_LEN];
        sprintf ( deleteFlagStrTmp, "%d", upDeleteFlag );
        capi_write_parameter ( ID_ZWAVE_UNIT, PARA_ZWAVE_UNIT_DEL_FLAG,
                               deleteFlagStrTmp );
        tcmLog_notice ( "Update zunit delete flag to %s", deleteFlagStrTmp );
    }

    capi_write_to_flash();
    return 0;

}


int getDeviceAddandDeleteFlag ( int* gtAddFlag, int* gtDeleteFlag )
{
    char addFlagStrTmp[MAX_VALUE_LEN];
    char deleteFlagStrTmp[MAX_VALUE_LEN];
    capi_read_parameter ( ID_ZWAVE_UNIT, PARA_ZWAVE_UNIT_ADD_FLAG,
                          addFlagStrTmp );
    capi_read_parameter ( ID_ZWAVE_UNIT, PARA_ZWAVE_UNIT_DEL_FLAG,
                          deleteFlagStrTmp );

    if ( NULL != gtAddFlag )
        *gtAddFlag = atoi ( addFlagStrTmp );

    if ( NULL != gtDeleteFlag )
        *gtDeleteFlag = atoi ( deleteFlagStrTmp );
    return 0;
}



int updateZunit ( int upRowIndex, int upType, char* upName, int upX,
                  int upY, char* upValue, char* upSchedSun,
                  char* upSchedMon, char* upSchedTue,
                  char* upSchedWed, char* upSchedThu,
                  char* upSchedFri, char* upSchedSat )
{
    tcmLog_debug ( "Start updateZunit upRowIndex %d upValue %s",
                   upRowIndex, upValue );
    char IDTmp[MAX_VALUE_LEN];
    char TypeTmp[MAX_VALUE_LEN];
    char XTmp[MAX_VALUE_LEN];
    char YTmp[MAX_VALUE_LEN];

    if ( upType != NOT_CHANGE_TYPE )
    {
        if ( upType == DEVICE_BINARY_SWITCH )
        {
            strcpy ( TypeTmp, UNIT_LIGHT );
        }

        if ( upType == DEVICE_DIMMER )
        {
            strcpy ( TypeTmp , UNIT_DIMMER_LIGHT );
        }

        if ( upType == DEVICE_IR_SENSOR )
        {
            strcpy ( TypeTmp, UNIT_IR_SENSOR );
        }

        if ( upType == DEVICE_CURTAIN_CONTROLLER )
        {
            strcpy ( TypeTmp, UNIT_CURTAIN );
        }

        if ( upType == DEVICE_POWER_METER )
        {
            strcpy ( TypeTmp, UNIT_METER );
        }

        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_TYPE, TypeTmp );
    }

    if ( upName != NOT_CHANGE_NAME )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_NAME, upName );
    }
    
    if ( upX != NOT_CHANGE_X )
    {
        sprintf ( XTmp, "%d", upX );
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_X, XTmp );
    }

    if ( upY != NOT_CHANGE_Y )
    {
        sprintf ( YTmp, "%d", upY );
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_Y, YTmp );
    }

    if ( upValue != NOT_CHANGE_VALUE )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_VALUE, upValue );
    }

    if ( upSchedSun != NOT_CHANGE_SUN )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_SCHEDULE_SUN,
                                     upSchedSun );
    }
    
    if ( upSchedMon != NOT_CHANGE_SUN )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_SCHEDULE_MON,
                                     upSchedMon );
    }

    if ( upSchedTue != NOT_CHANGE_SUN )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_SCHEDULE_TUE,
                                     upSchedTue );
    }

    if ( upSchedWed != NOT_CHANGE_SUN )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_SCHEDULE_WED,
                                     upSchedWed );
    }

    if ( upSchedThu != NOT_CHANGE_SUN )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_SCHEDULE_THU,
                                     upSchedThu );
    }

    if ( upSchedFri != NOT_CHANGE_SUN )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_SCHEDULE_FRI,
                                     upSchedFri );
    }

    if ( upSchedSat != NOT_CHANGE_SUN )
    {
        capi_write_table_parameter ( ID_ZWAVE_UNIT, upRowIndex,
                                     TAB_PARA_ZWAVE_UNIT_SCHEDULE_SAT,
                                     upSchedSat );
    }

    capi_write_to_flash();
    tcmLog_debug ( "End updateZunit" );
}

int updateZUnitByNodeID ( int upID, int upType, char* upName, int upX,
                          int upY, char* upValue, char* upSchedSun,
                          char* upSchedMon, char* upSchedTue,
                          char* upSchedWed, char* upSchedThu,
                          char* upSchedFri, char* upSchedSat )

{
    tcmLog_debug ( "Start updateZUnitByNodeID,  value %s", upValue );
    int i;
    char IDStrTmp[MAX_VALUE_LEN];
    char IDStrResult[MAX_VALUE_LEN];

    sprintf ( IDStrTmp, "%d", upID );

    for ( i = 1; i < MAXTABLE_ZUNIT + 1 ; i++ )
    {
        if ( 0 == capi_read_table_parameter ( ID_ZWAVE_UNIT, i,
                                              TAB_PARA_ZWAVE_UNIT_ID,
                                              IDStrResult ) )
            if ( 0 == strcasecmp ( IDStrTmp, IDStrResult ) )
            {
                tcmLog_debug ( "Find node id %s in database row index %d",
                               IDStrTmp, i );
                updateZunit ( i, upType, upName, upX, upY, upValue, upSchedSun,
                              upSchedMon, upSchedTue, upSchedWed,
                              upSchedThu, upSchedFri, upSchedSat  );
                break;
            }
    }

    tcmLog_debug ( "End updateZUnitByNodeID" );
    return 0;
}

int updateByZWUnit ( ZWUnit adZWUnit )
{
    return updateZUnitByNodeID ( adZWUnit.ID, adZWUnit.type, adZWUnit.name,
                                 adZWUnit.x, adZWUnit.y, adZWUnit.value,
                                 adZWUnit.schedSun, adZWUnit.schedMon,
                                 adZWUnit.schedTue, adZWUnit.schedWed,
                                 adZWUnit.schedThu, adZWUnit.schedFri,
                                 adZWUnit.schedSat );
}

int addZUnitToDB ( int adID, char* adProfileID,int adType, char* adName, int adX, 
                   int adY, char* adValue,  char* adSchedSun, char* adSchedMon,
                   char* adSchedTue, char* adSchedWed,
                   char* adSchedThu, char* adSchedFri, char* adSchedSat )
{
    char* value[TAB_PARA_ZWAVE_UNIT_END-1];
    char IDTmp[MAX_VALUE_LEN];
    char ProfileSetTmp[MAX_VALUE_LEN];
    char ZUnitMatchID[MAX_VALUE_LEN];
    char TypeTmp[MAX_VALUE_LEN];
    char XTmp[MAX_VALUE_LEN];
    char YTmp[MAX_VALUE_LEN];
    int rowIndex;

    sprintf ( IDTmp, "%d", adID );
    strcpy ( ZUnitMatchID, "0" );
    strcpy ( ProfileSetTmp, "true" );
    sprintf ( XTmp, "%d", adX );
    sprintf ( YTmp, "%d", adY );

    if ( adType == DEVICE_BINARY_SWITCH )
    {
        strcpy ( TypeTmp, UNIT_LIGHT );
    }

    if ( adType == DEVICE_DIMMER )
    {
        strcpy ( TypeTmp , UNIT_DIMMER_LIGHT );
    }

    if ( adType == DEVICE_IR_SENSOR )
    {
        strcpy ( TypeTmp, UNIT_IR_SENSOR );
    }

    if ( adType == DEVICE_CURTAIN_CONTROLLER )
    {
        strcpy ( TypeTmp, UNIT_CURTAIN );
    }

    if ( adType == DEVICE_POWER_METER )
    {
        strcpy ( TypeTmp, UNIT_METER );
    }

    if ( adType == DEVICE_TV )
    {
        strcpy ( TypeTmp, UNIT_TV );
    }
    
    if ( adType == DEVICE_DVD )
    {
        strcpy ( TypeTmp, UNIT_DVD_PLAYER );
    }

    if ( adType == DEVICE_ACCONDITION )
    {
        strcpy ( TypeTmp, UNIT_AC );
    }
    
    if ( TypeTmp == 0 )
    {
        tcmLog_notice ( "This type node is ignored" );
        return TCMRET_FAILURE;
    }

    value[TAB_PARA_ZWAVE_UNIT_ID-1]   = IDTmp;
    value[TAB_PARA_ZWAVE_UNIT_PROFILE_ID-1] = adProfileID;
    value[TAB_PARA_ZWAVE_UNIT_PROFILE_SET-1] = ProfileSetTmp;
    value[TAB_PARA_ZWAVE_UNIT_TYPE-1] = TypeTmp;
    value[TAB_PARA_ZWAVE_UNIT_NAME-1] = adName;
    value[TAB_PARA_ZWAVE_UNIT_X-1]    = XTmp;
    value[TAB_PARA_ZWAVE_UNIT_Y-1]    = YTmp;
    value[TAB_PARA_ZWAVE_UNIT_VALUE-1] = adValue;
    value[TAB_PARA_ZWAVE_UNIT_MATCHINDEX-1] = ZUnitMatchID;
    value[TAB_PARA_ZWAVE_UNIT_SCHEDULE_SUN-1] = adSchedSun;
    value[TAB_PARA_ZWAVE_UNIT_SCHEDULE_MON-1] = adSchedMon;
    value[TAB_PARA_ZWAVE_UNIT_SCHEDULE_TUE-1] = adSchedTue;
    value[TAB_PARA_ZWAVE_UNIT_SCHEDULE_WED-1] = adSchedWed;
    value[TAB_PARA_ZWAVE_UNIT_SCHEDULE_THU-1] = adSchedThu;
    value[TAB_PARA_ZWAVE_UNIT_SCHEDULE_FRI-1] = adSchedFri;
    value[TAB_PARA_ZWAVE_UNIT_SCHEDULE_SAT-1] = adSchedSat;

    if ( 0 == capi_add_table_row ( ID_ZWAVE_UNIT, 0, value, &rowIndex ) )
    {
        tcmLog_debug ( "Add zunit index %d into db OK ", rowIndex );
        capi_write_to_flash();
        return rowIndex;
    }
    else
    {
        tcmLog_error ( "Add zunit id %d failed", adID );
        return -1;
    }
}

int addZUnitToDBByZWUnit ( ZWUnit adZWUnit )
{
    return addZUnitToDB ( adZWUnit.ID, adZWUnit.profileID,
                          adZWUnit.type, adZWUnit.name, adZWUnit.x,
                          adZWUnit.y, adZWUnit.value, adZWUnit.schedSun,
                          adZWUnit.schedMon, adZWUnit.schedTue,
                          adZWUnit.schedWed, adZWUnit.schedThu,
                          adZWUnit.schedFri, adZWUnit.schedSat );
}

int checkZUnitExist ( int ckID, int ckType, int* delRowIndex )
{
    int i;
    char IDStrTmp[MAX_VALUE_LEN];
    char IDStrResult[MAX_VALUE_LEN];
    char TypeTmp[MAX_VALUE_LEN];
    char TypeStrResult[MAX_VALUE_LEN];

    sprintf ( IDStrTmp, "%d", ckID );

    if ( ckType == DEVICE_BINARY_SWITCH )
    {
        strcpy ( TypeTmp, UNIT_LIGHT );
    }

    else if ( ckType == DEVICE_DIMMER )
    {
        strcpy ( TypeTmp , UNIT_DIMMER_LIGHT );
    }

    else if ( ckType == DEVICE_IR_SENSOR )
    {
        strcpy ( TypeTmp, UNIT_IR_SENSOR );
    }

    else if ( ckType == DEVICE_CURTAIN_CONTROLLER )
    {
        strcpy ( TypeTmp, UNIT_CURTAIN );
    }
    else if ( ckType == DEVICE_POWER_METER )
    {
        strcpy ( TypeTmp, UNIT_METER );
    }
    else if( ckType == DEVICE_ACCONDITION )
    {
        strcpy( TypeTmp, UNIT_AC );
    }
    else if( ckType == DEVICE_TV )
    {
        strcpy( TypeTmp, UNIT_TV );
    }
    else if( ckType == DEVICE_DVD )
    {
        strcpy( TypeTmp, UNIT_DVD_PLAYER );
    }
    else
    {
        tcmLog_error ( "Invalid device type" );
        /*invalid device type*/
        return -2;
    }

    for ( i = 1 ; i < MAXTABLE_ZUNIT + 1; i++ )
    {
        if ( 0 ==  capi_read_table_parameter ( ID_ZWAVE_UNIT, i ,
                                               TAB_PARA_ZWAVE_UNIT_ID,
                                               IDStrResult ) )
            if ( strcasecmp ( IDStrTmp, IDStrResult ) == 0 )
                if ( 0 ==  capi_read_table_parameter ( ID_ZWAVE_UNIT, i ,
                                                       TAB_PARA_ZWAVE_UNIT_TYPE,
                                                       TypeStrResult ) )
                    if ( strcasecmp ( TypeTmp, TypeStrResult ) == 0 )
                    {
                        *delRowIndex = i;
                        /*The device with the same ID and type exists in db*/
                        return -1;
                    }

        if ( 0 ==  capi_read_table_parameter ( ID_ZWAVE_UNIT, i ,
                                               TAB_PARA_ZWAVE_UNIT_ID,
                                               IDStrResult ) )
            if ( strcasecmp ( IDStrTmp, IDStrResult ) == 0 )
                if ( 0 ==  capi_read_table_parameter ( ID_ZWAVE_UNIT, i ,
                                                       TAB_PARA_ZWAVE_UNIT_TYPE,
                                                       TypeStrResult ) )
                    if ( strcasecmp ( TypeTmp, TypeStrResult ) != 0 )
                    {
                        /*The device with the same ID existed in db, but its
                        type is different*/
                        return i;
                    }
    }

    /*No device with the same ID exist in DB*/
    return 0;
}

tcmRet addZunitIntoDb ( EACH_NODE_INFO addNodeInfo )
{
    int ret ;
    int delRowIndex;
    ret = checkZUnitExist ( addNodeInfo.id, addNodeInfo.type, &delRowIndex );

    if ( ret == -1 )
    {
        tcmLog_error ( "ZUnit ID %d Type %d has been existed in db",
                       addNodeInfo.id, addNodeInfo.type );
        return TCMRET_FAILURE;
    }

    if ( ret == -2 )
    {
        tcmLog_error ( "Zunit type is invalid ID %d type %d",
                       addNodeInfo.id, addNodeInfo.type );
        return TCMRET_FAILURE;
    }

    /*donot consider this situation temporarily*/
    if ( ret > 0 )
    {

        tcmLog_notice ( "Update row index %d zunit type %d", ret,
                       addNodeInfo.type );
        return TCMRET_FAILURE;
    }

    char zunitName[MAX_VALUE_LEN];
    char zunitValue[MAX_VALUE_LEN];
    char zunitSun[MAX_VALUE_LEN];
    char zunitMon[MAX_VALUE_LEN];
    char zunitTue[MAX_VALUE_LEN];
    char zunitWed[MAX_VALUE_LEN];
    char zunitThu[MAX_VALUE_LEN];
    char zunitFri[MAX_VALUE_LEN];
    char zunitSat[MAX_VALUE_LEN];
    int zunitID, zunitType, zunitX, zunitY;
    zunitID = addNodeInfo.id;
    zunitType = addNodeInfo.type;
    zunitX = 300;
    zunitY = 300;

    tcmLog_debug ( "zunit id is %d", addNodeInfo.id );
    strcpy ( zunitName, "defaultname" );
    strcpy ( zunitValue, "defaultvalue" );
    strcpy ( zunitSun, "defaultsun" );
    strcpy ( zunitMon, "defaultmon" );
    strcpy ( zunitTue, "defaulttue" );
    strcpy ( zunitWed, "defaultwed" );
    strcpy ( zunitThu, "defaultthu" );
    strcpy ( zunitFri, "defaultfri" );
    strcpy ( zunitSat, "defaultsat" );

    ZWUnit zwUnitTmp;
    zwUnitTmp.ID = zunitID;
    zwUnitTmp.type = zunitType;
    zwUnitTmp.name = zunitName;
    zwUnitTmp.x = zunitX;
    zwUnitTmp.y = zunitY;
    zwUnitTmp.value = zunitValue;
    zwUnitTmp.schedSun = zunitSun;
    zwUnitTmp.schedMon = zunitMon;
    zwUnitTmp.schedTue = zunitTue;
    zwUnitTmp.schedWed = zunitWed;
    zwUnitTmp.schedThu = zunitThu;
    zwUnitTmp.schedFri = zunitFri;
    zwUnitTmp.schedSat = zunitSat;

    addZUnitToDBByZWUnit ( zwUnitTmp );
    return TCMRET_SUCCESS;
}

tcmRet addZunitIntoDbByNode( ADDREMOVE_NODE_INFO addNodeInfo, int profileID )
{
    int ret ;
    int delRowIndex;
    ret = checkZUnitExist ( addNodeInfo.id[0], addNodeInfo.type[0], &delRowIndex );

    if ( ret == -1 )
    {
        tcmLog_error ( "ZUnit ID %d Type %d has been existed in db",
                       addNodeInfo.id[0], addNodeInfo.type[0] );
        return TCMRET_FAILURE;
    }

    if ( ret == -2 )
    {
        tcmLog_error ( "Zunit type is invalid ID %d type %d",
                       addNodeInfo.id[0], addNodeInfo.type[0] );
        return TCMRET_FAILURE;
    }

    /*donot consider this situation temporarily*/
    if ( ret > 0 )
    {

        tcmLog_notice ( "Update row index %d zunit type %d", ret,
                       addNodeInfo.type[0] );
        return TCMRET_FAILURE;
    }
    char zunitProfileID[MAX_VALUE_LEN];
    char zunitName[MAX_VALUE_LEN];
    char zunitValue[MAX_VALUE_LEN];
    char zunitSun[MAX_VALUE_LEN];
    char zunitMon[MAX_VALUE_LEN];
    char zunitTue[MAX_VALUE_LEN];
    char zunitWed[MAX_VALUE_LEN];
    char zunitThu[MAX_VALUE_LEN];
    char zunitFri[MAX_VALUE_LEN];
    char zunitSat[MAX_VALUE_LEN];
    int zunitID, zunitType, zunitX, zunitY;
    zunitID = addNodeInfo.id[0];
    zunitType = addNodeInfo.type[0];
    zunitX = 300;
    zunitY = 300;

    tcmLog_debug ( "zunit id is %d", addNodeInfo.id[0] );
    sprintf( zunitProfileID, "%d", profileID );
    strcpy ( zunitName, "defaultname" );
    strcpy ( zunitValue, "defaultvalue" );
    strcpy ( zunitSun, "defaultsun" );
    strcpy ( zunitMon, "defaultmon" );
    strcpy ( zunitTue, "defaulttue" );
    strcpy ( zunitWed, "defaultwed" );
    strcpy ( zunitThu, "defaultthu" );
    strcpy ( zunitFri, "defaultfri" );
    strcpy ( zunitSat, "defaultsat" );

    ZWUnit zwUnitTmp;
    zwUnitTmp.ID = zunitID;
    zwUnitTmp.profileID = zunitProfileID;
    zwUnitTmp.type = zunitType;
    zwUnitTmp.name = zunitName;
    zwUnitTmp.x = zunitX;
    zwUnitTmp.y = zunitY;
    zwUnitTmp.value = zunitValue;
    zwUnitTmp.schedSun = zunitSun;
    zwUnitTmp.schedMon = zunitMon;
    zwUnitTmp.schedTue = zunitTue;
    zwUnitTmp.schedWed = zunitWed;
    zwUnitTmp.schedThu = zunitThu;
    zwUnitTmp.schedFri = zunitFri;
    zwUnitTmp.schedSat = zunitSat;

    addZUnitToDBByZWUnit ( zwUnitTmp );
    return TCMRET_SUCCESS;
   
}

tcmRet deleteZunitFromDb ( EACH_NODE_INFO addNodeInfo )
{
    int ret ;
    int delRowIndex;

    ret = checkZUnitExist ( addNodeInfo.id, addNodeInfo.type, &delRowIndex );

    if ( ret == -1 )
    {
        tcmLog_error ( "ZUnit ID %d Type %d  row index %d will be deleted",
                       addNodeInfo.id, addNodeInfo.type, delRowIndex );
        capi_delete_table_row ( ID_ZWAVE_UNIT, delRowIndex );
        capi_write_to_flash();
        updateZunitAddandDeleteFlag( NOT_UPDATE_ADD_DELETE_FLAG, addNodeInfo.id );
        return TCMRET_SUCCESS;
    }

    return TCMRET_FAILURE;
}

tcmRet deleteZunitFromDbByNode ( ADDREMOVE_NODE_INFO addNodeInfo )
{
    int ret ;
    int delRowIndex;

    ret = checkZUnitExist ( addNodeInfo.id[0], addNodeInfo.type[0], &delRowIndex );

    if ( ret == -1 )
    {
        tcmLog_error ( "ZUnit ID %d Type %d  row index %d will be deleted",
                       addNodeInfo.id[0], addNodeInfo.type[0], delRowIndex );
        capi_delete_table_row ( ID_ZWAVE_UNIT, delRowIndex );
        capi_write_to_flash();
        updateZunitAddandDeleteFlag( NOT_UPDATE_ADD_DELETE_FLAG, addNodeInfo.id[0] );
        return TCMRET_SUCCESS;
    }

    return TCMRET_FAILURE;
}

tcmRet addZunitIntoDbByDongleInfo ( DONGLE_INFO* addDongleInfo )
{
    tcmLog_debug ( "Start add node info to db" );
    int i;

    for ( i = 0; i < ZW_MAX_NODES; i++ )
    {
        if ( addDongleInfo->node_info_table[i].id != 0 )
        {
            tcmLog_debug ( "Add NodeID  %d, NodeType %d",
                           addDongleInfo->node_info_table[i].id,
                           addDongleInfo->node_info_table[i].type );
            addZunitIntoDb ( addDongleInfo->node_info_table[i] );
        }
    }
    tcmLog_debug ( "End add node into db" );
    return TCMRET_SUCCESS;
}

tcmRet deleteZunitFromDbByDongleInfo ( DONGLE_INFO* addDongleInfo )
{
    tcmLog_debug ( "Start delete node from db" );
    int i;

    for ( i = 0; i < ZW_MAX_NODES; i++ )
    {
        if ( addDongleInfo->node_info_table[i].id != 0 )
        {
            tcmLog_debug ( "Delete NodeID  %d, NodeType %d",
                           addDongleInfo->node_info_table[i].id,
                           addDongleInfo->node_info_table[i].type );
            deleteZunitFromDb ( addDongleInfo->node_info_table[i] );
            updateZunitAddandDeleteFlag ( NOT_UPDATE_ADD_DELETE_FLAG,
                                          addDongleInfo->node_info_table[i].id );
        }
    }

    tcmLog_debug ( "End delete node from db" );
}

int getTypeByID ( int gtID )
{
    tcmLog_debug( "gtID %d", gtID );
    int i ;
    char IDStrTmp[MAX_VALUE_LEN];
    char TypeStrTmp[MAX_VALUE_LEN];
    for ( i = 1; i < MAXTABLE_ZUNIT + 1; i++ )
    {
        if ( 0 == capi_read_table_parameter ( ID_ZWAVE_UNIT, i,
                                              TAB_PARA_ZWAVE_UNIT_ID,
                                              IDStrTmp ) )
        {
            tcmLog_debug( "IDStrTmp is %s", IDStrTmp );
            if ( gtID == atoi ( IDStrTmp ) )
            {
                if ( 0 == capi_read_table_parameter ( ID_ZWAVE_UNIT, i,
                                                      TAB_PARA_ZWAVE_UNIT_TYPE,
                                                      TypeStrTmp ) )
                {
                    tcmLog_debug( "TypeStrTmp is %s", TypeStrTmp );
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_LIGHT ) )
                        return DEVICE_BINARY_SWITCH;
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_DIMMER_LIGHT ) )
                        return DEVICE_DIMMER;
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_IR_SENSOR ) )
                        return DEVICE_IR_SENSOR;
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_CURTAIN ) )
                        return DEVICE_CURTAIN_CONTROLLER;
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_METER ) )
                        return DEVICE_POWER_METER;
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_TV ) )
                        return DEVICE_TV;
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_DVD_PLAYER ) )
                        return DEVICE_DVD;
                    if ( 0 == strcasecmp ( TypeStrTmp, UNIT_AC ) )
                        return DEVICE_ACCONDITION;
                        
                }
            }
        }
    }

    return -1;
}
