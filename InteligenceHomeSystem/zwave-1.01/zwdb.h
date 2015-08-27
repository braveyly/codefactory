#ifndef _ZW_DB_H_
#define _ZW_DB_H_
#include <conflib.h>
#include <tcm_mem.h>
#include <tcm_log.h>
#include "zwave.h"

#define UNIT_LIGHT "light"
#define UNIT_DIMMER_LIGHT "dimmerlight"
#define UNIT_AC "AC"
#define UNIT_IR_SENSOR "irsensor"
#define UNIT_DVD_PLAYER "dvdplayer"
#define UNIT_TV "TV"
#define UNIT_CURTAIN "curtain"
#define UNIT_METER "meter"

#define NOT_CHANGE_PROFILEID -1
#define NOT_CHANGE_PROFILESET -1
#define NOT_CHANGE_TYPE   -1
#define NOT_CHANGE_NAME   -1
#define NOT_CHANGE_X      -1
#define NOT_CHANGE_Y      -1
#define NOT_CHANGE_VALUE  -1
#define NOT_CHANGE_MATCH_INDEX -1
#define NOT_CHANGE_SUN    -1
#define NOT_CHANGE_MON    -1
#define NOT_CHANGE_TUE    -1
#define NOT_CHANGE_WED    -1
#define NOT_CHANGE_THU    -1
#define NOT_CHANGE_FRI    -1
#define NOT_CHANGE_SAT    -1
#define NOT_UPDATE_ADD_DELETE_FLAG -100

typedef struct ZWAVEUNIT
{
    int ID;
    char* profileID;
    char* profileSet;
    int type;
    char* name;
    int x;
    int y;
    char* value;
    int matchIndex;
    char* schedSun;
    char* schedMon;
    char* schedTue;
    char* schedWed;
    char* schedThu;
    char* schedFri;
    char* schedSat;
}ZWUnit;

tcmRet addZunitIntoDb ( EACH_NODE_INFO addNodeInfo );
tcmRet addZunitIntoDbByDongleInfo ( DONGLE_INFO* addDongleInfo );
#endif
