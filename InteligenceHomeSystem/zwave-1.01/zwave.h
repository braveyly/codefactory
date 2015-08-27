/*
 *	Auther : Haas Tsai
 *	
 *	(1)	Revision : 1.01, 2011-01-10 AP for z-wave initial and control.
 * 
 */
 
#ifndef _ZWAVE_H
#define _ZWAVE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

#include <string.h>  /* String function definitions */             
#include <unistd.h>  /* UNIX standard function definitions */       
#include <termios.h> /* POSIX terminal control definitions */




#define ZWAVE_DEV0 "/dev/ttyUSB0"
#define ZWAVE_DEV1 "/dev/ttyUSB1"
#define ZWAVE_DEV2 "/dev/ttyUSB2"

#define NEW_NODEINFO


//====== 1000107, add for Z-wave
#define ZW_MAX_NODES        232		// Max number of nodes in a Z-wave system
#define MAX_NODEMASK_LENGTH   (ZW_MAX_NODES/8)	//The max length of a node mask


#define BUF_SIZE  74
#define BUF_SIZE_RX 74
#define BUF_SIZE_TX 60

#define UART_RX_BUFSIZE 128 //32
#define UART_TX_BUFSIZE 128

#define INVALID_TIMER_HANDLE 255

typedef unsigned short WORD;
#define BYTE 	unsigned char
#define BOOL  unsigned char

#define TRUE  (1)
#define FALSE (0)

#define IDX_CMD   2
#define IDX_DATA  3

#define TIMER_MAX 10  /* Max. number of simultaneously active timers. */

#define auto
#define code
#define CODE
//#define CODE   code        /* Used for defining callback function which allways */
                           /* resides in code space. */
                           
#define VOID_CALLBACKFUNC(completedFunc)  void (CODE *completedFunc)

/* Max number of times a frame will be transmitted to PC */
#define MAX_SERIAL_RETRY 		3
#define TIMEOUT_TIME 				250
//#define TIMEOUT_TIME 				50

/* One second timeout */
#define TIMER_ONE_SECOND   100

/* One time timer */
#define TIMER_ONE_TIME     0
#define TIMER_FOREVER      (BYTE)-1

/* Transmit complete codes */
#define TRANSMIT_COMPLETE_OK          0x00
#define TRANSMIT_COMPLETE_NO_ACK      0x01  /* retransmission error */
#define TRANSMIT_COMPLETE_FAIL        0x02  /* transmit error */


/* Offsets into frame received */
#define OFFSET_CLASSCMD                       0x00
#define OFFSET_CMD                            0x01
#define OFFSET_PARAM_1                        0x02
#define OFFSET_PARAM_2                        0x03
#define OFFSET_PARAM_3                        0x04
#define OFFSET_PARAM_4                        0x05
#define OFFSET_PARAM_5                        0x06
#define OFFSET_PARAM_6                        0x07
#define OFFSET_PARAM_7                        0x08
#define OFFSET_PARAM_8                        0x09
#define OFFSET_PARAM_9                        0x0a
#define OFFSET_PARAM_10                        0x0b
#define OFFSET_PARAM_11                        0x0c
#define OFFSET_PARAM_12                        0x0d
#define OFFSET_PARAM_13                        0x0e
#define OFFSET_PARAM_14                        0x0f
#define OFFSET_PARAM_15                        0x10

#define STATUS_IDLE 				0x00
#define STATUS_PROCESSING		0x01
#define STATUS_FAIL					0x02

#define	INIT_OK 						0x00
#define INIT_FAIL						0xFF

//define the return status of function SendData
#define STATUS_NONE						0xFF
#define STATUS_INPUT_ERROR		0xFE
#define STATUS_OK							0x00
#define STATUS_APIPROCESSING	0x01
#define STATUS_NORESPONSE			0x02
#define STATUS_NOREQUEST   		0x03
#define STATUS_SET_DONGLEBUSY	0x04
#define STATUS_GET_DONGLEBUSY	0x05
#define STATUS_NOACK					0x06
#define STATUS_NETWORKBUSY		0x07


#define STATUS_ADDNODEREADY		0x08
#define STATUS_ADDNODENOW			0x09
#define STATUS_ADDNODEDONE		0x0A
#define STATUS_ADDNODEFAIL		0x0B
#define STATUS_ADDNODESTOP		0x0C

#define STATUS_REMOVENODEREADY	0x0D
#define STATUS_REMOVENODENOW		0x0E
#define STATUS_REMOVENODEDONE		0x0F
#define STATUS_REMOVENODEFAIL		0x10
#define STATUS_REMOVENODESTOP		0x11
#define STATUS_TIMEOUT					0x12

#define STATUS_GETINFO_FAIL			0x13
#define STATUS_NODE_WELL				0x14
#define STATUS_NODE_FAIL				0x15


#define GET_STATUS_ON					0x01
#define GET_STATUS_OFF				0x00


#define CURTAIN_OFF						0x01
#define CURTAIN_ON						0x00
#define CURTAIN_STOP					0x02	

#define GET_POWER							0x00
#define GET_VOLTAGE						0x01
#define GET_CURRENT						0x02


#define ASSOCIATIONSET				0x00
#define ASSOCIATIONREMOVE			0x01
#define ASSOCIATIONGET				0x02


//===== Use for Simple AV control
#define CHANNEL_TV1						0x01
#define CHANNEL_DVD1					0x02
#define CHANNEL_SAT1					0x03
#define CHANNEL_AUX1					0x04
#define CHANNEL_TV2						0x05
#define CHANNEL_DVD2					0x06
#define CHANNEL_SAT2					0x07
#define CHANNEL_AUX2					0x08


#define KEY_DOWN							0x00
#define KEY_UP								0x01
#define KEY_ALIVE							0x02

#define ITEMID_NONE_MSB				0x00
#define ITEMID_NONE_LSB				0x00
					
#define TV_POWER							0x0027
#define TV_CHANNELUP					0x0004
#define TV_CHANNELDOWN				0x0005
#define TV_VOLUMEUP						0x0003
#define TV_VOLUMEDOWN					0x0002
#define TV_MUTE								0x0001

#define DVD_POWER							0x0027
#define DVD_PLAY							0x0013
#define DVD_STOP							0x0014
#define DVD_PAUSE							0x0015
#define DVD_EJECT							0x0025
#define DVD_VOLUMEUP					0x0003
#define DVD_VOLUMEDOWN				0x0002
#define DVD_FASTFORWARD				0x0016
#define DVD_BACKWARD					0x0017

#define AC_POWER							0x0027
#define AC_TEMPRATURE_UP			0x0003
#define AC_TEMPRATURE_DOWN		0x0002
#define AC_FAN_SPEED_UP				0x0004
#define AC_FAN_SPEED_DOWN			0x0005
//=====

#define SENSOR_DETECTED			0xFF
#define SENSOR_DETECTED2		0x63
#define SENSOR_NORMAL				0x00


#define MULTI_CHANNEL_NUM	8
typedef struct
{
	BYTE dev_num;											// Carry how may device has added/removed	
	WORD id[MULTI_CHANNEL_NUM];				// MSB: channel, LSB:nodeID
	BYTE type[MULTI_CHANNEL_NUM];
	BYTE timeout;
	BYTE alarmstatus;
}ADDREMOVE_NODE_INFO;


typedef struct
{
	BYTE instance;		// each cmd class support how many instance end point

}CMDCLASS_SUPPORT;

#define SENSOR_TYPE_POWER 	0x05
#define SENSOR_TYPE_VOLTAGE 0x0F
#define SENSOR_TYPE_CURRENT 0x10

typedef struct
{
	BYTE id;			
	BYTE type;		// node type, ex: DEVICE_PORTABLE_CONTROLLER
	BYTE status;	// use for binary switch, dimmer, curtain
	BYTE exe_status;
	
	BYTE cmdclass_bitmask[32];					// store the class this node support
	CMDCLASS_SUPPORT cmdclass[0xFF];		// max support 0xFF command class
	
	int voltage;
	int current;
	int power;
	int sec;
	BYTE precision;
	
	BYTE powerlevel; 		// for battery device(0%~100%, 0x00~0x64, battery low is 0xff)
	int wakeupinterval; // for battery device present wake up time interval		
}EACH_NODE_INFO;


#define STATUS_BINARY_SWITCH_ON		0x0000
#define STATUS_BINARY_SWITCH_OFF	0x0001



	
typedef struct
{
	BYTE total_device_number;											// store the total device number 
	BYTE device_bit_mask[MAX_NODEMASK_LENGTH];		// 29 bytes	
	EACH_NODE_INFO node_info_table[ZW_MAX_NODES];	// 232 nodes		
}DONGLE_INFO;


#define DEVICE_PORTABLE_CONTROLLER	0x01
#define DEVICE_STATIC_CONTROLLER		0x02
#define DEVICE_BINARY_SWITCH				0x03
#define DEVICE_IR_SENSOR						0x04
#define DEVICE_DIMMER								0x05
#define DEVICE_CURTAIN_CONTROLLER		0x06
#define DEVICE_POWER_METER					0x07
#define DEVICE_IR_CONTROLLER				0x08
#define DEVICE_BINARY_SWITCH_AND_METER	0x09
#define DEVICE_UNKNOW								0xff


#define DEVICE_TV										0x20
#define DEVICE_DVD									0x21
#define DEVICE_SAT									0x22
#define DEVICE_AUX									0x23
#define DEVICE_ACCONDITION					0x24



//#define CMD_TYPE_BASIC_GET					0x0000
//#define CMD_TYPE_BASIC_SET					0x0001
//#define CMD_TYPE_BINARY_GET					0x0002
//#define CMD_TYPE_BINARY_SET					0x0003


typedef struct 
{
	BYTE buffer[BUF_SIZE];	//74 bytes
	BYTE idx;
	BYTE byLen;
	BYTE byCompletedFunc;
}SCMD;
typedef SCMD *HCMD;


/* Node type structure present in NODEINFO */
typedef struct _NODE_TYPE_
{
  BYTE basic;                 /* Basic Device Type, is it a Controller, Controller_Static, */
                              /* Slave or a Slave_Routing Device Type */
  BYTE generic;               /* Generic Device Type */
  BYTE specific;              /* Specific Device Type */
} NODE_TYPE_ZW;


/* Node info stored within the non-volatile memory */
/* This are the first (protocol part) payload bytes from the Node Infomation frame */
typedef struct _NODEINFO_
{
  BYTE        capability;     /* Network capabilities */
  BYTE        security;       /* Network security */
  BYTE        reserved;
  NODE_TYPE_ZW   nodeType;       /* Basic, Generic and Specific Device types - Basic is generated... */
} NODEINFO;
/*Global used when getting nodeinformation from protocol*/
NODEINFO nodeInfo;


/* Learn node state information passed by the call back function */
typedef struct _LEARN_INFO_
{
  BYTE  bStatus;      /* Status of learn mode */
  BYTE  bSource;      /* Node id of the node that send node info */
  BYTE  *pCmd;        /* Pointer to Application Node information */
  BYTE  bLen;         /* Node info length                        */
} LEARN_INFO;



/* ApplicationcControllerUpdate status */
#define UPDATE_STATE_NODE_INFO_RECEIVED     0x84
#define UPDATE_STATE_NODE_INFO_REQ_DONE     0x82
#define UPDATE_STATE_NODE_INFO_REQ_FAILED   0x81
#define UPDATE_STATE_ROUTING_PENDING        0x80
#define UPDATE_STATE_ADD_DONE               0x40
#define UPDATE_STATE_DELETE_DONE            0x20
#define UPDATE_STATE_SUC_ID                 0x10

// valid powerlevel values used in ZW_RF_POWERLEVEL_SET and ZW_RF_POWERLEVEL_GET 
enum
{
  normalPower = 0,
  minus1dBm,
  minus2dBm,
  minus3dBm,
  minus4dBm,
  minus5dBm,
  minus6dBm,
  minus7dBm,
  minus8dBm,
  minus9dBm,
  miniumPower = minus9dBm
};


/* ZW_RemoveFailedNode and ZW_ReplaceFailedNode return value definitions */
#define  NOT_PRIMARY_CONTROLLER             1 /* The removing process was */
                                              /* aborted because the controller */
                                              /* is not the primary one */

#define  NO_CALLBACK_FUNCTION               2 /* The removing process was */
                                              /* aborted because no call back */
                                              /* function is used */
#define  FAILED_NODE_NOT_FOUND              3 /* The removing process aborted */
                                              /* because the node was node */
                                              /* found */
#define  FAILED_NODE_REMOVE_PROCESS_BUSY    4 /* The removing process is busy */
#define  FAILED_NODE_REMOVE_FAIL            5 /* The removing process could not */
                                              /* be started */

#define ZW_FAILED_NODE_REMOVE_STARTED       0 /* The removing/replacing failed node process started */
#define ZW_NOT_PRIMARY_CONTROLLER           (1 << NOT_PRIMARY_CONTROLLER)
#define ZW_NO_CALLBACK_FUNCTION             (1 << NO_CALLBACK_FUNCTION)
#define ZW_FAILED_NODE_NOT_FOUND            (1 << FAILED_NODE_NOT_FOUND)
#define ZW_FAILED_NODE_REMOVE_PROCESS_BUSY  (1 << FAILED_NODE_REMOVE_PROCESS_BUSY)
#define ZW_FAILED_NODE_REMOVE_FAIL          (1 << FAILED_NODE_REMOVE_FAIL)


/* ZW_RemoveFailedNode and ZW_ReplaceFailedNode callback status definitions */
#define ZW_NODE_OK                          0 /* The node is working properly (removed from the failed nodes list ) */

/* ZW_RemoveFailedNode callback status definitions */
#define ZW_FAILED_NODE_REMOVED              1 /* The failed node was removed from the failed nodes list */
#define ZW_FAILED_NODE_NOT_REMOVED          2 /* The failed node was not removed from the failing nodes list */

/* ZW_ReplaceFailedNode callback status definitions */
#define ZW_FAILED_NODE_REPLACE              3 /* The failed node are ready to be replaced and controller */
                                              /* is ready to add new node with nodeID of the failed node */
#define ZW_FAILED_NODE_REPLACE_DONE         4 /* The failed node has been replaced */
#define ZW_FAILED_NODE_REPLACE_FAILED       5 /* The failed node has not been replaced */


/**
  * \ingroup SerialAPI
  * \defgroup SAUSC UART Status Codes
  * @{
  */
#define STATUS_RXTIMEOUT 1
#define STATUS_FRAMESENT 2
#define STATUS_FRAMEERROR 3
#define STATUS_FRAMERECEIVED 4

#define SOF 0x01  /* Start Of Frame */
#define ACK 0x06  /* Acknowledge successfull frame reception */
#define NAK 0x15  /* Not Acknowledge successfull frame reception - please retransmit... */
#define CAN 0x18  /* Frame received (from host) was dropped - waiting for ACK */

/* Frame types */
#define REQUEST                                         0x00
#define RESPONSE                                        0x01

/* Flags used in SERIAL_API_GET_INIT_DATA */
#define GET_INIT_DATA_FLAG_SLAVE_API                    0x01
#define GET_INIT_DATA_FLAG_TIMER_SUPPORT                0x02
#define GET_INIT_DATA_FLAG_CONTROLLER_STATUS            0x04 /* Obsolete. USE next */
#define GET_INIT_DATA_FLAG_SECONDARY_CTRL               0x04
#define GET_INIT_DATA_FLAG_IS_SUC                       0x08

/* Function IDs */
#define FUNC_ID_SERIAL_API_GET_INIT_DATA                0x02
#define FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION        0x03
#define FUNC_ID_APPLICATION_COMMAND_HANDLER             0x04
#define FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES          0x05

/* SERIAL API ver 4 added - START */
#define FUNC_ID_SERIAL_API_SET_TIMEOUTS                 0x06
#define FUNC_ID_SERIAL_API_GET_CAPABILITIES             0x07
#define FUNC_ID_SERIAL_API_SOFT_RESET                   0x08
/* SERIAL API ver 4 added - END */

#define FUNC_ID_ZW_SET_RF_RECEIVE_MODE                  0x10
#define FUNC_ID_ZW_SET_SLEEP_MODE                       0x11
#define FUNC_ID_ZW_SEND_NODE_INFORMATION                0x12
#define FUNC_ID_ZW_SEND_DATA                            0x13
#define FUNC_ID_ZW_SEND_DATA_MULTI                      0x14
#define FUNC_ID_ZW_GET_VERSION                          0x15

/* SERIAL API ver 4 added - START */
#define FUNC_ID_ZW_SEND_DATA_ABORT                      0x16
#define FUNC_ID_ZW_RF_POWER_LEVEL_SET                   0x17
#define FUNC_ID_ZW_SEND_DATA_META                       0x18
/* SERIAL API ver 4 added - END */
#define FUNC_ID_ZW_SEND_DATA_MR                         0x19
#define FUNC_ID_ZW_SEND_DATA_META_MR                    0x1A
#define FUNC_ID_ZW_SET_ROUTING_INFO                     0x1B

#define FUNC_ID_ZW_GET_RANDOM                           0x1C
#define FUNC_ID_ZW_RANDOM                               0x1D
#define FUNC_ID_ZW_RF_POWER_LEVEL_REDISCOVERY_SET       0x1E
/* ZW030x only */
#define FUNC_ID_APPLICATION_RF_NOTIFY                   0x1F
/* ZW030x only end */

#define FUNC_ID_MEMORY_GET_ID                           0x20
#define FUNC_ID_MEMORY_GET_BYTE                         0x21
#define FUNC_ID_MEMORY_PUT_BYTE                         0x22
#define FUNC_ID_MEMORY_GET_BUFFER                       0x23
#define FUNC_ID_MEMORY_PUT_BUFFER                       0x24
/* Unimplemented - START */
#define FUNC_ID_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET  0x25
/* Unimplemented - END */

#define FUNC_ID_CLOCK_SET                               0x30
#define FUNC_ID_CLOCK_GET                               0x31
#define FUNC_ID_CLOCK_CMP                               0x32
#define FUNC_ID_RTC_TIMER_CREATE                        0x33
#define FUNC_ID_RTC_TIMER_READ                          0x34
#define FUNC_ID_RTC_TIMER_DELETE                        0x35
#define FUNC_ID_RTC_TIMER_CALL                          0x36

#define FUNC_ID_ZW_SET_LEARN_NODE_STATE                 0x40
#define FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO               0x41
#define FUNC_ID_ZW_SET_DEFAULT                          0x42
#define FUNC_ID_ZW_NEW_CONTROLLER                       0x43
#define FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE         0x44
#define FUNC_ID_ZW_REPLICATION_SEND_DATA                0x45
#define FUNC_ID_ZW_ASSIGN_RETURN_ROUTE                  0x46
#define FUNC_ID_ZW_DELETE_RETURN_ROUTE                  0x47
#define FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE         0x48
#define FUNC_ID_ZW_APPLICATION_UPDATE                   0x49

/*Obsolete use ZW_APPLICATION_UPDATE */
#define FUNC_ID_ZW_APPLICATION_CONTROLLER_UPDATE        0x49

#define FUNC_ID_ZW_ADD_NODE_TO_NETWORK                  0x4A
#define FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK             0x4B
#define FUNC_ID_ZW_CREATE_NEW_PRIMARY                   0x4C
#define FUNC_ID_ZW_CONTROLLER_CHANGE                    0x4D

#define FUNC_ID_ZW_RESERVED_FN                          0x4E
#define FUNC_ID_ZW_RESERVED_AR                          0x4F

#define FUNC_ID_ZW_SET_LEARN_MODE                       0x50

#define FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE              0x51
#define FUNC_ID_ZW_ENABLE_SUC                           0x52
#define FUNC_ID_ZW_REQUEST_NETWORK_UPDATE               0x53
#define FUNC_ID_ZW_SET_SUC_NODE_ID                      0x54
#define FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE              0x55
#define FUNC_ID_ZW_GET_SUC_NODE_ID                      0x56
#define FUNC_ID_ZW_SEND_SUC_ID                          0x57

#define FUNC_ID_ZW_RESERVED_ASR                         0x58
#define FUNC_ID_ZW_REDISCOVERY_NEEDED                   0x59

#define FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE_OPTION  0x5A

/* Slave only */
#define FUNC_ID_ZW_SUPPORT9600_ONLY                     0x5B
/* Slave only end */

/* Enhanced/Routing Slave only */
#define FUNC_ID_ZW_REQUEST_NEW_ROUTE_DESTINATIONS       0x5C
#define FUNC_ID_ZW_IS_NODE_WITHIN_DIRECT_RANGE          0x5D
/* Enhanced/Routing Slave only end */

#define FUNC_ID_ZW_EXPLORE_REQUEST_INCLUSION            0x5E

#define FUNC_ID_ZW_REQUEST_NODE_INFO                    0x60
#define FUNC_ID_ZW_REMOVE_FAILED_NODE_ID                0x61
#define FUNC_ID_ZW_IS_FAILED_NODE_ID                    0x62
#define FUNC_ID_ZW_REPLACE_FAILED_NODE                  0x63

#define FUNC_ID_TIMER_START                             0x70
#define FUNC_ID_TIMER_RESTART                           0x71
#define FUNC_ID_TIMER_CANCEL                            0x72
#define FUNC_ID_TIMER_CALL                              0x73

/* Installer API */
#define FUNC_ID_GET_ROUTING_TABLE_LINE                  0x80
#define FUNC_ID_GET_TX_COUNTER                          0x81
#define FUNC_ID_RESET_TX_COUNTER                        0x82
#define FUNC_ID_STORE_NODEINFO                          0x83
#define FUNC_ID_STORE_HOMEID                            0x84
/* Installer API only end */

#define FUNC_ID_LOCK_ROUTE_RESPONSE                     0x90

#ifdef ZW_CONTROLLER_SINGLE
#define FUNC_ID_SERIAL_API_TEST                         0x95
#endif

/* ZW_CONTROLLER_BRIDGE only START */
#define FUNC_ID_SERIAL_API_APPL_SLAVE_NODE_INFORMATION  0xA0
/* OBSOLETE: In DevKit 4.5x Controller Bridge applications, this is obsoleted */
/* by the FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE */
#define FUNC_ID_APPLICATION_SLAVE_COMMAND_HANDLER       0xA1
#define FUNC_ID_ZW_SEND_SLAVE_NODE_INFORMATION          0xA2
#define FUNC_ID_ZW_SEND_SLAVE_DATA                      0xA3
#define FUNC_ID_ZW_SET_SLAVE_LEARN_MODE                 0xA4
#define FUNC_ID_ZW_GET_VIRTUAL_NODES                    0xA5
#define FUNC_ID_ZW_IS_VIRTUAL_NODE                      0xA6
#define FUNC_ID_ZW_RESERVED_SSD                         0xA7

/* DevKit 4.5x added - obsoletes FUNC_ID_APPLICATION_SLAVE_COMMAND_HANDLER and */
/* FUNC_ID_APPLICATION_COMMAND_HANDLER for the Controller Bridge applications as */
/* this handles both cases - only for 4.5x based Controller Bridge applications */
#define FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE      0xA8
/* DevKit 4.5x added - Adds sourceNodeID to the parameter list */
#define FUNC_ID_ZW_SEND_DATA_BRIDGE                     0xA9
#define FUNC_ID_ZW_SEND_DATA_META_BRIDGE                0xAA
#define FUNC_ID_ZW_SEND_DATA_MULTI_BRIDGE               0xAB
/* ZW_CONTROLLER_BRIDGE only END */

#define FUNC_ID_PWR_SETSTOPMODE                         0xB0    // ZW102 only
#define FUNC_ID_PWR_CLK_PD                              0xB1    // ZW102 only
#define FUNC_ID_PWR_CLK_PUP                             0xB2    // ZW102 only
#define FUNC_ID_PWR_SELECT_CLK                          0xB3    // ZW102 only
#define FUNC_ID_ZW_SET_WUT_TIMEOUT                      0xB4    // ZW201 only
#define FUNC_ID_ZW_IS_WUT_KICKED                        0xB5    // ZW201 only

#define FUNC_ID_ZW_WATCHDOG_ENABLE                      0xB6
#define FUNC_ID_ZW_WATCHDOG_DISABLE                     0xB7
#define FUNC_ID_ZW_WATCHDOG_KICK                        0xB8
#define FUNC_ID_ZW_SET_EXT_INT_LEVEL                    0xB9    // ZW201 only

#define FUNC_ID_ZW_RF_POWER_LEVEL_GET                   0xBA
#define FUNC_ID_ZW_GET_NEIGHBOR_COUNT                   0xBB
#define FUNC_ID_ZW_ARE_NODES_NEIGHBOURS                 0xBC

#define FUNC_ID_ZW_TYPE_LIBRARY                         0xBD
#define FUNC_ID_ZW_SEND_TEST_FRAME                      0xBE
#define FUNC_ID_ZW_GET_PROTOCOL_STATUS                  0xBF

#define FUNC_ID_ZW_SET_PROMISCUOUS_MODE                 0xD0
/* SERIAL API ver 5 added - START */
#define FUNC_ID_PROMISCUOUS_APPLICATION_COMMAND_HANDLER 0xD1
/* SERIAL API ver 5 added - END */

#define FUNC_ID_ZW_WATCHDOG_START                       0xD2
#define FUNC_ID_ZW_WATCHDOG_STOP                        0xD3

#define FUNC_ID_ZW_SET_ROUTING_MAX                      0xD4
#define FUNC_ID_ZW_GET_ROUTING_MAX                      0xD5

/* Allocated for proprietary serial API commands */
#define FUNC_ID_PROPRIETARY_0                           0xF0
#define FUNC_ID_PROPRIETARY_1                           0xF1
#define FUNC_ID_PROPRIETARY_2                           0xF2
#define FUNC_ID_PROPRIETARY_3                           0xF3
#define FUNC_ID_PROPRIETARY_4                           0xF4
#define FUNC_ID_PROPRIETARY_5                           0xF5
#define FUNC_ID_PROPRIETARY_6                           0xF6
#define FUNC_ID_PROPRIETARY_7                           0xF7
#define FUNC_ID_PROPRIETARY_8                           0xF8
#define FUNC_ID_PROPRIETARY_9                           0xF9
#define FUNC_ID_PROPRIETARY_A                           0xFA
#define FUNC_ID_PROPRIETARY_B                           0xFB
#define FUNC_ID_PROPRIETARY_C                           0xFC
#define FUNC_ID_PROPRIETARY_D                           0xFD
#define FUNC_ID_PROPRIETARY_E                           0xFE


/* Illegal function ID */
#define FUNC_ID_UNKNOWN                                 0xFF


/* Mode parameters to ZW_AddNodeToNetwork */
#define ADD_NODE_ANY          1
#define ADD_NODE_CONTROLLER   2
#define ADD_NODE_SLAVE        3
#define ADD_NODE_EXISTING     4
#define ADD_NODE_STOP         5
#define ADD_NODE_STOP_FAILED  6

#define ADD_NODE_MODE_MASK                   0x0F
#define ADD_NODE_OPTION_HIGH_POWER           0x80
#define ADD_NODE_OPTION_NETWORK_WIDE         0x40

/* Callback states from ZW_AddNodeToNetwork */
#define ADD_NODE_STATUS_LEARN_READY          1
#define ADD_NODE_STATUS_NODE_FOUND           2
#define ADD_NODE_STATUS_ADDING_SLAVE         3
#define ADD_NODE_STATUS_ADDING_CONTROLLER    4
#define ADD_NODE_STATUS_PROTOCOL_DONE        5
#define ADD_NODE_STATUS_DONE                 6
#define ADD_NODE_STATUS_FAILED               7
#define ADD_NODE_STATUS_NOT_PRIMARY          0x23

/* Mode parameters to ZW_RemoveNodeFromNetwork */
#define REMOVE_NODE_ANY                     ADD_NODE_ANY
#define REMOVE_NODE_CONTROLLER              ADD_NODE_CONTROLLER
#define REMOVE_NODE_SLAVE                   ADD_NODE_SLAVE
#define REMOVE_NODE_STOP                    ADD_NODE_STOP

/* Callback states from ZW_RemoveNodeFromNetwork */
#define REMOVE_NODE_STATUS_LEARN_READY          ADD_NODE_STATUS_LEARN_READY
#define REMOVE_NODE_STATUS_NODE_FOUND           ADD_NODE_STATUS_NODE_FOUND
#define REMOVE_NODE_STATUS_REMOVING_SLAVE       ADD_NODE_STATUS_ADDING_SLAVE
#define REMOVE_NODE_STATUS_REMOVING_CONTROLLER  ADD_NODE_STATUS_ADDING_CONTROLLER
#define REMOVE_NODE_STATUS_DONE                 ADD_NODE_STATUS_DONE
#define REMOVE_NODE_STATUS_FAILED               ADD_NODE_STATUS_FAILED

/* Callback states from ZW_SetLearnMode */
#define LEARN_MODE_STARTED                  ADD_NODE_STATUS_LEARN_READY
#define LEARN_MODE_DONE                     ADD_NODE_STATUS_DONE
#define LEARN_MODE_FAILED                   ADD_NODE_STATUS_FAILED
#define LEARN_MODE_DELETED                  0x80


/* Transmit frame option flags */
#define TRANSMIT_OPTION_ACK           0x01    /* request acknowledge from destination node */
#define TRANSMIT_OPTION_LOW_POWER     0x02    /* transmit at low output power level (1/3 of normal RF range)*/
#define TRANSMIT_OPTION_AUTO_ROUTE    0x04    /* request retransmission via repeater nodes */
#define TRANSMIT_OPTION_NO_ROUTE      0x10		/* do not use response route - Even if available */
#define TRANSMIT_OPTION_EXPLORE       0x20		/* Use explore frame if needed */


/************ Basic Device Class identifiers **************/
#define BASIC_TYPE_CONTROLLER                                                            0x01 /*Node is a portable controller */
#define BASIC_TYPE_ROUTING_SLAVE                                                         0x04 /*Node is a slave with routing capabilities*/
#define BASIC_TYPE_SLAVE                                                                 0x03 /*Node is a slave*/
#define BASIC_TYPE_STATIC_CONTROLLER                                                     0x02 /*Node is a static controller*/


/***** Generic and Specific Device Class identifiers ******/
/* Device class Av Control Point */
#define GENERIC_TYPE_AV_CONTROL_POINT                                                    0x03 /*AV Control Point*/
#define SPECIFIC_TYPE_DOORBELL                                                           0x12
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SATELLITE_RECEIVER                                                 0x04 /*Satellite Receiver*/
#define SPECIFIC_TYPE_SATELLITE_RECEIVER_V2                                              0x11 /*Satellite Receiver V2*/

/* Device class Display */
#define GENERIC_TYPE_DISPLAY                                                             0x04
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_SIMPLE_DISPLAY                                                     0x01

/* Device class Entry Control */
#define GENERIC_TYPE_ENTRY_CONTROL                                                       0x40 /*Entry Control*/
#define SPECIFIC_TYPE_ADVANCED_DOOR_LOCK                                                 0x02 /*Advanced Door Lock*/
#define SPECIFIC_TYPE_DOOR_LOCK                                                          0x01 /*Door Lock*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class Not Used*/
#define SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK                                            0x03

/* Device class Generic Controller */
#define GENERIC_TYPE_GENERIC_CONTROLLER                                                  0x01 /*Remote Controller*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_PORTABLE_INSTALLER_TOOL                                            0x03
#define SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER                                         0x01 /*Portable Remote Controller*/
#define SPECIFIC_TYPE_PORTABLE_SCENE_CONTROLLER                                          0x02 /*Portable Scene Controller*/

/* Device class Meter */
#define GENERIC_TYPE_METER                                                               0x31 /*Meter*/
#define SPECIFIC_TYPE_ADV_ENERGY_CONTROL                                                 0x02
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SIMPLE_METER                                                       0x01 /*Simple Meter*/

/* Device class Meter Pulse */
#define GENERIC_TYPE_METER_PULSE                                                         0x30 /*Pulse Meter*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/

/* Device class Non Interoperable */
#define GENERIC_TYPE_NON_INTEROPERABLE                                                   0xFF /*Non interoperable*/

/* Device class Repeater Slave */
#define GENERIC_TYPE_REPEATER_SLAVE                                                      0x0F /*Repeater Slave*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_REPEATER_SLAVE                                                     0x01 /*Basic Repeater Slave*/

/* Device class Security Panel */
#define GENERIC_TYPE_SECURITY_PANEL                                                      0x17
#define SPECIFIC_TYPE_ZONED_SECURITY_PANEL                                               0x01

/* Device class Semi Interoperable */
#define GENERIC_TYPE_SEMI_INTEROPERABLE                                                  0x50 /*Semi Interoperable*/
#define SPECIFIC_TYPE_ENERGY_PRODUCTION                                                  0x01 /*Energy Production*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/

/* Device class Sensor Alarm */
#define GENERIC_TYPE_SENSOR_ALARM                                                        0xA1
#define SPECIFIC_TYPE_ADV_ZENSOR_NET_ALARM_SENSOR                                        0x05
#define SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR                                        0x0A
#define SPECIFIC_TYPE_BASIC_ROUTING_ALARM_SENSOR                                         0x01
#define SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR                                         0x06
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_ALARM_SENSOR                                      0x03
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR                                      0x08
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ROUTING_ALARM_SENSOR                                               0x02
#define SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR                                               0x07
#define SPECIFIC_TYPE_ZENSOR_NET_ALARM_SENSOR                                            0x04
#define SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR                                            0x09

/* Device class Sensor Binary */
#define GENERIC_TYPE_SENSOR_BINARY                                                       0x20 /*Binary Sensor*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_ROUTING_SENSOR_BINARY                                              0x01 /*Routing Binary Sensor*/

/* Device class Sensor Multilevel */
#define GENERIC_TYPE_SENSOR_MULTILEVEL                                                   0x21 /*Multilevel Sensor*/
#define SPECIFIC_TYPE_CHIMNEY_FAN                                                        0x02
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL                                          0x01 /*Routing Multilevel Sensor*/

/* Device class Static Controller */
#define GENERIC_TYPE_STATIC_CONTROLLER                                                   0x02 /*Static Controller*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_PC_CONTROLLER                                                      0x01 /*PC Controller*/
#define SPECIFIC_TYPE_SCENE_CONTROLLER                                                   0x02 /*Scene Controller*/
#define SPECIFIC_TYPE_STATIC_INSTALLER_TOOL                                              0x03

/* Device class Switch Binary */
#define GENERIC_TYPE_SWITCH_BINARY                                                       0x10 /*Binary Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_POWER_SWITCH_BINARY                                                0x01 /*Binary Power Switch*/
#define SPECIFIC_TYPE_SCENE_SWITCH_BINARY                                                0x03 /*Binary Scene Switch*/

/* Device class Switch Multilevel */
#define GENERIC_TYPE_SWITCH_MULTILEVEL                                                   0x11 /*Multilevel Switch*/
#define SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL                                              0x05
#define SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL                                              0x06
#define SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL                                              0x07
#define SPECIFIC_TYPE_MOTOR_MULTIPOSITION                                                0x03 /*Multiposition Motor*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL                                            0x01 /*Multilevel Power Switch*/
#define SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL                                            0x04 /*Multilevel Scene Switch*/

/* Device class Switch Remote */
#define GENERIC_TYPE_SWITCH_REMOTE                                                       0x12 /*Remote Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_BINARY                                               0x01 /*Binary Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_MULTILEVEL                                           0x02 /*Multilevel Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_BINARY                                        0x03 /*Binary Toggle Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_MULTILEVEL                                    0x04 /*Multilevel Toggle Remote Switch*/

/* Device class Switch Toggle */
#define GENERIC_TYPE_SWITCH_TOGGLE                                                       0x13 /*Toggle Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SWITCH_TOGGLE_BINARY                                               0x01 /*Binary Toggle Switch*/
#define SPECIFIC_TYPE_SWITCH_TOGGLE_MULTILEVEL                                           0x02 /*Multilevel Toggle Switch*/

/* Device class Thermostat */
#define GENERIC_TYPE_THERMOSTAT                                                          0x08 /*Thermostat*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SETBACK_SCHEDULE_THERMOSTAT                                        0x03 /*Setback Schedule Thermostat*/
#define SPECIFIC_TYPE_SETBACK_THERMOSTAT                                                 0x05
#define SPECIFIC_TYPE_SETPOINT_THERMOSTAT                                                0x04
#define SPECIFIC_TYPE_THERMOSTAT_GENERAL                                                 0x02 /*Thermostat General*/
#define SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2                                              0x06 /*Thermostat General V2*/
#define SPECIFIC_TYPE_THERMOSTAT_HEATING                                                 0x01 /*Thermostat Heating*/

/* Device class Ventilation */
#define GENERIC_TYPE_VENTILATION                                                         0x16
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_RESIDENTIAL_HRV                                                    0x01

/* Device class Window Covering */
#define GENERIC_TYPE_WINDOW_COVERING                                                     0x09 /*Window Covering*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING                                             0x01 /*Simple Window Covering Control*/

/* Device class Zip Gateway */
#define GENERIC_TYPE_ZIP_GATEWAY                                                         0x14
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ZIP_ADV_GATEWAY                                                    0x02
#define SPECIFIC_TYPE_ZIP_TUN_GATEWAY                                                    0x01

/* Device class Zip Node */
#define GENERIC_TYPE_ZIP_NODE                                                            0x15
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ZIP_ADV_NODE                                                       0x02
#define SPECIFIC_TYPE_ZIP_TUN_NODE                                                       0x01





//================== in file "ZW_classcmd.h"
/****************************************************************************
 *    TYPES and DEFINITIONS
 ***************************************************************************/
#define ZW_FRAME_COLLECTION_MACRO\
  ZW_COMMON_FRAME                                       ZW_Common;\
/* Command class Alarm */\
  ZW_ALARM_GET_FRAME                                           ZW_AlarmGetFrame;\
  ZW_ALARM_REPORT_FRAME                                        ZW_AlarmReportFrame;\
/* Command class Alarm V2 */\
  ZW_ALARM_GET_V2_FRAME                                        ZW_AlarmGetV2Frame;\
  ZW_ALARM_REPORT_1BYTE_V2_FRAME                               ZW_AlarmReport1byteV2Frame;\
  ZW_ALARM_REPORT_2BYTE_V2_FRAME                               ZW_AlarmReport2byteV2Frame;\
  ZW_ALARM_REPORT_3BYTE_V2_FRAME                               ZW_AlarmReport3byteV2Frame;\
  ZW_ALARM_REPORT_4BYTE_V2_FRAME                               ZW_AlarmReport4byteV2Frame;\
  ZW_ALARM_SET_V2_FRAME                                        ZW_AlarmSetV2Frame;\
  ZW_ALARM_TYPE_SUPPORTED_GET_V2_FRAME                         ZW_AlarmTypeSupportedGetV2Frame;\
  ZW_ALARM_TYPE_SUPPORTED_REPORT_1BYTE_V2_FRAME                ZW_AlarmTypeSupportedReport1byteV2Frame;\
  ZW_ALARM_TYPE_SUPPORTED_REPORT_2BYTE_V2_FRAME                ZW_AlarmTypeSupportedReport2byteV2Frame;\
  ZW_ALARM_TYPE_SUPPORTED_REPORT_3BYTE_V2_FRAME                ZW_AlarmTypeSupportedReport3byteV2Frame;\
  ZW_ALARM_TYPE_SUPPORTED_REPORT_4BYTE_V2_FRAME                ZW_AlarmTypeSupportedReport4byteV2Frame;\
/* Command class Application Status */\
  ZW_APPLICATION_BUSY_FRAME                                    ZW_ApplicationBusyFrame;\
  ZW_APPLICATION_REJECTED_REQUEST_FRAME                        ZW_ApplicationRejectedRequestFrame;\
/* Command class Association Command Configuration */\
  ZW_COMMAND_CONFIGURATION_GET_FRAME                           ZW_CommandConfigurationGetFrame;\
  ZW_COMMAND_CONFIGURATION_REPORT_1BYTE_FRAME                  ZW_CommandConfigurationReport1byteFrame;\
  ZW_COMMAND_CONFIGURATION_REPORT_2BYTE_FRAME                  ZW_CommandConfigurationReport2byteFrame;\
  ZW_COMMAND_CONFIGURATION_REPORT_3BYTE_FRAME                  ZW_CommandConfigurationReport3byteFrame;\
  ZW_COMMAND_CONFIGURATION_REPORT_4BYTE_FRAME                  ZW_CommandConfigurationReport4byteFrame;\
  ZW_COMMAND_CONFIGURATION_SET_1BYTE_FRAME                     ZW_CommandConfigurationSet1byteFrame;\
  ZW_COMMAND_CONFIGURATION_SET_2BYTE_FRAME                     ZW_CommandConfigurationSet2byteFrame;\
  ZW_COMMAND_CONFIGURATION_SET_3BYTE_FRAME                     ZW_CommandConfigurationSet3byteFrame;\
  ZW_COMMAND_CONFIGURATION_SET_4BYTE_FRAME                     ZW_CommandConfigurationSet4byteFrame;\
  ZW_COMMAND_RECORDS_SUPPORTED_GET_FRAME                       ZW_CommandRecordsSupportedGetFrame;\
  ZW_COMMAND_RECORDS_SUPPORTED_REPORT_FRAME                    ZW_CommandRecordsSupportedReportFrame;\
/* Command class Association */\
  ZW_ASSOCIATION_GET_FRAME                                     ZW_AssociationGetFrame;\
  ZW_ASSOCIATION_GROUPINGS_GET_FRAME                           ZW_AssociationGroupingsGetFrame;\
  ZW_ASSOCIATION_GROUPINGS_REPORT_FRAME                        ZW_AssociationGroupingsReportFrame;\
  ZW_ASSOCIATION_REMOVE_1BYTE_FRAME                            ZW_AssociationRemove1byteFrame;\
  ZW_ASSOCIATION_REMOVE_2BYTE_FRAME                            ZW_AssociationRemove2byteFrame;\
  ZW_ASSOCIATION_REMOVE_3BYTE_FRAME                            ZW_AssociationRemove3byteFrame;\
  ZW_ASSOCIATION_REMOVE_4BYTE_FRAME                            ZW_AssociationRemove4byteFrame;\
  ZW_ASSOCIATION_REPORT_1BYTE_FRAME                            ZW_AssociationReport1byteFrame;\
  ZW_ASSOCIATION_REPORT_2BYTE_FRAME                            ZW_AssociationReport2byteFrame;\
  ZW_ASSOCIATION_REPORT_3BYTE_FRAME                            ZW_AssociationReport3byteFrame;\
  ZW_ASSOCIATION_REPORT_4BYTE_FRAME                            ZW_AssociationReport4byteFrame;\
  ZW_ASSOCIATION_SET_1BYTE_FRAME                               ZW_AssociationSet1byteFrame;\
  ZW_ASSOCIATION_SET_2BYTE_FRAME                               ZW_AssociationSet2byteFrame;\
  ZW_ASSOCIATION_SET_3BYTE_FRAME                               ZW_AssociationSet3byteFrame;\
  ZW_ASSOCIATION_SET_4BYTE_FRAME                               ZW_AssociationSet4byteFrame;\
/* Command class Association V2 */\
  ZW_ASSOCIATION_GET_V2_FRAME                                  ZW_AssociationGetV2Frame;\
  ZW_ASSOCIATION_GROUPINGS_GET_V2_FRAME                        ZW_AssociationGroupingsGetV2Frame;\
  ZW_ASSOCIATION_GROUPINGS_REPORT_V2_FRAME                     ZW_AssociationGroupingsReportV2Frame;\
  ZW_ASSOCIATION_REMOVE_1BYTE_V2_FRAME                         ZW_AssociationRemove1byteV2Frame;\
  ZW_ASSOCIATION_REMOVE_2BYTE_V2_FRAME                         ZW_AssociationRemove2byteV2Frame;\
  ZW_ASSOCIATION_REMOVE_3BYTE_V2_FRAME                         ZW_AssociationRemove3byteV2Frame;\
  ZW_ASSOCIATION_REMOVE_4BYTE_V2_FRAME                         ZW_AssociationRemove4byteV2Frame;\
  ZW_ASSOCIATION_REPORT_1BYTE_V2_FRAME                         ZW_AssociationReport1byteV2Frame;\
  ZW_ASSOCIATION_REPORT_2BYTE_V2_FRAME                         ZW_AssociationReport2byteV2Frame;\
  ZW_ASSOCIATION_REPORT_3BYTE_V2_FRAME                         ZW_AssociationReport3byteV2Frame;\
  ZW_ASSOCIATION_REPORT_4BYTE_V2_FRAME                         ZW_AssociationReport4byteV2Frame;\
  ZW_ASSOCIATION_SET_1BYTE_V2_FRAME                            ZW_AssociationSet1byteV2Frame;\
  ZW_ASSOCIATION_SET_2BYTE_V2_FRAME                            ZW_AssociationSet2byteV2Frame;\
  ZW_ASSOCIATION_SET_3BYTE_V2_FRAME                            ZW_AssociationSet3byteV2Frame;\
  ZW_ASSOCIATION_SET_4BYTE_V2_FRAME                            ZW_AssociationSet4byteV2Frame;\
  ZW_ASSOCIATION_SPECIFIC_GROUP_GET_V2_FRAME                   ZW_AssociationSpecificGroupGetV2Frame;\
  ZW_ASSOCIATION_SPECIFIC_GROUP_REPORT_V2_FRAME                ZW_AssociationSpecificGroupReportV2Frame;\
/* Command class Av Content Directory Md */\
  ZW_AV_CONTENT_BROWSE_MD_BY_LETTER_GET_FRAME                  ZW_AvContentBrowseMdByLetterGetFrame;\
  ZW_AV_CONTENT_BROWSE_MD_BY_LETTER_REPORT_FRAME               ZW_AvContentBrowseMdByLetterReportFrame;\
  ZW_AV_CONTENT_BROWSE_MD_CHILD_COUNT_GET_FRAME                ZW_AvContentBrowseMdChildCountGetFrame;\
  ZW_AV_CONTENT_BROWSE_MD_CHILD_COUNT_REPORT_FRAME             ZW_AvContentBrowseMdChildCountReportFrame;\
  ZW_AV_CONTENT_BROWSE_MD_GET_FRAME                            ZW_AvContentBrowseMdGetFrame;\
  ZW_AV_CONTENT_BROWSE_MD_REPORT_FRAME                         ZW_AvContentBrowseMdReportFrame;\
  ZW_AV_MATCH_ITEM_TO_RENDERER_MD_GET_FRAME                    ZW_AvMatchItemToRendererMdGetFrame;\
  ZW_AV_MATCH_ITEM_TO_RENDERER_MD_REPORT_FRAME                 ZW_AvMatchItemToRendererMdReportFrame;\
/* Command class Av Content Search Md */\
  ZW_AV_CONTENT_SEARCH_MD_GET_FRAME                            ZW_AvContentSearchMdGetFrame;\
  ZW_AV_CONTENT_SEARCH_MD_REPORT_FRAME                         ZW_AvContentSearchMdReportFrame;\
/* Command class Av Renderer Status */\
  ZW_AV_RENDERER_STATUS_GET_FRAME                              ZW_AvRendererStatusGetFrame;\
  ZW_AV_RENDERER_STATUS_REPORT_FRAME                           ZW_AvRendererStatusReportFrame;\
/* Command class Av Tagging Md */\
  ZW_AV_TAGGING_MD_GET_FRAME                                   ZW_AvTaggingMdGetFrame;\
  ZW_AV_TAGGING_MD_REPORT_FRAME                                ZW_AvTaggingMdReportFrame;\
/* Command class Basic Tariff Info */\
  ZW_BASIC_TARIFF_INFO_GET_FRAME                               ZW_BasicTariffInfoGetFrame;\
  ZW_BASIC_TARIFF_INFO_REPORT_FRAME                            ZW_BasicTariffInfoReportFrame;\
/* Command class Basic Window Covering */\
  ZW_BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_FRAME            ZW_BasicWindowCoveringStartLevelChangeFrame;\
  ZW_BASIC_WINDOW_COVERING_STOP_LEVEL_CHANGE_FRAME             ZW_BasicWindowCoveringStopLevelChangeFrame;\
/* Command class Basic */\
  ZW_BASIC_GET_FRAME                                           ZW_BasicGetFrame;\
  ZW_BASIC_REPORT_FRAME                                        ZW_BasicReportFrame;\
  ZW_BASIC_SET_FRAME                                           ZW_BasicSetFrame;\
/* Command class Battery */\
  ZW_BATTERY_GET_FRAME                                         ZW_BatteryGetFrame;\
  ZW_BATTERY_REPORT_FRAME                                      ZW_BatteryReportFrame;\
/* Command class Chimney Fan */\
  ZW_CHIMNEY_FAN_ALARM_LOG_GET_FRAME                           ZW_ChimneyFanAlarmLogGetFrame;\
  ZW_CHIMNEY_FAN_ALARM_LOG_REPORT_FRAME                        ZW_ChimneyFanAlarmLogReportFrame;\
  ZW_CHIMNEY_FAN_ALARM_LOG_SET_FRAME                           ZW_ChimneyFanAlarmLogSetFrame;\
  ZW_CHIMNEY_FAN_ALARM_STATUS_GET_FRAME                        ZW_ChimneyFanAlarmStatusGetFrame;\
  ZW_CHIMNEY_FAN_ALARM_STATUS_REPORT_FRAME                     ZW_ChimneyFanAlarmStatusReportFrame;\
  ZW_CHIMNEY_FAN_ALARM_STATUS_SET_FRAME                        ZW_ChimneyFanAlarmStatusSetFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_GET_FRAME                          ZW_ChimneyFanAlarmTempGetFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_1BYTE_FRAME                 ZW_ChimneyFanAlarmTempReport1byteFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_2BYTE_FRAME                 ZW_ChimneyFanAlarmTempReport2byteFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_3BYTE_FRAME                 ZW_ChimneyFanAlarmTempReport3byteFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_4BYTE_FRAME                 ZW_ChimneyFanAlarmTempReport4byteFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_SET_1BYTE_FRAME                    ZW_ChimneyFanAlarmTempSet1byteFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_SET_2BYTE_FRAME                    ZW_ChimneyFanAlarmTempSet2byteFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_SET_3BYTE_FRAME                    ZW_ChimneyFanAlarmTempSet3byteFrame;\
  ZW_CHIMNEY_FAN_ALARM_TEMP_SET_4BYTE_FRAME                    ZW_ChimneyFanAlarmTempSet4byteFrame;\
  ZW_CHIMNEY_FAN_BOOST_TIME_GET_FRAME                          ZW_ChimneyFanBoostTimeGetFrame;\
  ZW_CHIMNEY_FAN_BOOST_TIME_REPORT_FRAME                       ZW_ChimneyFanBoostTimeReportFrame;\
  ZW_CHIMNEY_FAN_BOOST_TIME_SET_FRAME                          ZW_ChimneyFanBoostTimeSetFrame;\
  ZW_CHIMNEY_FAN_DEFAULT_SET_FRAME                             ZW_ChimneyFanDefaultSetFrame;\
  ZW_CHIMNEY_FAN_MIN_SPEED_GET_FRAME                           ZW_ChimneyFanMinSpeedGetFrame;\
  ZW_CHIMNEY_FAN_MIN_SPEED_REPORT_FRAME                        ZW_ChimneyFanMinSpeedReportFrame;\
  ZW_CHIMNEY_FAN_MIN_SPEED_SET_FRAME                           ZW_ChimneyFanMinSpeedSetFrame;\
  ZW_CHIMNEY_FAN_MODE_GET_FRAME                                ZW_ChimneyFanModeGetFrame;\
  ZW_CHIMNEY_FAN_MODE_REPORT_FRAME                             ZW_ChimneyFanModeReportFrame;\
  ZW_CHIMNEY_FAN_MODE_SET_FRAME                                ZW_ChimneyFanModeSetFrame;\
  ZW_CHIMNEY_FAN_SETUP_GET_FRAME                               ZW_ChimneyFanSetupGetFrame;\
  ZW_CHIMNEY_FAN_SETUP_REPORT_1BYTE_FRAME                      ZW_ChimneyFanSetupReport1byteFrame;\
  ZW_CHIMNEY_FAN_SETUP_REPORT_2BYTE_FRAME                      ZW_ChimneyFanSetupReport2byteFrame;\
  ZW_CHIMNEY_FAN_SETUP_REPORT_3BYTE_FRAME                      ZW_ChimneyFanSetupReport3byteFrame;\
  ZW_CHIMNEY_FAN_SETUP_REPORT_4BYTE_FRAME                      ZW_ChimneyFanSetupReport4byteFrame;\
  ZW_CHIMNEY_FAN_SETUP_SET_1BYTE_FRAME                         ZW_ChimneyFanSetupSet1byteFrame;\
  ZW_CHIMNEY_FAN_SETUP_SET_2BYTE_FRAME                         ZW_ChimneyFanSetupSet2byteFrame;\
  ZW_CHIMNEY_FAN_SETUP_SET_3BYTE_FRAME                         ZW_ChimneyFanSetupSet3byteFrame;\
  ZW_CHIMNEY_FAN_SETUP_SET_4BYTE_FRAME                         ZW_ChimneyFanSetupSet4byteFrame;\
  ZW_CHIMNEY_FAN_SPEED_GET_FRAME                               ZW_ChimneyFanSpeedGetFrame;\
  ZW_CHIMNEY_FAN_SPEED_REPORT_FRAME                            ZW_ChimneyFanSpeedReportFrame;\
  ZW_CHIMNEY_FAN_SPEED_SET_FRAME                               ZW_ChimneyFanSpeedSetFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_GET_FRAME                          ZW_ChimneyFanStartTempGetFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_REPORT_1BYTE_FRAME                 ZW_ChimneyFanStartTempReport1byteFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_REPORT_2BYTE_FRAME                 ZW_ChimneyFanStartTempReport2byteFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_REPORT_3BYTE_FRAME                 ZW_ChimneyFanStartTempReport3byteFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_REPORT_4BYTE_FRAME                 ZW_ChimneyFanStartTempReport4byteFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_SET_1BYTE_FRAME                    ZW_ChimneyFanStartTempSet1byteFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_SET_2BYTE_FRAME                    ZW_ChimneyFanStartTempSet2byteFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_SET_3BYTE_FRAME                    ZW_ChimneyFanStartTempSet3byteFrame;\
  ZW_CHIMNEY_FAN_START_TEMP_SET_4BYTE_FRAME                    ZW_ChimneyFanStartTempSet4byteFrame;\
  ZW_CHIMNEY_FAN_STATE_GET_FRAME                               ZW_ChimneyFanStateGetFrame;\
  ZW_CHIMNEY_FAN_STATE_REPORT_FRAME                            ZW_ChimneyFanStateReportFrame;\
  ZW_CHIMNEY_FAN_STATE_SET_FRAME                               ZW_ChimneyFanStateSetFrame;\
  ZW_CHIMNEY_FAN_STATUS_GET_FRAME                              ZW_ChimneyFanStatusGetFrame;\
  ZW_CHIMNEY_FAN_STATUS_REPORT_1BYTE_FRAME                     ZW_ChimneyFanStatusReport1byteFrame;\
  ZW_CHIMNEY_FAN_STATUS_REPORT_2BYTE_FRAME                     ZW_ChimneyFanStatusReport2byteFrame;\
  ZW_CHIMNEY_FAN_STATUS_REPORT_3BYTE_FRAME                     ZW_ChimneyFanStatusReport3byteFrame;\
  ZW_CHIMNEY_FAN_STATUS_REPORT_4BYTE_FRAME                     ZW_ChimneyFanStatusReport4byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_GET_FRAME                           ZW_ChimneyFanStopTempGetFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_1BYTE_FRAME                  ZW_ChimneyFanStopTempReport1byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_2BYTE_FRAME                  ZW_ChimneyFanStopTempReport2byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_3BYTE_FRAME                  ZW_ChimneyFanStopTempReport3byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_4BYTE_FRAME                  ZW_ChimneyFanStopTempReport4byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_SET_1BYTE_FRAME                     ZW_ChimneyFanStopTempSet1byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_SET_2BYTE_FRAME                     ZW_ChimneyFanStopTempSet2byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_SET_3BYTE_FRAME                     ZW_ChimneyFanStopTempSet3byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TEMP_SET_4BYTE_FRAME                     ZW_ChimneyFanStopTempSet4byteFrame;\
  ZW_CHIMNEY_FAN_STOP_TIME_GET_FRAME                           ZW_ChimneyFanStopTimeGetFrame;\
  ZW_CHIMNEY_FAN_STOP_TIME_REPORT_FRAME                        ZW_ChimneyFanStopTimeReportFrame;\
  ZW_CHIMNEY_FAN_STOP_TIME_SET_FRAME                           ZW_ChimneyFanStopTimeSetFrame;\
/* Command class Climate Control Schedule */\
  ZW_SCHEDULE_CHANGED_GET_FRAME                                ZW_ScheduleChangedGetFrame;\
  ZW_SCHEDULE_CHANGED_REPORT_FRAME                             ZW_ScheduleChangedReportFrame;\
  ZW_SCHEDULE_GET_FRAME                                        ZW_ScheduleGetFrame;\
  ZW_SCHEDULE_OVERRIDE_GET_FRAME                               ZW_ScheduleOverrideGetFrame;\
  ZW_SCHEDULE_OVERRIDE_REPORT_FRAME                            ZW_ScheduleOverrideReportFrame;\
  ZW_SCHEDULE_OVERRIDE_SET_FRAME                               ZW_ScheduleOverrideSetFrame;\
  ZW_SCHEDULE_REPORT_FRAME                                     ZW_ScheduleReportFrame;\
  ZW_SCHEDULE_SET_FRAME                                        ZW_ScheduleSetFrame;\
/* Command class Clock */\
  ZW_CLOCK_GET_FRAME                                           ZW_ClockGetFrame;\
  ZW_CLOCK_REPORT_FRAME                                        ZW_ClockReportFrame;\
  ZW_CLOCK_SET_FRAME                                           ZW_ClockSetFrame;\
/* Command class Configuration */\
  ZW_CONFIGURATION_GET_FRAME                                   ZW_ConfigurationGetFrame;\
  ZW_CONFIGURATION_REPORT_1BYTE_FRAME                          ZW_ConfigurationReport1byteFrame;\
  ZW_CONFIGURATION_REPORT_2BYTE_FRAME                          ZW_ConfigurationReport2byteFrame;\
  ZW_CONFIGURATION_REPORT_3BYTE_FRAME                          ZW_ConfigurationReport3byteFrame;\
  ZW_CONFIGURATION_REPORT_4BYTE_FRAME                          ZW_ConfigurationReport4byteFrame;\
  ZW_CONFIGURATION_SET_1BYTE_FRAME                             ZW_ConfigurationSet1byteFrame;\
  ZW_CONFIGURATION_SET_2BYTE_FRAME                             ZW_ConfigurationSet2byteFrame;\
  ZW_CONFIGURATION_SET_3BYTE_FRAME                             ZW_ConfigurationSet3byteFrame;\
  ZW_CONFIGURATION_SET_4BYTE_FRAME                             ZW_ConfigurationSet4byteFrame;\
/* Command class Configuration V2 */\
  ZW_CONFIGURATION_BULK_GET_V2_FRAME                           ZW_ConfigurationBulkGetV2Frame;\
  VG_CONFIGURATION_BULK_REPORT_1BYTE_V2_VG                     VG_ConfigurationBulkReport1byteV2VGroup;\
  VG_CONFIGURATION_BULK_REPORT_2BYTE_V2_VG                     VG_ConfigurationBulkReport2byteV2VGroup;\
  VG_CONFIGURATION_BULK_REPORT_3BYTE_V2_VG                     VG_ConfigurationBulkReport3byteV2VGroup;\
  VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG                     VG_ConfigurationBulkReport4byteV2VGroup;\
  ZW_CONFIGURATION_BULK_REPORT_1BYTE_V2_FRAME                  ZW_ConfigurationBulkReport1byteV2Frame;\
  ZW_CONFIGURATION_BULK_REPORT_2BYTE_V2_FRAME                  ZW_ConfigurationBulkReport2byteV2Frame;\
  ZW_CONFIGURATION_BULK_REPORT_3BYTE_V2_FRAME                  ZW_ConfigurationBulkReport3byteV2Frame;\
  ZW_CONFIGURATION_BULK_REPORT_4BYTE_V2_FRAME                  ZW_ConfigurationBulkReport4byteV2Frame;\
  VG_CONFIGURATION_BULK_SET_1BYTE_V2_VG                        VG_ConfigurationBulkSet1byteV2VGroup;\
  VG_CONFIGURATION_BULK_SET_2BYTE_V2_VG                        VG_ConfigurationBulkSet2byteV2VGroup;\
  VG_CONFIGURATION_BULK_SET_3BYTE_V2_VG                        VG_ConfigurationBulkSet3byteV2VGroup;\
  VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG                        VG_ConfigurationBulkSet4byteV2VGroup;\
  ZW_CONFIGURATION_BULK_SET_1BYTE_V2_FRAME                     ZW_ConfigurationBulkSet1byteV2Frame;\
  ZW_CONFIGURATION_BULK_SET_2BYTE_V2_FRAME                     ZW_ConfigurationBulkSet2byteV2Frame;\
  ZW_CONFIGURATION_BULK_SET_3BYTE_V2_FRAME                     ZW_ConfigurationBulkSet3byteV2Frame;\
  ZW_CONFIGURATION_BULK_SET_4BYTE_V2_FRAME                     ZW_ConfigurationBulkSet4byteV2Frame;\
  ZW_CONFIGURATION_GET_V2_FRAME                                ZW_ConfigurationGetV2Frame;\
  ZW_CONFIGURATION_REPORT_1BYTE_V2_FRAME                       ZW_ConfigurationReport1byteV2Frame;\
  ZW_CONFIGURATION_REPORT_2BYTE_V2_FRAME                       ZW_ConfigurationReport2byteV2Frame;\
  ZW_CONFIGURATION_REPORT_3BYTE_V2_FRAME                       ZW_ConfigurationReport3byteV2Frame;\
  ZW_CONFIGURATION_REPORT_4BYTE_V2_FRAME                       ZW_ConfigurationReport4byteV2Frame;\
  ZW_CONFIGURATION_SET_1BYTE_V2_FRAME                          ZW_ConfigurationSet1byteV2Frame;\
  ZW_CONFIGURATION_SET_2BYTE_V2_FRAME                          ZW_ConfigurationSet2byteV2Frame;\
  ZW_CONFIGURATION_SET_3BYTE_V2_FRAME                          ZW_ConfigurationSet3byteV2Frame;\
  ZW_CONFIGURATION_SET_4BYTE_V2_FRAME                          ZW_ConfigurationSet4byteV2Frame;\
/* Command class Controller Replication */\
  ZW_CTRL_REPLICATION_TRANSFER_GROUP_FRAME                     ZW_CtrlReplicationTransferGroupFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_1BYTE_FRAME          ZW_CtrlReplicationTransferGroupName1byteFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_2BYTE_FRAME          ZW_CtrlReplicationTransferGroupName2byteFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_3BYTE_FRAME          ZW_CtrlReplicationTransferGroupName3byteFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_4BYTE_FRAME          ZW_CtrlReplicationTransferGroupName4byteFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_SCENE_FRAME                     ZW_CtrlReplicationTransferSceneFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_1BYTE_FRAME          ZW_CtrlReplicationTransferSceneName1byteFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_2BYTE_FRAME          ZW_CtrlReplicationTransferSceneName2byteFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_3BYTE_FRAME          ZW_CtrlReplicationTransferSceneName3byteFrame;\
  ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_4BYTE_FRAME          ZW_CtrlReplicationTransferSceneName4byteFrame;\
/* Command class Dcp Config */\
  ZW_DCP_LIST_REMOVE_FRAME                                     ZW_DcpListRemoveFrame;\
  VG_DCP_LIST_SET_VG                                           VG_DcpListSetVGroup;\
  ZW_DCP_LIST_SET_1BYTE_FRAME                                  ZW_DcpListSet1byteFrame;\
  ZW_DCP_LIST_SET_2BYTE_FRAME                                  ZW_DcpListSet2byteFrame;\
  ZW_DCP_LIST_SET_3BYTE_FRAME                                  ZW_DcpListSet3byteFrame;\
  ZW_DCP_LIST_SET_4BYTE_FRAME                                  ZW_DcpListSet4byteFrame;\
  ZW_DCP_LIST_SUPPORTED_GET_FRAME                              ZW_DcpListSupportedGetFrame;\
  ZW_DCP_LIST_SUPPORTED_REPORT_FRAME                           ZW_DcpListSupportedReportFrame;\
/* Command class Dcp Monitor */\
  ZW_DCP_EVENT_STATUS_GET_FRAME                                ZW_DcpEventStatusGetFrame;\
  ZW_DCP_EVENT_STATUS_REPORT_FRAME                             ZW_DcpEventStatusReportFrame;\
  ZW_DCP_LIST_GET_FRAME                                        ZW_DcpListGetFrame;\
  VG_DCP_LIST_REPORT_VG                                        VG_DcpListReportVGroup;\
  ZW_DCP_LIST_REPORT_1BYTE_FRAME                               ZW_DcpListReport1byteFrame;\
  ZW_DCP_LIST_REPORT_2BYTE_FRAME                               ZW_DcpListReport2byteFrame;\
  ZW_DCP_LIST_REPORT_3BYTE_FRAME                               ZW_DcpListReport3byteFrame;\
  ZW_DCP_LIST_REPORT_4BYTE_FRAME                               ZW_DcpListReport4byteFrame;\
/* Command class Door Lock Logging */\
  ZW_DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_GET_FRAME             ZW_DoorLockLoggingRecordsSupportedGetFrame;\
  ZW_DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT_FRAME          ZW_DoorLockLoggingRecordsSupportedReportFrame;\
  ZW_RECORD_GET_FRAME                                          ZW_RecordGetFrame;\
  ZW_RECORD_REPORT_1BYTE_FRAME                                 ZW_RecordReport1byteFrame;\
  ZW_RECORD_REPORT_2BYTE_FRAME                                 ZW_RecordReport2byteFrame;\
  ZW_RECORD_REPORT_3BYTE_FRAME                                 ZW_RecordReport3byteFrame;\
  ZW_RECORD_REPORT_4BYTE_FRAME                                 ZW_RecordReport4byteFrame;\
/* Command class Door Lock */\
  ZW_DOOR_LOCK_CONFIGURATION_GET_FRAME                         ZW_DoorLockConfigurationGetFrame;\
  ZW_DOOR_LOCK_CONFIGURATION_REPORT_FRAME                      ZW_DoorLockConfigurationReportFrame;\
  ZW_DOOR_LOCK_CONFIGURATION_SET_FRAME                         ZW_DoorLockConfigurationSetFrame;\
  ZW_DOOR_LOCK_OPERATION_GET_FRAME                             ZW_DoorLockOperationGetFrame;\
  ZW_DOOR_LOCK_OPERATION_REPORT_FRAME                          ZW_DoorLockOperationReportFrame;\
  ZW_DOOR_LOCK_OPERATION_SET_FRAME                             ZW_DoorLockOperationSetFrame;\
/* Command class Energy Production */\
  ZW_ENERGY_PRODUCTION_GET_FRAME                               ZW_EnergyProductionGetFrame;\
  ZW_ENERGY_PRODUCTION_REPORT_1BYTE_FRAME                      ZW_EnergyProductionReport1byteFrame;\
  ZW_ENERGY_PRODUCTION_REPORT_2BYTE_FRAME                      ZW_EnergyProductionReport2byteFrame;\
  ZW_ENERGY_PRODUCTION_REPORT_3BYTE_FRAME                      ZW_EnergyProductionReport3byteFrame;\
  ZW_ENERGY_PRODUCTION_REPORT_4BYTE_FRAME                      ZW_EnergyProductionReport4byteFrame;\
/* Command class Firmware Update Md */\
  ZW_FIRMWARE_MD_GET_FRAME                                     ZW_FirmwareMdGetFrame;\
  ZW_FIRMWARE_MD_REPORT_FRAME                                  ZW_FirmwareMdReportFrame;\
  ZW_FIRMWARE_UPDATE_MD_GET_FRAME                              ZW_FirmwareUpdateMdGetFrame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_1BYTE_FRAME                     ZW_FirmwareUpdateMdReport1byteFrame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_2BYTE_FRAME                     ZW_FirmwareUpdateMdReport2byteFrame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_3BYTE_FRAME                     ZW_FirmwareUpdateMdReport3byteFrame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_4BYTE_FRAME                     ZW_FirmwareUpdateMdReport4byteFrame;\
  ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_FRAME                      ZW_FirmwareUpdateMdRequestGetFrame;\
  ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_FRAME                   ZW_FirmwareUpdateMdRequestReportFrame;\
  ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_FRAME                    ZW_FirmwareUpdateMdStatusReportFrame;\
/* Command class Firmware Update Md V2 */\
  ZW_FIRMWARE_MD_GET_V2_FRAME                                  ZW_FirmwareMdGetV2Frame;\
  ZW_FIRMWARE_MD_REPORT_V2_FRAME                               ZW_FirmwareMdReportV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_GET_V2_FRAME                           ZW_FirmwareUpdateMdGetV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_1BYTE_V2_FRAME                  ZW_FirmwareUpdateMdReport1byteV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_2BYTE_V2_FRAME                  ZW_FirmwareUpdateMdReport2byteV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_3BYTE_V2_FRAME                  ZW_FirmwareUpdateMdReport3byteV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_REPORT_4BYTE_V2_FRAME                  ZW_FirmwareUpdateMdReport4byteV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V2_FRAME                   ZW_FirmwareUpdateMdRequestGetV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_V2_FRAME                ZW_FirmwareUpdateMdRequestReportV2Frame;\
  ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V2_FRAME                 ZW_FirmwareUpdateMdStatusReportV2Frame;\
/* Command class Geographic Location */\
  ZW_GEOGRAPHIC_LOCATION_GET_FRAME                             ZW_GeographicLocationGetFrame;\
  ZW_GEOGRAPHIC_LOCATION_REPORT_FRAME                          ZW_GeographicLocationReportFrame;\
  ZW_GEOGRAPHIC_LOCATION_SET_FRAME                             ZW_GeographicLocationSetFrame;\
/* Command class Grouping Name */\
  ZW_GROUPING_NAME_GET_FRAME                                   ZW_GroupingNameGetFrame;\
  ZW_GROUPING_NAME_REPORT_FRAME                                ZW_GroupingNameReportFrame;\
  ZW_GROUPING_NAME_SET_FRAME                                   ZW_GroupingNameSetFrame;\
/* Command class Hail */\
  ZW_HAIL_FRAME                                                ZW_HailFrame;\
/* Command class Hrv Control */\
  ZW_HRV_CONTROL_BYPASS_GET_FRAME                              ZW_HrvControlBypassGetFrame;\
  ZW_HRV_CONTROL_BYPASS_REPORT_FRAME                           ZW_HrvControlBypassReportFrame;\
  ZW_HRV_CONTROL_BYPASS_SET_FRAME                              ZW_HrvControlBypassSetFrame;\
  ZW_HRV_CONTROL_MODE_GET_FRAME                                ZW_HrvControlModeGetFrame;\
  ZW_HRV_CONTROL_MODE_REPORT_FRAME                             ZW_HrvControlModeReportFrame;\
  ZW_HRV_CONTROL_MODE_SET_FRAME                                ZW_HrvControlModeSetFrame;\
  ZW_HRV_CONTROL_MODE_SUPPORTED_GET_FRAME                      ZW_HrvControlModeSupportedGetFrame;\
  ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_1BYTE_FRAME             ZW_HrvControlModeSupportedReport1byteFrame;\
  ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_2BYTE_FRAME             ZW_HrvControlModeSupportedReport2byteFrame;\
  ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_3BYTE_FRAME             ZW_HrvControlModeSupportedReport3byteFrame;\
  ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_4BYTE_FRAME             ZW_HrvControlModeSupportedReport4byteFrame;\
  ZW_HRV_CONTROL_VENTILATION_RATE_GET_FRAME                    ZW_HrvControlVentilationRateGetFrame;\
  ZW_HRV_CONTROL_VENTILATION_RATE_REPORT_FRAME                 ZW_HrvControlVentilationRateReportFrame;\
  ZW_HRV_CONTROL_VENTILATION_RATE_SET_FRAME                    ZW_HrvControlVentilationRateSetFrame;\
/* Command class Hrv Status */\
  ZW_HRV_STATUS_GET_FRAME                                      ZW_HrvStatusGetFrame;\
  ZW_HRV_STATUS_REPORT_1BYTE_FRAME                             ZW_HrvStatusReport1byteFrame;\
  ZW_HRV_STATUS_REPORT_2BYTE_FRAME                             ZW_HrvStatusReport2byteFrame;\
  ZW_HRV_STATUS_REPORT_3BYTE_FRAME                             ZW_HrvStatusReport3byteFrame;\
  ZW_HRV_STATUS_REPORT_4BYTE_FRAME                             ZW_HrvStatusReport4byteFrame;\
  ZW_HRV_STATUS_SUPPORTED_GET_FRAME                            ZW_HrvStatusSupportedGetFrame;\
  ZW_HRV_STATUS_SUPPORTED_REPORT_1BYTE_FRAME                   ZW_HrvStatusSupportedReport1byteFrame;\
  ZW_HRV_STATUS_SUPPORTED_REPORT_2BYTE_FRAME                   ZW_HrvStatusSupportedReport2byteFrame;\
  ZW_HRV_STATUS_SUPPORTED_REPORT_3BYTE_FRAME                   ZW_HrvStatusSupportedReport3byteFrame;\
  ZW_HRV_STATUS_SUPPORTED_REPORT_4BYTE_FRAME                   ZW_HrvStatusSupportedReport4byteFrame;\
/* Command class Indicator */\
  ZW_INDICATOR_GET_FRAME                                       ZW_IndicatorGetFrame;\
  ZW_INDICATOR_REPORT_FRAME                                    ZW_IndicatorReportFrame;\
  ZW_INDICATOR_SET_FRAME                                       ZW_IndicatorSetFrame;\
/* Command class Ip Configuration */\
  ZW_IP_CONFIGURATION_GET_FRAME                                ZW_IpConfigurationGetFrame;\
  ZW_IP_CONFIGURATION_RELEASE_FRAME                            ZW_IpConfigurationReleaseFrame;\
  ZW_IP_CONFIGURATION_RENEW_FRAME                              ZW_IpConfigurationRenewFrame;\
  ZW_IP_CONFIGURATION_REPORT_FRAME                             ZW_IpConfigurationReportFrame;\
  ZW_IP_CONFIGURATION_SET_FRAME                                ZW_IpConfigurationSetFrame;\
/* Command class Language */\
  ZW_LANGUAGE_GET_FRAME                                        ZW_LanguageGetFrame;\
  ZW_LANGUAGE_REPORT_FRAME                                     ZW_LanguageReportFrame;\
  ZW_LANGUAGE_SET_FRAME                                        ZW_LanguageSetFrame;\
/* Command class Lock */\
  ZW_LOCK_GET_FRAME                                            ZW_LockGetFrame;\
  ZW_LOCK_REPORT_FRAME                                         ZW_LockReportFrame;\
  ZW_LOCK_SET_FRAME                                            ZW_LockSetFrame;\
/* Command class Manufacturer Specific */\
  ZW_MANUFACTURER_SPECIFIC_GET_FRAME                           ZW_ManufacturerSpecificGetFrame;\
  ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME                        ZW_ManufacturerSpecificReportFrame;\
/* Command class Meter Pulse */\
  ZW_METER_PULSE_GET_FRAME                                     ZW_MeterPulseGetFrame;\
  ZW_METER_PULSE_REPORT_FRAME                                  ZW_MeterPulseReportFrame;\
/* Command class Meter Tbl Config */\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_1BYTE_FRAME              ZW_MeterTblTablePointAdmNoSet1byteFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_2BYTE_FRAME              ZW_MeterTblTablePointAdmNoSet2byteFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_3BYTE_FRAME              ZW_MeterTblTablePointAdmNoSet3byteFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_4BYTE_FRAME              ZW_MeterTblTablePointAdmNoSet4byteFrame;\
/* Command class Meter Tbl Monitor */\
  VG_METER_TBL_STATUS_REPORT_VG                                VG_MeterTblStatusReportVGroup;\
  ZW_METER_TBL_STATUS_REPORT_1BYTE_FRAME                       ZW_MeterTblStatusReport1byteFrame;\
  ZW_METER_TBL_STATUS_REPORT_2BYTE_FRAME                       ZW_MeterTblStatusReport2byteFrame;\
  ZW_METER_TBL_STATUS_REPORT_3BYTE_FRAME                       ZW_MeterTblStatusReport3byteFrame;\
  ZW_METER_TBL_STATUS_REPORT_4BYTE_FRAME                       ZW_MeterTblStatusReport4byteFrame;\
  ZW_METER_TBL_STATUS_DATE_GET_FRAME                           ZW_MeterTblStatusDateGetFrame;\
  ZW_METER_TBL_STATUS_DEPTH_GET_FRAME                          ZW_MeterTblStatusDepthGetFrame;\
  ZW_METER_TBL_STATUS_SUPPORTED_GET_FRAME                      ZW_MeterTblStatusSupportedGetFrame;\
  ZW_METER_TBL_STATUS_SUPPORTED_REPORT_FRAME                   ZW_MeterTblStatusSupportedReportFrame;\
  ZW_METER_TBL_CURRENT_DATA_GET_FRAME                          ZW_MeterTblCurrentDataGetFrame;\
  VG_METER_TBL_CURRENT_DATA_REPORT_VG                          VG_MeterTblCurrentDataReportVGroup;\
  ZW_METER_TBL_CURRENT_DATA_REPORT_1BYTE_FRAME                 ZW_MeterTblCurrentDataReport1byteFrame;\
  ZW_METER_TBL_CURRENT_DATA_REPORT_2BYTE_FRAME                 ZW_MeterTblCurrentDataReport2byteFrame;\
  ZW_METER_TBL_CURRENT_DATA_REPORT_3BYTE_FRAME                 ZW_MeterTblCurrentDataReport3byteFrame;\
  ZW_METER_TBL_CURRENT_DATA_REPORT_4BYTE_FRAME                 ZW_MeterTblCurrentDataReport4byteFrame;\
  ZW_METER_TBL_HISTORICAL_DATA_GET_FRAME                       ZW_MeterTblHistoricalDataGetFrame;\
  VG_METER_TBL_HISTORICAL_DATA_REPORT_VG                       VG_MeterTblHistoricalDataReportVGroup;\
  ZW_METER_TBL_HISTORICAL_DATA_REPORT_1BYTE_FRAME              ZW_MeterTblHistoricalDataReport1byteFrame;\
  ZW_METER_TBL_HISTORICAL_DATA_REPORT_2BYTE_FRAME              ZW_MeterTblHistoricalDataReport2byteFrame;\
  ZW_METER_TBL_HISTORICAL_DATA_REPORT_3BYTE_FRAME              ZW_MeterTblHistoricalDataReport3byteFrame;\
  ZW_METER_TBL_HISTORICAL_DATA_REPORT_4BYTE_FRAME              ZW_MeterTblHistoricalDataReport4byteFrame;\
  ZW_METER_TBL_REPORT_FRAME                                    ZW_MeterTblReportFrame;\
  ZW_METER_TBL_TABLE_CAPABILITY_GET_FRAME                      ZW_MeterTblTableCapabilityGetFrame;\
  ZW_METER_TBL_TABLE_ID_GET_FRAME                              ZW_MeterTblTableIdGetFrame;\
  ZW_METER_TBL_TABLE_ID_REPORT_1BYTE_FRAME                     ZW_MeterTblTableIdReport1byteFrame;\
  ZW_METER_TBL_TABLE_ID_REPORT_2BYTE_FRAME                     ZW_MeterTblTableIdReport2byteFrame;\
  ZW_METER_TBL_TABLE_ID_REPORT_3BYTE_FRAME                     ZW_MeterTblTableIdReport3byteFrame;\
  ZW_METER_TBL_TABLE_ID_REPORT_4BYTE_FRAME                     ZW_MeterTblTableIdReport4byteFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_GET_FRAME                    ZW_MeterTblTablePointAdmNoGetFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_1BYTE_FRAME           ZW_MeterTblTablePointAdmNoReport1byteFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_2BYTE_FRAME           ZW_MeterTblTablePointAdmNoReport2byteFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_3BYTE_FRAME           ZW_MeterTblTablePointAdmNoReport3byteFrame;\
  ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_4BYTE_FRAME           ZW_MeterTblTablePointAdmNoReport4byteFrame;\
/* Command class Meter Tbl Push */\
  ZW_METER_TBL_PUSH_CONFIGURATION_GET_FRAME                    ZW_MeterTblPushConfigurationGetFrame;\
  ZW_METER_TBL_PUSH_CONFIGURATION_REPORT_FRAME                 ZW_MeterTblPushConfigurationReportFrame;\
  ZW_METER_TBL_PUSH_CONFIGURATION_SET_FRAME                    ZW_MeterTblPushConfigurationSetFrame;\
/* Command class Meter */\
  ZW_METER_GET_FRAME                                           ZW_MeterGetFrame;\
  ZW_METER_REPORT_1BYTE_FRAME                                  ZW_MeterReport1byteFrame;\
  ZW_METER_REPORT_2BYTE_FRAME                                  ZW_MeterReport2byteFrame;\
  ZW_METER_REPORT_3BYTE_FRAME                                  ZW_MeterReport3byteFrame;\
  ZW_METER_REPORT_4BYTE_FRAME                                  ZW_MeterReport4byteFrame;\
/* Command class Meter V2 */\
  ZW_METER_GET_V2_FRAME                                        ZW_MeterGetV2Frame;\
  ZW_METER_REPORT_1BYTE_V2_FRAME                               ZW_MeterReport1byteV2Frame;\
  ZW_METER_REPORT_2BYTE_V2_FRAME                               ZW_MeterReport2byteV2Frame;\
  ZW_METER_REPORT_3BYTE_V2_FRAME                               ZW_MeterReport3byteV2Frame;\
  ZW_METER_REPORT_4BYTE_V2_FRAME                               ZW_MeterReport4byteV2Frame;\
  ZW_METER_RESET_V2_FRAME                                      ZW_MeterResetV2Frame;\
  ZW_METER_SUPPORTED_GET_V2_FRAME                              ZW_MeterSupportedGetV2Frame;\
  ZW_METER_SUPPORTED_REPORT_V2_FRAME                           ZW_MeterSupportedReportV2Frame;\
/* Command class Meter V3 */\
  ZW_METER_GET_V3_FRAME                                        ZW_MeterGetV3Frame;\
  ZW_METER_REPORT_1BYTE_V3_FRAME                               ZW_MeterReport1byteV3Frame;\
  ZW_METER_REPORT_2BYTE_V3_FRAME                               ZW_MeterReport2byteV3Frame;\
  ZW_METER_REPORT_3BYTE_V3_FRAME                               ZW_MeterReport3byteV3Frame;\
  ZW_METER_REPORT_4BYTE_V3_FRAME                               ZW_MeterReport4byteV3Frame;\
  ZW_METER_RESET_V3_FRAME                                      ZW_MeterResetV3Frame;\
  ZW_METER_SUPPORTED_GET_V3_FRAME                              ZW_MeterSupportedGetV3Frame;\
  ZW_METER_SUPPORTED_REPORT_V3_FRAME                           ZW_MeterSupportedReportV3Frame;\
/* Command class Mtp Window Covering */\
  ZW_MOVE_TO_POSITION_GET_FRAME                                ZW_MoveToPositionGetFrame;\
  ZW_MOVE_TO_POSITION_REPORT_FRAME                             ZW_MoveToPositionReportFrame;\
  ZW_MOVE_TO_POSITION_SET_FRAME                                ZW_MoveToPositionSetFrame;\
/* Command class Multi Channel Association V2 */\
  ZW_MULTI_CHANNEL_ASSOCIATION_GET_V2_FRAME                    ZW_MultiChannelAssociationGetV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V2_FRAME          ZW_MultiChannelAssociationGroupingsGetV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2_FRAME       ZW_MultiChannelAssociationGroupingsReportV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_1BYTE_V2_FRAME           ZW_MultiChannelAssociationRemove1byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_2BYTE_V2_FRAME           ZW_MultiChannelAssociationRemove2byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_3BYTE_V2_FRAME           ZW_MultiChannelAssociationRemove3byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_4BYTE_V2_FRAME           ZW_MultiChannelAssociationRemove4byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_1BYTE_V2_FRAME           ZW_MultiChannelAssociationReport1byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_2BYTE_V2_FRAME           ZW_MultiChannelAssociationReport2byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_3BYTE_V2_FRAME           ZW_MultiChannelAssociationReport3byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_4BYTE_V2_FRAME           ZW_MultiChannelAssociationReport4byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME              ZW_MultiChannelAssociationSet1byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_SET_2BYTE_V2_FRAME              ZW_MultiChannelAssociationSet2byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_SET_3BYTE_V2_FRAME              ZW_MultiChannelAssociationSet3byteV2Frame;\
  ZW_MULTI_CHANNEL_ASSOCIATION_SET_4BYTE_V2_FRAME              ZW_MultiChannelAssociationSet4byteV2Frame;\
/* Command class Multi Channel V2 */\
  ZW_MULTI_CHANNEL_CAPABILITY_GET_V2_FRAME                     ZW_MultiChannelCapabilityGetV2Frame;\
  ZW_MULTI_CHANNEL_CAPABILITY_REPORT_1BYTE_V2_FRAME            ZW_MultiChannelCapabilityReport1byteV2Frame;\
  ZW_MULTI_CHANNEL_CAPABILITY_REPORT_2BYTE_V2_FRAME            ZW_MultiChannelCapabilityReport2byteV2Frame;\
  ZW_MULTI_CHANNEL_CAPABILITY_REPORT_3BYTE_V2_FRAME            ZW_MultiChannelCapabilityReport3byteV2Frame;\
  ZW_MULTI_CHANNEL_CAPABILITY_REPORT_4BYTE_V2_FRAME            ZW_MultiChannelCapabilityReport4byteV2Frame;\
  ZW_MULTI_CHANNEL_END_POINT_FIND_V2_FRAME                     ZW_MultiChannelEndPointFindV2Frame;\
  VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG                 VG_MultiChannelEndPointFindReportV2VGroup;\
  ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_1BYTE_V2_FRAME        ZW_MultiChannelEndPointFindReport1byteV2Frame;\
  ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_2BYTE_V2_FRAME        ZW_MultiChannelEndPointFindReport2byteV2Frame;\
  ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_3BYTE_V2_FRAME        ZW_MultiChannelEndPointFindReport3byteV2Frame;\
  ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_4BYTE_V2_FRAME        ZW_MultiChannelEndPointFindReport4byteV2Frame;\
  ZW_MULTI_CHANNEL_END_POINT_GET_V2_FRAME                      ZW_MultiChannelEndPointGetV2Frame;\
  ZW_MULTI_CHANNEL_END_POINT_REPORT_V2_FRAME                   ZW_MultiChannelEndPointReportV2Frame;\
  ZW_MULTI_INSTANCE_GET_V2_FRAME                               ZW_MultiInstanceGetV2Frame;\
  ZW_MULTI_INSTANCE_REPORT_V2_FRAME                            ZW_MultiInstanceReportV2Frame;\
/* Command class Multi Instance Association */\
  ZW_MULTI_INSTANCE_ASSOCIATION_GET_FRAME                      ZW_MultiInstanceAssociationGetFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_GROUPINGS_GET_FRAME            ZW_MultiInstanceAssociationGroupingsGetFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_GROUPINGS_REPORT_FRAME         ZW_MultiInstanceAssociationGroupingsReportFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_1BYTE_FRAME             ZW_MultiInstanceAssociationRemove1byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_2BYTE_FRAME             ZW_MultiInstanceAssociationRemove2byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_3BYTE_FRAME             ZW_MultiInstanceAssociationRemove3byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_4BYTE_FRAME             ZW_MultiInstanceAssociationRemove4byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_1BYTE_FRAME             ZW_MultiInstanceAssociationReport1byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_2BYTE_FRAME             ZW_MultiInstanceAssociationReport2byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_3BYTE_FRAME             ZW_MultiInstanceAssociationReport3byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_4BYTE_FRAME             ZW_MultiInstanceAssociationReport4byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_SET_1BYTE_FRAME                ZW_MultiInstanceAssociationSet1byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_SET_2BYTE_FRAME                ZW_MultiInstanceAssociationSet2byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_SET_3BYTE_FRAME                ZW_MultiInstanceAssociationSet3byteFrame;\
  ZW_MULTI_INSTANCE_ASSOCIATION_SET_4BYTE_FRAME                ZW_MultiInstanceAssociationSet4byteFrame;\
/* Command class Multi Instance */\
  ZW_MULTI_INSTANCE_GET_FRAME                                  ZW_MultiInstanceGetFrame;\
  ZW_MULTI_INSTANCE_REPORT_FRAME                               ZW_MultiInstanceReportFrame;\
/* Command class Network Management Info */\
  ZW_NODE_INFO_GET_FRAME                                       ZW_NodeInfoGetFrame;\
  ZW_NODE_INFO_REPORT_1BYTE_FRAME                              ZW_NodeInfoReport1byteFrame;\
  ZW_NODE_INFO_REPORT_2BYTE_FRAME                              ZW_NodeInfoReport2byteFrame;\
  ZW_NODE_INFO_REPORT_3BYTE_FRAME                              ZW_NodeInfoReport3byteFrame;\
  ZW_NODE_INFO_REPORT_4BYTE_FRAME                              ZW_NodeInfoReport4byteFrame;\
/* Command class Network Management List */\
  ZW_NODE_INFO_REDUCED_GET_FRAME                               ZW_NodeInfoReducedGetFrame;\
  ZW_NODE_INFO_REDUCED_REPORT_1BYTE_FRAME                      ZW_NodeInfoReducedReport1byteFrame;\
  ZW_NODE_INFO_REDUCED_REPORT_2BYTE_FRAME                      ZW_NodeInfoReducedReport2byteFrame;\
  ZW_NODE_INFO_REDUCED_REPORT_3BYTE_FRAME                      ZW_NodeInfoReducedReport3byteFrame;\
  ZW_NODE_INFO_REDUCED_REPORT_4BYTE_FRAME                      ZW_NodeInfoReducedReport4byteFrame;\
  ZW_NODE_LIST_GET_FRAME                                       ZW_NodeListGetFrame;\
  ZW_NODE_LIST_REPORT_1BYTE_FRAME                              ZW_NodeListReport1byteFrame;\
  ZW_NODE_LIST_REPORT_2BYTE_FRAME                              ZW_NodeListReport2byteFrame;\
  ZW_NODE_LIST_REPORT_3BYTE_FRAME                              ZW_NodeListReport3byteFrame;\
  ZW_NODE_LIST_REPORT_4BYTE_FRAME                              ZW_NodeListReport4byteFrame;\
/* Command class Network Management Node */\
  ZW_LEARN_MODE_SET_FRAME                                      ZW_LearnModeSetFrame;\
  ZW_LEARN_MODE_STATUS_FRAME                                   ZW_LearnModeStatusFrame;\
/* Command class Network Management Pri Ctrl */\
  ZW_DEFAULT_SET_FRAME                                         ZW_DefaultSetFrame;\
  ZW_DEFAULT_SET_COMPLETE_FRAME                                ZW_DefaultSetCompleteFrame;\
  ZW_NODE_ADD_FRAME                                            ZW_NodeAddFrame;\
  ZW_NODE_ADD_STATUS_1BYTE_FRAME                               ZW_NodeAddStatus1byteFrame;\
  ZW_NODE_ADD_STATUS_2BYTE_FRAME                               ZW_NodeAddStatus2byteFrame;\
  ZW_NODE_ADD_STATUS_3BYTE_FRAME                               ZW_NodeAddStatus3byteFrame;\
  ZW_NODE_ADD_STATUS_4BYTE_FRAME                               ZW_NodeAddStatus4byteFrame;\
  ZW_NODE_REMOVE_FRAME                                         ZW_NodeRemoveFrame;\
  ZW_NODE_REMOVE_STATUS_FRAME                                  ZW_NodeRemoveStatusFrame;\
/* Command class Node Naming */\
  ZW_NODE_NAMING_NODE_LOCATION_REPORT_FRAME                    ZW_NodeNamingNodeLocationReportFrame;\
  ZW_NODE_NAMING_NODE_LOCATION_SET_FRAME                       ZW_NodeNamingNodeLocationSetFrame;\
  ZW_NODE_NAMING_NODE_LOCATION_GET_FRAME                       ZW_NodeNamingNodeLocationGetFrame;\
  ZW_NODE_NAMING_NODE_NAME_GET_FRAME                           ZW_NodeNamingNodeNameGetFrame;\
  ZW_NODE_NAMING_NODE_NAME_REPORT_FRAME                        ZW_NodeNamingNodeNameReportFrame;\
  ZW_NODE_NAMING_NODE_NAME_SET_FRAME                           ZW_NodeNamingNodeNameSetFrame;\
/* Command class Powerlevel */\
  ZW_POWERLEVEL_GET_FRAME                                      ZW_PowerlevelGetFrame;\
  ZW_POWERLEVEL_REPORT_FRAME                                   ZW_PowerlevelReportFrame;\
  ZW_POWERLEVEL_SET_FRAME                                      ZW_PowerlevelSetFrame;\
  ZW_POWERLEVEL_TEST_NODE_GET_FRAME                            ZW_PowerlevelTestNodeGetFrame;\
  ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME                         ZW_PowerlevelTestNodeReportFrame;\
  ZW_POWERLEVEL_TEST_NODE_SET_FRAME                            ZW_PowerlevelTestNodeSetFrame;\
/* Command class Prepayment Encapsulation */\
  ZW_CMD_ENCAPSULATION_1BYTE_FRAME                             ZW_CmdEncapsulation1byteFrame;\
  ZW_CMD_ENCAPSULATION_2BYTE_FRAME                             ZW_CmdEncapsulation2byteFrame;\
  ZW_CMD_ENCAPSULATION_3BYTE_FRAME                             ZW_CmdEncapsulation3byteFrame;\
  ZW_CMD_ENCAPSULATION_4BYTE_FRAME                             ZW_CmdEncapsulation4byteFrame;\
/* Command class Prepayment */\
  ZW_PREPAYMENT_BALANCE_GET_FRAME                              ZW_PrepaymentBalanceGetFrame;\
  ZW_PREPAYMENT_BALANCE_REPORT_FRAME                           ZW_PrepaymentBalanceReportFrame;\
  ZW_PREPAYMENT_SUPPORTED_GET_FRAME                            ZW_PrepaymentSupportedGetFrame;\
  ZW_PREPAYMENT_SUPPORTED_REPORT_FRAME                         ZW_PrepaymentSupportedReportFrame;\
/* Command class Proprietary */\
  ZW_PROPRIETARY_GET_1BYTE_FRAME                               ZW_ProprietaryGet1byteFrame;\
  ZW_PROPRIETARY_GET_2BYTE_FRAME                               ZW_ProprietaryGet2byteFrame;\
  ZW_PROPRIETARY_GET_3BYTE_FRAME                               ZW_ProprietaryGet3byteFrame;\
  ZW_PROPRIETARY_GET_4BYTE_FRAME                               ZW_ProprietaryGet4byteFrame;\
  ZW_PROPRIETARY_REPORT_1BYTE_FRAME                            ZW_ProprietaryReport1byteFrame;\
  ZW_PROPRIETARY_REPORT_2BYTE_FRAME                            ZW_ProprietaryReport2byteFrame;\
  ZW_PROPRIETARY_REPORT_3BYTE_FRAME                            ZW_ProprietaryReport3byteFrame;\
  ZW_PROPRIETARY_REPORT_4BYTE_FRAME                            ZW_ProprietaryReport4byteFrame;\
  ZW_PROPRIETARY_SET_1BYTE_FRAME                               ZW_ProprietarySet1byteFrame;\
  ZW_PROPRIETARY_SET_2BYTE_FRAME                               ZW_ProprietarySet2byteFrame;\
  ZW_PROPRIETARY_SET_3BYTE_FRAME                               ZW_ProprietarySet3byteFrame;\
  ZW_PROPRIETARY_SET_4BYTE_FRAME                               ZW_ProprietarySet4byteFrame;\
/* Command class Protection */\
  ZW_PROTECTION_GET_FRAME                                      ZW_ProtectionGetFrame;\
  ZW_PROTECTION_REPORT_FRAME                                   ZW_ProtectionReportFrame;\
  ZW_PROTECTION_SET_FRAME                                      ZW_ProtectionSetFrame;\
/* Command class Protection V2 */\
  ZW_PROTECTION_EC_GET_V2_FRAME                                ZW_ProtectionEcGetV2Frame;\
  ZW_PROTECTION_EC_REPORT_V2_FRAME                             ZW_ProtectionEcReportV2Frame;\
  ZW_PROTECTION_EC_SET_V2_FRAME                                ZW_ProtectionEcSetV2Frame;\
  ZW_PROTECTION_GET_V2_FRAME                                   ZW_ProtectionGetV2Frame;\
  ZW_PROTECTION_REPORT_V2_FRAME                                ZW_ProtectionReportV2Frame;\
  ZW_PROTECTION_SET_V2_FRAME                                   ZW_ProtectionSetV2Frame;\
  ZW_PROTECTION_SUPPORTED_GET_V2_FRAME                         ZW_ProtectionSupportedGetV2Frame;\
  ZW_PROTECTION_SUPPORTED_REPORT_V2_FRAME                      ZW_ProtectionSupportedReportV2Frame;\
  ZW_PROTECTION_TIMEOUT_GET_V2_FRAME                           ZW_ProtectionTimeoutGetV2Frame;\
  ZW_PROTECTION_TIMEOUT_REPORT_V2_FRAME                        ZW_ProtectionTimeoutReportV2Frame;\
  ZW_PROTECTION_TIMEOUT_SET_V2_FRAME                           ZW_ProtectionTimeoutSetV2Frame;\
/* Command class Rate Tbl Config */\
  ZW_RATE_TBL_REMOVE_1BYTE_FRAME                               ZW_RateTblRemove1byteFrame;\
  ZW_RATE_TBL_REMOVE_2BYTE_FRAME                               ZW_RateTblRemove2byteFrame;\
  ZW_RATE_TBL_REMOVE_3BYTE_FRAME                               ZW_RateTblRemove3byteFrame;\
  ZW_RATE_TBL_REMOVE_4BYTE_FRAME                               ZW_RateTblRemove4byteFrame;\
  ZW_RATE_TBL_SET_1BYTE_FRAME                                  ZW_RateTblSet1byteFrame;\
  ZW_RATE_TBL_SET_2BYTE_FRAME                                  ZW_RateTblSet2byteFrame;\
  ZW_RATE_TBL_SET_3BYTE_FRAME                                  ZW_RateTblSet3byteFrame;\
  ZW_RATE_TBL_SET_4BYTE_FRAME                                  ZW_RateTblSet4byteFrame;\
/* Command class Rate Tbl Monitor */\
  ZW_RATE_TBL_ACTIVE_RATE_GET_FRAME                            ZW_RateTblActiveRateGetFrame;\
  ZW_RATE_TBL_ACTIVE_RATE_REPORT_FRAME                         ZW_RateTblActiveRateReportFrame;\
  ZW_RATE_TBL_CURRENT_DATA_GET_FRAME                           ZW_RateTblCurrentDataGetFrame;\
  VG_RATE_TBL_CURRENT_DATA_REPORT_VG                           VG_RateTblCurrentDataReportVGroup;\
  ZW_RATE_TBL_CURRENT_DATA_REPORT_1BYTE_FRAME                  ZW_RateTblCurrentDataReport1byteFrame;\
  ZW_RATE_TBL_CURRENT_DATA_REPORT_2BYTE_FRAME                  ZW_RateTblCurrentDataReport2byteFrame;\
  ZW_RATE_TBL_CURRENT_DATA_REPORT_3BYTE_FRAME                  ZW_RateTblCurrentDataReport3byteFrame;\
  ZW_RATE_TBL_CURRENT_DATA_REPORT_4BYTE_FRAME                  ZW_RateTblCurrentDataReport4byteFrame;\
  ZW_RATE_TBL_GET_FRAME                                        ZW_RateTblGetFrame;\
  ZW_RATE_TBL_HISTORICAL_DATA_GET_FRAME                        ZW_RateTblHistoricalDataGetFrame;\
  VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG                        VG_RateTblHistoricalDataReportVGroup;\
  ZW_RATE_TBL_HISTORICAL_DATA_REPORT_1BYTE_FRAME               ZW_RateTblHistoricalDataReport1byteFrame;\
  ZW_RATE_TBL_HISTORICAL_DATA_REPORT_2BYTE_FRAME               ZW_RateTblHistoricalDataReport2byteFrame;\
  ZW_RATE_TBL_HISTORICAL_DATA_REPORT_3BYTE_FRAME               ZW_RateTblHistoricalDataReport3byteFrame;\
  ZW_RATE_TBL_HISTORICAL_DATA_REPORT_4BYTE_FRAME               ZW_RateTblHistoricalDataReport4byteFrame;\
  ZW_RATE_TBL_REPORT_1BYTE_FRAME                               ZW_RateTblReport1byteFrame;\
  ZW_RATE_TBL_REPORT_2BYTE_FRAME                               ZW_RateTblReport2byteFrame;\
  ZW_RATE_TBL_REPORT_3BYTE_FRAME                               ZW_RateTblReport3byteFrame;\
  ZW_RATE_TBL_REPORT_4BYTE_FRAME                               ZW_RateTblReport4byteFrame;\
  ZW_RATE_TBL_SUPPORTED_GET_FRAME                              ZW_RateTblSupportedGetFrame;\
  ZW_RATE_TBL_SUPPORTED_REPORT_FRAME                           ZW_RateTblSupportedReportFrame;\
/* Command class Remote Association Activate */\
  ZW_REMOTE_ASSOCIATION_ACTIVATE_FRAME                         ZW_RemoteAssociationActivateFrame;\
/* Command class Remote Association */\
  ZW_REMOTE_ASSOCIATION_CONFIGURATION_GET_FRAME                ZW_RemoteAssociationConfigurationGetFrame;\
  ZW_REMOTE_ASSOCIATION_CONFIGURATION_REPORT_FRAME             ZW_RemoteAssociationConfigurationReportFrame;\
  ZW_REMOTE_ASSOCIATION_CONFIGURATION_SET_FRAME                ZW_RemoteAssociationConfigurationSetFrame;\
/* Command class Scene Activation */\
  ZW_SCENE_ACTIVATION_SET_FRAME                                ZW_SceneActivationSetFrame;\
/* Command class Scene Actuator Conf */\
  ZW_SCENE_ACTUATOR_CONF_GET_FRAME                             ZW_SceneActuatorConfGetFrame;\
  ZW_SCENE_ACTUATOR_CONF_REPORT_FRAME                          ZW_SceneActuatorConfReportFrame;\
  ZW_SCENE_ACTUATOR_CONF_SET_FRAME                             ZW_SceneActuatorConfSetFrame;\
/* Command class Scene Controller Conf */\
  ZW_SCENE_CONTROLLER_CONF_GET_FRAME                           ZW_SceneControllerConfGetFrame;\
  ZW_SCENE_CONTROLLER_CONF_REPORT_FRAME                        ZW_SceneControllerConfReportFrame;\
  ZW_SCENE_CONTROLLER_CONF_SET_FRAME                           ZW_SceneControllerConfSetFrame;\
/* Command class Schedule Entry Lock */\
  ZW_SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_FRAME                  ZW_ScheduleEntryLockEnableAllSetFrame;\
  ZW_SCHEDULE_ENTRY_LOCK_ENABLE_SET_FRAME                      ZW_ScheduleEntryLockEnableSetFrame;\
  ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_FRAME                    ZW_ScheduleEntryLockWeekDayGetFrame;\
  ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_FRAME                 ZW_ScheduleEntryLockWeekDayReportFrame;\
  ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_FRAME                    ZW_ScheduleEntryLockWeekDaySetFrame;\
  ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_FRAME                    ZW_ScheduleEntryLockYearDayGetFrame;\
  ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_FRAME                 ZW_ScheduleEntryLockYearDayReportFrame;\
  ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_FRAME                    ZW_ScheduleEntryLockYearDaySetFrame;\
  ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_FRAME                   ZW_ScheduleEntryTypeSupportedGetFrame;\
  ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_FRAME                ZW_ScheduleEntryTypeSupportedReportFrame;\
/* Command class Schedule Entry Lock V2 */\
  ZW_SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_V2_FRAME               ZW_ScheduleEntryLockEnableAllSetV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_ENABLE_SET_V2_FRAME                   ZW_ScheduleEntryLockEnableSetV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V2_FRAME              ZW_ScheduleEntryLockTimeOffsetGetV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2_FRAME           ZW_ScheduleEntryLockTimeOffsetReportV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V2_FRAME              ZW_ScheduleEntryLockTimeOffsetSetV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_V2_FRAME                 ZW_ScheduleEntryLockWeekDayGetV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_V2_FRAME              ZW_ScheduleEntryLockWeekDayReportV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_V2_FRAME                 ZW_ScheduleEntryLockWeekDaySetV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_V2_FRAME                 ZW_ScheduleEntryLockYearDayGetV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_V2_FRAME              ZW_ScheduleEntryLockYearDayReportV2Frame;\
  ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_V2_FRAME                 ZW_ScheduleEntryLockYearDaySetV2Frame;\
  ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_V2_FRAME                ZW_ScheduleEntryTypeSupportedGetV2Frame;\
  ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_V2_FRAME             ZW_ScheduleEntryTypeSupportedReportV2Frame;\
/* Command class Screen Attributes */\
  ZW_SCREEN_ATTRIBUTES_GET_FRAME                               ZW_ScreenAttributesGetFrame;\
  ZW_SCREEN_ATTRIBUTES_REPORT_FRAME                            ZW_ScreenAttributesReportFrame;\
/* Command class Screen Attributes V2 */\
  ZW_SCREEN_ATTRIBUTES_GET_V2_FRAME                            ZW_ScreenAttributesGetV2Frame;\
  ZW_SCREEN_ATTRIBUTES_REPORT_V2_FRAME                         ZW_ScreenAttributesReportV2Frame;\
/* Command class Screen Md */\
  ZW_SCREEN_MD_GET_FRAME                                       ZW_ScreenMdGetFrame;\
  VG_SCREEN_MD_REPORT_1BYTE_VG                                 VG_ScreenMdReport1byteVGroup;\
  VG_SCREEN_MD_REPORT_2BYTE_VG                                 VG_ScreenMdReport2byteVGroup;\
  VG_SCREEN_MD_REPORT_3BYTE_VG                                 VG_ScreenMdReport3byteVGroup;\
  VG_SCREEN_MD_REPORT_4BYTE_VG                                 VG_ScreenMdReport4byteVGroup;\
  ZW_SCREEN_MD_REPORT_1BYTE_FRAME                              ZW_ScreenMdReport1byteFrame;\
  ZW_SCREEN_MD_REPORT_2BYTE_FRAME                              ZW_ScreenMdReport2byteFrame;\
  ZW_SCREEN_MD_REPORT_3BYTE_FRAME                              ZW_ScreenMdReport3byteFrame;\
  ZW_SCREEN_MD_REPORT_4BYTE_FRAME                              ZW_ScreenMdReport4byteFrame;\
/* Command class Screen Md V2 */\
  ZW_SCREEN_MD_GET_V2_FRAME                                    ZW_ScreenMdGetV2Frame;\
  VG_SCREEN_MD_REPORT_1BYTE_V2_VG                              VG_ScreenMdReport1byteV2VGroup;\
  VG_SCREEN_MD_REPORT_2BYTE_V2_VG                              VG_ScreenMdReport2byteV2VGroup;\
  VG_SCREEN_MD_REPORT_3BYTE_V2_VG                              VG_ScreenMdReport3byteV2VGroup;\
  VG_SCREEN_MD_REPORT_4BYTE_V2_VG                              VG_ScreenMdReport4byteV2VGroup;\
  ZW_SCREEN_MD_REPORT_1BYTE_V2_FRAME                           ZW_ScreenMdReport1byteV2Frame;\
  ZW_SCREEN_MD_REPORT_2BYTE_V2_FRAME                           ZW_ScreenMdReport2byteV2Frame;\
  ZW_SCREEN_MD_REPORT_3BYTE_V2_FRAME                           ZW_ScreenMdReport3byteV2Frame;\
  ZW_SCREEN_MD_REPORT_4BYTE_V2_FRAME                           ZW_ScreenMdReport4byteV2Frame;\
/* Command class Security Panel Mode */\
  ZW_SECURITY_PANEL_MODE_GET_FRAME                             ZW_SecurityPanelModeGetFrame;\
  ZW_SECURITY_PANEL_MODE_REPORT_FRAME                          ZW_SecurityPanelModeReportFrame;\
  ZW_SECURITY_PANEL_MODE_SET_FRAME                             ZW_SecurityPanelModeSetFrame;\
  ZW_SECURITY_PANEL_MODE_SUPPORTED_GET_FRAME                   ZW_SecurityPanelModeSupportedGetFrame;\
  ZW_SECURITY_PANEL_MODE_SUPPORTED_REPORT_FRAME                ZW_SecurityPanelModeSupportedReportFrame;\
/* Command class Security Panel Zone Sensor */\
  ZW_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_REPORT_FRAME         ZW_CommandClassSecurityPanelZoneSensorInstalledReportFrame;\
  ZW_SECURITY_PANEL__ZONE_SENSOR_TYPE_GET_FRAME                ZW_SecurityPanelZoneSensorTypeGetFrame;\
  ZW_SECURITY_PANEL__ZONE_SENSOR_TYPE_REPORT_FRAME             ZW_SecurityPanelZoneSensorTypeReportFrame;\
  ZW_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_GET_FRAME            ZW_SecurityPanelZoneSensorInstalledGetFrame;\
  ZW_SECURITY_PANEL_ZONE_SENSOR_STATE_GET_FRAME                ZW_SecurityPanelZoneSensorStateGetFrame;\
  ZW_SECURITY_PANEL_ZONE_SENSOR_STATE_REPORT_FRAME             ZW_SecurityPanelZoneSensorStateReportFrame;\
/* Command class Security Panel Zone */\
  ZW_SECURITY_PANEL_ZONE_NUMBER_SUPPORTED_GET_FRAME            ZW_SecurityPanelZoneNumberSupportedGetFrame;\
  ZW_SECURITY_PANEL_ZONE_STATE_GET_FRAME                       ZW_SecurityPanelZoneStateGetFrame;\
  ZW_SECURITY_PANEL_ZONE_STATE_REPORT_FRAME                    ZW_SecurityPanelZoneStateReportFrame;\
  ZW_SECURITY_PANEL_ZONE_SUPPORTED_REPORT_FRAME                ZW_SecurityPanelZoneSupportedReportFrame;\
  ZW_SECURITY_PANEL_ZONE_TYPE_GET_FRAME                        ZW_SecurityPanelZoneTypeGetFrame;\
  ZW_SECURITY_PANEL_ZONE_TYPE_REPORT_FRAME                     ZW_SecurityPanelZoneTypeReportFrame;\
/* Command class Security */\
  ZW_NETWORK_KEY_SET_1BYTE_FRAME                               ZW_NetworkKeySet1byteFrame;\
  ZW_NETWORK_KEY_SET_2BYTE_FRAME                               ZW_NetworkKeySet2byteFrame;\
  ZW_NETWORK_KEY_SET_3BYTE_FRAME                               ZW_NetworkKeySet3byteFrame;\
  ZW_NETWORK_KEY_SET_4BYTE_FRAME                               ZW_NetworkKeySet4byteFrame;\
  ZW_NETWORK_KEY_VERIFY_FRAME                                  ZW_NetworkKeyVerifyFrame;\
  ZW_SECURITY_COMMANDS_SUPPORTED_GET_FRAME                     ZW_SecurityCommandsSupportedGetFrame;\
  ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_1BYTE_FRAME            ZW_SecurityCommandsSupportedReport1byteFrame;\
  ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_2BYTE_FRAME            ZW_SecurityCommandsSupportedReport2byteFrame;\
  ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_3BYTE_FRAME            ZW_SecurityCommandsSupportedReport3byteFrame;\
  ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_4BYTE_FRAME            ZW_SecurityCommandsSupportedReport4byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_1BYTE_FRAME                ZW_SecurityMessageEncapsulation1byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_2BYTE_FRAME                ZW_SecurityMessageEncapsulation2byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_3BYTE_FRAME                ZW_SecurityMessageEncapsulation3byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_4BYTE_FRAME                ZW_SecurityMessageEncapsulation4byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_1BYTE_FRAME      ZW_SecurityMessageEncapsulationNonceGet1byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_2BYTE_FRAME      ZW_SecurityMessageEncapsulationNonceGet2byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_3BYTE_FRAME      ZW_SecurityMessageEncapsulationNonceGet3byteFrame;\
  ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_4BYTE_FRAME      ZW_SecurityMessageEncapsulationNonceGet4byteFrame;\
  ZW_SECURITY_NONCE_GET_FRAME                                  ZW_SecurityNonceGetFrame;\
  ZW_SECURITY_NONCE_REPORT_FRAME                               ZW_SecurityNonceReportFrame;\
  ZW_SECURITY_SCHEME_GET_FRAME                                 ZW_SecuritySchemeGetFrame;\
  ZW_SECURITY_SCHEME_INHERIT_FRAME                             ZW_SecuritySchemeInheritFrame;\
  ZW_SECURITY_SCHEME_REPORT_FRAME                              ZW_SecuritySchemeReportFrame;\
/* Command class Sensor Alarm */\
  ZW_SENSOR_ALARM_GET_FRAME                                    ZW_SensorAlarmGetFrame;\
  ZW_SENSOR_ALARM_REPORT_FRAME                                 ZW_SensorAlarmReportFrame;\
  ZW_SENSOR_ALARM_SUPPORTED_GET_FRAME                          ZW_SensorAlarmSupportedGetFrame;\
  ZW_SENSOR_ALARM_SUPPORTED_REPORT_1BYTE_FRAME                 ZW_SensorAlarmSupportedReport1byteFrame;\
  ZW_SENSOR_ALARM_SUPPORTED_REPORT_2BYTE_FRAME                 ZW_SensorAlarmSupportedReport2byteFrame;\
  ZW_SENSOR_ALARM_SUPPORTED_REPORT_3BYTE_FRAME                 ZW_SensorAlarmSupportedReport3byteFrame;\
  ZW_SENSOR_ALARM_SUPPORTED_REPORT_4BYTE_FRAME                 ZW_SensorAlarmSupportedReport4byteFrame;\
/* Command class Sensor Binary */\
  ZW_SENSOR_BINARY_GET_FRAME                                   ZW_SensorBinaryGetFrame;\
  ZW_SENSOR_BINARY_REPORT_FRAME                                ZW_SensorBinaryReportFrame;\
/* Command class Sensor Configuration */\
  ZW_SENSOR_TRIGGER_LEVEL_GET_FRAME                            ZW_SensorTriggerLevelGetFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_REPORT_1BYTE_FRAME                   ZW_SensorTriggerLevelReport1byteFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_REPORT_2BYTE_FRAME                   ZW_SensorTriggerLevelReport2byteFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_REPORT_3BYTE_FRAME                   ZW_SensorTriggerLevelReport3byteFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_REPORT_4BYTE_FRAME                   ZW_SensorTriggerLevelReport4byteFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_SET_1BYTE_FRAME                      ZW_SensorTriggerLevelSet1byteFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_SET_2BYTE_FRAME                      ZW_SensorTriggerLevelSet2byteFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_SET_3BYTE_FRAME                      ZW_SensorTriggerLevelSet3byteFrame;\
  ZW_SENSOR_TRIGGER_LEVEL_SET_4BYTE_FRAME                      ZW_SensorTriggerLevelSet4byteFrame;\
/* Command class Sensor Multilevel */\
  ZW_SENSOR_MULTILEVEL_GET_FRAME                               ZW_SensorMultilevelGetFrame;\
  ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_FRAME                      ZW_SensorMultilevelReport1byteFrame;\
  ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_FRAME                      ZW_SensorMultilevelReport2byteFrame;\
  ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_FRAME                      ZW_SensorMultilevelReport3byteFrame;\
  ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_FRAME                      ZW_SensorMultilevelReport4byteFrame;\
/* Command class Sensor Multilevel V2 */\
  ZW_SENSOR_MULTILEVEL_GET_V2_FRAME                            ZW_SensorMultilevelGetV2Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_V2_FRAME                   ZW_SensorMultilevelReport1byteV2Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_V2_FRAME                   ZW_SensorMultilevelReport2byteV2Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_V2_FRAME                   ZW_SensorMultilevelReport3byteV2Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_V2_FRAME                   ZW_SensorMultilevelReport4byteV2Frame;\
/* Command class Sensor Multilevel V3 */\
  ZW_SENSOR_MULTILEVEL_GET_V3_FRAME                            ZW_SensorMultilevelGetV3Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_V3_FRAME                   ZW_SensorMultilevelReport1byteV3Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_V3_FRAME                   ZW_SensorMultilevelReport2byteV3Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_V3_FRAME                   ZW_SensorMultilevelReport3byteV3Frame;\
  ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_V3_FRAME                   ZW_SensorMultilevelReport4byteV3Frame;\
/* Command class Silence Alarm */\
  ZW_SENSOR_ALARM_SET_1BYTE_FRAME                              ZW_SensorAlarmSet1byteFrame;\
  ZW_SENSOR_ALARM_SET_2BYTE_FRAME                              ZW_SensorAlarmSet2byteFrame;\
  ZW_SENSOR_ALARM_SET_3BYTE_FRAME                              ZW_SensorAlarmSet3byteFrame;\
  ZW_SENSOR_ALARM_SET_4BYTE_FRAME                              ZW_SensorAlarmSet4byteFrame;\
/* Command class Simple Av Control */\
  ZW_SIMPLE_AV_CONTROL_GET_FRAME                               ZW_SimpleAvControlGetFrame;\
  ZW_SIMPLE_AV_CONTROL_REPORT_FRAME                            ZW_SimpleAvControlReportFrame;\
  VG_SIMPLE_AV_CONTROL_SET_VG                                  VG_SimpleAvControlSetVGroup;\
  ZW_SIMPLE_AV_CONTROL_SET_1BYTE_FRAME                         ZW_SimpleAvControlSet1byteFrame;\
  ZW_SIMPLE_AV_CONTROL_SET_2BYTE_FRAME                         ZW_SimpleAvControlSet2byteFrame;\
  ZW_SIMPLE_AV_CONTROL_SET_3BYTE_FRAME                         ZW_SimpleAvControlSet3byteFrame;\
  ZW_SIMPLE_AV_CONTROL_SET_4BYTE_FRAME                         ZW_SimpleAvControlSet4byteFrame;\
  ZW_SIMPLE_AV_CONTROL_SUPPORTED_GET_FRAME                     ZW_SimpleAvControlSupportedGetFrame;\
  ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_1BYTE_FRAME            ZW_SimpleAvControlSupportedReport1byteFrame;\
  ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_2BYTE_FRAME            ZW_SimpleAvControlSupportedReport2byteFrame;\
  ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_3BYTE_FRAME            ZW_SimpleAvControlSupportedReport3byteFrame;\
  ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_4BYTE_FRAME            ZW_SimpleAvControlSupportedReport4byteFrame;\
/* Command class Switch All */\
  ZW_SWITCH_ALL_GET_FRAME                                      ZW_SwitchAllGetFrame;\
  ZW_SWITCH_ALL_OFF_FRAME                                      ZW_SwitchAllOffFrame;\
  ZW_SWITCH_ALL_ON_FRAME                                       ZW_SwitchAllOnFrame;\
  ZW_SWITCH_ALL_REPORT_FRAME                                   ZW_SwitchAllReportFrame;\
  ZW_SWITCH_ALL_SET_FRAME                                      ZW_SwitchAllSetFrame;\
/* Command class Switch Binary */\
  ZW_SWITCH_BINARY_GET_FRAME                                   ZW_SwitchBinaryGetFrame;\
  ZW_SWITCH_BINARY_REPORT_FRAME                                ZW_SwitchBinaryReportFrame;\
  ZW_SWITCH_BINARY_SET_FRAME                                   ZW_SwitchBinarySetFrame;\
/* Command class Switch Multilevel */\
  ZW_SWITCH_MULTILEVEL_GET_FRAME                               ZW_SwitchMultilevelGetFrame;\
  ZW_SWITCH_MULTILEVEL_REPORT_FRAME                            ZW_SwitchMultilevelReportFrame;\
  ZW_SWITCH_MULTILEVEL_SET_FRAME                               ZW_SwitchMultilevelSetFrame;\
  ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_FRAME                ZW_SwitchMultilevelStartLevelChangeFrame;\
  ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_FRAME                 ZW_SwitchMultilevelStopLevelChangeFrame;\
/* Command class Switch Multilevel V2 */\
  ZW_SWITCH_MULTILEVEL_GET_V2_FRAME                            ZW_SwitchMultilevelGetV2Frame;\
  ZW_SWITCH_MULTILEVEL_REPORT_V2_FRAME                         ZW_SwitchMultilevelReportV2Frame;\
  ZW_SWITCH_MULTILEVEL_SET_V2_FRAME                            ZW_SwitchMultilevelSetV2Frame;\
  ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V2_FRAME             ZW_SwitchMultilevelStartLevelChangeV2Frame;\
  ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V2_FRAME              ZW_SwitchMultilevelStopLevelChangeV2Frame;\
/* Command class Switch Multilevel V3 */\
  ZW_SWITCH_MULTILEVEL_GET_V3_FRAME                            ZW_SwitchMultilevelGetV3Frame;\
  ZW_SWITCH_MULTILEVEL_REPORT_V3_FRAME                         ZW_SwitchMultilevelReportV3Frame;\
  ZW_SWITCH_MULTILEVEL_SET_V3_FRAME                            ZW_SwitchMultilevelSetV3Frame;\
  ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V3_FRAME             ZW_SwitchMultilevelStartLevelChangeV3Frame;\
  ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V3_FRAME              ZW_SwitchMultilevelStopLevelChangeV3Frame;\
  ZW_SWITCH_MULTILEVEL_SUPPORTED_GET_V3_FRAME                  ZW_SwitchMultilevelSupportedGetV3Frame;\
  ZW_SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3_FRAME               ZW_SwitchMultilevelSupportedReportV3Frame;\
/* Command class Switch Toggle Binary */\
  ZW_SWITCH_TOGGLE_BINARY_SET_FRAME                            ZW_SwitchToggleBinarySetFrame;\
  ZW_SWITCH_TOGGLE_BINARY_GET_FRAME                            ZW_SwitchToggleBinaryGetFrame;\
  ZW_SWITCH_TOGGLE_BINARY_REPORT_FRAME                         ZW_SwitchToggleBinaryReportFrame;\
/* Command class Switch Toggle Multilevel */\
  ZW_SWITCH_TOGGLE_MULTILEVEL_SET_FRAME                        ZW_SwitchToggleMultilevelSetFrame;\
  ZW_SWITCH_TOGGLE_MULTILEVEL_GET_FRAME                        ZW_SwitchToggleMultilevelGetFrame;\
  ZW_SWITCH_TOGGLE_MULTILEVEL_REPORT_FRAME                     ZW_SwitchToggleMultilevelReportFrame;\
  ZW_SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_FRAME         ZW_SwitchToggleMultilevelStartLevelChangeFrame;\
  ZW_SWITCH_TOGGLE_MULTILEVEL_STOP_LEVEL_CHANGE_FRAME          ZW_SwitchToggleMultilevelStopLevelChangeFrame;\
/* Command class Tariff Config */\
  ZW_TARIFF_TBL_REMOVE_1BYTE_FRAME                             ZW_TariffTblRemove1byteFrame;\
  ZW_TARIFF_TBL_REMOVE_2BYTE_FRAME                             ZW_TariffTblRemove2byteFrame;\
  ZW_TARIFF_TBL_REMOVE_3BYTE_FRAME                             ZW_TariffTblRemove3byteFrame;\
  ZW_TARIFF_TBL_REMOVE_4BYTE_FRAME                             ZW_TariffTblRemove4byteFrame;\
  ZW_TARIFF_TBL_SET_FRAME                                      ZW_TariffTblSetFrame;\
  ZW_TARIFF_TBL_SUPPLIER_SET_1BYTE_FRAME                       ZW_TariffTblSupplierSet1byteFrame;\
  ZW_TARIFF_TBL_SUPPLIER_SET_2BYTE_FRAME                       ZW_TariffTblSupplierSet2byteFrame;\
  ZW_TARIFF_TBL_SUPPLIER_SET_3BYTE_FRAME                       ZW_TariffTblSupplierSet3byteFrame;\
  ZW_TARIFF_TBL_SUPPLIER_SET_4BYTE_FRAME                       ZW_TariffTblSupplierSet4byteFrame;\
/* Command class Tariff Tbl Monitor */\
  ZW_TARIFF_TBL_COST_GET_FRAME                                 ZW_TariffTblCostGetFrame;\
  ZW_TARIFF_TBL_COST_REPORT_FRAME                              ZW_TariffTblCostReportFrame;\
  ZW_TARIFF_TBL_GET_FRAME                                      ZW_TariffTblGetFrame;\
  ZW_TARIFF_TBL_REPORT_FRAME                                   ZW_TariffTblReportFrame;\
  ZW_TARIFF_TBL_SUPPLIER_GET_FRAME                             ZW_TariffTblSupplierGetFrame;\
  ZW_TARIFF_TBL_SUPPLIER_REPORT_1BYTE_FRAME                    ZW_TariffTblSupplierReport1byteFrame;\
  ZW_TARIFF_TBL_SUPPLIER_REPORT_2BYTE_FRAME                    ZW_TariffTblSupplierReport2byteFrame;\
  ZW_TARIFF_TBL_SUPPLIER_REPORT_3BYTE_FRAME                    ZW_TariffTblSupplierReport3byteFrame;\
  ZW_TARIFF_TBL_SUPPLIER_REPORT_4BYTE_FRAME                    ZW_TariffTblSupplierReport4byteFrame;\
/* Command class Thermostat Fan Mode */\
  ZW_THERMOSTAT_FAN_MODE_GET_FRAME                             ZW_ThermostatFanModeGetFrame;\
  ZW_THERMOSTAT_FAN_MODE_REPORT_FRAME                          ZW_ThermostatFanModeReportFrame;\
  ZW_THERMOSTAT_FAN_MODE_SET_FRAME                             ZW_ThermostatFanModeSetFrame;\
  ZW_THERMOSTAT_FAN_MODE_SUPPORTED_GET_FRAME                   ZW_ThermostatFanModeSupportedGetFrame;\
  ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_1BYTE_FRAME          ZW_ThermostatFanModeSupportedReport1byteFrame;\
  ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_2BYTE_FRAME          ZW_ThermostatFanModeSupportedReport2byteFrame;\
  ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_3BYTE_FRAME          ZW_ThermostatFanModeSupportedReport3byteFrame;\
  ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_4BYTE_FRAME          ZW_ThermostatFanModeSupportedReport4byteFrame;\
/* Command class Thermostat Fan State */\
  ZW_THERMOSTAT_FAN_STATE_GET_FRAME                            ZW_ThermostatFanStateGetFrame;\
  ZW_THERMOSTAT_FAN_STATE_REPORT_FRAME                         ZW_ThermostatFanStateReportFrame;\
/* Command class Thermostat Heating */\
  ZW_THERMOSTAT_HEATING_STATUS_REPORT_FRAME                    ZW_ThermostatHeatingStatusReportFrame;\
  ZW_THERMOSTAT_HEATING_MODE_GET_FRAME                         ZW_ThermostatHeatingModeGetFrame;\
  ZW_THERMOSTAT_HEATING_MODE_REPORT_FRAME                      ZW_ThermostatHeatingModeReportFrame;\
  ZW_THERMOSTAT_HEATING_MODE_SET_FRAME                         ZW_ThermostatHeatingModeSetFrame;\
  ZW_THERMOSTAT_HEATING_RELAY_STATUS_GET_FRAME                 ZW_ThermostatHeatingRelayStatusGetFrame;\
  ZW_THERMOSTAT_HEATING_RELAY_STATUS_REPORT_FRAME              ZW_ThermostatHeatingRelayStatusReportFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_GET_FRAME                     ZW_ThermostatHeatingSetpointGetFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_1BYTE_FRAME            ZW_ThermostatHeatingSetpointReport1byteFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_2BYTE_FRAME            ZW_ThermostatHeatingSetpointReport2byteFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_3BYTE_FRAME            ZW_ThermostatHeatingSetpointReport3byteFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_4BYTE_FRAME            ZW_ThermostatHeatingSetpointReport4byteFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_SET_1BYTE_FRAME               ZW_ThermostatHeatingSetpointSet1byteFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_SET_2BYTE_FRAME               ZW_ThermostatHeatingSetpointSet2byteFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_SET_3BYTE_FRAME               ZW_ThermostatHeatingSetpointSet3byteFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_SET_4BYTE_FRAME               ZW_ThermostatHeatingSetpointSet4byteFrame;\
  ZW_THERMOSTAT_HEATING_STATUS_GET_FRAME                       ZW_ThermostatHeatingStatusGetFrame;\
  ZW_THERMOSTAT_HEATING_STATUS_SET_FRAME                       ZW_ThermostatHeatingStatusSetFrame;\
  ZW_THERMOSTAT_HEATING_TIMED_OFF_SET_FRAME                    ZW_ThermostatHeatingTimedOffSetFrame;\
/* Command class Thermostat Mode */\
  ZW_THERMOSTAT_MODE_GET_FRAME                                 ZW_ThermostatModeGetFrame;\
  ZW_THERMOSTAT_MODE_REPORT_FRAME                              ZW_ThermostatModeReportFrame;\
  ZW_THERMOSTAT_MODE_SET_FRAME                                 ZW_ThermostatModeSetFrame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_GET_FRAME                       ZW_ThermostatModeSupportedGetFrame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_1BYTE_FRAME              ZW_ThermostatModeSupportedReport1byteFrame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_2BYTE_FRAME              ZW_ThermostatModeSupportedReport2byteFrame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_3BYTE_FRAME              ZW_ThermostatModeSupportedReport3byteFrame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_4BYTE_FRAME              ZW_ThermostatModeSupportedReport4byteFrame;\
/* Command class Thermostat Mode V2 */\
  ZW_THERMOSTAT_MODE_GET_V2_FRAME                              ZW_ThermostatModeGetV2Frame;\
  ZW_THERMOSTAT_MODE_REPORT_V2_FRAME                           ZW_ThermostatModeReportV2Frame;\
  ZW_THERMOSTAT_MODE_SET_V2_FRAME                              ZW_ThermostatModeSetV2Frame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_GET_V2_FRAME                    ZW_ThermostatModeSupportedGetV2Frame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_1BYTE_V2_FRAME           ZW_ThermostatModeSupportedReport1byteV2Frame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_2BYTE_V2_FRAME           ZW_ThermostatModeSupportedReport2byteV2Frame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_3BYTE_V2_FRAME           ZW_ThermostatModeSupportedReport3byteV2Frame;\
  ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_4BYTE_V2_FRAME           ZW_ThermostatModeSupportedReport4byteV2Frame;\
/* Command class Thermostat Operating State */\
  ZW_THERMOSTAT_OPERATING_STATE_GET_FRAME                      ZW_ThermostatOperatingStateGetFrame;\
  ZW_THERMOSTAT_OPERATING_STATE_REPORT_FRAME                   ZW_ThermostatOperatingStateReportFrame;\
/* Command class Thermostat Setback */\
  ZW_THERMOSTAT_SETBACK_GET_FRAME                              ZW_ThermostatSetbackGetFrame;\
  ZW_THERMOSTAT_SETBACK_REPORT_FRAME                           ZW_ThermostatSetbackReportFrame;\
  ZW_THERMOSTAT_SETBACK_SET_FRAME                              ZW_ThermostatSetbackSetFrame;\
/* Command class Thermostat Setpoint */\
  ZW_THERMOSTAT_SETPOINT_GET_FRAME                             ZW_ThermostatSetpointGetFrame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_1BYTE_FRAME                    ZW_ThermostatSetpointReport1byteFrame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_2BYTE_FRAME                    ZW_ThermostatSetpointReport2byteFrame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_3BYTE_FRAME                    ZW_ThermostatSetpointReport3byteFrame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_4BYTE_FRAME                    ZW_ThermostatSetpointReport4byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SET_1BYTE_FRAME                       ZW_ThermostatSetpointSet1byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SET_2BYTE_FRAME                       ZW_ThermostatSetpointSet2byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SET_3BYTE_FRAME                       ZW_ThermostatSetpointSet3byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SET_4BYTE_FRAME                       ZW_ThermostatSetpointSet4byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_GET_FRAME                   ZW_ThermostatSetpointSupportedGetFrame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_1BYTE_FRAME          ZW_ThermostatSetpointSupportedReport1byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_2BYTE_FRAME          ZW_ThermostatSetpointSupportedReport2byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_3BYTE_FRAME          ZW_ThermostatSetpointSupportedReport3byteFrame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_4BYTE_FRAME          ZW_ThermostatSetpointSupportedReport4byteFrame;\
/* Command class Thermostat Setpoint V2 */\
  ZW_THERMOSTAT_SETPOINT_GET_V2_FRAME                          ZW_ThermostatSetpointGetV2Frame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_1BYTE_V2_FRAME                 ZW_ThermostatSetpointReport1byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_2BYTE_V2_FRAME                 ZW_ThermostatSetpointReport2byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_3BYTE_V2_FRAME                 ZW_ThermostatSetpointReport3byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_REPORT_4BYTE_V2_FRAME                 ZW_ThermostatSetpointReport4byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SET_1BYTE_V2_FRAME                    ZW_ThermostatSetpointSet1byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SET_2BYTE_V2_FRAME                    ZW_ThermostatSetpointSet2byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SET_3BYTE_V2_FRAME                    ZW_ThermostatSetpointSet3byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SET_4BYTE_V2_FRAME                    ZW_ThermostatSetpointSet4byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_GET_V2_FRAME                ZW_ThermostatSetpointSupportedGetV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_1BYTE_V2_FRAME       ZW_ThermostatSetpointSupportedReport1byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_2BYTE_V2_FRAME       ZW_ThermostatSetpointSupportedReport2byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_3BYTE_V2_FRAME       ZW_ThermostatSetpointSupportedReport3byteV2Frame;\
  ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_4BYTE_V2_FRAME       ZW_ThermostatSetpointSupportedReport4byteV2Frame;\
/* Command class Time Parameters */\
  ZW_TIME_PARAMETERS_GET_FRAME                                 ZW_TimeParametersGetFrame;\
  ZW_TIME_PARAMETERS_REPORT_FRAME                              ZW_TimeParametersReportFrame;\
  ZW_TIME_PARAMETERS_SET_FRAME                                 ZW_TimeParametersSetFrame;\
/* Command class Time */\
  ZW_DATE_GET_FRAME                                            ZW_DateGetFrame;\
  ZW_DATE_REPORT_FRAME                                         ZW_DateReportFrame;\
  ZW_TIME_GET_FRAME                                            ZW_TimeGetFrame;\
  ZW_TIME_REPORT_FRAME                                         ZW_TimeReportFrame;\
/* Command class Time V2 */\
  ZW_DATE_GET_V2_FRAME                                         ZW_DateGetV2Frame;\
  ZW_DATE_REPORT_V2_FRAME                                      ZW_DateReportV2Frame;\
  ZW_TIME_GET_V2_FRAME                                         ZW_TimeGetV2Frame;\
  ZW_TIME_OFFSET_GET_V2_FRAME                                  ZW_TimeOffsetGetV2Frame;\
  ZW_TIME_OFFSET_REPORT_V2_FRAME                               ZW_TimeOffsetReportV2Frame;\
  ZW_TIME_OFFSET_SET_V2_FRAME                                  ZW_TimeOffsetSetV2Frame;\
  ZW_TIME_REPORT_V2_FRAME                                      ZW_TimeReportV2Frame;\
/* Command class User Code */\
  ZW_USER_CODE_GET_FRAME                                       ZW_UserCodeGetFrame;\
  ZW_USER_CODE_REPORT_FRAME                                    ZW_UserCodeReportFrame;\
  ZW_USER_CODE_SET_FRAME                                       ZW_UserCodeSetFrame;\
  ZW_USERS_NUMBER_GET_FRAME                                    ZW_UsersNumberGetFrame;\
  ZW_USERS_NUMBER_REPORT_FRAME                                 ZW_UsersNumberReportFrame;\
/* Command class Version */\
  ZW_VERSION_COMMAND_CLASS_GET_FRAME                           ZW_VersionCommandClassGetFrame;\
  ZW_VERSION_COMMAND_CLASS_REPORT_FRAME                        ZW_VersionCommandClassReportFrame;\
  ZW_VERSION_GET_FRAME                                         ZW_VersionGetFrame;\
  ZW_VERSION_REPORT_FRAME                                      ZW_VersionReportFrame;\
/* Command class Wake Up */\
  ZW_WAKE_UP_INTERVAL_GET_FRAME                                ZW_WakeUpIntervalGetFrame;\
  ZW_WAKE_UP_INTERVAL_REPORT_FRAME                             ZW_WakeUpIntervalReportFrame;\
  ZW_WAKE_UP_INTERVAL_SET_FRAME                                ZW_WakeUpIntervalSetFrame;\
  ZW_WAKE_UP_NO_MORE_INFORMATION_FRAME                         ZW_WakeUpNoMoreInformationFrame;\
  ZW_WAKE_UP_NOTIFICATION_FRAME                                ZW_WakeUpNotificationFrame;\
/* Command class Wake Up V2 */\
  ZW_WAKE_UP_INTERVAL_CAPABILITIES_GET_V2_FRAME                ZW_WakeUpIntervalCapabilitiesGetV2Frame;\
  ZW_WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2_FRAME             ZW_WakeUpIntervalCapabilitiesReportV2Frame;\
  ZW_WAKE_UP_INTERVAL_GET_V2_FRAME                             ZW_WakeUpIntervalGetV2Frame;\
  ZW_WAKE_UP_INTERVAL_REPORT_V2_FRAME                          ZW_WakeUpIntervalReportV2Frame;\
  ZW_WAKE_UP_INTERVAL_SET_V2_FRAME                             ZW_WakeUpIntervalSetV2Frame;\
  ZW_WAKE_UP_NO_MORE_INFORMATION_V2_FRAME                      ZW_WakeUpNoMoreInformationV2Frame;\
  ZW_WAKE_UP_NOTIFICATION_V2_FRAME                             ZW_WakeUpNotificationV2Frame;\
/* Command class Zip 6lowpan */\
  ZW_LOWPAN_FIRST_FRAGMENT_1BYTE_FRAME                         ZW_LowpanFirstFragment1byteFrame;\
  ZW_LOWPAN_FIRST_FRAGMENT_2BYTE_FRAME                         ZW_LowpanFirstFragment2byteFrame;\
  ZW_LOWPAN_FIRST_FRAGMENT_3BYTE_FRAME                         ZW_LowpanFirstFragment3byteFrame;\
  ZW_LOWPAN_FIRST_FRAGMENT_4BYTE_FRAME                         ZW_LowpanFirstFragment4byteFrame;\
  ZW_LOWPAN_SUBSEQUENT_FRAGMENT_1BYTE_FRAME                    ZW_LowpanSubsequentFragment1byteFrame;\
  ZW_LOWPAN_SUBSEQUENT_FRAGMENT_2BYTE_FRAME                    ZW_LowpanSubsequentFragment2byteFrame;\
  ZW_LOWPAN_SUBSEQUENT_FRAGMENT_3BYTE_FRAME                    ZW_LowpanSubsequentFragment3byteFrame;\
  ZW_LOWPAN_SUBSEQUENT_FRAGMENT_4BYTE_FRAME                    ZW_LowpanSubsequentFragment4byteFrame;\
/* Command class Zip Services */\
  ZW_COMMAND_ZIP_PACKET_1BYTE_FRAME                            ZW_CommandZipPacket1byteFrame;\
  ZW_COMMAND_ZIP_PACKET_2BYTE_FRAME                            ZW_CommandZipPacket2byteFrame;\
  ZW_COMMAND_ZIP_PACKET_3BYTE_FRAME                            ZW_CommandZipPacket3byteFrame;\
  ZW_COMMAND_ZIP_PACKET_4BYTE_FRAME                            ZW_CommandZipPacket4byteFrame;\


/************ Basic Device Class identifiers **************/
#define BASIC_TYPE_CONTROLLER                                                            0x01 /*Node is a portable controller */
#define BASIC_TYPE_ROUTING_SLAVE                                                         0x04 /*Node is a slave with routing capabilities*/
#define BASIC_TYPE_SLAVE                                                                 0x03 /*Node is a slave*/
#define BASIC_TYPE_STATIC_CONTROLLER                                                     0x02 /*Node is a static controller*/


/***** Generic and Specific Device Class identifiers ******/
/* Device class Av Control Point */
#define GENERIC_TYPE_AV_CONTROL_POINT                                                    0x03 /*AV Control Point*/
#define SPECIFIC_TYPE_DOORBELL                                                           0x12
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SATELLITE_RECEIVER                                                 0x04 /*Satellite Receiver*/
#define SPECIFIC_TYPE_SATELLITE_RECEIVER_V2                                              0x11 /*Satellite Receiver V2*/

/* Device class Display */
#define GENERIC_TYPE_DISPLAY                                                             0x04
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_SIMPLE_DISPLAY                                                     0x01

/* Device class Entry Control */
#define GENERIC_TYPE_ENTRY_CONTROL                                                       0x40 /*Entry Control*/
#define SPECIFIC_TYPE_ADVANCED_DOOR_LOCK                                                 0x02 /*Advanced Door Lock*/
#define SPECIFIC_TYPE_DOOR_LOCK                                                          0x01 /*Door Lock*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class Not Used*/
#define SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK                                            0x03

/* Device class Generic Controller */
#define GENERIC_TYPE_GENERIC_CONTROLLER                                                  0x01 /*Remote Controller*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_PORTABLE_INSTALLER_TOOL                                            0x03
#define SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER                                         0x01 /*Portable Remote Controller*/
#define SPECIFIC_TYPE_PORTABLE_SCENE_CONTROLLER                                          0x02 /*Portable Scene Controller*/

/* Device class Meter */
#define GENERIC_TYPE_METER                                                               0x31 /*Meter*/
#define SPECIFIC_TYPE_ADV_ENERGY_CONTROL                                                 0x02
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SIMPLE_METER                                                       0x01 /*Simple Meter*/

/* Device class Meter Pulse */
#define GENERIC_TYPE_METER_PULSE                                                         0x30 /*Pulse Meter*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/

/* Device class Non Interoperable */
#define GENERIC_TYPE_NON_INTEROPERABLE                                                   0xFF /*Non interoperable*/

/* Device class Repeater Slave */
#define GENERIC_TYPE_REPEATER_SLAVE                                                      0x0F /*Repeater Slave*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_REPEATER_SLAVE                                                     0x01 /*Basic Repeater Slave*/

/* Device class Security Panel */
#define GENERIC_TYPE_SECURITY_PANEL                                                      0x17
#define SPECIFIC_TYPE_ZONED_SECURITY_PANEL                                               0x01

/* Device class Semi Interoperable */
#define GENERIC_TYPE_SEMI_INTEROPERABLE                                                  0x50 /*Semi Interoperable*/
#define SPECIFIC_TYPE_ENERGY_PRODUCTION                                                  0x01 /*Energy Production*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/

/* Device class Sensor Alarm */
#define GENERIC_TYPE_SENSOR_ALARM                                                        0xA1
#define SPECIFIC_TYPE_ADV_ZENSOR_NET_ALARM_SENSOR                                        0x05
#define SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR                                        0x0A
#define SPECIFIC_TYPE_BASIC_ROUTING_ALARM_SENSOR                                         0x01
#define SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR                                         0x06
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_ALARM_SENSOR                                      0x03
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR                                      0x08
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ROUTING_ALARM_SENSOR                                               0x02
#define SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR                                               0x07
#define SPECIFIC_TYPE_ZENSOR_NET_ALARM_SENSOR                                            0x04
#define SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR                                            0x09

/* Device class Sensor Binary */
#define GENERIC_TYPE_SENSOR_BINARY                                                       0x20 /*Binary Sensor*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_ROUTING_SENSOR_BINARY                                              0x01 /*Routing Binary Sensor*/

/* Device class Sensor Multilevel */
#define GENERIC_TYPE_SENSOR_MULTILEVEL                                                   0x21 /*Multilevel Sensor*/
#define SPECIFIC_TYPE_CHIMNEY_FAN                                                        0x02
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL                                          0x01 /*Routing Multilevel Sensor*/

/* Device class Static Controller */
#define GENERIC_TYPE_STATIC_CONTROLLER                                                   0x02 /*Static Controller*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_PC_CONTROLLER                                                      0x01 /*PC Controller*/
#define SPECIFIC_TYPE_SCENE_CONTROLLER                                                   0x02 /*Scene Controller*/
#define SPECIFIC_TYPE_STATIC_INSTALLER_TOOL                                              0x03

/* Device class Switch Binary */
#define GENERIC_TYPE_SWITCH_BINARY                                                       0x10 /*Binary Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_POWER_SWITCH_BINARY                                                0x01 /*Binary Power Switch*/
#define SPECIFIC_TYPE_SCENE_SWITCH_BINARY                                                0x03 /*Binary Scene Switch*/

/* Device class Switch Multilevel */
#define GENERIC_TYPE_SWITCH_MULTILEVEL                                                   0x11 /*Multilevel Switch*/
#define SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL                                              0x05
#define SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL                                              0x06
#define SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL                                              0x07
#define SPECIFIC_TYPE_MOTOR_MULTIPOSITION                                                0x03 /*Multiposition Motor*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL                                            0x01 /*Multilevel Power Switch*/
#define SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL                                            0x04 /*Multilevel Scene Switch*/

/* Device class Switch Remote */
#define GENERIC_TYPE_SWITCH_REMOTE                                                       0x12 /*Remote Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_BINARY                                               0x01 /*Binary Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_MULTILEVEL                                           0x02 /*Multilevel Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_BINARY                                        0x03 /*Binary Toggle Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_MULTILEVEL                                    0x04 /*Multilevel Toggle Remote Switch*/

/* Device class Switch Toggle */
#define GENERIC_TYPE_SWITCH_TOGGLE                                                       0x13 /*Toggle Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SWITCH_TOGGLE_BINARY                                               0x01 /*Binary Toggle Switch*/
#define SPECIFIC_TYPE_SWITCH_TOGGLE_MULTILEVEL                                           0x02 /*Multilevel Toggle Switch*/

/* Device class Thermostat */
#define GENERIC_TYPE_THERMOSTAT                                                          0x08 /*Thermostat*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SETBACK_SCHEDULE_THERMOSTAT                                        0x03 /*Setback Schedule Thermostat*/
#define SPECIFIC_TYPE_SETBACK_THERMOSTAT                                                 0x05
#define SPECIFIC_TYPE_SETPOINT_THERMOSTAT                                                0x04
#define SPECIFIC_TYPE_THERMOSTAT_GENERAL                                                 0x02 /*Thermostat General*/
#define SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2                                              0x06 /*Thermostat General V2*/
#define SPECIFIC_TYPE_THERMOSTAT_HEATING                                                 0x01 /*Thermostat Heating*/

/* Device class Ventilation */
#define GENERIC_TYPE_VENTILATION                                                         0x16
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_RESIDENTIAL_HRV                                                    0x01

/* Device class Window Covering */
#define GENERIC_TYPE_WINDOW_COVERING                                                     0x09 /*Window Covering*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING                                             0x01 /*Simple Window Covering Control*/

/* Device class Zip Gateway */
#define GENERIC_TYPE_ZIP_GATEWAY                                                         0x14
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ZIP_ADV_GATEWAY                                                    0x02
#define SPECIFIC_TYPE_ZIP_TUN_GATEWAY                                                    0x01

/* Device class Zip Node */
#define GENERIC_TYPE_ZIP_NODE                                                            0x15
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ZIP_ADV_NODE                                                       0x02
#define SPECIFIC_TYPE_ZIP_TUN_NODE                                                       0x01



/*************** command class identifiers ****************/
#define COMMAND_CLASS_ALARM                                                              0x71
#define COMMAND_CLASS_ALARM_V2                                                           0x71
#define COMMAND_CLASS_APPLICATION_STATUS                                                 0x22
#define COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION                                  0x9B
#define COMMAND_CLASS_ASSOCIATION                                                        0x85
#define COMMAND_CLASS_ASSOCIATION_V2                                                     0x85
#define COMMAND_CLASS_AV_CONTENT_DIRECTORY_MD                                            0x95
#define COMMAND_CLASS_AV_CONTENT_SEARCH_MD                                               0x97
#define COMMAND_CLASS_AV_RENDERER_STATUS                                                 0x96
#define COMMAND_CLASS_AV_TAGGING_MD                                                      0x99
#define COMMAND_CLASS_BASIC_TARIFF_INFO                                                  0x36
#define COMMAND_CLASS_BASIC_WINDOW_COVERING                                              0x50
#define COMMAND_CLASS_BASIC                                                              0x20
#define COMMAND_CLASS_BATTERY                                                            0x80
#define COMMAND_CLASS_CHIMNEY_FAN                                                        0x2A
#define COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE                                           0x46
#define COMMAND_CLASS_CLOCK                                                              0x81
#define COMMAND_CLASS_CONFIGURATION                                                      0x70
#define COMMAND_CLASS_CONFIGURATION_V2                                                   0x70
#define COMMAND_CLASS_CONTROLLER_REPLICATION                                             0x21
#define COMMAND_CLASS_DCP_CONFIG                                                         0x3A
#define COMMAND_CLASS_DCP_MONITOR                                                        0x3B
#define COMMAND_CLASS_DOOR_LOCK_LOGGING                                                  0x4C
#define COMMAND_CLASS_DOOR_LOCK                                                          0x62
#define COMMAND_CLASS_ENERGY_PRODUCTION                                                  0x90
#define COMMAND_CLASS_FIRMWARE_UPDATE_MD                                                 0x7A
#define COMMAND_CLASS_FIRMWARE_UPDATE_MD_V2                                              0x7A
#define COMMAND_CLASS_GEOGRAPHIC_LOCATION                                                0x8C
#define COMMAND_CLASS_GROUPING_NAME                                                      0x7B
#define COMMAND_CLASS_HAIL                                                               0x82
#define COMMAND_CLASS_HRV_CONTROL                                                        0x39
#define COMMAND_CLASS_HRV_STATUS                                                         0x37
#define COMMAND_CLASS_INDICATOR                                                          0x87
#define COMMAND_CLASS_IP_CONFIGURATION                                                   0x9A
#define COMMAND_CLASS_LANGUAGE                                                           0x89
#define COMMAND_CLASS_LOCK                                                               0x76
#define COMMAND_CLASS_MANUFACTURER_PROPRIETARY                                           0x91
#define COMMAND_CLASS_MANUFACTURER_SPECIFIC                                              0x72
#define COMMAND_CLASS_MARK                                                               0xEF
#define COMMAND_CLASS_METER_PULSE                                                        0x35
#define COMMAND_CLASS_METER_TBL_CONFIG                                                   0x3C
#define COMMAND_CLASS_METER_TBL_MONITOR                                                  0x3D
#define COMMAND_CLASS_METER_TBL_PUSH                                                     0x3E
#define COMMAND_CLASS_METER                                                              0x32
#define COMMAND_CLASS_METER_V2                                                           0x32
#define COMMAND_CLASS_METER_V3                                                           0x32
#define COMMAND_CLASS_MTP_WINDOW_COVERING                                                0x51
#define COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2                                       0x8E
#define COMMAND_CLASS_MULTI_CHANNEL_V2                                                   0x60
#define COMMAND_CLASS_MULTI_CMD                                                          0x8F
#define COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION                                         0x8E
#define COMMAND_CLASS_MULTI_INSTANCE                                                     0x60
#define COMMAND_CLASS_NETWORK_MANAGEMENT_INFO                                            0x33
#define COMMAND_CLASS_NETWORK_MANAGEMENT_LIST                                            0x52
#define COMMAND_CLASS_NETWORK_MANAGEMENT_NODE                                            0x4D
#define COMMAND_CLASS_NETWORK_MANAGEMENT_PRI_CTRL                                        0x34
#define COMMAND_CLASS_NO_OPERATION                                                       0x00
#define COMMAND_CLASS_NODE_NAMING                                                        0x77
#define COMMAND_CLASS_NON_INTEROPERABLE                                                  0xF0
#define COMMAND_CLASS_POWERLEVEL                                                         0x73
#define COMMAND_CLASS_PREPAYMENT_ENCAPSULATION                                           0x41
#define COMMAND_CLASS_PREPAYMENT                                                         0x3F
#define COMMAND_CLASS_PROPRIETARY                                                        0x88
#define COMMAND_CLASS_PROTECTION                                                         0x75
#define COMMAND_CLASS_PROTECTION_V2                                                      0x75
#define COMMAND_CLASS_RATE_TBL_CONFIG                                                    0x48
#define COMMAND_CLASS_RATE_TBL_MONITOR                                                   0x49
#define COMMAND_CLASS_REMOTE_ASSOCIATION_ACTIVATE                                        0x7C
#define COMMAND_CLASS_REMOTE_ASSOCIATION                                                 0x7D
#define COMMAND_CLASS_SCENE_ACTIVATION                                                   0x2B
#define COMMAND_CLASS_SCENE_ACTUATOR_CONF                                                0x2C
#define COMMAND_CLASS_SCENE_CONTROLLER_CONF                                              0x2D
#define COMMAND_CLASS_SCHEDULE_ENTRY_LOCK                                                0x4E
#define COMMAND_CLASS_SCHEDULE_ENTRY_LOCK_V2                                             0x4E
#define COMMAND_CLASS_SCREEN_ATTRIBUTES                                                  0x93
#define COMMAND_CLASS_SCREEN_ATTRIBUTES_V2                                               0x93
#define COMMAND_CLASS_SCREEN_MD                                                          0x92
#define COMMAND_CLASS_SCREEN_MD_V2                                                       0x92
#define COMMAND_CLASS_SECURITY_PANEL_MODE                                                0x24
#define COMMAND_CLASS_SECURITY_PANEL_ZONE_SENSOR                                         0x2F
#define COMMAND_CLASS_SECURITY_PANEL_ZONE                                                0x2E
#define COMMAND_CLASS_SECURITY                                                           0x98
#define COMMAND_CLASS_SENSOR_ALARM                                                       0x9C /*SDS10963-4 The Sensor Alarm command class can be used to realize Sensor Alarms.*/
#define COMMAND_CLASS_SENSOR_BINARY                                                      0x30
#define COMMAND_CLASS_SENSOR_CONFIGURATION                                               0x9E /*This command class adds the possibility for sensors to act on either a measured value or on a*/
#define COMMAND_CLASS_SENSOR_MULTILEVEL                                                  0x31
#define COMMAND_CLASS_SENSOR_MULTILEVEL_V2                                               0x31
#define COMMAND_CLASS_SENSOR_MULTILEVEL_V3                                               0x31
#define COMMAND_CLASS_SILENCE_ALARM                                                      0x9D /*SDS10963-4 The Alarm Silence command class can be used to nuisance silence to temporarily disable the sounding*/
#define COMMAND_CLASS_SIMPLE_AV_CONTROL                                                  0x94
#define COMMAND_CLASS_SWITCH_ALL                                                         0x27
#define COMMAND_CLASS_SWITCH_BINARY                                                      0x25
#define COMMAND_CLASS_SWITCH_MULTILEVEL                                                  0x26
#define COMMAND_CLASS_SWITCH_MULTILEVEL_V2                                               0x26
#define COMMAND_CLASS_SWITCH_MULTILEVEL_V3                                               0x26
#define COMMAND_CLASS_SWITCH_TOGGLE_BINARY                                               0x28
#define COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL                                           0x29
#define COMMAND_CLASS_TARIFF_CONFIG                                                      0x4A
#define COMMAND_CLASS_TARIFF_TBL_MONITOR                                                 0x4B
#define COMMAND_CLASS_THERMOSTAT_FAN_MODE                                                0x44
#define COMMAND_CLASS_THERMOSTAT_FAN_STATE                                               0x45
#define COMMAND_CLASS_THERMOSTAT_HEATING                                                 0x38
#define COMMAND_CLASS_THERMOSTAT_MODE                                                    0x40
#define COMMAND_CLASS_THERMOSTAT_MODE_V2                                                 0x40
#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE                                         0x42
#define COMMAND_CLASS_THERMOSTAT_SETBACK                                                 0x47
#define COMMAND_CLASS_THERMOSTAT_SETPOINT                                                0x43
#define COMMAND_CLASS_THERMOSTAT_SETPOINT_V2                                             0x43
#define COMMAND_CLASS_TIME_PARAMETERS                                                    0x8B
#define COMMAND_CLASS_TIME                                                               0x8A
#define COMMAND_CLASS_TIME_V2                                                            0x8A
#define COMMAND_CLASS_USER_CODE                                                          0x63
#define COMMAND_CLASS_VERSION                                                            0x86
#define COMMAND_CLASS_WAKE_UP                                                            0x84
#define COMMAND_CLASS_WAKE_UP_V2                                                         0x84
#define COMMAND_CLASS_ZIP_6LOWPAN                                                        0x4F
#define COMMAND_CLASS_ZIP_SERVICES                                                       0x23

/* Unknown command class commands */
#define UNKNOWN_VERSION                                 0x00

/* Alarm command class commands */
#define ALARM_VERSION                                                                    0x01
#define ALARM_GET                                                                        0x04
#define ALARM_REPORT                                                                     0x05

/* Alarm command class commands */
#define ALARM_VERSION_V2                                                                 0x02
#define ALARM_GET_V2                                                                     0x04
#define ALARM_REPORT_V2                                                                  0x05
#define ALARM_SET_V2                                                                     0x06
#define ALARM_TYPE_SUPPORTED_GET_V2                                                      0x07
#define ALARM_TYPE_SUPPORTED_REPORT_V2                                                   0x08
/* Values used for Alarm Get command */
#define ALARM_GET_SMOKE_V2                                                               0x01
#define ALARM_GET_CO_V2                                                                  0x02
#define ALARM_GET_CO2_V2                                                                 0x03
#define ALARM_GET_HEAT_V2                                                                0x04
#define ALARM_GET_WATER_V2                                                               0x05
#define ALARM_GET_ACCESS_CONTROL_V2                                                      0x06
#define ALARM_GET_BURGLAR_V2                                                             0x07
#define ALARM_GET_POWER_MANAGEMENT_V2                                                    0x08
#define ALARM_GET_SYSTEM_V2                                                              0x09
#define ALARM_GET_EMERGENCY_V2                                                           0x0A
#define ALARM_GET_CLOCK_V2                                                               0x0B
#define ALARM_GET_FIRST_V2                                                               0xFF
/* Values used for Alarm Report command */
#define ALARM_REPORT_SMOKE_V2                                                            0x01
#define ALARM_REPORT_CO_V2                                                               0x02
#define ALARM_REPORT_CO2_V2                                                              0x03
#define ALARM_REPORT_HEAT_V2                                                             0x04
#define ALARM_REPORT_WATER_V2                                                            0x05
#define ALARM_REPORT_ACCESS_CONTROL_V2                                                   0x06
#define ALARM_REPORT_BURGLAR_V2                                                          0x07
#define ALARM_REPORT_POWER_MANAGEMENT_V2                                                 0x08
#define ALARM_REPORT_SYSTEM_V2                                                           0x09
#define ALARM_REPORT_EMERGENCY_V2                                                        0x0A
#define ALARM_REPORT_CLOCK_V2                                                            0x0B
#define ALARM_REPORT_FIRST_V2                                                            0xFF
/* Values used for Alarm Set command */
#define ALARM_SET_SMOKE_V2                                                               0x01
#define ALARM_SET_CO_V2                                                                  0x02
#define ALARM_SET_CO2_V2                                                                 0x03
#define ALARM_SET_HEAT_V2                                                                0x04
#define ALARM_SET_WATER_V2                                                               0x05
#define ALARM_SET_ACCESS_CONTROL_V2                                                      0x06
#define ALARM_SET_BURGLAR_V2                                                             0x07
#define ALARM_SET_POWER_MANAGEMENT_V2                                                    0x08
#define ALARM_SET_SYSTEM_V2                                                              0x09
#define ALARM_SET_EMERGENCY_V2                                                           0x0A
#define ALARM_SET_CLOCK_V2                                                               0x0B
#define ALARM_SET_FIRST_V2                                                               0xFF
/* Values used for Alarm Type Supported Report command */
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V2              0x1F
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V2                         0x60
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                        0x05
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_V1_ALARM_BIT_MASK_V2                     0x80

/* Application Status command class commands */
#define APPLICATION_STATUS_VERSION                                                       0x01
#define APPLICATION_BUSY                                                                 0x01
#define APPLICATION_REJECTED_REQUEST                                                     0x02
/* Values used for Application Busy command */
#define APPLICATION_BUSY_TRY_AGAIN_LATER                                                 0x00
#define APPLICATION_BUSY_TRY_AGAIN_IN_WAIT_TIME_SECONDS                                  0x01
#define APPLICATION_BUSY_REQUEST_QUEUED_EXECUTED_LATER                                   0x02

/* Association Command Configuration command class commands */
#define ASSOCIATION_COMMAND_CONFIGURATION_VERSION                                        0x01
#define COMMAND_CONFIGURATION_GET                                                        0x04
#define COMMAND_CONFIGURATION_REPORT                                                     0x05
#define COMMAND_CONFIGURATION_SET                                                        0x03
#define COMMAND_RECORDS_SUPPORTED_GET                                                    0x01
#define COMMAND_RECORDS_SUPPORTED_REPORT                                                 0x02
/* Values used for Command Configuration Report command */
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_REPORTS_TO_FOLLOW_MASK                  0x0F
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_MASK                           0x70
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_SHIFT                          0x04
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_FIRST_BIT_MASK                          0x80
/* Values used for Command Records Supported Report command */
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_CONF_CMD_BIT_MASK                   0x01
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_V_C_BIT_MASK                        0x02
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_MAX_COMMAND_LENGTH_MASK             0xFC
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_MAX_COMMAND_LENGTH_SHIFT            0x02

/* Association command class commands */
#define ASSOCIATION_VERSION                                                              0x01
#define ASSOCIATION_GET                                                                  0x02
#define ASSOCIATION_GROUPINGS_GET                                                        0x05
#define ASSOCIATION_GROUPINGS_REPORT                                                     0x06
#define ASSOCIATION_REMOVE                                                               0x04
#define ASSOCIATION_REPORT                                                               0x03
#define ASSOCIATION_SET                                                                  0x01

/* Association command class commands */
#define ASSOCIATION_VERSION_V2                                                           0x02
#define ASSOCIATION_GET_V2                                                               0x02
#define ASSOCIATION_GROUPINGS_GET_V2                                                     0x05
#define ASSOCIATION_GROUPINGS_REPORT_V2                                                  0x06
#define ASSOCIATION_REMOVE_V2                                                            0x04
#define ASSOCIATION_REPORT_V2                                                            0x03
#define ASSOCIATION_SET_V2                                                               0x01
#define ASSOCIATION_SPECIFIC_GROUP_GET_V2                                                0x0B
#define ASSOCIATION_SPECIFIC_GROUP_REPORT_V2                                             0x0C

/* Av Content Directory Md command class commands */
#define AV_CONTENT_DIRECTORY_MD_VERSION                                                  0x01
#define AV_CONTENT_BROWSE_MD_BY_LETTER_GET                                               0x03
#define AV_CONTENT_BROWSE_MD_BY_LETTER_REPORT                                            0x04
#define AV_CONTENT_BROWSE_MD_CHILD_COUNT_GET                                             0x05
#define AV_CONTENT_BROWSE_MD_CHILD_COUNT_REPORT                                          0x06
#define AV_CONTENT_BROWSE_MD_GET                                                         0x01
#define AV_CONTENT_BROWSE_MD_REPORT                                                      0x02
#define AV_MATCH_ITEM_TO_RENDERER_MD_GET                                                 0x07
#define AV_MATCH_ITEM_TO_RENDERER_MD_REPORT                                              0x08

/* Av Content Search Md command class commands */
#define AV_CONTENT_SEARCH_MD_VERSION                                                     0x01
#define AV_CONTENT_SEARCH_MD_GET                                                         0x01
#define AV_CONTENT_SEARCH_MD_REPORT                                                      0x02

/* Av Renderer Status command class commands */
#define AV_RENDERER_STATUS_VERSION                                                       0x01
#define AV_RENDERER_STATUS_GET                                                           0x01
#define AV_RENDERER_STATUS_REPORT                                                        0x02

/* Av Tagging Md command class commands */
#define AV_TAGGING_MD_VERSION                                                            0x01
#define AV_TAGGING_MD_GET                                                                0x01
#define AV_TAGGING_MD_REPORT                                                             0x02

/* Basic Tariff Info command class commands */
#define BASIC_TARIFF_INFO_VERSION                                                        0x01
#define BASIC_TARIFF_INFO_GET                                                            0x01
#define BASIC_TARIFF_INFO_REPORT                                                         0x02
/* Values used for Basic Tariff Info Report command */
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_TOTAL_NO_IMPORT_RATES_MASK                  0x0F
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_RESERVED_MASK                               0x70
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_RESERVED_SHIFT                              0x04
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_DUAL_BIT_MASK                               0x80
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES2_E1_CURRENT_RATE_IN_USE_MASK                 0x0F
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES2_RESERVED_MASK                               0xF0
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES2_RESERVED_SHIFT                              0x04
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES3_E2_CURRENT_RATE_IN_USE_MASK                 0x0F
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES3_RESERVED_MASK                               0xF0
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES3_RESERVED_SHIFT                              0x04

/* Basic Window Covering command class commands */
#define BASIC_WINDOW_COVERING_VERSION                                                    0x01
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE                                         0x01
#define BASIC_WINDOW_COVERING_STOP_LEVEL_CHANGE                                          0x02
/* Values used for Basic Window Covering Start Level Change command */
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_LEVEL_RESERVED_MASK                     0x3F
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_LEVEL_OPEN_CLOSE_BIT_MASK               0x40
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_LEVEL_RESERVED_BIT_MASK                 0x80

/* Basic command class commands */
#define BASIC_VERSION                                                                    0x01
#define BASIC_GET                                                                        0x02
#define BASIC_REPORT                                                                     0x03
#define BASIC_SET                                                                        0x01

/* Basic level definitions */
#define BASIC_ON                                        0xFF
#define BASIC_OFF                                       0x00


/* Battery command class commands */
#define BATTERY_VERSION                                                                  0x01
#define BATTERY_GET                                                                      0x02
#define BATTERY_REPORT                                                                   0x03

/* Chimney Fan command class commands */
#define CHIMNEY_FAN_VERSION                                                              0x01
#define CHIMNEY_FAN_ALARM_LOG_GET                                                        0x20
#define CHIMNEY_FAN_ALARM_LOG_REPORT                                                     0x21
#define CHIMNEY_FAN_ALARM_LOG_SET                                                        0x1F
#define CHIMNEY_FAN_ALARM_STATUS_GET                                                     0x23
#define CHIMNEY_FAN_ALARM_STATUS_REPORT                                                  0x24
#define CHIMNEY_FAN_ALARM_STATUS_SET                                                     0x22
#define CHIMNEY_FAN_ALARM_TEMP_GET                                                       0x0E
#define CHIMNEY_FAN_ALARM_TEMP_REPORT                                                    0x0F
#define CHIMNEY_FAN_ALARM_TEMP_SET                                                       0x0D
#define CHIMNEY_FAN_BOOST_TIME_GET                                                       0x11
#define CHIMNEY_FAN_BOOST_TIME_REPORT                                                    0x12
#define CHIMNEY_FAN_BOOST_TIME_SET                                                       0x10
#define CHIMNEY_FAN_DEFAULT_SET                                                          0x28
#define CHIMNEY_FAN_MIN_SPEED_GET                                                        0x26
#define CHIMNEY_FAN_MIN_SPEED_REPORT                                                     0x27
#define CHIMNEY_FAN_MIN_SPEED_SET                                                        0x25
#define CHIMNEY_FAN_MODE_GET                                                             0x17
#define CHIMNEY_FAN_MODE_REPORT                                                          0x18
#define CHIMNEY_FAN_MODE_SET                                                             0x16
#define CHIMNEY_FAN_SETUP_GET                                                            0x1A
#define CHIMNEY_FAN_SETUP_REPORT                                                         0x1B
#define CHIMNEY_FAN_SETUP_SET                                                            0x19
#define CHIMNEY_FAN_SPEED_GET                                                            0x05
#define CHIMNEY_FAN_SPEED_REPORT                                                         0x06
#define CHIMNEY_FAN_SPEED_SET                                                            0x04
#define CHIMNEY_FAN_START_TEMP_GET                                                       0x08
#define CHIMNEY_FAN_START_TEMP_REPORT                                                    0x09
#define CHIMNEY_FAN_START_TEMP_SET                                                       0x07
#define CHIMNEY_FAN_STATE_GET                                                            0x02
#define CHIMNEY_FAN_STATE_REPORT                                                         0x03
#define CHIMNEY_FAN_STATE_SET                                                            0x01
#define CHIMNEY_FAN_STATUS_GET                                                           0x1D
#define CHIMNEY_FAN_STATUS_REPORT                                                        0x1E
#define CHIMNEY_FAN_STOP_TEMP_GET                                                        0x0B
#define CHIMNEY_FAN_STOP_TEMP_REPORT                                                     0x0C
#define CHIMNEY_FAN_STOP_TEMP_SET                                                        0x0A
#define CHIMNEY_FAN_STOP_TIME_GET                                                        0x14
#define CHIMNEY_FAN_STOP_TIME_REPORT                                                     0x15
#define CHIMNEY_FAN_STOP_TIME_SET                                                        0x13
/* Values used for Chimney Fan Alarm Log Report command */
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_EXTERNAL_ALARM_1_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_SENSOR_ERROR_1_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_ALARM_TEMPERATURE_EXCEEDED_1_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_ALARM_STILL_ACTIVE_1_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_EXTERNAL_ALARM_2_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_SENSOR_ERROR_2_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_ALARM_TEMPERATURE_EXCEEDED_2_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_ALARM_STILL_ACTIVE_2_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_EXTERNAL_ALARM_3_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_SENSOR_ERROR_3_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_ALARM_TEMPERATURE_EXCEEDED_3_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_ALARM_STILL_ACTIVE_3_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_EXTERNAL_ALARM_4_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_SENSOR_ERROR_4_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_ALARM_TEMPERATURE_EXCEEDED_4_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_ALARM_STILL_ACTIVE_4_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_EXTERNAL_ALARM_5_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_SENSOR_ERROR_5_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_ALARM_TEMPERATURE_EXCEEDED_5_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_ALARM_STILL_ACTIVE_5_BIT_MASK         0x80
/* Values used for Chimney Fan Alarm Log Set command */
#define CHIMNEY_FAN_ALARM_LOG_SET_RESET_LOG                                              0x08
/* Values used for Chimney Fan Alarm Status Report command */
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_SERVICE_BIT_MASK                    0x01
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_EXTERNAL_ALARM_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_SENSOR_ERROR_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_ALARM_TEMPERATURE_EXCEEDED_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_NOT_USED_MASK                       0x30
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_NOT_USED_SHIFT                      0x04
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_SPEED_CHANGE_ENABLE_BIT_MASK        0x40
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_START_TEMPERATURE_EXCEEDED_BIT_MASK 0x80
/* Values used for Chimney Fan Alarm Status Set command */
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_NOT_USED_BIT_MASK                           0x01
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_ACKNOWLEDGE_EXTERNAL_ALARM_BIT_MASK         0x02
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_ACKNOWLEDGE_SENSOR_ERROR_BIT_MASK           0x04
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_ACKNOWLEDGE_ALARM_TEMPERATURE_EXCEEDED_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_NOT_USED_MASK                               0xF0
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_NOT_USED_SHIFT                              0x04
/* Values used for Chimney Fan Alarm Temp Report command */
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_SIZE_MASK                              0x07
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_SCALE_MASK                             0x18
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_SCALE_SHIFT                            0x03
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_PRECISION_MASK                         0xE0
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_PRECISION_SHIFT                        0x05
/* Values used for Chimney Fan Alarm Temp Set command */
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_SIZE_MASK                                 0x07
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_SCALE_MASK                                0x18
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_SCALE_SHIFT                               0x03
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_PRECISION_MASK                            0xE0
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_PRECISION_SHIFT                           0x05
/* Values used for Chimney Fan Mode Report command */
#define CHIMNEY_FAN_MODE_REPORT_OFF                                                      0x00
#define CHIMNEY_FAN_MODE_REPORT_ON                                                       0xFF
/* Values used for Chimney Fan Mode Set command */
#define CHIMNEY_FAN_MODE_SET_OFF                                                         0x00
#define CHIMNEY_FAN_MODE_SET_ON                                                          0xFF
/* Values used for Chimney Fan Setup Report command */
#define CHIMNEY_FAN_SETUP_REPORT_OFF                                                     0x00
#define CHIMNEY_FAN_SETUP_REPORT_ON                                                      0xFF
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_SIZE_1_MASK                                 0x07
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_SCALE_1_MASK                                0x18
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_SCALE_1_SHIFT                               0x03
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_PRECISION_1_MASK                            0xE0
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_PRECISION_1_SHIFT                           0x05
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_SIZE_2_MASK                                 0x07
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_SCALE_2_MASK                                0x18
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_SCALE_2_SHIFT                               0x03
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_PRECISION_2_MASK                            0xE0
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_PRECISION_2_SHIFT                           0x05
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_SIZE_3_MASK                                 0x07
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_SCALE_3_MASK                                0x18
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_SCALE_3_SHIFT                               0x03
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_PRECISION_3_MASK                            0xE0
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_PRECISION_3_SHIFT                           0x05
/* Values used for Chimney Fan Setup Set command */
#define CHIMNEY_FAN_SETUP_SET_OFF                                                        0x00
#define CHIMNEY_FAN_SETUP_SET_ON                                                         0xFF
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_SIZE_1_MASK                                    0x07
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_SCALE_1_MASK                                   0x18
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_SCALE_1_SHIFT                                  0x03
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_PRECISION_1_MASK                               0xE0
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_PRECISION_1_SHIFT                              0x05
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_SIZE_2_MASK                                    0x07
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_SCALE_2_MASK                                   0x18
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_SCALE_2_SHIFT                                  0x03
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_PRECISION_2_MASK                               0xE0
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_PRECISION_2_SHIFT                              0x05
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_SIZE_3_MASK                                    0x07
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_SCALE_3_MASK                                   0x18
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_SCALE_3_SHIFT                                  0x03
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_PRECISION_3_MASK                               0xE0
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_PRECISION_3_SHIFT                              0x05
/* Values used for Chimney Fan Start Temp Report command */
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_SIZE_MASK                              0x07
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_SCALE_MASK                             0x18
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_SCALE_SHIFT                            0x03
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_PRECISION_MASK                         0xE0
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_PRECISION_SHIFT                        0x05
/* Values used for Chimney Fan Start Temp Set command */
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_SIZE_MASK                                 0x07
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_SCALE_MASK                                0x18
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_SCALE_SHIFT                               0x03
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_PRECISION_MASK                            0xE0
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_PRECISION_SHIFT                           0x05
/* Values used for Chimney Fan State Report command */
#define CHIMNEY_FAN_STATE_REPORT_OFF                                                     0x00
#define CHIMNEY_FAN_STATE_REPORT_BOOST                                                   0x01
#define CHIMNEY_FAN_STATE_REPORT_EXHAUST                                                 0x02
#define CHIMNEY_FAN_STATE_REPORT_RELOAD                                                  0x03
#define CHIMNEY_FAN_STATE_REPORT_VENTING                                                 0x04
#define CHIMNEY_FAN_STATE_REPORT_STOP                                                    0x05
#define CHIMNEY_FAN_STATE_REPORT_VENTING_EX                                              0x06
#define CHIMNEY_FAN_STATE_REPORT_SERVICE                                                 0x07
#define CHIMNEY_FAN_STATE_REPORT_SENSOR_FAILURE                                          0x08
#define CHIMNEY_FAN_STATE_REPORT_CHIMNEY_FIRE                                            0x09
#define CHIMNEY_FAN_STATE_REPORT_EXTERNAL_ALARM                                          0x0A
/* Values used for Chimney Fan State Set command */
#define CHIMNEY_FAN_STATE_SET_NEXT_STATE                                                 0x01
/* Values used for Chimney Fan Status Report command */
#define CHIMNEY_FAN_STATUS_REPORT_OFF                                                    0x00
#define CHIMNEY_FAN_STATUS_REPORT_BOOST                                                  0x01
#define CHIMNEY_FAN_STATUS_REPORT_EXHAUST                                                0x02
#define CHIMNEY_FAN_STATUS_REPORT_RELOAD                                                 0x03
#define CHIMNEY_FAN_STATUS_REPORT_VENTING                                                0x04
#define CHIMNEY_FAN_STATUS_REPORT_STOP                                                   0x05
#define CHIMNEY_FAN_STATUS_REPORT_VENTING_EX                                             0x06
#define CHIMNEY_FAN_STATUS_REPORT_SERVICE                                                0x07
#define CHIMNEY_FAN_STATUS_REPORT_SENSOR_FAILURE                                         0x08
#define CHIMNEY_FAN_STATUS_REPORT_CHIMNEY_FIRE                                           0x09
#define CHIMNEY_FAN_STATUS_REPORT_EXTERNAL_ALARM                                         0x0A
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_SERVICE_BIT_MASK                          0x01
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_EXTERNAL_ALARM_BIT_MASK                   0x02
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_SENSOR_ERROR_BIT_MASK                     0x04
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_ALARM_TEMPERATURE_EXCEEDED_BIT_MASK       0x08
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_NOT_USED_MASK                             0x30
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_NOT_USED_SHIFT                            0x04
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_SPEED_CHANGE_ENABLE_BIT_MASK              0x40
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_START_TEMPERATURE_EXCEEDED_BIT_MASK       0x80
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_SIZE_MASK                                  0x07
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_SCALE_MASK                                 0x18
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_SCALE_SHIFT                                0x03
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_PRECISION_MASK                             0xE0
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_PRECISION_SHIFT                            0x05
/* Values used for Chimney Fan Stop Temp Report command */
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_SIZE_MASK                               0x07
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_SCALE_MASK                              0x18
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_SCALE_SHIFT                             0x03
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_PRECISION_MASK                          0xE0
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_PRECISION_SHIFT                         0x05
/* Values used for Chimney Fan Stop Temp Set command */
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_SIZE_MASK                                  0x07
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_SCALE_MASK                                 0x18
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_SCALE_SHIFT                                0x03
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_PRECISION_MASK                             0xE0
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_PRECISION_SHIFT                            0x05

/* Climate Control Schedule command class commands */
#define CLIMATE_CONTROL_SCHEDULE_VERSION                                                 0x01
#define SCHEDULE_CHANGED_GET                                                             0x04
#define SCHEDULE_CHANGED_REPORT                                                          0x05
#define SCHEDULE_GET                                                                     0x02
#define SCHEDULE_OVERRIDE_GET                                                            0x07
#define SCHEDULE_OVERRIDE_REPORT                                                         0x08
#define SCHEDULE_OVERRIDE_SET                                                            0x06
#define SCHEDULE_REPORT                                                                  0x03
#define SCHEDULE_SET                                                                     0x01
/* Values used for Schedule Get command */
#define SCHEDULE_GET_PROPERTIES1_WEEKDAY_MASK                                            0x07
#define SCHEDULE_GET_PROPERTIES1_RESERVED_MASK                                           0xF8
#define SCHEDULE_GET_PROPERTIES1_RESERVED_SHIFT                                          0x03
/* Values used for Schedule Override Report command */
#define SCHEDULE_OVERRIDE_REPORT_PROPERTIES1_OVERRIDE_TYPE_MASK                          0x03
#define SCHEDULE_OVERRIDE_REPORT_PROPERTIES1_RESERVED_MASK                               0xFC
#define SCHEDULE_OVERRIDE_REPORT_PROPERTIES1_RESERVED_SHIFT                              0x02
#define SCHEDULE_OVERRIDE_REPORT_NO_OVERRIDE                                             0x00
#define SCHEDULE_OVERRIDE_REPORT_TEMPORARY_OVERRIDE                                      0x01
#define SCHEDULE_OVERRIDE_REPORT_PERMANENT_OVERRIDE                                      0x02
#define SCHEDULE_OVERRIDE_REPORT_RESERVED                                                0x03
/* Values used for Schedule Override Set command */
#define SCHEDULE_OVERRIDE_SET_PROPERTIES1_OVERRIDE_TYPE_MASK                             0x03
#define SCHEDULE_OVERRIDE_SET_PROPERTIES1_RESERVED_MASK                                  0xFC
#define SCHEDULE_OVERRIDE_SET_PROPERTIES1_RESERVED_SHIFT                                 0x02
#define SCHEDULE_OVERRIDE_SET_NO_OVERRIDE                                                0x00
#define SCHEDULE_OVERRIDE_SET_TEMPORARY_OVERRIDE                                         0x01
#define SCHEDULE_OVERRIDE_SET_PERMANENT_OVERRIDE                                         0x02
#define SCHEDULE_OVERRIDE_SET_RESERVED                                                   0x03
/* Values used for Schedule Report command */
#define SCHEDULE_REPORT_PROPERTIES1_WEEKDAY_MASK                                         0x07
#define SCHEDULE_REPORT_PROPERTIES1_RESERVED_MASK                                        0xF8
#define SCHEDULE_REPORT_PROPERTIES1_RESERVED_SHIFT                                       0x03
/* Values used for Schedule Set command */
#define SCHEDULE_SET_PROPERTIES1_WEEKDAY_MASK                                            0x07
#define SCHEDULE_SET_PROPERTIES1_RESERVED_MASK                                           0xF8
#define SCHEDULE_SET_PROPERTIES1_RESERVED_SHIFT                                          0x03

/* Clock command class commands */
#define CLOCK_VERSION                                                                    0x01
#define CLOCK_GET                                                                        0x05
#define CLOCK_REPORT                                                                     0x06
#define CLOCK_SET                                                                        0x04
/* Values used for Clock Report command */
#define CLOCK_REPORT_LEVEL_HOUR_MASK                                                     0x1F
#define CLOCK_REPORT_LEVEL_WEEKDAY_MASK                                                  0xE0
#define CLOCK_REPORT_LEVEL_WEEKDAY_SHIFT                                                 0x05
/* Values used for Clock Set command */
#define CLOCK_SET_LEVEL_HOUR_MASK                                                        0x1F
#define CLOCK_SET_LEVEL_WEEKDAY_MASK                                                     0xE0
#define CLOCK_SET_LEVEL_WEEKDAY_SHIFT                                                    0x05

/* Configuration command class commands */
#define CONFIGURATION_VERSION                                                            0x01
#define CONFIGURATION_GET                                                                0x05
#define CONFIGURATION_REPORT                                                             0x06
#define CONFIGURATION_SET                                                                0x04
/* Values used for Configuration Report command */
#define CONFIGURATION_REPORT_LEVEL_SIZE_MASK                                             0x07
#define CONFIGURATION_REPORT_LEVEL_RESERVED_MASK                                         0xF8
#define CONFIGURATION_REPORT_LEVEL_RESERVED_SHIFT                                        0x03
/* Values used for Configuration Set command */
#define CONFIGURATION_SET_LEVEL_SIZE_MASK                                                0x07
#define CONFIGURATION_SET_LEVEL_RESERVED_MASK                                            0x78
#define CONFIGURATION_SET_LEVEL_RESERVED_SHIFT                                           0x03
#define CONFIGURATION_SET_LEVEL_DEFAULT_BIT_MASK                                         0x80

/* Configuration command class commands */
#define CONFIGURATION_VERSION_V2                                                         0x02
#define CONFIGURATION_BULK_GET_V2                                                        0x08
#define CONFIGURATION_BULK_REPORT_V2                                                     0x09
#define CONFIGURATION_BULK_SET_V2                                                        0x07
#define CONFIGURATION_GET_V2                                                             0x05
#define CONFIGURATION_REPORT_V2                                                          0x06
#define CONFIGURATION_SET_V2                                                             0x04
/* Values used for Configuration Bulk Report command */
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_SIZE_MASK_V2                               0x07
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_RESERVED_MASK_V2                           0x38
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                          0x03
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_HANDSHAKE_BIT_MASK_V2                      0x40
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_DEFAULT_BIT_MASK_V2                        0x80
/* Values used for Configuration Bulk Set command */
#define CONFIGURATION_BULK_SET_PROPERTIES1_SIZE_MASK_V2                                  0x07
#define CONFIGURATION_BULK_SET_PROPERTIES1_RESERVED_MASK_V2                              0x38
#define CONFIGURATION_BULK_SET_PROPERTIES1_RESERVED_SHIFT_V2                             0x03
#define CONFIGURATION_BULK_SET_PROPERTIES1_HANDSHAKE_BIT_MASK_V2                         0x40
#define CONFIGURATION_BULK_SET_PROPERTIES1_DEFAULT_BIT_MASK_V2                           0x80
/* Values used for Configuration Report command */
#define CONFIGURATION_REPORT_LEVEL_SIZE_MASK_V2                                          0x07
#define CONFIGURATION_REPORT_LEVEL_RESERVED_MASK_V2                                      0xF8
#define CONFIGURATION_REPORT_LEVEL_RESERVED_SHIFT_V2                                     0x03
/* Values used for Configuration Set command */
#define CONFIGURATION_SET_LEVEL_SIZE_MASK_V2                                             0x07
#define CONFIGURATION_SET_LEVEL_RESERVED_MASK_V2                                         0x78
#define CONFIGURATION_SET_LEVEL_RESERVED_SHIFT_V2                                        0x03
#define CONFIGURATION_SET_LEVEL_DEFAULT_BIT_MASK_V2                                      0x80

/* Controller Replication command class commands */
#define CONTROLLER_REPLICATION_VERSION                                                   0x01
#define CTRL_REPLICATION_TRANSFER_GROUP                                                  0x31
#define CTRL_REPLICATION_TRANSFER_GROUP_NAME                                             0x32
#define CTRL_REPLICATION_TRANSFER_SCENE                                                  0x33
#define CTRL_REPLICATION_TRANSFER_SCENE_NAME                                             0x34

/* Dcp Config command class commands */
#define DCP_CONFIG_VERSION                                                               0x01
#define DCP_LIST_REMOVE                                                                  0x04
#define DCP_LIST_SET                                                                     0x03
#define DCP_LIST_SUPPORTED_GET                                                           0x01
#define DCP_LIST_SUPPORTED_REPORT                                                        0x02
/* Values used for Dcp List Set command */
#define DCP_LIST_SET_PROPERTIES1_NUMBER_OF_DC_MASK                                       0x03
#define DCP_LIST_SET_PROPERTIES1_RESERVED_MASK                                           0xFC
#define DCP_LIST_SET_PROPERTIES1_RESERVED_SHIFT                                          0x02

/* Dcp Monitor command class commands */
#define DCP_MONITOR_VERSION                                                              0x01
#define DCP_EVENT_STATUS_GET                                                             0x03
#define DCP_EVENT_STATUS_REPORT                                                          0x04
#define DCP_LIST_GET                                                                     0x01
#define DCP_LIST_REPORT                                                                  0x02
/* Values used for Dcp List Report command */
#define DCP_LIST_REPORT_PROPERTIES1_NUMBER_OF_DC_MASK                                    0x03
#define DCP_LIST_REPORT_PROPERTIES1_RESERVED_MASK                                        0xFC
#define DCP_LIST_REPORT_PROPERTIES1_RESERVED_SHIFT                                       0x02

/* Door Lock Logging command class commands */
#define DOOR_LOCK_LOGGING_VERSION                                                        0x01
#define DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_GET                                          0x01
#define DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT                                       0x02
#define RECORD_GET                                                                       0x03
#define RECORD_REPORT                                                                    0x04
/* Values used for Record Report command */
#define RECORD_REPORT_PROPERTIES1_HOUR_LOCAL_TIME_MASK                                   0x1F
#define RECORD_REPORT_PROPERTIES1_RECORD_STATUS_MASK                                     0xE0
#define RECORD_REPORT_PROPERTIES1_RECORD_STATUS_SHIFT                                    0x05

/* Door Lock command class commands */
#define DOOR_LOCK_VERSION                                                                0x01
#define DOOR_LOCK_CONFIGURATION_GET                                                      0x05
#define DOOR_LOCK_CONFIGURATION_REPORT                                                   0x06
#define DOOR_LOCK_CONFIGURATION_SET                                                      0x04
#define DOOR_LOCK_OPERATION_GET                                                          0x02
#define DOOR_LOCK_OPERATION_REPORT                                                       0x03
#define DOOR_LOCK_OPERATION_SET                                                          0x01
/* Values used for Door Lock Configuration Report command */
#define DOOR_LOCK_CONFIGURATION_REPORT_CONSTANT_OPERATION                                0x01
#define DOOR_LOCK_CONFIGURATION_REPORT_TIMED_OPERATION                                   0x02
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_INSIDE_DOOR_HANDLES_STATE_MASK        0x0F
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_MASK       0xF0
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_SHIFT      0x04
/* Values used for Door Lock Configuration Set command */
#define DOOR_LOCK_CONFIGURATION_SET_CONSTANT_OPERATION                                   0x01
#define DOOR_LOCK_CONFIGURATION_SET_TIMED_OPERATION                                      0x02
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_INSIDE_DOOR_HANDLES_STATE_MASK           0x0F
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_MASK          0xF0
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_SHIFT         0x04
/* Values used for Door Lock Operation Report command */
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED                                        0x00
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_WITH_TIMEOUT                           0x01
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES                0x10
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT   0x11
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES               0x20
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT  0x21
#define DOOR_LOCK_OPERATION_REPORT_DOOR_SECURED                                          0xFF
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_INSIDE_DOOR_HANDLES_MODE_MASK             0x0F
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_MODE_MASK            0xF0
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_MODE_SHIFT           0x04
/* Values used for Door Lock Operation Set command */
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED                                           0x00
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_WITH_TIMEOUT                              0x01
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES                   0x10
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT      0x11
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES                  0x20
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT     0x21
#define DOOR_LOCK_OPERATION_SET_DOOR_SECURED                                             0xFF

/* Energy Production command class commands */
#define ENERGY_PRODUCTION_VERSION                                                        0x01
#define ENERGY_PRODUCTION_GET                                                            0x02
#define ENERGY_PRODUCTION_REPORT                                                         0x03
/* Values used for Energy Production Get command */
#define ENERGY_PRODUCTION_GET_INSTANT_ENERGY_PRODUCTION                                  0x00
#define ENERGY_PRODUCTION_GET_TOTAL_ENERGY_PRODUCTION                                    0x01
#define ENERGY_PRODUCTION_GET_ENERGY_PRODUCTION_TODAY                                    0x02
#define ENERGY_PRODUCTION_GET_TOTAL_PRODUCTION_TIME                                      0x03
/* Values used for Energy Production Report command */
#define ENERGY_PRODUCTION_REPORT_INSTANT_ENERGY_PRODUCTION                               0x00
#define ENERGY_PRODUCTION_REPORT_TOTAL_ENERGY_PRODUCTION                                 0x01
#define ENERGY_PRODUCTION_REPORT_ENERGY_PRODUCTION_TODAY                                 0x02
#define ENERGY_PRODUCTION_REPORT_TOTAL_PRODUCTION_TIME                                   0x03
#define ENERGY_PRODUCTION_REPORT_LEVEL_SIZE_MASK                                         0x07
#define ENERGY_PRODUCTION_REPORT_LEVEL_SCALE_MASK                                        0x18
#define ENERGY_PRODUCTION_REPORT_LEVEL_SCALE_SHIFT                                       0x03
#define ENERGY_PRODUCTION_REPORT_LEVEL_PRECISION_MASK                                    0xE0
#define ENERGY_PRODUCTION_REPORT_LEVEL_PRECISION_SHIFT                                   0x05

/* Firmware Update Md command class commands */
#define FIRMWARE_UPDATE_MD_VERSION                                                       0x01
#define FIRMWARE_MD_GET                                                                  0x01
#define FIRMWARE_MD_REPORT                                                               0x02
#define FIRMWARE_UPDATE_MD_GET                                                           0x05
#define FIRMWARE_UPDATE_MD_REPORT                                                        0x06
#define FIRMWARE_UPDATE_MD_REQUEST_GET                                                   0x03
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT                                                0x04
#define FIRMWARE_UPDATE_MD_STATUS_REPORT                                                 0x07
/* Values used for Firmware Update Md Get command */
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_REPORT_NUMBER_1_MASK                          0x7F
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_ZERO_BIT_MASK                                 0x80
/* Values used for Firmware Update Md Report command */
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_REPORT_NUMBER_1_MASK                       0x7F
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_LAST_BIT_MASK                              0x80
/* Values used for Firmware Update Md Request Report command */
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_INVALID_COMBINATION                            0x00
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_REQUIRES_AUTHENTICATION                        0x01
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION                              0xFF
/* Values used for Firmware Update Md Status Report command */
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_WITHOUT_CHECKSUM_ERROR        0x00
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE                               0x01
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY                                    0xFF

/* Firmware Update Md command class commands */
#define FIRMWARE_UPDATE_MD_VERSION_V2                                                    0x02
#define FIRMWARE_MD_GET_V2                                                               0x01
#define FIRMWARE_MD_REPORT_V2                                                            0x02
#define FIRMWARE_UPDATE_MD_GET_V2                                                        0x05
#define FIRMWARE_UPDATE_MD_REPORT_V2                                                     0x06
#define FIRMWARE_UPDATE_MD_REQUEST_GET_V2                                                0x03
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_V2                                             0x04
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_V2                                              0x07
/* Values used for Firmware Update Md Get command */
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_REPORT_NUMBER_1_MASK_V2                       0x7F
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_ZERO_BIT_MASK_V2                              0x80
/* Values used for Firmware Update Md Report command */
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_REPORT_NUMBER_1_MASK_V2                    0x7F
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_LAST_BIT_MASK_V2                           0x80
/* Values used for Firmware Update Md Request Report command */
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_INVALID_COMBINATION_V2                         0x00
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_REQUIRES_AUTHENTICATION_V2                     0x01
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION_V2                           0xFF
/* Values used for Firmware Update Md Status Report command */
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_WITHOUT_CHECKSUM_ERROR_V2     0x00
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_V2                            0x01
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V2                                 0xFF

/* Geographic Location command class commands */
#define GEOGRAPHIC_LOCATION_VERSION                                                      0x01
#define GEOGRAPHIC_LOCATION_GET                                                          0x02
#define GEOGRAPHIC_LOCATION_REPORT                                                       0x03
#define GEOGRAPHIC_LOCATION_SET                                                          0x01
/* Values used for Geographic Location Report command */
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL_LONGITUDE_DEGREES_MASK                          0x7F
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL_LONG_SIGN_BIT_MASK                              0x80
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL2_LATITUDE_DEGREES_MASK                          0x7F
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL2_LAT_SIGN_BIT_MASK                              0x80
/* Values used for Geographic Location Set command */
#define GEOGRAPHIC_LOCATION_SET_LEVEL_LONGITUDE_DEGREES_MASK                             0x7F
#define GEOGRAPHIC_LOCATION_SET_LEVEL_LONG_SIGN_BIT_MASK                                 0x80
#define GEOGRAPHIC_LOCATION_SET_LEVEL2_LATITUDE_DEGREES_MASK                             0x7F
#define GEOGRAPHIC_LOCATION_SET_LEVEL2_LAT_SIGN_BIT_MASK                                 0x80

/* Grouping Name command class commands */
#define GROUPING_NAME_VERSION                                                            0x01
#define GROUPING_NAME_GET                                                                0x02
#define GROUPING_NAME_REPORT                                                             0x03
#define GROUPING_NAME_SET                                                                0x01
/* Values used for Grouping Name Report command */
#define GROUPING_NAME_REPORT_PROPERTIES1_CHAR_PRESENTATION_MASK                          0x07
#define GROUPING_NAME_REPORT_PROPERTIES1_RESERVED_MASK                                   0xF8
#define GROUPING_NAME_REPORT_PROPERTIES1_RESERVED_SHIFT                                  0x03
/* Values used for Grouping Name Set command */
#define GROUPING_NAME_SET_PROPERTIES1_CHAR_PRESENTATION_MASK                             0x07
#define GROUPING_NAME_SET_PROPERTIES1_RESERVED_MASK                                      0xF8
#define GROUPING_NAME_SET_PROPERTIES1_RESERVED_SHIFT                                     0x03

/* Hail command class commands */
#define HAIL_VERSION                                                                     0x01
#define HAIL                                                                             0x01

/* Hrv Control command class commands */
#define HRV_CONTROL_VERSION                                                              0x01
#define HRV_CONTROL_BYPASS_GET                                                           0x05
#define HRV_CONTROL_BYPASS_REPORT                                                        0x06
#define HRV_CONTROL_BYPASS_SET                                                           0x04
#define HRV_CONTROL_MODE_GET                                                             0x02
#define HRV_CONTROL_MODE_REPORT                                                          0x03
#define HRV_CONTROL_MODE_SET                                                             0x01
#define HRV_CONTROL_MODE_SUPPORTED_GET                                                   0x0A
#define HRV_CONTROL_MODE_SUPPORTED_REPORT                                                0x0B
#define HRV_CONTROL_VENTILATION_RATE_GET                                                 0x08
#define HRV_CONTROL_VENTILATION_RATE_REPORT                                              0x09
#define HRV_CONTROL_VENTILATION_RATE_SET                                                 0x07
/* Values used for Hrv Control Mode Report command */
#define HRV_CONTROL_MODE_REPORT_PROPERTIES1_MODE_MASK                                    0x1F
#define HRV_CONTROL_MODE_REPORT_MODE_OFF                                                 0x00
#define HRV_CONTROL_MODE_REPORT_MODE_DEMAND_AUTOMATIC                                    0x01
#define HRV_CONTROL_MODE_REPORT_MODE_SCHEDULE                                            0x02
#define HRV_CONTROL_MODE_REPORT_MODE_ENERGY_SAVINGS_MODE                                 0x03
#define HRV_CONTROL_MODE_REPORT_MODE_MANUAL                                              0x04
#define HRV_CONTROL_MODE_REPORT_PROPERTIES1_RESERVED_MASK                                0xE0
#define HRV_CONTROL_MODE_REPORT_PROPERTIES1_RESERVED_SHIFT                               0x05
/* Values used for Hrv Control Mode Set command */
#define HRV_CONTROL_MODE_SET_PROPERTIES1_MODE_MASK                                       0x1F
#define HRV_CONTROL_MODE_SET_MODE_OFF                                                    0x00
#define HRV_CONTROL_MODE_SET_MODE_DEMAND_AUTOMATIC                                       0x01
#define HRV_CONTROL_MODE_SET_MODE_SCHEDULE                                               0x02
#define HRV_CONTROL_MODE_SET_MODE_ENERGY_SAVINGS_MODE                                    0x03
#define HRV_CONTROL_MODE_SET_MODE_MANUAL                                                 0x04
#define HRV_CONTROL_MODE_SET_PROPERTIES1_RESERVED_MASK                                   0xE0
#define HRV_CONTROL_MODE_SET_PROPERTIES1_RESERVED_SHIFT                                  0x05
/* Values used for Hrv Control Mode Supported Report command */
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_PROPERTIES1_MANUAL_CONTROL_SUPPORTED_MASK      0x0F
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_BYPASS_OPEN_CLOSE     0x00
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_BYPASS_AUTO           0x01
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_MODULATED_BYPASS      0x02
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_VENTILATION_RATE      0x03
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK                      0xF0
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT                     0x04

/* Hrv Status command class commands */
#define HRV_STATUS_VERSION                                                               0x01
#define HRV_STATUS_GET                                                                   0x01
#define HRV_STATUS_REPORT                                                                0x02
#define HRV_STATUS_SUPPORTED_GET                                                         0x03
#define HRV_STATUS_SUPPORTED_REPORT                                                      0x04
/* Values used for Hrv Status Get command */
#define HRV_STATUS_GET_OUTDOOR_AIR_TEMPERATURE                                           0x00
#define HRV_STATUS_GET_SUPPLY_AIR_TEMPERATURE                                            0x01
#define HRV_STATUS_GET_EXHAUST_AIR_TEMPERATURE                                           0x02
#define HRV_STATUS_GET_DISCHARGE_AIR_TEMPERATURE                                         0x03
#define HRV_STATUS_GET_ROOM_TEMPERATURE                                                  0x04
#define HRV_STATUS_GET_RELATIVE_HUMIDITY_IN_ROOM                                         0x05
#define HRV_STATUS_GET_REMAINING_FILTER_LIFE                                             0x06
/* Values used for Hrv Status Report command */
#define HRV_STATUS_REPORT_OUTDOOR_AIR_TEMPERATURE                                        0x00
#define HRV_STATUS_REPORT_SUPPLY_AIR_TEMPERATURE                                         0x01
#define HRV_STATUS_REPORT_EXHAUST_AIR_TEMPERATURE                                        0x02
#define HRV_STATUS_REPORT_DISCHARGE_AIR_TEMPERATURE                                      0x03
#define HRV_STATUS_REPORT_ROOM_TEMPERATURE                                               0x04
#define HRV_STATUS_REPORT_RELATIVE_HUMIDITY_IN_ROOM                                      0x05
#define HRV_STATUS_REPORT_REMAINING_FILTER_LIFE                                          0x06
#define HRV_STATUS_REPORT_PROPERTIES1_SIZE_MASK                                          0x07
#define HRV_STATUS_REPORT_PROPERTIES1_SCALE_MASK                                         0x18
#define HRV_STATUS_REPORT_PROPERTIES1_SCALE_SHIFT                                        0x03
#define HRV_STATUS_REPORT_PROPERTIES1_PRECISION_MASK                                     0xE0
#define HRV_STATUS_REPORT_PROPERTIES1_PRECISION_SHIFT                                    0x05

/* Indicator command class commands */
#define INDICATOR_VERSION                                                                0x01
#define INDICATOR_GET                                                                    0x02
#define INDICATOR_REPORT                                                                 0x03
#define INDICATOR_SET                                                                    0x01

/* Ip Configuration command class commands */
#define IP_CONFIGURATION_VERSION                                                         0x01
#define IP_CONFIGURATION_GET                                                             0x02
#define IP_CONFIGURATION_RELEASE                                                         0x04
#define IP_CONFIGURATION_RENEW                                                           0x05
#define IP_CONFIGURATION_REPORT                                                          0x03
#define IP_CONFIGURATION_SET                                                             0x01
/* Values used for Ip Configuration Report command */
#define IP_CONFIGURATION_REPORT_PROPERTIES1_AUTO_DNS_BIT_MASK                            0x01
#define IP_CONFIGURATION_REPORT_PROPERTIES1_AUTO_IP_BIT_MASK                             0x02
#define IP_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_MASK                                0xFC
#define IP_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_SHIFT                               0x02
/* Values used for Ip Configuration Set command */
#define IP_CONFIGURATION_SET_PROPERTIES1_AUTO_DNS_BIT_MASK                               0x01
#define IP_CONFIGURATION_SET_PROPERTIES1_AUTO_IP_BIT_MASK                                0x02
#define IP_CONFIGURATION_SET_PROPERTIES1_RESERVED_MASK                                   0xFC
#define IP_CONFIGURATION_SET_PROPERTIES1_RESERVED_SHIFT                                  0x02

/* Language command class commands */
#define LANGUAGE_VERSION                                                                 0x01
#define LANGUAGE_GET                                                                     0x02
#define LANGUAGE_REPORT                                                                  0x03
#define LANGUAGE_SET                                                                     0x01

/* Lock command class commands */
#define LOCK_VERSION                                                                     0x01
#define LOCK_GET                                                                         0x02
#define LOCK_REPORT                                                                      0x03
#define LOCK_SET                                                                         0x01

/* Manufacturer Proprietary command class commands */
#define MANUFACTURER_PROPRIETARY_VERSION                                                 0x01

/* Manufacturer Specific command class commands */
#define MANUFACTURER_SPECIFIC_VERSION                                                    0x01
#define MANUFACTURER_SPECIFIC_GET                                                        0x04
#define MANUFACTURER_SPECIFIC_REPORT                                                     0x05

/* Mark command class commands */
#define MARK_VERSION                                                                     0x01

/* Meter Pulse command class commands */
#define METER_PULSE_VERSION                                                              0x01
#define METER_PULSE_GET                                                                  0x04
#define METER_PULSE_REPORT                                                               0x05

/* Meter Tbl Config command class commands */
#define METER_TBL_CONFIG_VERSION                                                         0x01
#define METER_TBL_TABLE_POINT_ADM_NO_SET                                                 0x01
/* Values used for Meter Tbl Table Point Adm No Set command */
#define METER_TBL_TABLE_POINT_ADM_NO_SET_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK           0x1F
#define METER_TBL_TABLE_POINT_ADM_NO_SET_PROPERTIES1_RESERVED_MASK                       0xE0
#define METER_TBL_TABLE_POINT_ADM_NO_SET_PROPERTIES1_RESERVED_SHIFT                      0x05

/* Meter Tbl Monitor command class commands */
#define METER_TBL_MONITOR_VERSION                                                        0x01
#define METER_TBL_STATUS_REPORT                                                          0x0B
#define METER_TBL_STATUS_DATE_GET                                                        0x0A
#define METER_TBL_STATUS_DEPTH_GET                                                       0x09
#define METER_TBL_STATUS_SUPPORTED_GET                                                   0x07
#define METER_TBL_STATUS_SUPPORTED_REPORT                                                0x08
#define METER_TBL_CURRENT_DATA_GET                                                       0x0C
#define METER_TBL_CURRENT_DATA_REPORT                                                    0x0D
#define METER_TBL_HISTORICAL_DATA_GET                                                    0x0E
#define METER_TBL_HISTORICAL_DATA_REPORT                                                 0x0F
#define METER_TBL_REPORT                                                                 0x06
#define METER_TBL_TABLE_CAPABILITY_GET                                                   0x05
#define METER_TBL_TABLE_ID_GET                                                           0x03
#define METER_TBL_TABLE_ID_REPORT                                                        0x04
#define METER_TBL_TABLE_POINT_ADM_NO_GET                                                 0x01
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT                                              0x02
/* Values used for Meter Tbl Current Data Report command */
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RATE_TYPE_MASK                         0x03
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RESERVED_MASK                          0xFC
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RESERVED_SHIFT                         0x02
/* Values used for Meter Tbl Historical Data Report command */
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RATE_TYPE_MASK                      0x03
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RESERVED_MASK                       0xFC
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RESERVED_SHIFT                      0x02
/* Values used for Meter Tbl Report command */
#define METER_TBL_REPORT_PROPERTIES1_METER_TYPE_MASK                                     0x3F
#define METER_TBL_REPORT_PROPERTIES1_RATE_TYPE_MASK                                      0xC0
#define METER_TBL_REPORT_PROPERTIES1_RATE_TYPE_SHIFT                                     0x06
#define METER_TBL_REPORT_PROPERTIES2_PAY_METER_MASK                                      0x0F
#define METER_TBL_REPORT_PAY_METER_RESERVED                                              0x00
#define METER_TBL_REPORT_PAY_METER_CREDITMETER                                           0x01
#define METER_TBL_REPORT_PAY_METER_PREPAYMENT_METER                                      0x02
#define METER_TBL_REPORT_PAY_METER_PREPAYMENT_METER_DEBT                                 0x03
#define METER_TBL_REPORT_PROPERTIES2_RESERVED_MASK                                       0xF0
#define METER_TBL_REPORT_PROPERTIES2_RESERVED_SHIFT                                      0x04
/* Values used for Meter Tbl Table Id Report command */
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK                  0x1F
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_RESERVED_MASK                              0xE0
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_RESERVED_SHIFT                             0x05
/* Values used for Meter Tbl Table Point Adm No Report command */
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK        0x1F
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_RESERVED_MASK                    0xE0
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_RESERVED_SHIFT                   0x05

/* Meter Tbl Push command class commands */
#define METER_TBL_PUSH_VERSION                                                           0x01
#define METER_TBL_PUSH_CONFIGURATION_GET                                                 0x02
#define METER_TBL_PUSH_CONFIGURATION_REPORT                                              0x03
#define METER_TBL_PUSH_CONFIGURATION_SET                                                 0x01
/* Values used for Meter Tbl Push Configuration Report command */
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_OPERATING_STATUS_PUSH_MODE_MASK  0x0F
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_PS_BIT_MASK                      0x10
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_MASK                    0xE0
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_SHIFT                   0x05
/* Values used for Meter Tbl Push Configuration Set command */
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_OPERATING_STATUS_PUSH_MODE_MASK     0x0F
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_PS_BIT_MASK                         0x10
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_RESERVED_MASK                       0xE0
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_RESERVED_SHIFT                      0x05

/* Meter command class commands */
#define METER_VERSION                                                                    0x01
#define METER_GET                                                                        0x01
#define METER_REPORT                                                                     0x02
/* Values used for Meter Report command */
#define METER_REPORT_ELECTRIC_METER                                                      0x01
#define METER_REPORT_GAS_METER                                                           0x02
#define METER_REPORT_WATER_METER                                                         0x03
#define METER_REPORT_PROPERTIES1_SIZE_MASK                                               0x07
#define METER_REPORT_PROPERTIES1_SCALE_MASK                                              0x18
#define METER_REPORT_PROPERTIES1_SCALE_SHIFT                                             0x03
#define METER_REPORT_PROPERTIES1_PRECISION_MASK                                          0xE0
#define METER_REPORT_PROPERTIES1_PRECISION_SHIFT                                         0x05

/* Meter command class commands */
#define METER_VERSION_V2                                                                 0x02
#define METER_GET_V2                                                                     0x01
#define METER_REPORT_V2                                                                  0x02
#define METER_RESET_V2                                                                   0x05
#define METER_SUPPORTED_GET_V2                                                           0x03
#define METER_SUPPORTED_REPORT_V2                                                        0x04
/* Values used for Meter Get command */
#define METER_GET_PROPERTIES1_RESERVED_MASK_V2                                           0x07
#define METER_GET_PROPERTIES1_SCALE_MASK_V2                                              0x18
#define METER_GET_PROPERTIES1_SCALE_SHIFT_V2                                             0x03
#define METER_GET_PROPERTIES1_RESERVED2_MASK_V2                                          0xE0
#define METER_GET_PROPERTIES1_RESERVED2_SHIFT_V2                                         0x05
/* Values used for Meter Report command */
#define METER_REPORT_PROPERTIES1_METER_TYPE_MASK_V2                                      0x1F
#define METER_REPORT_PROPERTIES1_RATE_TYPE_MASK_V2                                       0x60
#define METER_REPORT_PROPERTIES1_RATE_TYPE_SHIFT_V2                                      0x05
#define METER_REPORT_PROPERTIES1_RESERVED_BIT_MASK_V2                                    0x80
#define METER_REPORT_PROPERTIES2_SIZE_MASK_V2                                            0x07
#define METER_REPORT_PROPERTIES2_SCALE_MASK_V2                                           0x18
#define METER_REPORT_PROPERTIES2_SCALE_SHIFT_V2                                          0x03
#define METER_REPORT_PROPERTIES2_PRECISION_MASK_V2                                       0xE0
#define METER_REPORT_PROPERTIES2_PRECISION_SHIFT_V2                                      0x05
/* Values used for Meter Supported Report command */
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_TYPE_MASK_V2                            0x1F
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V2                              0x60
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                             0x05
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_RESET_BIT_MASK_V2                       0x80
#define METER_SUPPORTED_REPORT_PROPERTIES2_SCALE_SUPPORTED_MASK_V2                       0x0F
#define METER_SUPPORTED_REPORT_PROPERTIES2_RESERVED_MASK_V2                              0xF0
#define METER_SUPPORTED_REPORT_PROPERTIES2_RESERVED_SHIFT_V2                             0x04

/* Meter command class commands */
#define METER_VERSION_V3                                                                 0x03
#define METER_GET_V3                                                                     0x01
#define METER_REPORT_V3                                                                  0x02
#define METER_RESET_V3                                                                   0x05
#define METER_SUPPORTED_GET_V3                                                           0x03
#define METER_SUPPORTED_REPORT_V3                                                        0x04
/* Values used for Meter Get command */
#define METER_GET_PROPERTIES1_RESERVED_MASK_V3                                           0x07
#define METER_GET_PROPERTIES1_SCALE_MASK_V3                                              0x38
#define METER_GET_PROPERTIES1_SCALE_SHIFT_V3                                             0x03
#define METER_GET_PROPERTIES1_RESERVED2_MASK_V3                                          0xC0
#define METER_GET_PROPERTIES1_RESERVED2_SHIFT_V3                                         0x06
/* Values used for Meter Report command */
#define METER_REPORT_PROPERTIES1_METER_TYPE_MASK_V3                                      0x1F
#define METER_REPORT_PROPERTIES1_RATE_TYPE_MASK_V3                                       0x60
#define METER_REPORT_PROPERTIES1_RATE_TYPE_SHIFT_V3                                      0x05
#define METER_REPORT_PROPERTIES1_SCALE2_BIT_MASK_V3                                      0x80
#define METER_REPORT_PROPERTIES2_SIZE_MASK_V3                                            0x07
#define METER_REPORT_PROPERTIES2_SCALE_MASK_V3                                           0x18
#define METER_REPORT_PROPERTIES2_SCALE_SHIFT_V3                                          0x03
#define METER_REPORT_PROPERTIES2_PRECISION_MASK_V3                                       0xE0
#define METER_REPORT_PROPERTIES2_PRECISION_SHIFT_V3                                      0x05
/* Values used for Meter Supported Report command */
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_TYPE_MASK_V3                            0x1F
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V3                              0x60
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                             0x05
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_RESET_BIT_MASK_V3                       0x80

/* Mtp Window Covering command class commands */
#define MTP_WINDOW_COVERING_VERSION                                                      0x01
#define MOVE_TO_POSITION_GET                                                             0x02
#define MOVE_TO_POSITION_REPORT                                                          0x03
#define MOVE_TO_POSITION_SET                                                             0x01

/* Multi Channel Association command class commands */
#define MULTI_CHANNEL_ASSOCIATION_VERSION_V2                                             0x02
#define MULTI_CHANNEL_ASSOCIATION_GET_V2                                                 0x02
#define MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V2                                       0x05
#define MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2                                    0x06
#define MULTI_CHANNEL_ASSOCIATION_REMOVE_V2                                              0x04
#define MULTI_CHANNEL_ASSOCIATION_REPORT_V2                                              0x03
#define MULTI_CHANNEL_ASSOCIATION_SET_V2                                                 0x01
/* Values used for Multi Channel Association Remove command */
#define MULTI_CHANNEL_ASSOCIATION_REMOVE_MARKER_V2                                       0x00
/* Values used for Multi Channel Association Report command */
#define MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V2                                       0x00
/* Values used for Multi Channel Association Set command */
#define MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2                                          0x00

/* Multi Channel command class commands */
#define MULTI_CHANNEL_VERSION_V2                                                         0x02
#define MULTI_CHANNEL_CAPABILITY_GET_V2                                                  0x09
#define MULTI_CHANNEL_CAPABILITY_REPORT_V2                                               0x0A
#define MULTI_CHANNEL_CMD_ENCAP_V2                                                       0x0D
#define MULTI_CHANNEL_END_POINT_FIND_V2                                                  0x0B
#define MULTI_CHANNEL_END_POINT_FIND_REPORT_V2                                           0x0C
#define MULTI_CHANNEL_END_POINT_GET_V2                                                   0x07
#define MULTI_CHANNEL_END_POINT_REPORT_V2                                                0x08
#define MULTI_INSTANCE_CMD_ENCAP_V2                                                      0x06
#define MULTI_INSTANCE_GET_V2                                                            0x04
#define MULTI_INSTANCE_REPORT_V2                                                         0x05
/* Values used for Multi Channel Capability Get command */
#define MULTI_CHANNEL_CAPABILITY_GET_PROPERTIES1_END_POINT_MASK_V2                       0x7F
#define MULTI_CHANNEL_CAPABILITY_GET_PROPERTIES1_RES_BIT_MASK_V2                         0x80
/* Values used for Multi Channel Capability Report command */
#define MULTI_CHANNEL_CAPABILITY_REPORT_PROPERTIES1_END_POINT_MASK_V2                    0x7F
#define MULTI_CHANNEL_CAPABILITY_REPORT_PROPERTIES1_DYNAMIC_BIT_MASK_V2                  0x80
/* Values used for Multi Channel Cmd Encap command */
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES1_SOURCE_END_POINT_MASK_V2                     0x7F
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES1_RES_BIT_MASK_V2                              0x80
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES2_DESTINATION_END_POINT_MASK_V2                0x7F
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES2_BIT_ADDRESS_BIT_MASK_V2                      0x80
/* Values used for Multi Channel End Point Report command */
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_RES_MASK_V2                           0x3F
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_IDENTICAL_BIT_MASK_V2                 0x40
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_DYNAMIC_BIT_MASK_V2                   0x80
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES2_END_POINTS_MASK_V2                    0x7F
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES2_RES_BIT_MASK_V2                       0x80
/* Values used for Multi Instance Cmd Encap command */
#define MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_INSTANCE_MASK_V2                            0x7F
#define MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_RES_BIT_MASK_V2                             0x80
/* Values used for Multi Instance Report command */
#define MULTI_INSTANCE_REPORT_PROPERTIES1_INSTANCES_MASK_V2                              0x7F
#define MULTI_INSTANCE_REPORT_PROPERTIES1_RES_BIT_MASK_V2                                0x80

/* Multi Cmd command class commands */
#define MULTI_CMD_VERSION                                                                0x01
#define MULTI_CMD_ENCAP                                                                  0x01

/* Multi Instance Association command class commands */
#define MULTI_INSTANCE_ASSOCIATION_VERSION                                               0x01
#define MULTI_INSTANCE_ASSOCIATION_GET                                                   0x02
#define MULTI_INSTANCE_ASSOCIATION_GROUPINGS_GET                                         0x05
#define MULTI_INSTANCE_ASSOCIATION_GROUPINGS_REPORT                                      0x06
#define MULTI_INSTANCE_ASSOCIATION_REMOVE                                                0x04
#define MULTI_INSTANCE_ASSOCIATION_REPORT                                                0x03
#define MULTI_INSTANCE_ASSOCIATION_SET                                                   0x01
/* Values used for Multi Instance Association Remove command */
#define MULTI_INSTANCE_ASSOCIATION_REMOVE_MARKER                                         0x00
/* Values used for Multi Instance Association Report command */
#define MULTI_INSTANCE_ASSOCIATION_REPORT_MARKER                                         0x00
/* Values used for Multi Instance Association Set command */
#define MULTI_INSTANCE_ASSOCIATION_SET_MARKER                                            0x00

/* Multi Instance command class commands */
#define MULTI_INSTANCE_VERSION                                                           0x01
#define MULTI_INSTANCE_CMD_ENCAP                                                         0x06
#define MULTI_INSTANCE_GET                                                               0x04
#define MULTI_INSTANCE_REPORT                                                            0x05

/* Network Management Info command class commands */
#define NETWORK_MANAGEMENT_INFO_VERSION                                                  0x01
#define NODE_INFO_GET                                                                    0x03
#define NODE_INFO_REPORT                                                                 0x04
/* Values used for Node Info Report command */
#define NODE_INFO_REPORT_PROPERTIES1_CAPABILITY_MASK                                     0x7F
#define NODE_INFO_REPORT_PROPERTIES1_LISTENING_BIT_MASK                                  0x80
#define NODE_INFO_REPORT_PROPERTIES2_SECURITY_MASK                                       0x7F
#define NODE_INFO_REPORT_PROPERTIES2_OPT_BIT_MASK                                        0x80

/* Network Management List command class commands */
#define NETWORK_MANAGEMENT_LIST_VERSION                                                  0x01
#define NODE_INFO_REDUCED_GET                                                            0x03
#define NODE_INFO_REDUCED_REPORT                                                         0x04
#define NODE_LIST_GET                                                                    0x01
#define NODE_LIST_REPORT                                                                 0x02
/* Values used for Node Info Reduced Report command */
#define NODE_INFO_REDUCED_REPORT_PROPERTIES1_CAPABILITY_MASK                             0x7F
#define NODE_INFO_REDUCED_REPORT_PROPERTIES1_LISTENING_BIT_MASK                          0x80
#define NODE_INFO_REDUCED_REPORT_PROPERTIES2_SECURITY_MASK                               0x7F
#define NODE_INFO_REDUCED_REPORT_PROPERTIES2_OPT_BIT_MASK                                0x80
/* Values used for Node List Report command */
#define NODE_LIST_REPORT_OK                                                              0x00
#define NODE_LIST_REPORT_GET_NODE_LIST                                                   0x01
#define NODE_LIST_REPORT_UPDATE_PENDING                                                  0x02

/* Network Management Node command class commands */
#define NETWORK_MANAGEMENT_NODE_VERSION                                                  0x01
#define LEARN_MODE_SET                                                                   0x01
#define LEARN_MODE_STATUS                                                                0x02

/* Network Management Pri Ctrl command class commands */
#define NETWORK_MANAGEMENT_PRI_CTRL_VERSION                                              0x01
#define DEFAULT_SET                                                                      0x05
#define DEFAULT_SET_COMPLETE                                                             0x06
#define NODE_ADD                                                                         0x01
#define NODE_ADD_STATUS                                                                  0x02
#define NODE_REMOVE                                                                      0x03
#define NODE_REMOVE_STATUS                                                               0x04

/* No Operation command class commands */
#define NO_OPERATION_VERSION                                                             0x01

/* Node Naming command class commands */
#define NODE_NAMING_VERSION                                                              0x01
#define NODE_NAMING_NODE_LOCATION_REPORT                                                 0x06
#define NODE_NAMING_NODE_LOCATION_SET                                                    0x04
#define NODE_NAMING_NODE_LOCATION_GET                                                    0x05
#define NODE_NAMING_NODE_NAME_GET                                                        0x02
#define NODE_NAMING_NODE_NAME_REPORT                                                     0x03
#define NODE_NAMING_NODE_NAME_SET                                                        0x01
/* Values used for Node Naming Node Location Report command */
#define NODE_NAMING_NODE_LOCATION_REPORT_LEVEL_CHAR_PRESENTATION_MASK                    0x07
#define NODE_NAMING_NODE_LOCATION_REPORT_LEVEL_RESERVED_MASK                             0xF8
#define NODE_NAMING_NODE_LOCATION_REPORT_LEVEL_RESERVED_SHIFT                            0x03
/* Values used for Node Naming Node Location Set command */
#define NODE_NAMING_NODE_LOCATION_SET_LEVEL_CHAR_PRESENTATION_MASK                       0x07
#define NODE_NAMING_NODE_LOCATION_SET_LEVEL_RESERVED_MASK                                0xF8
#define NODE_NAMING_NODE_LOCATION_SET_LEVEL_RESERVED_SHIFT                               0x03
/* Values used for Node Naming Node Name Report command */
#define NODE_NAMING_NODE_NAME_REPORT_LEVEL_CHAR_PRESENTATION_MASK                        0x07
#define NODE_NAMING_NODE_NAME_REPORT_LEVEL_RESERVED_MASK                                 0xF8
#define NODE_NAMING_NODE_NAME_REPORT_LEVEL_RESERVED_SHIFT                                0x03
/* Values used for Node Naming Node Name Set command */
#define NODE_NAMING_NODE_NAME_SET_LEVEL_CHAR_PRESENTATION_MASK                           0x07
#define NODE_NAMING_NODE_NAME_SET_LEVEL_RESERVED_MASK                                    0xF8
#define NODE_NAMING_NODE_NAME_SET_LEVEL_RESERVED_SHIFT                                   0x03

/* Non Interoperable command class commands */
#define NON_INTEROPERABLE_VERSION                                                        0x01

/* Powerlevel command class commands */
#define POWERLEVEL_VERSION                                                               0x01
#define POWERLEVEL_GET                                                                   0x02
#define POWERLEVEL_REPORT                                                                0x03
#define POWERLEVEL_SET                                                                   0x01
#define POWERLEVEL_TEST_NODE_GET                                                         0x05
#define POWERLEVEL_TEST_NODE_REPORT                                                      0x06
#define POWERLEVEL_TEST_NODE_SET                                                         0x04
/* Values used for Powerlevel Report command */
#define POWERLEVEL_REPORT_NORMALPOWER                                                    0x00
#define POWERLEVEL_REPORT_MINUS1DBM                                                      0x01
#define POWERLEVEL_REPORT_MINUS2DBM                                                      0x02
#define POWERLEVEL_REPORT_MINUS3DBM                                                      0x03
#define POWERLEVEL_REPORT_MINUS4DBM                                                      0x04
#define POWERLEVEL_REPORT_MINUS5DBM                                                      0x05
#define POWERLEVEL_REPORT_MINUS6DBM                                                      0x06
#define POWERLEVEL_REPORT_MINUS7DBM                                                      0x07
#define POWERLEVEL_REPORT_MINUS8DBM                                                      0x08
#define POWERLEVEL_REPORT_MINUS9DBM                                                      0x09
/* Values used for Powerlevel Set command */
#define POWERLEVEL_SET_NORMALPOWER                                                       0x00
#define POWERLEVEL_SET_MINUS1DBM                                                         0x01
#define POWERLEVEL_SET_MINUS2DBM                                                         0x02
#define POWERLEVEL_SET_MINUS3DBM                                                         0x03
#define POWERLEVEL_SET_MINUS4DBM                                                         0x04
#define POWERLEVEL_SET_MINUS5DBM                                                         0x05
#define POWERLEVEL_SET_MINUS6DBM                                                         0x06
#define POWERLEVEL_SET_MINUS7DBM                                                         0x07
#define POWERLEVEL_SET_MINUS8DBM                                                         0x08
#define POWERLEVEL_SET_MINUS9DBM                                                         0x09
/* Values used for Powerlevel Test Node Report command */
#define POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_FAILED                                       0x00
#define POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_SUCCES                                       0x01
#define POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_INPROGRESS                                   0x02
/* Values used for Powerlevel Test Node Set command */
#define POWERLEVEL_TEST_NODE_SET_NORMALPOWER                                             0x00
#define POWERLEVEL_TEST_NODE_SET_MINUS1DBM                                               0x01
#define POWERLEVEL_TEST_NODE_SET_MINUS2DBM                                               0x02
#define POWERLEVEL_TEST_NODE_SET_MINUS3DBM                                               0x03
#define POWERLEVEL_TEST_NODE_SET_MINUS4DBM                                               0x04
#define POWERLEVEL_TEST_NODE_SET_MINUS5DBM                                               0x05
#define POWERLEVEL_TEST_NODE_SET_MINUS6DBM                                               0x06
#define POWERLEVEL_TEST_NODE_SET_MINUS7DBM                                               0x07
#define POWERLEVEL_TEST_NODE_SET_MINUS8DBM                                               0x08
#define POWERLEVEL_TEST_NODE_SET_MINUS9DBM                                               0x09

/* Prepayment Encapsulation command class commands */
#define PREPAYMENT_ENCAPSULATION_VERSION                                                 0x01
#define CMD_ENCAPSULATION                                                                0x01

/* Prepayment command class commands */
#define PREPAYMENT_VERSION                                                               0x01
#define PREPAYMENT_BALANCE_GET                                                           0x01
#define PREPAYMENT_BALANCE_REPORT                                                        0x02
#define PREPAYMENT_SUPPORTED_GET                                                         0x03
#define PREPAYMENT_SUPPORTED_REPORT                                                      0x04
/* Values used for Prepayment Balance Get command */
#define PREPAYMENT_BALANCE_GET_PROPERTIES1_RESERVED_MASK                                 0x3F
#define PREPAYMENT_BALANCE_GET_PROPERTIES1_BALANCE_TYPE_MASK                             0xC0
#define PREPAYMENT_BALANCE_GET_PROPERTIES1_BALANCE_TYPE_SHIFT                            0x06
#define PREPAYMENT_BALANCE_GET_BALANCE_TYPE_UTILITY                                      0x00
#define PREPAYMENT_BALANCE_GET_BALANCE_TYPE_MONETARY                                     0x01
/* Values used for Prepayment Balance Report command */
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES1_METER_TYPE_MASK                            0x3F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES1_BALANCE_TYPE_MASK                          0xC0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES1_BALANCE_TYPE_SHIFT                         0x06
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES2_SCALE_MASK                                 0x1F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES2_BALANCE_PRECISION_MASK                     0xE0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES2_BALANCE_PRECISION_SHIFT                    0x05
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES3_RESERVED_MASK                              0x1F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES3_DEBT_PRECISION_MASK                        0xE0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES3_DEBT_PRECISION_SHIFT                       0x05
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES4_RESERVED_MASK                              0x1F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES4_EMER_CREDIT_PRECISION_MASK                 0xE0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES4_EMER_CREDIT_PRECISION_SHIFT                0x05
/* Values used for Prepayment Supported Report command */
#define PREPAYMENT_SUPPORTED_REPORT_PROPERTIES1_TYPES_SUPPORTED_MASK                     0x0F
#define PREPAYMENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK                            0xF0
#define PREPAYMENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT                           0x04

/* Proprietary command class commands */
#define PROPRIETARY_VERSION                                                              0x01
#define PROPRIETARY_GET                                                                  0x02
#define PROPRIETARY_REPORT                                                               0x03
#define PROPRIETARY_SET                                                                  0x01

/* Protection command class commands */
#define PROTECTION_VERSION                                                               0x01
#define PROTECTION_GET                                                                   0x02
#define PROTECTION_REPORT                                                                0x03
#define PROTECTION_SET                                                                   0x01
/* Values used for Protection Report command */
#define PROTECTION_REPORT_UNPROTECTED                                                    0x00
#define PROTECTION_REPORT_PROTECTION_BY_SEQUENCE                                         0x01
#define PROTECTION_REPORT_NO_OPERATION_POSSIBLE                                          0x02
/* Values used for Protection Set command */
#define PROTECTION_SET_UNPROTECTED                                                       0x00
#define PROTECTION_SET_PROTECTION_BY_SEQUENCE                                            0x01
#define PROTECTION_SET_NO_OPERATION_POSSIBLE                                             0x02

/* Protection command class commands */
#define PROTECTION_VERSION_V2                                                            0x02
#define PROTECTION_EC_GET_V2                                                             0x07
#define PROTECTION_EC_REPORT_V2                                                          0x08
#define PROTECTION_EC_SET_V2                                                             0x06
#define PROTECTION_GET_V2                                                                0x02
#define PROTECTION_REPORT_V2                                                             0x03
#define PROTECTION_SET_V2                                                                0x01
#define PROTECTION_SUPPORTED_GET_V2                                                      0x04
#define PROTECTION_SUPPORTED_REPORT_V2                                                   0x05
#define PROTECTION_TIMEOUT_GET_V2                                                        0x0A
#define PROTECTION_TIMEOUT_REPORT_V2                                                     0x0B
#define PROTECTION_TIMEOUT_SET_V2                                                        0x09
/* Values used for Protection Report command */
#define PROTECTION_REPORT_LEVEL_LOCAL_PROTECTION_STATE_MASK_V2                           0x0F
#define PROTECTION_REPORT_LEVEL_RESERVED_MASK_V2                                         0xF0
#define PROTECTION_REPORT_LEVEL_RESERVED_SHIFT_V2                                        0x04
#define PROTECTION_REPORT_LEVEL2_RF_PROTECTION_STATE_MASK_V2                             0x0F
#define PROTECTION_REPORT_LEVEL2_RESERVED_MASK_V2                                        0xF0
#define PROTECTION_REPORT_LEVEL2_RESERVED_SHIFT_V2                                       0x04
/* Values used for Protection Set command */
#define PROTECTION_SET_LEVEL_LOCAL_PROTECTION_STATE_MASK_V2                              0x0F
#define PROTECTION_SET_LEVEL_RESERVED_MASK_V2                                            0xF0
#define PROTECTION_SET_LEVEL_RESERVED_SHIFT_V2                                           0x04
#define PROTECTION_SET_LEVEL2_RF_PROTECTION_STATE_MASK_V2                                0x0F
#define PROTECTION_SET_LEVEL2_RESERVED_MASK_V2                                           0xF0
#define PROTECTION_SET_LEVEL2_RESERVED_SHIFT_V2                                          0x04
/* Values used for Protection Supported Report command */
#define PROTECTION_SUPPORTED_REPORT_LEVEL_TIMEOUT_BIT_MASK_V2                            0x01
#define PROTECTION_SUPPORTED_REPORT_LEVEL_EXCLUSIVE_CONTROL_BIT_MASK_V2                  0x02
#define PROTECTION_SUPPORTED_REPORT_LEVEL_RESERVED_MASK_V2                               0xFC
#define PROTECTION_SUPPORTED_REPORT_LEVEL_RESERVED_SHIFT_V2                              0x02

/* Rate Tbl Config command class commands */
#define RATE_TBL_CONFIG_VERSION                                                          0x01
#define RATE_TBL_REMOVE                                                                  0x02
#define RATE_TBL_SET                                                                     0x01
/* Values used for Rate Tbl Remove command */
#define RATE_TBL_REMOVE_PROPERTIES1_RATE_PARAMETER_SET_IDS_MASK                          0x3F
#define RATE_TBL_REMOVE_PROPERTIES1_RESERVED_MASK                                        0xC0
#define RATE_TBL_REMOVE_PROPERTIES1_RESERVED_SHIFT                                       0x06
/* Values used for Rate Tbl Set command */
#define RATE_TBL_SET_PROPERTIES1_NUMBER_OF_RATE_CHAR_MASK                                0x1F
#define RATE_TBL_SET_PROPERTIES1_RATE_TYPE_MASK                                          0x60
#define RATE_TBL_SET_PROPERTIES1_RATE_TYPE_SHIFT                                         0x05
#define RATE_TBL_SET_PROPERTIES1_RESERVED_BIT_MASK                                       0x80
#define RATE_TBL_SET_PROPERTIES2_CONSUMPTION_SCALE_MASK                                  0x1F
#define RATE_TBL_SET_PROPERTIES2_CONSUMPTION_PRECISION_MASK                              0xE0
#define RATE_TBL_SET_PROPERTIES2_CONSUMPTION_PRECISION_SHIFT                             0x05
#define RATE_TBL_SET_PROPERTIES3_MAX_DEMAND_SCALE_MASK                                   0x1F
#define RATE_TBL_SET_PROPERTIES3_MAX_DEMAND_PRECISION_MASK                               0xE0
#define RATE_TBL_SET_PROPERTIES3_MAX_DEMAND_PRECISION_SHIFT                              0x05

/* Rate Tbl Monitor command class commands */
#define RATE_TBL_MONITOR_VERSION                                                         0x01
#define RATE_TBL_ACTIVE_RATE_GET                                                         0x05
#define RATE_TBL_ACTIVE_RATE_REPORT                                                      0x06
#define RATE_TBL_CURRENT_DATA_GET                                                        0x07
#define RATE_TBL_CURRENT_DATA_REPORT                                                     0x08
#define RATE_TBL_GET                                                                     0x03
#define RATE_TBL_HISTORICAL_DATA_GET                                                     0x09
#define RATE_TBL_HISTORICAL_DATA_REPORT                                                  0x0A
#define RATE_TBL_REPORT                                                                  0x04
#define RATE_TBL_SUPPORTED_GET                                                           0x01
#define RATE_TBL_SUPPORTED_REPORT                                                        0x02
/* Values used for Rate Tbl Report command */
#define RATE_TBL_REPORT_PROPERTIES1_NUMBER_OF_RATE_CHAR_MASK                             0x1F
#define RATE_TBL_REPORT_PROPERTIES1_RATE_TYPE_MASK                                       0x60
#define RATE_TBL_REPORT_PROPERTIES1_RATE_TYPE_SHIFT                                      0x05
#define RATE_TBL_REPORT_PROPERTIES1_RESERVED_BIT_MASK                                    0x80
#define RATE_TBL_REPORT_PROPERTIES2_CONSUMPTION_SCALE_MASK                               0x1F
#define RATE_TBL_REPORT_PROPERTIES2_CONSUMPTION_PRECISION_MASK                           0xE0
#define RATE_TBL_REPORT_PROPERTIES2_CONSUMPTION_PRECISION_SHIFT                          0x05
#define RATE_TBL_REPORT_PROPERTIES3_MAX_DEMAND_SCALE_MASK                                0x1F
#define RATE_TBL_REPORT_PROPERTIES3_MAX_DEMAND_PRECISION_MASK                            0xE0
#define RATE_TBL_REPORT_PROPERTIES3_MAX_DEMAND_PRECISION_SHIFT                           0x05

/* Remote Association Activate command class commands */
#define REMOTE_ASSOCIATION_ACTIVATE_VERSION                                              0x01
#define REMOTE_ASSOCIATION_ACTIVATE                                                      0x01

/* Remote Association command class commands */
#define REMOTE_ASSOCIATION_VERSION                                                       0x01
#define REMOTE_ASSOCIATION_CONFIGURATION_GET                                             0x02
#define REMOTE_ASSOCIATION_CONFIGURATION_REPORT                                          0x03
#define REMOTE_ASSOCIATION_CONFIGURATION_SET                                             0x01

/* Scene Activation command class commands */
#define SCENE_ACTIVATION_VERSION                                                         0x01
#define SCENE_ACTIVATION_SET                                                             0x01

/* Scene Actuator Conf command class commands */
#define SCENE_ACTUATOR_CONF_VERSION                                                      0x01
#define SCENE_ACTUATOR_CONF_GET                                                          0x02
#define SCENE_ACTUATOR_CONF_REPORT                                                       0x03
#define SCENE_ACTUATOR_CONF_SET                                                          0x01
/* Values used for Scene Actuator Conf Set command */
#define SCENE_ACTUATOR_CONF_SET_LEVEL2_RESERVED_MASK                                     0x7F
#define SCENE_ACTUATOR_CONF_SET_LEVEL2_OVERRIDE_BIT_MASK                                 0x80

/* Scene Controller Conf command class commands */
#define SCENE_CONTROLLER_CONF_VERSION                                                    0x01
#define SCENE_CONTROLLER_CONF_GET                                                        0x02
#define SCENE_CONTROLLER_CONF_REPORT                                                     0x03
#define SCENE_CONTROLLER_CONF_SET                                                        0x01

/* Schedule Entry Lock command class commands */
#define SCHEDULE_ENTRY_LOCK_VERSION                                                      0x01
#define SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET                                               0x02
#define SCHEDULE_ENTRY_LOCK_ENABLE_SET                                                   0x01
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET                                                 0x04
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT                                              0x05
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET                                                 0x03
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET                                                 0x07
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT                                              0x08
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET                                                 0x06
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_GET                                                0x09
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT                                             0x0A

/* Schedule Entry Lock command class commands */
#define SCHEDULE_ENTRY_LOCK_VERSION_V2                                                   0x02
#define SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_V2                                            0x02
#define SCHEDULE_ENTRY_LOCK_ENABLE_SET_V2                                                0x01
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V2                                           0x0B
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2                                        0x0C
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V2                                           0x0D
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_V2                                              0x04
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_V2                                           0x05
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_V2                                              0x03
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_V2                                              0x07
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_V2                                           0x08
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_V2                                              0x06
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_V2                                             0x09
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_V2                                          0x0A
/* Values used for Schedule Entry Lock Time Offset Report command */
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL_HOUR_TZO_MASK_V2                    0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL_SIGN_TZO_BIT_MASK_V2                0x80
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL2_MINUTE_OFFSET_DST_MASK_V2          0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2        0x80
/* Values used for Schedule Entry Lock Time Offset Set command */
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL_HOUR_TZO_MASK_V2                       0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL_SIGN_TZO_BIT_MASK_V2                   0x80
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL2_MINUTE_OFFSET_DST_MASK_V2             0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2           0x80

/* Screen Attributes command class commands */
#define SCREEN_ATTRIBUTES_VERSION                                                        0x01
#define SCREEN_ATTRIBUTES_GET                                                            0x01
#define SCREEN_ATTRIBUTES_REPORT                                                         0x02
/* Values used for Screen Attributes Report command */
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_NUMBER_OF_LINES_MASK                        0x1F
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_MASK                               0xE0
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_SHIFT                              0x05

/* Screen Attributes command class commands */
#define SCREEN_ATTRIBUTES_VERSION_V2                                                     0x02
#define SCREEN_ATTRIBUTES_GET_V2                                                         0x01
#define SCREEN_ATTRIBUTES_REPORT_V2                                                      0x03
/* Values used for Screen Attributes Report command */
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_NUMBER_OF_LINES_MASK_V2                     0x1F
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_ESCAPE_SEQUENCE_BIT_MASK_V2                 0x20
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_MASK_V2                            0xC0
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                           0x06

/* Screen Md command class commands */
#define SCREEN_MD_VERSION                                                                0x01
#define SCREEN_MD_GET                                                                    0x01
#define SCREEN_MD_REPORT                                                                 0x02
/* Values used for Screen Md Report command */
#define SCREEN_MD_REPORT_PROPERTIES1_CHAR_PRESENTATION_MASK                              0x07
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_MASK                                0x38
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_SHIFT                               0x03
#define SCREEN_MD_REPORT_PROPERTIES1_RESERVED_BIT_MASK                                   0x40
#define SCREEN_MD_REPORT_PROPERTIES1_MORE_DATA_BIT_MASK                                  0x80

/* Screen Md command class commands */
#define SCREEN_MD_VERSION_V2                                                             0x02
#define SCREEN_MD_GET_V2                                                                 0x01
#define SCREEN_MD_REPORT_V2                                                              0x03
/* Values used for Screen Md Report command */
#define SCREEN_MD_REPORT_PROPERTIES1_CHAR_PRESENTATION_MASK_V2                           0x07
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_MASK_V2                             0x38
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_SHIFT_V2                            0x03
#define SCREEN_MD_REPORT_PROPERTIES1_RESERVED_BIT_MASK_V2                                0x40
#define SCREEN_MD_REPORT_PROPERTIES1_MORE_DATA_BIT_MASK_V2                               0x80
#define SCREEN_MD_REPORT_PROPERTIES2_SCREEN_TIMEOUT_BIT_MASK_V2                          0x01
#define SCREEN_MD_REPORT_PROPERTIES2_RESERVED_MASK_V2                                    0xFE
#define SCREEN_MD_REPORT_PROPERTIES2_RESERVED_SHIFT_V2                                   0x01

/* Security Panel Mode command class commands */
#define SECURITY_PANEL_MODE_VERSION                                                      0x01
#define SECURITY_PANEL_MODE_GET                                                          0x03
#define SECURITY_PANEL_MODE_REPORT                                                       0x04
#define SECURITY_PANEL_MODE_SET                                                          0x05
#define SECURITY_PANEL_MODE_SUPPORTED_GET                                                0x01
#define SECURITY_PANEL_MODE_SUPPORTED_REPORT                                             0x02

/* Security Panel Zone Sensor command class commands */
#define SECURITY_PANEL_ZONE_SENSOR_VERSION                                               0x01
#define COMMAND_CLASS_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_REPORT                        0x02
#define SECURITY_PANEL__ZONE_SENSOR_TYPE_GET                                             0x03
#define SECURITY_PANEL__ZONE_SENSOR_TYPE_REPORT                                          0x04
#define SECURITY_PANEL_ZONE_SENSOR_INSTALLED_GET                                         0x01
#define SECURITY_PANEL_ZONE_SENSOR_STATE_GET                                             0x05
#define SECURITY_PANEL_ZONE_SENSOR_STATE_REPORT                                          0x06

/* Security Panel Zone command class commands */
#define SECURITY_PANEL_ZONE_VERSION                                                      0x01
#define SECURITY_PANEL_ZONE_NUMBER_SUPPORTED_GET                                         0x01
#define SECURITY_PANEL_ZONE_STATE_GET                                                    0x05
#define SECURITY_PANEL_ZONE_STATE_REPORT                                                 0x06
#define SECURITY_PANEL_ZONE_SUPPORTED_REPORT                                             0x02
#define SECURITY_PANEL_ZONE_TYPE_GET                                                     0x03
#define SECURITY_PANEL_ZONE_TYPE_REPORT                                                  0x04
/* Values used for Security Panel Zone Supported Report command */
#define SECURITY_PANEL_ZONE_SUPPORTED_REPORT_PARAMETERS1_ZONES_SUPPORTED_MASK            0x7F
#define SECURITY_PANEL_ZONE_SUPPORTED_REPORT_PARAMETERS1_ZM_BIT_MASK                     0x80

/* Security command class commands */
#define SECURITY_VERSION                                                                 0x01
#define NETWORK_KEY_SET                                                                  0x06
#define NETWORK_KEY_VERIFY                                                               0x07
#define SECURITY_COMMANDS_SUPPORTED_GET                                                  0x02
#define SECURITY_COMMANDS_SUPPORTED_REPORT                                               0x03
#define SECURITY_MESSAGE_ENCAPSULATION                                                   0x81
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET                                         0xC1
#define SECURITY_NONCE_GET                                                               0x40
#define SECURITY_NONCE_REPORT                                                            0x80
#define SECURITY_SCHEME_GET                                                              0x04
#define SECURITY_SCHEME_INHERIT                                                          0x08
#define SECURITY_SCHEME_REPORT                                                           0x05
/* Values used for Security Commands Supported Report command */
#define SECURITY_COMMANDS_SUPPORTED_REPORT_COMMAND_CLASS_MARK                            0xEF
/* Values used for Security Message Encapsulation command */
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_SEQUENCE_COUNTER_MASK                 0x0F
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_SEQUENCED_BIT_MASK                    0x10
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_SECOND_FRAME_BIT_MASK                 0x20
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_RESERVED_MASK                         0xC0
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_RESERVED_SHIFT                        0x06
/* Values used for Security Message Encapsulation Nonce Get command */
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_SEQUENCE_COUNTER_MASK       0x0F
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_SEQUENCED_BIT_MASK          0x10
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_SECOND_FRAME_BIT_MASK       0x20
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_RESERVED_MASK               0xC0
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_RESERVED_SHIFT              0x06

/* Sensor Alarm command class commands */
#define SENSOR_ALARM_VERSION                                                             0x01
#define SENSOR_ALARM_GET                                                                 0x01
#define SENSOR_ALARM_REPORT                                                              0x02
#define SENSOR_ALARM_SUPPORTED_GET                                                       0x03
#define SENSOR_ALARM_SUPPORTED_REPORT                                                    0x04

/* Sensor Binary command class commands */
#define SENSOR_BINARY_VERSION                                                            0x01
#define SENSOR_BINARY_GET                                                                0x02
#define SENSOR_BINARY_REPORT                                                             0x03
/* Values used for Sensor Binary Report command */
#define SENSOR_BINARY_REPORT_IDLE                                                        0x00
#define SENSOR_BINARY_REPORT_DETECTED_AN_EVENT                                           0xFF

/* Sensor Configuration command class commands */
#define SENSOR_CONFIGURATION_VERSION                                                     0x01
#define SENSOR_TRIGGER_LEVEL_GET                                                         0x02
#define SENSOR_TRIGGER_LEVEL_REPORT                                                      0x03
#define SENSOR_TRIGGER_LEVEL_SET                                                         0x01
/* Values used for Sensor Trigger Level Report command */
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_SIZE_MASK                                0x07
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_SCALE_MASK                               0x18
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_SCALE_SHIFT                              0x03
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_PRECISION_MASK                           0xE0
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_PRECISION_SHIFT                          0x05
/* Values used for Sensor Trigger Level Set command */
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES1_RESERVED_MASK                               0x3F
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES1_CURRENT_BIT_MASK                            0x40
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES1_DEFAULT_BIT_MASK                            0x80
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_SIZE_MASK                                   0x07
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_SCALE_MASK                                  0x18
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_SCALE_SHIFT                                 0x03
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_PRECISION_MASK                              0xE0
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_PRECISION_SHIFT                             0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION                                                        0x01
#define SENSOR_MULTILEVEL_GET                                                            0x04
#define SENSOR_MULTILEVEL_REPORT                                                         0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1                                   0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1                         0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1                                     0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK                                         0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK                                        0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT                                       0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK                                    0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT                                   0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION_V2                                                     0x02
#define SENSOR_MULTILEVEL_GET_V2                                                         0x04
#define SENSOR_MULTILEVEL_REPORT_V2                                                      0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1_V2                                0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V2                      0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1_V2                                  0x03
#define SENSOR_MULTILEVEL_REPORT_POWER_VERSION_2_V2                                      0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY_VERSION_2_V2                          0x05
#define SENSOR_MULTILEVEL_REPORT_VELOCITY_VERSION_2_V2                                   0x06
#define SENSOR_MULTILEVEL_REPORT_DIRECTION_VERSION_2_V2                                  0x07
#define SENSOR_MULTILEVEL_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V2                       0x08
#define SENSOR_MULTILEVEL_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V2                        0x09
#define SENSOR_MULTILEVEL_REPORT_SOLAR_RADIATION_VERSION_2_V2                            0x0A
#define SENSOR_MULTILEVEL_REPORT_DEW_POINT_VERSION_2_V2                                  0x0B
#define SENSOR_MULTILEVEL_REPORT_RAIN_RATE_VERSION_2_V2                                  0x0C
#define SENSOR_MULTILEVEL_REPORT_TIDE_LEVEL_VERSION_2_V2                                 0x0D
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK_V2                                      0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK_V2                                     0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT_V2                                    0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK_V2                                 0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT_V2                                0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION_V3                                                     0x03
#define SENSOR_MULTILEVEL_GET_V3                                                         0x04
#define SENSOR_MULTILEVEL_REPORT_V3                                                      0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1_V3                                0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V3                      0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1_V3                                  0x03
#define SENSOR_MULTILEVEL_REPORT_POWER_VERSION_2_V3                                      0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY_VERSION_2_V3                          0x05
#define SENSOR_MULTILEVEL_REPORT_VELOCITY_VERSION_2_V3                                   0x06
#define SENSOR_MULTILEVEL_REPORT_DIRECTION_VERSION_2_V3                                  0x07
#define SENSOR_MULTILEVEL_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V3                       0x08
#define SENSOR_MULTILEVEL_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V3                        0x09
#define SENSOR_MULTILEVEL_REPORT_SOLAR_RADIATION_VERSION_2_V3                            0x0A
#define SENSOR_MULTILEVEL_REPORT_DEW_POINT_VERSION_2_V3                                  0x0B
#define SENSOR_MULTILEVEL_REPORT_RAIN_RATE_VERSION_2_V3                                  0x0C
#define SENSOR_MULTILEVEL_REPORT_TIDE_LEVEL_VERSION_2_V3                                 0x0D
#define SENSOR_MULTILEVEL_REPORT_WEIGHT_VERSION_3_V3                                     0x0E
#define SENSOR_MULTILEVEL_REPORT_VOLTAGE_VERSION_3_V3                                    0x0F
#define SENSOR_MULTILEVEL_REPORT_CURRENT_VERSION_3_V3                                    0x10
#define SENSOR_MULTILEVEL_REPORT_CO2_LEVEL_VERSION_3_V3                                  0x11
#define SENSOR_MULTILEVEL_REPORT_AIR_FLOW_VERSION_3_V3                                   0x12
#define SENSOR_MULTILEVEL_REPORT_TANK_CAPACITY_VERSION_3_V3                              0x13
#define SENSOR_MULTILEVEL_REPORT_DISTANCE_VERSION_3_V3                                   0x14
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK_V3                                      0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK_V3                                     0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT_V3                                    0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK_V3                                 0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT_V3                                0x05

/* Silence Alarm command class commands */
#define SILENCE_ALARM_VERSION                                                            0x01
#define SENSOR_ALARM_SET                                                                 0x01

/* Simple Av Control command class commands */
#define SIMPLE_AV_CONTROL_VERSION                                                        0x01
#define SIMPLE_AV_CONTROL_GET                                                            0x02
#define SIMPLE_AV_CONTROL_REPORT                                                         0x03
#define SIMPLE_AV_CONTROL_SET                                                            0x01
#define SIMPLE_AV_CONTROL_SUPPORTED_GET                                                  0x04
#define SIMPLE_AV_CONTROL_SUPPORTED_REPORT                                               0x05
/* Values used for Simple Av Control Set command */
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_KEY_ATTRIBUTES_MASK                            0x07
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_RESERVED_MASK                                  0xF8
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_RESERVED_SHIFT                                 0x03

/* Switch All command class commands */
#define SWITCH_ALL_VERSION                                                               0x01
#define SWITCH_ALL_GET                                                                   0x02
#define SWITCH_ALL_OFF                                                                   0x05
#define SWITCH_ALL_ON                                                                    0x04
#define SWITCH_ALL_REPORT                                                                0x03
#define SWITCH_ALL_SET                                                                   0x01
/* Values used for Switch All Report command */
#define SWITCH_ALL_REPORT_EXCLUDED_FROM_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                 0x00
#define SWITCH_ALL_REPORT_EXCLUDED_FROM_THE_ALL_ON_FUNCTIONALITY_BUT_NOT_ALL_OFF         0x01
#define SWITCH_ALL_REPORT_EXCLUDED_FROM_THE_ALL_OFF_FUNCTIONALITY_BUT_NOT_ALL_ON         0x02
#define SWITCH_ALL_REPORT_INCLUDED_IN_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                   0xFF
/* Values used for Switch All Set command */
#define SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                    0x00
#define SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_ON_FUNCTIONALITY_BUT_NOT_ALL_OFF            0x01
#define SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_OFF_FUNCTIONALITY_BUT_NOT_ALL_ON            0x02
#define SWITCH_ALL_SET_INCLUDED_IN_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                      0xFF

/* Switch Binary command class commands */
#define SWITCH_BINARY_VERSION                                                            0x01
#define SWITCH_BINARY_GET                                                                0x02
#define SWITCH_BINARY_REPORT                                                             0x03
#define SWITCH_BINARY_SET                                                                0x01

/* Switch Multilevel command class commands */
#define SWITCH_MULTILEVEL_VERSION                                                        0x01
#define SWITCH_MULTILEVEL_GET                                                            0x02
#define SWITCH_MULTILEVEL_REPORT                                                         0x03
#define SWITCH_MULTILEVEL_SET                                                            0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE                                             0x04
#define SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE                                              0x05
/* Values used for Switch Multilevel Start Level Change command */
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED_MASK                         0x1F
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_IGNORE_START_LEVEL_BIT_MASK           0x20
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_UP_DOWN_BIT_MASK                      0x40
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED_BIT_MASK                     0x80

/* Switch Multilevel command class commands */
#define SWITCH_MULTILEVEL_VERSION_V2                                                     0x02
#define SWITCH_MULTILEVEL_GET_V2                                                         0x02
#define SWITCH_MULTILEVEL_REPORT_V2                                                      0x03
#define SWITCH_MULTILEVEL_SET_V2                                                         0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V2                                          0x04
#define SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V2                                           0x05
/* Values used for Switch Multilevel Start Level Change command */
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_RESERVED_MASK_V2                0x1F
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_IGNORE_START_LEVEL_BIT_MASK_V2  0x20
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_UP_DOWN_BIT_MASK_V2             0x40
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_RESERVED_BIT_MASK_V2            0x80

/* Switch Multilevel command class commands */
#define SWITCH_MULTILEVEL_VERSION_V3                                                     0x03
#define SWITCH_MULTILEVEL_GET_V3                                                         0x02
#define SWITCH_MULTILEVEL_REPORT_V3                                                      0x03
#define SWITCH_MULTILEVEL_SET_V3                                                         0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V3                                          0x04
#define SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V3                                           0x05
#define SWITCH_MULTILEVEL_SUPPORTED_GET_V3                                               0x06
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3                                            0x07
/* Values used for Switch Multilevel Start Level Change command */
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_RESERVED_MASK_V3                0x07
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_INC_DEC_MASK_V3                 0x18
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_INC_DEC_SHIFT_V3                0x03
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_INCREMENT_V3                        0x00
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_DECREMENT_V3                        0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_RESERVED_V3                         0x02
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_NONE_V3                             0x03
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_IGNORE_START_LEVEL_BIT_MASK_V3  0x20
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_UP_DOWN_MASK_V3                 0xC0
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_UP_DOWN_SHIFT_V3                0x06
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_UP_V3                               0x00
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_DOWN_V3                             0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_RESERVED_V3                         0x02
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_NONE_V3                             0x03
/* Values used for Switch Multilevel Supported Report command */
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES1_PRIMARY_SWITCH_TYPE_MASK_V3       0x1F
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V3                  0xE0
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                 0x05
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES2_SECONDARY_SWITCH_TYPE_MASK_V3     0x1F
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES2_RESERVED_MASK_V3                  0xE0
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES2_RESERVED_SHIFT_V3                 0x05

/* Switch Toggle Binary command class commands */
#define SWITCH_TOGGLE_BINARY_VERSION                                                     0x01
#define SWITCH_TOGGLE_BINARY_SET                                                         0x01 /*SWITCH_TOGGLE_BINARY_SET*/
#define SWITCH_TOGGLE_BINARY_GET                                                         0x02
#define SWITCH_TOGGLE_BINARY_REPORT                                                      0x03

/* Switch Toggle Multilevel command class commands */
#define SWITCH_TOGGLE_MULTILEVEL_VERSION                                                 0x01
#define SWITCH_TOGGLE_MULTILEVEL_SET                                                     0x01 /*SWITCH_TOGGLE_MULTILEVEL_SET*/
#define SWITCH_TOGGLE_MULTILEVEL_GET                                                     0x02
#define SWITCH_TOGGLE_MULTILEVEL_REPORT                                                  0x03
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE                                      0x04
#define SWITCH_TOGGLE_MULTILEVEL_STOP_LEVEL_CHANGE                                       0x05
/* Values used for Switch Toggle Multilevel Start Level Change command */
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED_MASK                  0x1F
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_IGNORE_START_LEVEL_BIT_MASK    0x20
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED_BIT_MASK              0x40
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_ROLL_OVER_BIT_MASK             0x80

/* Tariff Config command class commands */
#define TARIFF_CONFIG_VERSION                                                            0x01
#define TARIFF_TBL_REMOVE                                                                0x03
#define TARIFF_TBL_SET                                                                   0x02
#define TARIFF_TBL_SUPPLIER_SET                                                          0x01
/* Values used for Tariff Tbl Remove command */
#define TARIFF_TBL_REMOVE_PROPERTIES1_RATE_PARAMETER_SET_IDS_MASK                        0x3F
#define TARIFF_TBL_REMOVE_PROPERTIES1_RESERVED_MASK                                      0xC0
#define TARIFF_TBL_REMOVE_PROPERTIES1_RESERVED_SHIFT                                     0x06
/* Values used for Tariff Tbl Set command */
#define TARIFF_TBL_SET_PROPERTIES1_RESERVED_MASK                                         0x1F
#define TARIFF_TBL_SET_PROPERTIES1_TARIFF_PRECISION_MASK                                 0xE0
#define TARIFF_TBL_SET_PROPERTIES1_TARIFF_PRECISION_SHIFT                                0x05
/* Values used for Tariff Tbl Supplier Set command */
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES1_STANDING_CHARGE_PERIOD_MASK                  0x1F
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES1_STANDING_CHARGE_PRECISION_MASK               0xE0
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES1_STANDING_CHARGE_PRECISION_SHIFT              0x05
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES2_NUMBER_OF_SUPPLIER_CHARACTERS_MASK           0x1F
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES2_RESERVED_MASK                                0xE0
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES2_RESERVED_SHIFT                               0x05

/* Tariff Tbl Monitor command class commands */
#define TARIFF_TBL_MONITOR_VERSION                                                       0x01
#define TARIFF_TBL_COST_GET                                                              0x05
#define TARIFF_TBL_COST_REPORT                                                           0x06
#define TARIFF_TBL_GET                                                                   0x03
#define TARIFF_TBL_REPORT                                                                0x04
#define TARIFF_TBL_SUPPLIER_GET                                                          0x01
#define TARIFF_TBL_SUPPLIER_REPORT                                                       0x02
/* Values used for Tariff Tbl Cost Report command */
#define TARIFF_TBL_COST_REPORT_PROPERTIES1_RATE_TYPE_MASK                                0x03
#define TARIFF_TBL_COST_REPORT_PROPERTIES1_RESERVED_MASK                                 0xFC
#define TARIFF_TBL_COST_REPORT_PROPERTIES1_RESERVED_SHIFT                                0x02
#define TARIFF_TBL_COST_REPORT_PROPERTIES2_RESERVED_MASK                                 0x1F
#define TARIFF_TBL_COST_REPORT_PROPERTIES2_COST_PRECISION_MASK                           0xE0
#define TARIFF_TBL_COST_REPORT_PROPERTIES2_COST_PRECISION_SHIFT                          0x05
/* Values used for Tariff Tbl Report command */
#define TARIFF_TBL_REPORT_PROPERTIES1_RESERVED_MASK                                      0x1F
#define TARIFF_TBL_REPORT_PROPERTIES1_TARIFF_PRECISION_MASK                              0xE0
#define TARIFF_TBL_REPORT_PROPERTIES1_TARIFF_PRECISION_SHIFT                             0x05
/* Values used for Tariff Tbl Supplier Report command */
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES1_STANDING_CHARGE_PERIOD_MASK               0x1F
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES1_STANDING_CHARGE_PRECISION_MASK            0xE0
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES1_STANDING_CHARGE_PRECISION_SHIFT           0x05
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES2_NUMBER_OF_SUPPLIER_CHARACTERS_MASK        0x1F
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES2_RESERVED_MASK                             0xE0
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES2_RESERVED_SHIFT                            0x05

/* Thermostat Fan Mode command class commands */
#define THERMOSTAT_FAN_MODE_VERSION                                                      0x01
#define THERMOSTAT_FAN_MODE_GET                                                          0x02
#define THERMOSTAT_FAN_MODE_REPORT                                                       0x03
#define THERMOSTAT_FAN_MODE_SET                                                          0x01
#define THERMOSTAT_FAN_MODE_SUPPORTED_GET                                                0x04
#define THERMOSTAT_FAN_MODE_SUPPORTED_REPORT                                             0x05
/* Values used for Thermostat Fan Mode Report command */
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_FAN_MODE_MASK                                   0x0F
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_RESERVED_MASK                                   0xF0
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_RESERVED_SHIFT                                  0x04
/* Values used for Thermostat Fan Mode Set command */
#define THERMOSTAT_FAN_MODE_SET_LEVEL_FAN_MODE_MASK                                      0x0F
#define THERMOSTAT_FAN_MODE_SET_LEVEL_RESERVED_MASK                                      0xF0
#define THERMOSTAT_FAN_MODE_SET_LEVEL_RESERVED_SHIFT                                     0x04

/* Thermostat Fan State command class commands */
#define THERMOSTAT_FAN_STATE_VERSION                                                     0x01
#define THERMOSTAT_FAN_STATE_GET                                                         0x02
#define THERMOSTAT_FAN_STATE_REPORT                                                      0x03
/* Values used for Thermostat Fan State Report command */
#define THERMOSTAT_FAN_STATE_REPORT_LEVEL_FAN_OPERATING_STATE_MASK                       0x0F
#define THERMOSTAT_FAN_STATE_REPORT_LEVEL_RESERVED_MASK                                  0xF0
#define THERMOSTAT_FAN_STATE_REPORT_LEVEL_RESERVED_SHIFT                                 0x04

/* Thermostat Heating command class commands */
#define THERMOSTAT_HEATING_VERSION                                                       0x01
#define THERMOSTAT_HEATING_STATUS_REPORT                                                 0x0D
#define THERMOSTAT_HEATING_MODE_GET                                                      0x02
#define THERMOSTAT_HEATING_MODE_REPORT                                                   0x03
#define THERMOSTAT_HEATING_MODE_SET                                                      0x01
#define THERMOSTAT_HEATING_RELAY_STATUS_GET                                              0x09
#define THERMOSTAT_HEATING_RELAY_STATUS_REPORT                                           0x0A
#define THERMOSTAT_HEATING_SETPOINT_GET                                                  0x05
#define THERMOSTAT_HEATING_SETPOINT_REPORT                                               0x06
#define THERMOSTAT_HEATING_SETPOINT_SET                                                  0x04
#define THERMOSTAT_HEATING_STATUS_GET                                                    0x0C
#define THERMOSTAT_HEATING_STATUS_SET                                                    0x0B
#define THERMOSTAT_HEATING_TIMED_OFF_SET                                                 0x11
/* Values used for Thermostat Heating Status Report command */
#define THERMOSTAT_HEATING_STATUS_REPORT_HEATING                                         0x00
#define THERMOSTAT_HEATING_STATUS_REPORT_COOLING                                         0x01
/* Values used for Thermostat Heating Mode Report command */
#define THERMOSTAT_HEATING_MODE_REPORT_OFF                                               0x00
#define THERMOSTAT_HEATING_MODE_REPORT_OFF_TIMED                                         0x01
#define THERMOSTAT_HEATING_MODE_REPORT_OFF_3_HOURS                                       0x02
#define THERMOSTAT_HEATING_MODE_REPORT_ANTI_FREEZE                                       0x03
#define THERMOSTAT_HEATING_MODE_REPORT_MANUAL                                            0x04
#define THERMOSTAT_HEATING_MODE_REPORT_TEMPORARY_MANUAL                                  0x05
#define THERMOSTAT_HEATING_MODE_REPORT_AUTOMATIC                                         0x06
#define THERMOSTAT_HEATING_MODE_REPORT_MANUAL_TIMED                                      0x07
/* Values used for Thermostat Heating Mode Set command */
#define THERMOSTAT_HEATING_MODE_SET_OFF                                                  0x00
#define THERMOSTAT_HEATING_MODE_SET_OFF_TIMED                                            0x01
#define THERMOSTAT_HEATING_MODE_SET_OFF_3_HOURS                                          0x02
#define THERMOSTAT_HEATING_MODE_SET_ANTI_FREEZE                                          0x03
#define THERMOSTAT_HEATING_MODE_SET_MANUAL                                               0x04
#define THERMOSTAT_HEATING_MODE_SET_TEMPORARY_MANUAL                                     0x05
#define THERMOSTAT_HEATING_MODE_SET_AUTOMATIC                                            0x06
#define THERMOSTAT_HEATING_MODE_SET_MANUAL_TIMED                                         0x07
/* Values used for Thermostat Heating Relay Status Report command */
#define THERMOSTAT_HEATING_RELAY_STATUS_REPORT_OFF                                       0x00
#define THERMOSTAT_HEATING_RELAY_STATUS_REPORT_ON                                        0x01
/* Values used for Thermostat Heating Setpoint Report command */
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_SIZE_MASK                         0x07
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_SCALE_MASK                        0x18
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_SCALE_SHIFT                       0x03
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_PRECISION_MASK                    0xE0
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_PRECISION_SHIFT                   0x05
/* Values used for Thermostat Heating Setpoint Set command */
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_SIZE_MASK                            0x07
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_SCALE_MASK                           0x18
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_SCALE_SHIFT                          0x03
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_PRECISION_MASK                       0xE0
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_PRECISION_SHIFT                      0x05
/* Values used for Thermostat Heating Status Set command */
#define THERMOSTAT_HEATING_STATUS_SET_HEATING                                            0x00
#define THERMOSTAT_HEATING_STATUS_SET_COOLING                                            0x01

/* Thermostat Mode command class commands */
#define THERMOSTAT_MODE_VERSION                                                          0x01
#define THERMOSTAT_MODE_GET                                                              0x02
#define THERMOSTAT_MODE_REPORT                                                           0x03
#define THERMOSTAT_MODE_SET                                                              0x01
#define THERMOSTAT_MODE_SUPPORTED_GET                                                    0x04
#define THERMOSTAT_MODE_SUPPORTED_REPORT                                                 0x05
/* Values used for Thermostat Mode Report command */
#define THERMOSTAT_MODE_REPORT_LEVEL_MODE_MASK                                           0x1F
#define THERMOSTAT_MODE_REPORT_MODE_OFF                                                  0x00
#define THERMOSTAT_MODE_REPORT_MODE_HEAT                                                 0x01
#define THERMOSTAT_MODE_REPORT_MODE_COOL                                                 0x02
#define THERMOSTAT_MODE_REPORT_MODE_AUTO                                                 0x03
#define THERMOSTAT_MODE_REPORT_MODE_AUXILIARY_HEAT                                       0x04
#define THERMOSTAT_MODE_REPORT_MODE_RESUME                                               0x05
#define THERMOSTAT_MODE_REPORT_MODE_FAN_ONLY                                             0x06
#define THERMOSTAT_MODE_REPORT_MODE_FURNACE                                              0x07
#define THERMOSTAT_MODE_REPORT_MODE_DRY_AIR                                              0x08
#define THERMOSTAT_MODE_REPORT_MODE_MOIST_AIR                                            0x09
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_CHANGEOVER                                      0x0A
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_MASK                                       0xE0
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_SHIFT                                      0x05
/* Values used for Thermostat Mode Set command */
#define THERMOSTAT_MODE_SET_LEVEL_MODE_MASK                                              0x1F
#define THERMOSTAT_MODE_SET_MODE_OFF                                                     0x00
#define THERMOSTAT_MODE_SET_MODE_HEAT                                                    0x01
#define THERMOSTAT_MODE_SET_MODE_COOL                                                    0x02
#define THERMOSTAT_MODE_SET_MODE_AUTO                                                    0x03
#define THERMOSTAT_MODE_SET_MODE_AUXILIARY_HEAT                                          0x04
#define THERMOSTAT_MODE_SET_MODE_RESUME                                                  0x05
#define THERMOSTAT_MODE_SET_MODE_FAN_ONLY                                                0x06
#define THERMOSTAT_MODE_SET_MODE_FURNACE                                                 0x07
#define THERMOSTAT_MODE_SET_MODE_DRY_AIR                                                 0x08
#define THERMOSTAT_MODE_SET_MODE_MOIST_AIR                                               0x09
#define THERMOSTAT_MODE_SET_MODE_AUTO_CHANGEOVER                                         0x0A
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_MASK                                          0xE0
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_SHIFT                                         0x05

/* Thermostat Mode command class commands */
#define THERMOSTAT_MODE_VERSION_V2                                                       0x02
#define THERMOSTAT_MODE_GET_V2                                                           0x02
#define THERMOSTAT_MODE_REPORT_V2                                                        0x03
#define THERMOSTAT_MODE_SET_V2                                                           0x01
#define THERMOSTAT_MODE_SUPPORTED_GET_V2                                                 0x04
#define THERMOSTAT_MODE_SUPPORTED_REPORT_V2                                              0x05
/* Values used for Thermostat Mode Report command */
#define THERMOSTAT_MODE_REPORT_LEVEL_MODE_MASK_V2                                        0x1F
#define THERMOSTAT_MODE_REPORT_MODE_OFF_V2                                               0x00
#define THERMOSTAT_MODE_REPORT_MODE_HEAT_V2                                              0x01
#define THERMOSTAT_MODE_REPORT_MODE_COOL_V2                                              0x02
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_V2                                              0x03
#define THERMOSTAT_MODE_REPORT_MODE_AUXILIARY_HEAT_V2                                    0x04
#define THERMOSTAT_MODE_REPORT_MODE_RESUME_V2                                            0x05
#define THERMOSTAT_MODE_REPORT_MODE_FAN_ONLY_V2                                          0x06
#define THERMOSTAT_MODE_REPORT_MODE_FURNACE_V2                                           0x07
#define THERMOSTAT_MODE_REPORT_MODE_DRY_AIR_V2                                           0x08
#define THERMOSTAT_MODE_REPORT_MODE_MOIST_AIR_V2                                         0x09
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_CHANGEOVER_V2                                   0x0A
#define THERMOSTAT_MODE_REPORT_MODE_ENERGY_SAVE_HEAT_V2                                  0x0B
#define THERMOSTAT_MODE_REPORT_MODE_ENERGY_SAVE_COOL_V2                                  0x0C
#define THERMOSTAT_MODE_REPORT_MODE_AWAY_V2                                              0x0D
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_MASK_V2                                    0xE0
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_SHIFT_V2                                   0x05
/* Values used for Thermostat Mode Set command */
#define THERMOSTAT_MODE_SET_LEVEL_MODE_MASK_V2                                           0x1F
#define THERMOSTAT_MODE_SET_MODE_OFF_V2                                                  0x00
#define THERMOSTAT_MODE_SET_MODE_HEAT_V2                                                 0x01
#define THERMOSTAT_MODE_SET_MODE_COOL_V2                                                 0x02
#define THERMOSTAT_MODE_SET_MODE_AUTO_V2                                                 0x03
#define THERMOSTAT_MODE_SET_MODE_AUXILIARY_HEAT_V2                                       0x04
#define THERMOSTAT_MODE_SET_MODE_RESUME_V2                                               0x05
#define THERMOSTAT_MODE_SET_MODE_FAN_ONLY_V2                                             0x06
#define THERMOSTAT_MODE_SET_MODE_FURNACE_V2                                              0x07
#define THERMOSTAT_MODE_SET_MODE_DRY_AIR_V2                                              0x08
#define THERMOSTAT_MODE_SET_MODE_MOIST_AIR_V2                                            0x09
#define THERMOSTAT_MODE_SET_MODE_AUTO_CHANGEOVER_V2                                      0x0A
#define THERMOSTAT_MODE_SET_MODE_ENERGY_SAVE_HEAT_V2                                     0x0B
#define THERMOSTAT_MODE_SET_MODE_ENERGY_SAVE_COOL_V2                                     0x0C
#define THERMOSTAT_MODE_SET_MODE_AWAY_V2                                                 0x0D
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_MASK_V2                                       0xE0
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_SHIFT_V2                                      0x05

/* Thermostat Operating State command class commands */
#define THERMOSTAT_OPERATING_STATE_VERSION                                               0x01
#define THERMOSTAT_OPERATING_STATE_GET                                                   0x02
#define THERMOSTAT_OPERATING_STATE_REPORT                                                0x03
/* Values used for Thermostat Operating State Report command */
#define THERMOSTAT_OPERATING_STATE_REPORT_LEVEL_OPERATING_STATE_MASK                     0x0F
#define THERMOSTAT_OPERATING_STATE_REPORT_LEVEL_RESERVED_MASK                            0xF0
#define THERMOSTAT_OPERATING_STATE_REPORT_LEVEL_RESERVED_SHIFT                           0x04

/* Thermostat Setback command class commands */
#define THERMOSTAT_SETBACK_VERSION                                                       0x01
#define THERMOSTAT_SETBACK_GET                                                           0x02
#define THERMOSTAT_SETBACK_REPORT                                                        0x03
#define THERMOSTAT_SETBACK_SET                                                           0x01
/* Values used for Thermostat Setback Report command */
#define THERMOSTAT_SETBACK_REPORT_PROPERTIES1_SETBACK_TYPE_MASK                          0x03
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_NO_OVERRIDE                               0x00
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_TEMPORARY_OVERRIDE                        0x01
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_PERMANENT_OVERRIDE                        0x02
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_RESERVED                                  0x03
#define THERMOSTAT_SETBACK_REPORT_PROPERTIES1_RESERVED_MASK                              0xFC
#define THERMOSTAT_SETBACK_REPORT_PROPERTIES1_RESERVED_SHIFT                             0x02
/* Values used for Thermostat Setback Set command */
#define THERMOSTAT_SETBACK_SET_PROPERTIES1_SETBACK_TYPE_MASK                             0x03
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_NO_OVERRIDE                                  0x00
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_TEMPORARY_OVERRIDE                           0x01
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_PERMANENT_OVERRIDE                           0x02
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_RESERVED                                     0x03
#define THERMOSTAT_SETBACK_SET_PROPERTIES1_RESERVED_MASK                                 0xFC
#define THERMOSTAT_SETBACK_SET_PROPERTIES1_RESERVED_SHIFT                                0x02

/* Thermostat Setpoint command class commands */
#define THERMOSTAT_SETPOINT_VERSION                                                      0x01
#define THERMOSTAT_SETPOINT_GET                                                          0x02
#define THERMOSTAT_SETPOINT_REPORT                                                       0x03
#define THERMOSTAT_SETPOINT_SET                                                          0x01
#define THERMOSTAT_SETPOINT_SUPPORTED_GET                                                0x04
#define THERMOSTAT_SETPOINT_SUPPORTED_REPORT                                             0x05
/* Values used for Thermostat Setpoint Get command */
#define THERMOSTAT_SETPOINT_GET_LEVEL_SETPOINT_TYPE_MASK                                 0x0F
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED                              0x00
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_HEATING_1                                  0x01
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_COOLING_1                                  0x02
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_FURNACE                                    0x07
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_DRY_AIR                                    0x08
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_MOIST_AIR                                  0x09
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AUTO_CHANGEOVER                            0x0A
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_MASK                                      0xF0
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_SHIFT                                     0x04
/* Values used for Thermostat Setpoint Report command */
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_SETPOINT_TYPE_MASK                              0x0F
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED                           0x00
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_HEATING_1                               0x01
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_COOLING_1                               0x02
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_FURNACE                                 0x07
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_DRY_AIR                                 0x08
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_MOIST_AIR                               0x09
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AUTO_CHANGEOVER                         0x0A
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_MASK                                   0xF0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_SHIFT                                  0x04
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SIZE_MASK                                      0x07
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_MASK                                     0x18
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_SHIFT                                    0x03
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_MASK                                 0xE0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_SHIFT                                0x05
/* Values used for Thermostat Setpoint Set command */
#define THERMOSTAT_SETPOINT_SET_LEVEL_SETPOINT_TYPE_MASK                                 0x0F
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED                              0x00
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_HEATING_1                                  0x01
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_COOLING_1                                  0x02
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_FURNACE                                    0x07
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_DRY_AIR                                    0x08
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_MOIST_AIR                                  0x09
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AUTO_CHANGEOVER                            0x0A
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_MASK                                      0xF0
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_SHIFT                                     0x04
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SIZE_MASK                                         0x07
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_MASK                                        0x18
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_SHIFT                                       0x03
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_MASK                                    0xE0
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_SHIFT                                   0x05

/* Thermostat Setpoint command class commands */
#define THERMOSTAT_SETPOINT_VERSION_V2                                                   0x02
#define THERMOSTAT_SETPOINT_GET_V2                                                       0x02
#define THERMOSTAT_SETPOINT_REPORT_V2                                                    0x03
#define THERMOSTAT_SETPOINT_SET_V2                                                       0x01
#define THERMOSTAT_SETPOINT_SUPPORTED_GET_V2                                             0x04
#define THERMOSTAT_SETPOINT_SUPPORTED_REPORT_V2                                          0x05
/* Values used for Thermostat Setpoint Get command */
#define THERMOSTAT_SETPOINT_GET_LEVEL_SETPOINT_TYPE_MASK_V2                              0x0F
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED_V2                           0x00
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_HEATING_1_V2                               0x01
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_COOLING_1_V2                               0x02
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_FURNACE_V2                                 0x07
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_DRY_AIR_V2                                 0x08
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_MOIST_AIR_V2                               0x09
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AUTO_CHANGEOVER_V2                         0x0A
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V2                     0x0B
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V2                     0x0C
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AWAY_HEATING_V2                            0x0D
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_MASK_V2                                   0xF0
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_SHIFT_V2                                  0x04
/* Values used for Thermostat Setpoint Report command */
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_SETPOINT_TYPE_MASK_V2                           0x0F
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED_V2                        0x00
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_HEATING_1_V2                            0x01
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_COOLING_1_V2                            0x02
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_FURNACE_V2                              0x07
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_DRY_AIR_V2                              0x08
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_MOIST_AIR_V2                            0x09
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AUTO_CHANGEOVER_V2                      0x0A
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V2                  0x0B
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V2                  0x0C
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AWAY_HEATING_V2                         0x0D
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_MASK_V2                                0xF0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_SHIFT_V2                               0x04
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SIZE_MASK_V2                                   0x07
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_MASK_V2                                  0x18
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_SHIFT_V2                                 0x03
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_MASK_V2                              0xE0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_SHIFT_V2                             0x05
/* Values used for Thermostat Setpoint Set command */
#define THERMOSTAT_SETPOINT_SET_LEVEL_SETPOINT_TYPE_MASK_V2                              0x0F
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED_V2                           0x00
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_HEATING_1_V2                               0x01
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_COOLING_1_V2                               0x02
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_FURNACE_V2                                 0x07
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_DRY_AIR_V2                                 0x08
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_MOIST_AIR_V2                               0x09
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AUTO_CHANGEOVER_V2                         0x0A
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V2                     0x0B
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V2                     0x0C
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AWAY_HEATING_V2                            0x0D
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_MASK_V2                                   0xF0
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_SHIFT_V2                                  0x04
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SIZE_MASK_V2                                      0x07
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_MASK_V2                                     0x18
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_SHIFT_V2                                    0x03
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_MASK_V2                                 0xE0
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_SHIFT_V2                                0x05

/* Time Parameters command class commands */
#define TIME_PARAMETERS_VERSION                                                          0x01
#define TIME_PARAMETERS_GET                                                              0x02
#define TIME_PARAMETERS_REPORT                                                           0x03
#define TIME_PARAMETERS_SET                                                              0x01

/* Time command class commands */
#define TIME_VERSION                                                                     0x01
#define DATE_GET                                                                         0x03
#define DATE_REPORT                                                                      0x04
#define TIME_GET                                                                         0x01
#define TIME_REPORT                                                                      0x02
/* Values used for Time Report command */
#define TIME_REPORT_HOUR_LOCAL_TIME_HOUR_LOCAL_TIME_MASK                                 0x1F
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_MASK                                        0x60
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_SHIFT                                       0x05
#define TIME_REPORT_HOUR_LOCAL_TIME_RTC_FAILURE_BIT_MASK                                 0x80

/* Time command class commands */
#define TIME_VERSION_V2                                                                  0x02
#define DATE_GET_V2                                                                      0x03
#define DATE_REPORT_V2                                                                   0x04
#define TIME_GET_V2                                                                      0x01
#define TIME_OFFSET_GET_V2                                                               0x06
#define TIME_OFFSET_REPORT_V2                                                            0x07
#define TIME_OFFSET_SET_V2                                                               0x05
#define TIME_REPORT_V2                                                                   0x02
/* Values used for Time Offset Report command */
#define TIME_OFFSET_REPORT_LEVEL_HOUR_TZO_MASK_V2                                        0x7F
#define TIME_OFFSET_REPORT_LEVEL_SIGN_TZO_BIT_MASK_V2                                    0x80
#define TIME_OFFSET_REPORT_LEVEL2_MINUTE_OFFSET_DST_MASK_V2                              0x7F
#define TIME_OFFSET_REPORT_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2                            0x80
/* Values used for Time Offset Set command */
#define TIME_OFFSET_SET_LEVEL_HOUR_TZO_MASK_V2                                           0x7F
#define TIME_OFFSET_SET_LEVEL_SIGN_TZO_BIT_MASK_V2                                       0x80
#define TIME_OFFSET_SET_LEVEL2_MINUTE_OFFSET_DST_MASK_V2                                 0x7F
#define TIME_OFFSET_SET_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2                               0x80
/* Values used for Time Report command */
#define TIME_REPORT_HOUR_LOCAL_TIME_HOUR_LOCAL_TIME_MASK_V2                              0x1F
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_MASK_V2                                     0x60
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_SHIFT_V2                                    0x05
#define TIME_REPORT_HOUR_LOCAL_TIME_RTC_FAILURE_BIT_MASK_V2                              0x80

/* User Code command class commands */
#define USER_CODE_VERSION                                                                0x01
#define USER_CODE_GET                                                                    0x02
#define USER_CODE_REPORT                                                                 0x03
#define USER_CODE_SET                                                                    0x01
#define USERS_NUMBER_GET                                                                 0x04
#define USERS_NUMBER_REPORT                                                              0x05
/* Values used for User Code Report command */
#define USER_CODE_REPORT_AVAILABLE_NOT_SET                                               0x00
#define USER_CODE_REPORT_OCCUPIED                                                        0x01
#define USER_CODE_REPORT_RESERVED_BY_ADMINISTRATOR                                       0x02
#define USER_CODE_REPORT_STATUS_NOT_AVAILABLE                                            0xFF
/* Values used for User Code Set command */
#define USER_CODE_SET_AVAILABLE_NOT_SET                                                  0x00
#define USER_CODE_SET_OCCUPIED                                                           0x01
#define USER_CODE_SET_RESERVED_BY_ADMINISTRATOR                                          0x02
#define USER_CODE_SET_STATUS_NOT_AVAILABLE                                               0xFF

/* Version command class commands */
#define VERSION_VERSION                                                                  0x01
#define VERSION_COMMAND_CLASS_GET                                                        0x13
#define VERSION_COMMAND_CLASS_REPORT                                                     0x14
#define VERSION_GET                                                                      0x11
#define VERSION_REPORT                                                                   0x12

/* Wake Up command class commands */
#define WAKE_UP_VERSION                                                                  0x01
#define WAKE_UP_INTERVAL_GET                                                             0x05
#define WAKE_UP_INTERVAL_REPORT                                                          0x06
#define WAKE_UP_INTERVAL_SET                                                             0x04
#define WAKE_UP_NO_MORE_INFORMATION                                                      0x08
#define WAKE_UP_NOTIFICATION                                                             0x07

/* Wake Up command class commands */
#define WAKE_UP_VERSION_V2                                                               0x02
#define WAKE_UP_INTERVAL_CAPABILITIES_GET_V2                                             0x09
#define WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2                                          0x0A
#define WAKE_UP_INTERVAL_GET_V2                                                          0x05
#define WAKE_UP_INTERVAL_REPORT_V2                                                       0x06
#define WAKE_UP_INTERVAL_SET_V2                                                          0x04
#define WAKE_UP_NO_MORE_INFORMATION_V2                                                   0x08
#define WAKE_UP_NOTIFICATION_V2                                                          0x07

/* Zip 6lowpan command class commands */
#define ZIP_6LOWPAN_VERSION                                                              0x01
#define LOWPAN_FIRST_FRAGMENT                                                            0xC0
#define LOWPAN_SUBSEQUENT_FRAGMENT                                                       0xE0
/* Values used for Lowpan First Fragment command */
#define LOWPAN_FIRST_FRAGMENT_MASK                                                       0xF8
#define LOWPAN_FIRST_FRAGMENT_PROPERTIES1_DATAGRAM_SIZE_1_MASK                           0x07
/* Values used for Lowpan Subsequent Fragment command */
#define LOWPAN_SUBSEQUENT_FRAGMENT_MASK                                                  0xF8
#define LOWPAN_SUBSEQUENT_FRAGMENT_PROPERTIES1_DATAGRAM_SIZE_1_MASK                      0x07

/* Zip Services command class commands */
#define ZIP_SERVICES_VERSION                                                             0x01
#define COMMAND_ZIP_PACKET                                                               0x02
/* Values used for Command Zip Packet command */
#define COMMAND_ZIP_PACKET_PROPERTIES1_USE_NO_Z_WAVE_ACK_BIT_MASK                        0x01
#define COMMAND_ZIP_PACKET_PROPERTIES1_RESERVED_BIT_MASK                                 0x02
#define COMMAND_ZIP_PACKET_PROPERTIES1_HEADER_EXT_NO_SUPPORT_RESPONSE_BIT_MASK           0x04
#define COMMAND_ZIP_PACKET_PROPERTIES1_WEB_KEEPALIVE_REQUEST_BIT_MASK                    0x08
#define COMMAND_ZIP_PACKET_PROPERTIES1_WAITING_RESPONSE_BIT_MASK                         0x10
#define COMMAND_ZIP_PACKET_PROPERTIES1_NACK_RESPONSE_BIT_MASK                            0x20
#define COMMAND_ZIP_PACKET_PROPERTIES1_ACK_RESPONSE_BIT_MASK                             0x40
#define COMMAND_ZIP_PACKET_PROPERTIES1_ACK_REQUEST_BIT_MASK                              0x80
#define COMMAND_ZIP_PACKET_PROPERTIES2_Z_WAVE_CMD_INCLUDED_BIT_MASK                      0x01
#define COMMAND_ZIP_PACKET_PROPERTIES2_RESERVED_BIT_MASK                                 0x02
#define COMMAND_ZIP_PACKET_PROPERTIES2_HEADER_EXT_INCLUDED_BIT_MASK                      0x04
#define COMMAND_ZIP_PACKET_PROPERTIES2_WEB_KEEPALIVE_DELAY_INCLUDED_BIT_MASK             0x08
#define COMMAND_ZIP_PACKET_PROPERTIES2_CUSTOMER_ID_INCLUDED_BIT_MASK                     0x10
#define COMMAND_ZIP_PACKET_PROPERTIES2_GATEWAY_MAC_INCLUDED_BIT_MASK                     0x20
#define COMMAND_ZIP_PACKET_PROPERTIES2_NODE_IDS_INCLUDED_BIT_MASK                        0x40
#define COMMAND_ZIP_PACKET_PROPERTIES2_HOME_ID_INCLUDED_BIT_MASK                         0x80



/* Max. frame size to allow routing over 4 hops */
#define META_DATA_MAX_DATA_SIZE                      48

/************************************************************/
/* Structs and unions that can be used by the application   */
/* to construct the frames to be sent                       */
/************************************************************/

/************************************************************/
/* Common for all command classes                           */
/************************************************************/
typedef struct _ZW_COMMON_FRAME_
{
  BYTE        cmdClass;  /* The command class */
  BYTE        cmd;       /* The command */
} ZW_COMMON_FRAME;

/************************************************************/
/* Alarm Get command class structs */                       
/************************************************************/
typedef struct _ZW_ALARM_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmType;                    /**/
} ZW_ALARM_GET_FRAME;

/************************************************************/
/* Alarm Report command class structs */                    
/************************************************************/
typedef struct _ZW_ALARM_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmType;                    /**/
    BYTE      alarmLevel;                   /**/
} ZW_ALARM_REPORT_FRAME;

/************************************************************/
/* Alarm Get V2 command class structs */                    
/************************************************************/
typedef struct _ZW_ALARM_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmType;                    /**/
    BYTE      zwaveAlarmType;               /**/
} ZW_ALARM_GET_V2_FRAME;

/************************************************************/
/* Alarm Report 1byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_ALARM_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmType;                    /**/
    BYTE      alarmLevel;                   /**/
    BYTE      zensorNetSourceNodeId;        /**/
    BYTE      zwaveAlarmStatus;             /**/
    BYTE      zwaveAlarmType;               /**/
    BYTE      zwaveAlarmEvent;              /**/
    BYTE      numberOfEventParameters;      /**/
    BYTE      eventParameter1;              
} ZW_ALARM_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Alarm Report 2byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_ALARM_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmType;                    /**/
    BYTE      alarmLevel;                   /**/
    BYTE      zensorNetSourceNodeId;        /**/
    BYTE      zwaveAlarmStatus;             /**/
    BYTE      zwaveAlarmType;               /**/
    BYTE      zwaveAlarmEvent;              /**/
    BYTE      numberOfEventParameters;      /**/
    BYTE      eventParameter1;              /* MSB */
    BYTE      eventParameter2;              /* LSB */
} ZW_ALARM_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Alarm Report 3byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_ALARM_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmType;                    /**/
    BYTE      alarmLevel;                   /**/
    BYTE      zensorNetSourceNodeId;        /**/
    BYTE      zwaveAlarmStatus;             /**/
    BYTE      zwaveAlarmType;               /**/
    BYTE      zwaveAlarmEvent;              /**/
    BYTE      numberOfEventParameters;      /**/
    BYTE      eventParameter1;              /* MSB */
    BYTE      eventParameter2;              
    BYTE      eventParameter3;              /* LSB */
} ZW_ALARM_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Alarm Report 4byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_ALARM_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmType;                    /**/
    BYTE      alarmLevel;                   /**/
    BYTE      zensorNetSourceNodeId;        /**/
    BYTE      zwaveAlarmStatus;             /**/
    BYTE      zwaveAlarmType;               /**/
    BYTE      zwaveAlarmEvent;              /**/
    BYTE      numberOfEventParameters;      /**/
    BYTE      eventParameter1;              /* MSB */
    BYTE      eventParameter2;              
    BYTE      eventParameter3;              
    BYTE      eventParameter4;              /* LSB */
} ZW_ALARM_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Alarm Set V2 command class structs */                    
/************************************************************/
typedef struct _ZW_ALARM_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zwaveAlarmType;               /**/
    BYTE      zwaveAlarmStatus;             /**/
} ZW_ALARM_SET_V2_FRAME;

/************************************************************/
/* Alarm Type Supported Get V2 command class structs */     
/************************************************************/
typedef struct _ZW_ALARM_TYPE_SUPPORTED_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_ALARM_TYPE_SUPPORTED_GET_V2_FRAME;

/************************************************************/
/* Alarm Type Supported Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_ALARM_TYPE_SUPPORTED_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     
} ZW_ALARM_TYPE_SUPPORTED_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Alarm Type Supported Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_ALARM_TYPE_SUPPORTED_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_ALARM_TYPE_SUPPORTED_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Alarm Type Supported Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_ALARM_TYPE_SUPPORTED_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_ALARM_TYPE_SUPPORTED_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Alarm Type Supported Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_ALARM_TYPE_SUPPORTED_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_ALARM_TYPE_SUPPORTED_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Application Busy command class structs */                
/************************************************************/
typedef struct _ZW_APPLICATION_BUSY_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /*The status field*/
    BYTE      waitTime;                     /**/
} ZW_APPLICATION_BUSY_FRAME;

/************************************************************/
/* Application Rejected Request command class structs */    
/************************************************************/
typedef struct _ZW_APPLICATION_REJECTED_REQUEST_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /**/
} ZW_APPLICATION_REJECTED_REQUEST_FRAME;

/************************************************************/
/* Command Configuration Get command class structs */       
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
} ZW_COMMAND_CONFIGURATION_GET_FRAME;

/************************************************************/
/* Command Configuration Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 
} ZW_COMMAND_CONFIGURATION_REPORT_1BYTE_FRAME;

/************************************************************/
/* Command Configuration Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 /* LSB */
} ZW_COMMAND_CONFIGURATION_REPORT_2BYTE_FRAME;

/************************************************************/
/* Command Configuration Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 /* LSB */
} ZW_COMMAND_CONFIGURATION_REPORT_3BYTE_FRAME;

/************************************************************/
/* Command Configuration Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 
    BYTE      commandByte4;                 /* LSB */
} ZW_COMMAND_CONFIGURATION_REPORT_4BYTE_FRAME;

/************************************************************/
/* Command Configuration Set 1byte command class structs */ 
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 
} ZW_COMMAND_CONFIGURATION_SET_1BYTE_FRAME;

/************************************************************/
/* Command Configuration Set 2byte command class structs */ 
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 /* LSB */
} ZW_COMMAND_CONFIGURATION_SET_2BYTE_FRAME;

/************************************************************/
/* Command Configuration Set 3byte command class structs */ 
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 /* LSB */
} ZW_COMMAND_CONFIGURATION_SET_3BYTE_FRAME;

/************************************************************/
/* Command Configuration Set 4byte command class structs */ 
/************************************************************/
typedef struct _ZW_COMMAND_CONFIGURATION_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId;                       /**/
    BYTE      commandLength;                /**/
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 
    BYTE      commandByte4;                 /* LSB */
} ZW_COMMAND_CONFIGURATION_SET_4BYTE_FRAME;

/************************************************************/
/* Command Records Supported Get command class structs */   
/************************************************************/
typedef struct _ZW_COMMAND_RECORDS_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_COMMAND_RECORDS_SUPPORTED_GET_FRAME;

/************************************************************/
/* Command Records Supported Report command class structs */
/************************************************************/
typedef struct _ZW_COMMAND_RECORDS_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      freeCommandRecords1;          /* MSB */
    BYTE      freeCommandRecords2;          /* LSB */
    BYTE      maxCommandRecords1;           /* MSB */
    BYTE      maxCommandRecords2;           /* LSB */
} ZW_COMMAND_RECORDS_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Association Get command class structs */                 
/************************************************************/
typedef struct _ZW_ASSOCIATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
} ZW_ASSOCIATION_GET_FRAME;

/************************************************************/
/* Association Groupings Get command class structs */       
/************************************************************/
typedef struct _ZW_ASSOCIATION_GROUPINGS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_ASSOCIATION_GROUPINGS_GET_FRAME;

/************************************************************/
/* Association Groupings Report command class structs */    
/************************************************************/
typedef struct _ZW_ASSOCIATION_GROUPINGS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedGroupings;           /**/
} ZW_ASSOCIATION_GROUPINGS_REPORT_FRAME;

/************************************************************/
/* Association Remove 1byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
} ZW_ASSOCIATION_REMOVE_1BYTE_FRAME;

/************************************************************/
/* Association Remove 2byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
} ZW_ASSOCIATION_REMOVE_2BYTE_FRAME;

/************************************************************/
/* Association Remove 3byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
} ZW_ASSOCIATION_REMOVE_3BYTE_FRAME;

/************************************************************/
/* Association Remove 4byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
} ZW_ASSOCIATION_REMOVE_4BYTE_FRAME;

/************************************************************/
/* Association Report 1byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      
} ZW_ASSOCIATION_REPORT_1BYTE_FRAME;

/************************************************************/
/* Association Report 2byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      /* MSB */
    BYTE      nodeid2;                      /* LSB */
} ZW_ASSOCIATION_REPORT_2BYTE_FRAME;

/************************************************************/
/* Association Report 3byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      /* MSB */
    BYTE      nodeid2;                      
    BYTE      nodeid3;                      /* LSB */
} ZW_ASSOCIATION_REPORT_3BYTE_FRAME;

/************************************************************/
/* Association Report 4byte command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      /* MSB */
    BYTE      nodeid2;                      
    BYTE      nodeid3;                      
    BYTE      nodeid4;                      /* LSB */
} ZW_ASSOCIATION_REPORT_4BYTE_FRAME;

/************************************************************/
/* Association Set 1byte command class structs */           
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
} ZW_ASSOCIATION_SET_1BYTE_FRAME;

/************************************************************/
/* Association Set 2byte command class structs */           
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
} ZW_ASSOCIATION_SET_2BYTE_FRAME;

/************************************************************/
/* Association Set 3byte command class structs */           
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
} ZW_ASSOCIATION_SET_3BYTE_FRAME;

/************************************************************/
/* Association Set 4byte command class structs */           
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
} ZW_ASSOCIATION_SET_4BYTE_FRAME;

/************************************************************/
/* Association Get V2 command class structs */              
/************************************************************/
typedef struct _ZW_ASSOCIATION_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
} ZW_ASSOCIATION_GET_V2_FRAME;

/************************************************************/
/* Association Groupings Get V2 command class structs */    
/************************************************************/
typedef struct _ZW_ASSOCIATION_GROUPINGS_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_ASSOCIATION_GROUPINGS_GET_V2_FRAME;

/************************************************************/
/* Association Groupings Report V2 command class structs */ 
/************************************************************/
typedef struct _ZW_ASSOCIATION_GROUPINGS_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedGroupings;           /**/
} ZW_ASSOCIATION_GROUPINGS_REPORT_V2_FRAME;

/************************************************************/
/* Association Remove 1byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
} ZW_ASSOCIATION_REMOVE_1BYTE_V2_FRAME;

/************************************************************/
/* Association Remove 2byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
} ZW_ASSOCIATION_REMOVE_2BYTE_V2_FRAME;

/************************************************************/
/* Association Remove 3byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
} ZW_ASSOCIATION_REMOVE_3BYTE_V2_FRAME;

/************************************************************/
/* Association Remove 4byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REMOVE_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
} ZW_ASSOCIATION_REMOVE_4BYTE_V2_FRAME;

/************************************************************/
/* Association Report 1byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      
} ZW_ASSOCIATION_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Association Report 2byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      /* MSB */
    BYTE      nodeid2;                      /* LSB */
} ZW_ASSOCIATION_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Association Report 3byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      /* MSB */
    BYTE      nodeid2;                      
    BYTE      nodeid3;                      /* LSB */
} ZW_ASSOCIATION_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Association Report 4byte V2 command class structs */     
/************************************************************/
typedef struct _ZW_ASSOCIATION_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeid1;                      /* MSB */
    BYTE      nodeid2;                      
    BYTE      nodeid3;                      
    BYTE      nodeid4;                      /* LSB */
} ZW_ASSOCIATION_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Association Set 1byte V2 command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
} ZW_ASSOCIATION_SET_1BYTE_V2_FRAME;

/************************************************************/
/* Association Set 2byte V2 command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
} ZW_ASSOCIATION_SET_2BYTE_V2_FRAME;

/************************************************************/
/* Association Set 3byte V2 command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
} ZW_ASSOCIATION_SET_3BYTE_V2_FRAME;

/************************************************************/
/* Association Set 4byte V2 command class structs */        
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
} ZW_ASSOCIATION_SET_4BYTE_V2_FRAME;

/************************************************************/
/* Association Specific Group Get V2 command class structs */
/************************************************************/
typedef struct _ZW_ASSOCIATION_SPECIFIC_GROUP_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_ASSOCIATION_SPECIFIC_GROUP_GET_V2_FRAME;

/************************************************************/
/* Association Specific Group Report V2 command class structs */
/************************************************************/
typedef struct _ZW_ASSOCIATION_SPECIFIC_GROUP_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      group;                        /**/
} ZW_ASSOCIATION_SPECIFIC_GROUP_REPORT_V2_FRAME;

/************************************************************/
/* Av Content Browse Md By Letter Get command class structs */
/************************************************************/
typedef struct _ZW_AV_CONTENT_BROWSE_MD_BY_LETTER_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_BROWSE_MD_BY_LETTER_GET_FRAME;

/************************************************************/
/* Av Content Browse Md By Letter Report command class structs */
/************************************************************/
typedef struct _ZW_AV_CONTENT_BROWSE_MD_BY_LETTER_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_BROWSE_MD_BY_LETTER_REPORT_FRAME;

/************************************************************/
/* Av Content Browse Md Child Count Get command class structs */
/************************************************************/
typedef struct _ZW_AV_CONTENT_BROWSE_MD_CHILD_COUNT_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_BROWSE_MD_CHILD_COUNT_GET_FRAME;

/************************************************************/
/* Av Content Browse Md Child Count Report command class structs */
/************************************************************/
typedef struct _ZW_AV_CONTENT_BROWSE_MD_CHILD_COUNT_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_BROWSE_MD_CHILD_COUNT_REPORT_FRAME;

/************************************************************/
/* Av Content Browse Md Get command class structs */        
/************************************************************/
typedef struct _ZW_AV_CONTENT_BROWSE_MD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_BROWSE_MD_GET_FRAME;

/************************************************************/
/* Av Content Browse Md Report command class structs */     
/************************************************************/
typedef struct _ZW_AV_CONTENT_BROWSE_MD_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_BROWSE_MD_REPORT_FRAME;

/************************************************************/
/* Av Match Item To Renderer Md Get command class structs */
/************************************************************/
typedef struct _ZW_AV_MATCH_ITEM_TO_RENDERER_MD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_MATCH_ITEM_TO_RENDERER_MD_GET_FRAME;

/************************************************************/
/* Av Match Item To Renderer Md Report command class structs */
/************************************************************/
typedef struct _ZW_AV_MATCH_ITEM_TO_RENDERER_MD_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_MATCH_ITEM_TO_RENDERER_MD_REPORT_FRAME;

/************************************************************/
/* Av Content Search Md Get command class structs */        
/************************************************************/
typedef struct _ZW_AV_CONTENT_SEARCH_MD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_SEARCH_MD_GET_FRAME;

/************************************************************/
/* Av Content Search Md Report command class structs */     
/************************************************************/
typedef struct _ZW_AV_CONTENT_SEARCH_MD_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_CONTENT_SEARCH_MD_REPORT_FRAME;

/************************************************************/
/* Av Renderer Status Get command class structs */          
/************************************************************/
typedef struct _ZW_AV_RENDERER_STATUS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_RENDERER_STATUS_GET_FRAME;

/************************************************************/
/* Av Renderer Status Report command class structs */       
/************************************************************/
typedef struct _ZW_AV_RENDERER_STATUS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_RENDERER_STATUS_REPORT_FRAME;

/************************************************************/
/* Av Tagging Md Get command class structs */               
/************************************************************/
typedef struct _ZW_AV_TAGGING_MD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_TAGGING_MD_GET_FRAME;

/************************************************************/
/* Av Tagging Md Report command class structs */            
/************************************************************/
typedef struct _ZW_AV_TAGGING_MD_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_AV_TAGGING_MD_REPORT_FRAME;

/************************************************************/
/* Basic Tariff Info Get command class structs */           
/************************************************************/
typedef struct _ZW_BASIC_TARIFF_INFO_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_BASIC_TARIFF_INFO_GET_FRAME;

/************************************************************/
/* Basic Tariff Info Report command class structs */        
/************************************************************/
typedef struct _ZW_BASIC_TARIFF_INFO_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      e1RateConsumptionRegister1;   /* MSB */
    BYTE      e1RateConsumptionRegister2;   
    BYTE      e1RateConsumptionRegister3;   
    BYTE      e1RateConsumptionRegister4;   /* LSB */
    BYTE      e1TimeForNextRateHours;       /**/
    BYTE      e1TimeForNextRateMinutes;     /**/
    BYTE      e1TimeForNextRateSeconds;     /**/
    BYTE      properties3;                  /* masked byte */
    BYTE      e2RateConsumptionRegister1;   /* MSB */
    BYTE      e2RateConsumptionRegister2;   
    BYTE      e2RateConsumptionRegister3;   
    BYTE      e2RateConsumptionRegister4;   /* LSB */
} ZW_BASIC_TARIFF_INFO_REPORT_FRAME;

/************************************************************/
/* Basic Window Covering Start Level Change command class structs */
/************************************************************/
typedef struct _ZW_BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_FRAME;

/************************************************************/
/* Basic Window Covering Stop Level Change command class structs */
/************************************************************/
typedef struct _ZW_BASIC_WINDOW_COVERING_STOP_LEVEL_CHANGE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_BASIC_WINDOW_COVERING_STOP_LEVEL_CHANGE_FRAME;

/************************************************************/
/* Basic Get command class structs */                       
/************************************************************/
typedef struct _ZW_BASIC_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_BASIC_GET_FRAME;

/************************************************************/
/* Basic Report command class structs */                    
/************************************************************/
typedef struct _ZW_BASIC_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_BASIC_REPORT_FRAME;

/************************************************************/
/* Basic Set command class structs */                       
/************************************************************/
typedef struct _ZW_BASIC_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_BASIC_SET_FRAME;

/************************************************************/
/* Battery Get command class structs */                     
/************************************************************/
typedef struct _ZW_BATTERY_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_BATTERY_GET_FRAME;

/************************************************************/
/* Battery Report command class structs */                  
/************************************************************/
typedef struct _ZW_BATTERY_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      batteryLevel;                 /**/
} ZW_BATTERY_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Alarm Log Get command class structs */       
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_LOG_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_ALARM_LOG_GET_FRAME;

/************************************************************/
/* Chimney Fan Alarm Log Report command class structs */    
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_LOG_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmEvent1;                  /* masked byte */
    BYTE      alarmEvent2;                  /* masked byte */
    BYTE      alarmEvent3;                  /* masked byte */
    BYTE      alarmEvent4;                  /* masked byte */
    BYTE      alarmEvent5;                  /* masked byte */
} ZW_CHIMNEY_FAN_ALARM_LOG_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Alarm Log Set command class structs */       
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_LOG_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      message;                      /**/
} ZW_CHIMNEY_FAN_ALARM_LOG_SET_FRAME;

/************************************************************/
/* Chimney Fan Alarm Status Get command class structs */    
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_STATUS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_ALARM_STATUS_GET_FRAME;

/************************************************************/
/* Chimney Fan Alarm Status Report command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_STATUS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      alarmStatus;                  /* masked byte */
} ZW_CHIMNEY_FAN_ALARM_STATUS_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Alarm Status Set command class structs */    
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_STATUS_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      message;                      /* masked byte */
} ZW_CHIMNEY_FAN_ALARM_STATUS_SET_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Get command class structs */      
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_ALARM_TEMP_GET_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_CHIMNEY_FAN_ALARM_TEMP_REPORT_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Set 1byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_CHIMNEY_FAN_ALARM_TEMP_SET_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Set 2byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_CHIMNEY_FAN_ALARM_TEMP_SET_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Set 3byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_CHIMNEY_FAN_ALARM_TEMP_SET_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Alarm Temp Set 4byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_ALARM_TEMP_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_CHIMNEY_FAN_ALARM_TEMP_SET_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Boost Time Get command class structs */      
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_BOOST_TIME_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_BOOST_TIME_GET_FRAME;

/************************************************************/
/* Chimney Fan Boost Time Report command class structs */   
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_BOOST_TIME_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      time;                         /**/
} ZW_CHIMNEY_FAN_BOOST_TIME_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Boost Time Set command class structs */      
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_BOOST_TIME_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      time;                         /**/
} ZW_CHIMNEY_FAN_BOOST_TIME_SET_FRAME;

/************************************************************/
/* Chimney Fan Default Set command class structs */         
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_DEFAULT_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_DEFAULT_SET_FRAME;

/************************************************************/
/* Chimney Fan Min Speed Get command class structs */       
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_MIN_SPEED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_MIN_SPEED_GET_FRAME;

/************************************************************/
/* Chimney Fan Min Speed Report command class structs */    
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_MIN_SPEED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      minSpeed;                     /**/
} ZW_CHIMNEY_FAN_MIN_SPEED_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Min Speed Set command class structs */       
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_MIN_SPEED_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      minSpeed;                     /**/
} ZW_CHIMNEY_FAN_MIN_SPEED_SET_FRAME;

/************************************************************/
/* Chimney Fan Mode Get command class structs */            
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_MODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_MODE_GET_FRAME;

/************************************************************/
/* Chimney Fan Mode Report command class structs */         
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_MODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_CHIMNEY_FAN_MODE_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Mode Set command class structs */            
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_MODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_CHIMNEY_FAN_MODE_SET_FRAME;

/************************************************************/
/* Chimney Fan Setup Get command class structs */           
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_SETUP_GET_FRAME;

/************************************************************/
/* Chimney Fan Setup Report 1byte command class structs */  
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       
} ZW_CHIMNEY_FAN_SETUP_REPORT_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Setup Report 2byte command class structs */  
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            /* MSB */
    BYTE      startTemperature2;            /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             /* MSB */
    BYTE      stopTemperature2;             /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       /* MSB */
    BYTE      alarmTemperatureValue2;       /* LSB */
} ZW_CHIMNEY_FAN_SETUP_REPORT_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Setup Report 3byte command class structs */  
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            /* MSB */
    BYTE      startTemperature2;            
    BYTE      startTemperature3;            /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             /* MSB */
    BYTE      stopTemperature2;             
    BYTE      stopTemperature3;             /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       /* MSB */
    BYTE      alarmTemperatureValue2;       
    BYTE      alarmTemperatureValue3;       /* LSB */
} ZW_CHIMNEY_FAN_SETUP_REPORT_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Setup Report 4byte command class structs */  
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            /* MSB */
    BYTE      startTemperature2;            
    BYTE      startTemperature3;            
    BYTE      startTemperature4;            /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             /* MSB */
    BYTE      stopTemperature2;             
    BYTE      stopTemperature3;             
    BYTE      stopTemperature4;             /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       /* MSB */
    BYTE      alarmTemperatureValue2;       
    BYTE      alarmTemperatureValue3;       
    BYTE      alarmTemperatureValue4;       /* LSB */
} ZW_CHIMNEY_FAN_SETUP_REPORT_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Setup Set 1byte command class structs */     
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       
} ZW_CHIMNEY_FAN_SETUP_SET_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Setup Set 2byte command class structs */     
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            /* MSB */
    BYTE      startTemperature2;            /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             /* MSB */
    BYTE      stopTemperature2;             /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       /* MSB */
    BYTE      alarmTemperatureValue2;       /* LSB */
} ZW_CHIMNEY_FAN_SETUP_SET_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Setup Set 3byte command class structs */     
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            /* MSB */
    BYTE      startTemperature2;            
    BYTE      startTemperature3;            /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             /* MSB */
    BYTE      stopTemperature2;             
    BYTE      stopTemperature3;             /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       /* MSB */
    BYTE      alarmTemperatureValue2;       
    BYTE      alarmTemperatureValue3;       /* LSB */
} ZW_CHIMNEY_FAN_SETUP_SET_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Setup Set 4byte command class structs */     
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SETUP_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      boostTime;                    /**/
    BYTE      stopTime;                     /**/
    BYTE      minSpeed;                     /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startTemperature1;            /* MSB */
    BYTE      startTemperature2;            
    BYTE      startTemperature3;            
    BYTE      startTemperature4;            /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      stopTemperature1;             /* MSB */
    BYTE      stopTemperature2;             
    BYTE      stopTemperature3;             
    BYTE      stopTemperature4;             /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      alarmTemperatureValue1;       /* MSB */
    BYTE      alarmTemperatureValue2;       
    BYTE      alarmTemperatureValue3;       
    BYTE      alarmTemperatureValue4;       /* LSB */
} ZW_CHIMNEY_FAN_SETUP_SET_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Speed Get command class structs */           
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SPEED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_SPEED_GET_FRAME;

/************************************************************/
/* Chimney Fan Speed Report command class structs */        
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SPEED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      speed;                        /**/
} ZW_CHIMNEY_FAN_SPEED_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Speed Set command class structs */           
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_SPEED_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      speed;                        /**/
} ZW_CHIMNEY_FAN_SPEED_SET_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Get command class structs */      
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_START_TEMP_GET_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_CHIMNEY_FAN_START_TEMP_REPORT_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_CHIMNEY_FAN_START_TEMP_REPORT_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_CHIMNEY_FAN_START_TEMP_REPORT_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_CHIMNEY_FAN_START_TEMP_REPORT_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Set 1byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_CHIMNEY_FAN_START_TEMP_SET_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Set 2byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_CHIMNEY_FAN_START_TEMP_SET_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Set 3byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_CHIMNEY_FAN_START_TEMP_SET_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Start Temp Set 4byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_START_TEMP_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_CHIMNEY_FAN_START_TEMP_SET_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan State Get command class structs */           
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_STATE_GET_FRAME;

/************************************************************/
/* Chimney Fan State Report command class structs */        
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      state;                        /**/
} ZW_CHIMNEY_FAN_STATE_REPORT_FRAME;

/************************************************************/
/* Chimney Fan State Set command class structs */           
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      state;                        /**/
} ZW_CHIMNEY_FAN_STATE_SET_FRAME;

/************************************************************/
/* Chimney Fan Status Get command class structs */          
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATUS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_STATUS_GET_FRAME;

/************************************************************/
/* Chimney Fan Status Report 1byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATUS_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      state;                        /**/
    BYTE      speed;                        /**/
    BYTE      alarmStatus;                  /* masked byte */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_CHIMNEY_FAN_STATUS_REPORT_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Status Report 2byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATUS_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      state;                        /**/
    BYTE      speed;                        /**/
    BYTE      alarmStatus;                  /* masked byte */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_CHIMNEY_FAN_STATUS_REPORT_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Status Report 3byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATUS_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      state;                        /**/
    BYTE      speed;                        /**/
    BYTE      alarmStatus;                  /* masked byte */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_CHIMNEY_FAN_STATUS_REPORT_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Status Report 4byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STATUS_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      state;                        /**/
    BYTE      speed;                        /**/
    BYTE      alarmStatus;                  /* masked byte */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_CHIMNEY_FAN_STATUS_REPORT_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Get command class structs */       
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_STOP_TEMP_GET_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_CHIMNEY_FAN_STOP_TEMP_REPORT_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Set 1byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_CHIMNEY_FAN_STOP_TEMP_SET_1BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Set 2byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_CHIMNEY_FAN_STOP_TEMP_SET_2BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Set 3byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_CHIMNEY_FAN_STOP_TEMP_SET_3BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Temp Set 4byte command class structs */ 
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TEMP_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_CHIMNEY_FAN_STOP_TEMP_SET_4BYTE_FRAME;

/************************************************************/
/* Chimney Fan Stop Time Get command class structs */       
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TIME_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CHIMNEY_FAN_STOP_TIME_GET_FRAME;

/************************************************************/
/* Chimney Fan Stop Time Report command class structs */    
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TIME_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      time;                         /**/
} ZW_CHIMNEY_FAN_STOP_TIME_REPORT_FRAME;

/************************************************************/
/* Chimney Fan Stop Time Set command class structs */       
/************************************************************/
typedef struct _ZW_CHIMNEY_FAN_STOP_TIME_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      time;                         /**/
} ZW_CHIMNEY_FAN_STOP_TIME_SET_FRAME;

/************************************************************/
/* Schedule Changed Get command class structs */            
/************************************************************/
typedef struct _ZW_SCHEDULE_CHANGED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SCHEDULE_CHANGED_GET_FRAME;

/************************************************************/
/* Schedule Changed Report command class structs */         
/************************************************************/
typedef struct _ZW_SCHEDULE_CHANGED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      changecounter;                /**/
} ZW_SCHEDULE_CHANGED_REPORT_FRAME;

/************************************************************/
/* Schedule Get command class structs */                    
/************************************************************/
typedef struct _ZW_SCHEDULE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_SCHEDULE_GET_FRAME;

/************************************************************/
/* Schedule Override Get command class structs */           
/************************************************************/
typedef struct _ZW_SCHEDULE_OVERRIDE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SCHEDULE_OVERRIDE_GET_FRAME;

/************************************************************/
/* Schedule Override Report command class structs */        
/************************************************************/
typedef struct _ZW_SCHEDULE_OVERRIDE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      overrideState;                /**/
} ZW_SCHEDULE_OVERRIDE_REPORT_FRAME;

/************************************************************/
/* Schedule Override Set command class structs */           
/************************************************************/
typedef struct _ZW_SCHEDULE_OVERRIDE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      overrideState;                /**/
} ZW_SCHEDULE_OVERRIDE_SET_FRAME;

/************************************************************/
/* Schedule Report command class structs */                 
/************************************************************/
typedef struct _ZW_SCHEDULE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      switchpoint01;                /* MSB */
    BYTE      switchpoint02;                
    BYTE      switchpoint03;                /* LSB */
    BYTE      switchpoint11;                /* MSB */
    BYTE      switchpoint12;                
    BYTE      switchpoint13;                /* LSB */
    BYTE      switchpoint21;                /* MSB */
    BYTE      switchpoint22;                
    BYTE      switchpoint23;                /* LSB */
    BYTE      switchpoint31;                /* MSB */
    BYTE      switchpoint32;                
    BYTE      switchpoint33;                /* LSB */
    BYTE      switchpoint41;                /* MSB */
    BYTE      switchpoint42;                
    BYTE      switchpoint43;                /* LSB */
    BYTE      switchpoint51;                /* MSB */
    BYTE      switchpoint52;                
    BYTE      switchpoint53;                /* LSB */
    BYTE      switchpoint61;                /* MSB */
    BYTE      switchpoint62;                
    BYTE      switchpoint63;                /* LSB */
    BYTE      switchpoint71;                /* MSB */
    BYTE      switchpoint72;                
    BYTE      switchpoint73;                /* LSB */
    BYTE      switchpoint81;                /* MSB */
    BYTE      switchpoint82;                
    BYTE      switchpoint83;                /* LSB */
} ZW_SCHEDULE_REPORT_FRAME;

/************************************************************/
/* Schedule Set command class structs */                    
/************************************************************/
typedef struct _ZW_SCHEDULE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      switchpoint01;                /* MSB */
    BYTE      switchpoint02;                
    BYTE      switchpoint03;                /* LSB */
    BYTE      switchpoint11;                /* MSB */
    BYTE      switchpoint12;                
    BYTE      switchpoint13;                /* LSB */
    BYTE      switchpoint21;                /* MSB */
    BYTE      switchpoint22;                
    BYTE      switchpoint23;                /* LSB */
    BYTE      switchpoint31;                /* MSB */
    BYTE      switchpoint32;                
    BYTE      switchpoint33;                /* LSB */
    BYTE      switchpoint41;                /* MSB */
    BYTE      switchpoint42;                
    BYTE      switchpoint43;                /* LSB */
    BYTE      switchpoint51;                /* MSB */
    BYTE      switchpoint52;                
    BYTE      switchpoint53;                /* LSB */
    BYTE      switchpoint61;                /* MSB */
    BYTE      switchpoint62;                
    BYTE      switchpoint63;                /* LSB */
    BYTE      switchpoint71;                /* MSB */
    BYTE      switchpoint72;                
    BYTE      switchpoint73;                /* LSB */
    BYTE      switchpoint81;                /* MSB */
    BYTE      switchpoint82;                
    BYTE      switchpoint83;                /* LSB */
} ZW_SCHEDULE_SET_FRAME;

/************************************************************/
/* Clock Get command class structs */                       
/************************************************************/
typedef struct _ZW_CLOCK_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_CLOCK_GET_FRAME;

/************************************************************/
/* Clock Report command class structs */                    
/************************************************************/
typedef struct _ZW_CLOCK_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      minute;                       /**/
} ZW_CLOCK_REPORT_FRAME;

/************************************************************/
/* Clock Set command class structs */                       
/************************************************************/
typedef struct _ZW_CLOCK_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      minute;                       /**/
} ZW_CLOCK_SET_FRAME;

/************************************************************/
/* Configuration Get command class structs */               
/************************************************************/
typedef struct _ZW_CONFIGURATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
} ZW_CONFIGURATION_GET_FRAME;

/************************************************************/
/* Configuration Report 1byte command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          
} ZW_CONFIGURATION_REPORT_1BYTE_FRAME;

/************************************************************/
/* Configuration Report 2byte command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          /* LSB */
} ZW_CONFIGURATION_REPORT_2BYTE_FRAME;

/************************************************************/
/* Configuration Report 3byte command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          /* LSB */
} ZW_CONFIGURATION_REPORT_3BYTE_FRAME;

/************************************************************/
/* Configuration Report 4byte command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          
    BYTE      configurationValue4;          /* LSB */
} ZW_CONFIGURATION_REPORT_4BYTE_FRAME;

/************************************************************/
/* Configuration Set 1byte command class structs */         
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          
} ZW_CONFIGURATION_SET_1BYTE_FRAME;

/************************************************************/
/* Configuration Set 2byte command class structs */         
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          /* LSB */
} ZW_CONFIGURATION_SET_2BYTE_FRAME;

/************************************************************/
/* Configuration Set 3byte command class structs */         
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          /* LSB */
} ZW_CONFIGURATION_SET_3BYTE_FRAME;

/************************************************************/
/* Configuration Set 4byte command class structs */         
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          
    BYTE      configurationValue4;          /* LSB */
} ZW_CONFIGURATION_SET_4BYTE_FRAME;

/************************************************************/
/* Configuration Bulk Get V2 command class structs */       
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
} ZW_CONFIGURATION_BULK_GET_V2_FRAME;

/************************************************************/
/* Configuration Bulk Report V2 variant group structs */    
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_REPORT_1BYTE_V2_VG_
{
    BYTE      parameter1;                   
} VG_CONFIGURATION_BULK_REPORT_1BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Report V2 variant group structs */    
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_REPORT_2BYTE_V2_VG_
{
    BYTE      parameter1;                   /* MSB */
    BYTE      parameter2;                   /* LSB */
} VG_CONFIGURATION_BULK_REPORT_2BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Report V2 variant group structs */    
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_REPORT_3BYTE_V2_VG_
{
    BYTE      parameter1;                   /* MSB */
    BYTE      parameter2;                   
    BYTE      parameter3;                   /* LSB */
} VG_CONFIGURATION_BULK_REPORT_3BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Report V2 variant group structs */    
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG_
{
    BYTE      parameter1;                   /* MSB */
    BYTE      parameter2;                   
    BYTE      parameter3;                   
    BYTE      parameter4;                   /* LSB */
} VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup1;                /**/
} ZW_CONFIGURATION_BULK_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Configuration Bulk Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup1;                /**/
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup2;                /**/
} ZW_CONFIGURATION_BULK_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Configuration Bulk Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup1;                /**/
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup2;                /**/
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup3;                /**/
} ZW_CONFIGURATION_BULK_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Configuration Bulk Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup1;                /**/
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup2;                /**/
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup3;                /**/
    VG_CONFIGURATION_BULK_REPORT_4BYTE_V2_VG variantgroup4;                /**/
} ZW_CONFIGURATION_BULK_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Configuration Bulk Set V2 variant group structs */       
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_SET_1BYTE_V2_VG_
{
    BYTE      parameter1;                   
} VG_CONFIGURATION_BULK_SET_1BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Set V2 variant group structs */       
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_SET_2BYTE_V2_VG_
{
    BYTE      parameter1;                   /* MSB */
    BYTE      parameter2;                   /* LSB */
} VG_CONFIGURATION_BULK_SET_2BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Set V2 variant group structs */       
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_SET_3BYTE_V2_VG_
{
    BYTE      parameter1;                   /* MSB */
    BYTE      parameter2;                   
    BYTE      parameter3;                   /* LSB */
} VG_CONFIGURATION_BULK_SET_3BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Set V2 variant group structs */       
/************************************************************/
typedef struct _VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG_
{
    BYTE      parameter1;                   /* MSB */
    BYTE      parameter2;                   
    BYTE      parameter3;                   
    BYTE      parameter4;                   /* LSB */
} VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG;

/************************************************************/
/* Configuration Bulk Set 1byte V2 command class structs */ 
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_SET_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup1;                /**/
} ZW_CONFIGURATION_BULK_SET_1BYTE_V2_FRAME;

/************************************************************/
/* Configuration Bulk Set 2byte V2 command class structs */ 
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_SET_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup1;                /**/
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup2;                /**/
} ZW_CONFIGURATION_BULK_SET_2BYTE_V2_FRAME;

/************************************************************/
/* Configuration Bulk Set 3byte V2 command class structs */ 
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_SET_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup1;                /**/
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup2;                /**/
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup3;                /**/
} ZW_CONFIGURATION_BULK_SET_3BYTE_V2_FRAME;

/************************************************************/
/* Configuration Bulk Set 4byte V2 command class structs */ 
/************************************************************/
typedef struct _ZW_CONFIGURATION_BULK_SET_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterOffset1;             /* MSB */
    BYTE      parameterOffset2;             /* LSB */
    BYTE      numberOfParameters;           /**/
    BYTE      properties1;                  /* masked byte */
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup1;                /**/
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup2;                /**/
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup3;                /**/
    VG_CONFIGURATION_BULK_SET_4BYTE_V2_VG variantgroup4;                /**/
} ZW_CONFIGURATION_BULK_SET_4BYTE_V2_FRAME;

/************************************************************/
/* Configuration Get V2 command class structs */            
/************************************************************/
typedef struct _ZW_CONFIGURATION_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
} ZW_CONFIGURATION_GET_V2_FRAME;

/************************************************************/
/* Configuration Report 1byte V2 command class structs */   
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          
} ZW_CONFIGURATION_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Configuration Report 2byte V2 command class structs */   
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          /* LSB */
} ZW_CONFIGURATION_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Configuration Report 3byte V2 command class structs */   
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          /* LSB */
} ZW_CONFIGURATION_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Configuration Report 4byte V2 command class structs */   
/************************************************************/
typedef struct _ZW_CONFIGURATION_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          
    BYTE      configurationValue4;          /* LSB */
} ZW_CONFIGURATION_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Configuration Set 1byte V2 command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          
} ZW_CONFIGURATION_SET_1BYTE_V2_FRAME;

/************************************************************/
/* Configuration Set 2byte V2 command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          /* LSB */
} ZW_CONFIGURATION_SET_2BYTE_V2_FRAME;

/************************************************************/
/* Configuration Set 3byte V2 command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          /* LSB */
} ZW_CONFIGURATION_SET_3BYTE_V2_FRAME;

/************************************************************/
/* Configuration Set 4byte V2 command class structs */      
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      configurationValue1;          /* MSB */
    BYTE      configurationValue2;          
    BYTE      configurationValue3;          
    BYTE      configurationValue4;          /* LSB */
} ZW_CONFIGURATION_SET_4BYTE_V2_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Group command class structs */ 
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_GROUP_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      groupId;                      /**/
    BYTE      nodeId;                       /**/
} ZW_CTRL_REPLICATION_TRANSFER_GROUP_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Group Name 1byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      groupId;                      /**/
    BYTE      groupName1;                   
} ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_1BYTE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Group Name 2byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      groupId;                      /**/
    BYTE      groupName1;                   /* MSB */
    BYTE      groupName2;                   /* LSB */
} ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_2BYTE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Group Name 3byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      groupId;                      /**/
    BYTE      groupName1;                   /* MSB */
    BYTE      groupName2;                   
    BYTE      groupName3;                   /* LSB */
} ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_3BYTE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Group Name 4byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      groupId;                      /**/
    BYTE      groupName1;                   /* MSB */
    BYTE      groupName2;                   
    BYTE      groupName3;                   
    BYTE      groupName4;                   /* LSB */
} ZW_CTRL_REPLICATION_TRANSFER_GROUP_NAME_4BYTE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Scene command class structs */ 
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_SCENE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      sceneId;                      /**/
    BYTE      nodeId;                       /**/
    BYTE      level;                        /**/
} ZW_CTRL_REPLICATION_TRANSFER_SCENE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Scene Name 1byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      sceneId;                      /**/
    BYTE      sceneName1;                   
} ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_1BYTE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Scene Name 2byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      sceneId;                      /**/
    BYTE      sceneName1;                   /* MSB */
    BYTE      sceneName2;                   /* LSB */
} ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_2BYTE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Scene Name 3byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      sceneId;                      /**/
    BYTE      sceneName1;                   /* MSB */
    BYTE      sceneName2;                   
    BYTE      sceneName3;                   /* LSB */
} ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_3BYTE_FRAME;

/************************************************************/
/* Ctrl Replication Transfer Scene Name 4byte command class structs */
/************************************************************/
typedef struct _ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      sceneId;                      /**/
    BYTE      sceneName1;                   /* MSB */
    BYTE      sceneName2;                   
    BYTE      sceneName3;                   
    BYTE      sceneName4;                   /* LSB */
} ZW_CTRL_REPLICATION_TRANSFER_SCENE_NAME_4BYTE_FRAME;

/************************************************************/
/* Dcp List Remove command class structs */                 
/************************************************************/
typedef struct _ZW_DCP_LIST_REMOVE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
} ZW_DCP_LIST_REMOVE_FRAME;

/************************************************************/
/* Dcp List Set variant group structs */                    
/************************************************************/
typedef struct _VG_DCP_LIST_SET_VG_
{
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
} VG_DCP_LIST_SET_VG;

/************************************************************/
/* Dcp List Set 1byte command class structs */              
/************************************************************/
typedef struct _ZW_DCP_LIST_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpRateId;                    /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_SET_VG variantgroup1;                /**/
} ZW_DCP_LIST_SET_1BYTE_FRAME;

/************************************************************/
/* Dcp List Set 2byte command class structs */              
/************************************************************/
typedef struct _ZW_DCP_LIST_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpRateId;                    /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_SET_VG variantgroup1;                /**/
    VG_DCP_LIST_SET_VG variantgroup2;                /**/
} ZW_DCP_LIST_SET_2BYTE_FRAME;

/************************************************************/
/* Dcp List Set 3byte command class structs */              
/************************************************************/
typedef struct _ZW_DCP_LIST_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpRateId;                    /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_SET_VG variantgroup1;                /**/
    VG_DCP_LIST_SET_VG variantgroup2;                /**/
    VG_DCP_LIST_SET_VG variantgroup3;                /**/
} ZW_DCP_LIST_SET_3BYTE_FRAME;

/************************************************************/
/* Dcp List Set 4byte command class structs */              
/************************************************************/
typedef struct _ZW_DCP_LIST_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpRateId;                    /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_SET_VG variantgroup1;                /**/
    VG_DCP_LIST_SET_VG variantgroup2;                /**/
    VG_DCP_LIST_SET_VG variantgroup3;                /**/
    VG_DCP_LIST_SET_VG variantgroup4;                /**/
} ZW_DCP_LIST_SET_4BYTE_FRAME;

/************************************************************/
/* Dcp List Supported Get command class structs */          
/************************************************************/
typedef struct _ZW_DCP_LIST_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_DCP_LIST_SUPPORTED_GET_FRAME;

/************************************************************/
/* Dcp List Supported Report command class structs */       
/************************************************************/
typedef struct _ZW_DCP_LIST_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      dcpListSize;                  /**/
    BYTE      freeDcpListEntries;           /**/
} ZW_DCP_LIST_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Dcp Event Status Get command class structs */            
/************************************************************/
typedef struct _ZW_DCP_EVENT_STATUS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
} ZW_DCP_EVENT_STATUS_GET_FRAME;

/************************************************************/
/* Dcp Event Status Report command class structs */         
/************************************************************/
typedef struct _ZW_DCP_EVENT_STATUS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      eventStatus;                  /**/
} ZW_DCP_EVENT_STATUS_REPORT_FRAME;

/************************************************************/
/* Dcp List Get command class structs */                    
/************************************************************/
typedef struct _ZW_DCP_LIST_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_DCP_LIST_GET_FRAME;

/************************************************************/
/* Dcp List Report variant group structs */                 
/************************************************************/
typedef struct _VG_DCP_LIST_REPORT_VG_
{
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
} VG_DCP_LIST_REPORT_VG;

/************************************************************/
/* Dcp List Report 1byte command class structs */           
/************************************************************/
typedef struct _ZW_DCP_LIST_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpId;                        /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_REPORT_VG variantgroup1;                /**/
} ZW_DCP_LIST_REPORT_1BYTE_FRAME;

/************************************************************/
/* Dcp List Report 2byte command class structs */           
/************************************************************/
typedef struct _ZW_DCP_LIST_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpId;                        /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_REPORT_VG variantgroup1;                /**/
    VG_DCP_LIST_REPORT_VG variantgroup2;                /**/
} ZW_DCP_LIST_REPORT_2BYTE_FRAME;

/************************************************************/
/* Dcp List Report 3byte command class structs */           
/************************************************************/
typedef struct _ZW_DCP_LIST_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpId;                        /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_REPORT_VG variantgroup1;                /**/
    VG_DCP_LIST_REPORT_VG variantgroup2;                /**/
    VG_DCP_LIST_REPORT_VG variantgroup3;                /**/
} ZW_DCP_LIST_REPORT_3BYTE_FRAME;

/************************************************************/
/* Dcp List Report 4byte command class structs */           
/************************************************************/
typedef struct _ZW_DCP_LIST_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      dcpId;                        /**/
    BYTE      properties1;                  /* masked byte */
    VG_DCP_LIST_REPORT_VG variantgroup1;                /**/
    VG_DCP_LIST_REPORT_VG variantgroup2;                /**/
    VG_DCP_LIST_REPORT_VG variantgroup3;                /**/
    VG_DCP_LIST_REPORT_VG variantgroup4;                /**/
} ZW_DCP_LIST_REPORT_4BYTE_FRAME;

/************************************************************/
/* Door Lock Logging Records Supported Get command class structs */
/************************************************************/
typedef struct _ZW_DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_GET_FRAME;

/************************************************************/
/* Door Lock Logging Records Supported Report command class structs */
/************************************************************/
typedef struct _ZW_DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      maxRecordsStored;             /**/
} ZW_DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Record Get command class structs */                      
/************************************************************/
typedef struct _ZW_RECORD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      recordNumber;                 /**/
} ZW_RECORD_GET_FRAME;

/************************************************************/
/* Record Report 1byte command class structs */             
/************************************************************/
typedef struct _ZW_RECORD_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      recordNumber;                 /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      eventType;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      userCodeLength;               /**/
    BYTE      user_code1;                   
} ZW_RECORD_REPORT_1BYTE_FRAME;

/************************************************************/
/* Record Report 2byte command class structs */             
/************************************************************/
typedef struct _ZW_RECORD_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      recordNumber;                 /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      eventType;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      userCodeLength;               /**/
    BYTE      user_code1;                   /* MSB */
    BYTE      user_code2;                   /* LSB */
} ZW_RECORD_REPORT_2BYTE_FRAME;

/************************************************************/
/* Record Report 3byte command class structs */             
/************************************************************/
typedef struct _ZW_RECORD_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      recordNumber;                 /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      eventType;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      userCodeLength;               /**/
    BYTE      user_code1;                   /* MSB */
    BYTE      user_code2;                   
    BYTE      user_code3;                   /* LSB */
} ZW_RECORD_REPORT_3BYTE_FRAME;

/************************************************************/
/* Record Report 4byte command class structs */             
/************************************************************/
typedef struct _ZW_RECORD_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      recordNumber;                 /**/
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      eventType;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      userCodeLength;               /**/
    BYTE      user_code1;                   /* MSB */
    BYTE      user_code2;                   
    BYTE      user_code3;                   
    BYTE      user_code4;                   /* LSB */
} ZW_RECORD_REPORT_4BYTE_FRAME;

/************************************************************/
/* Door Lock Configuration Get command class structs */     
/************************************************************/
typedef struct _ZW_DOOR_LOCK_CONFIGURATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_DOOR_LOCK_CONFIGURATION_GET_FRAME;

/************************************************************/
/* Door Lock Configuration Report command class structs */  
/************************************************************/
typedef struct _ZW_DOOR_LOCK_CONFIGURATION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      operationType;                /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      lockTimeoutMinutes;           /**/
    BYTE      lockTimeoutSeconds;           /**/
} ZW_DOOR_LOCK_CONFIGURATION_REPORT_FRAME;

/************************************************************/
/* Door Lock Configuration Set command class structs */     
/************************************************************/
typedef struct _ZW_DOOR_LOCK_CONFIGURATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      operationType;                /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      lockTimeoutMinutes;           /**/
    BYTE      lockTimeoutSeconds;           /**/
} ZW_DOOR_LOCK_CONFIGURATION_SET_FRAME;

/************************************************************/
/* Door Lock Operation Get command class structs */         
/************************************************************/
typedef struct _ZW_DOOR_LOCK_OPERATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_DOOR_LOCK_OPERATION_GET_FRAME;

/************************************************************/
/* Door Lock Operation Report command class structs */      
/************************************************************/
typedef struct _ZW_DOOR_LOCK_OPERATION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      doorLockMode;                 /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      doorCondition;                /**/
    BYTE      lockTimeoutMinutes;           /**/
    BYTE      lockTimeoutSeconds;           /**/
} ZW_DOOR_LOCK_OPERATION_REPORT_FRAME;

/************************************************************/
/* Door Lock Operation Set command class structs */         
/************************************************************/
typedef struct _ZW_DOOR_LOCK_OPERATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      doorLockMode;                 /**/
} ZW_DOOR_LOCK_OPERATION_SET_FRAME;

/************************************************************/
/* Energy Production Get command class structs */           
/************************************************************/
typedef struct _ZW_ENERGY_PRODUCTION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
} ZW_ENERGY_PRODUCTION_GET_FRAME;

/************************************************************/
/* Energy Production Report 1byte command class structs */  
/************************************************************/
typedef struct _ZW_ENERGY_PRODUCTION_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      value1;                       
} ZW_ENERGY_PRODUCTION_REPORT_1BYTE_FRAME;

/************************************************************/
/* Energy Production Report 2byte command class structs */  
/************************************************************/
typedef struct _ZW_ENERGY_PRODUCTION_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_ENERGY_PRODUCTION_REPORT_2BYTE_FRAME;

/************************************************************/
/* Energy Production Report 3byte command class structs */  
/************************************************************/
typedef struct _ZW_ENERGY_PRODUCTION_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_ENERGY_PRODUCTION_REPORT_3BYTE_FRAME;

/************************************************************/
/* Energy Production Report 4byte command class structs */  
/************************************************************/
typedef struct _ZW_ENERGY_PRODUCTION_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameterNumber;              /**/
    BYTE      level;                        /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_ENERGY_PRODUCTION_REPORT_4BYTE_FRAME;

/************************************************************/
/* Firmware Md Get command class structs */                 
/************************************************************/
typedef struct _ZW_FIRMWARE_MD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_FIRMWARE_MD_GET_FRAME;

/************************************************************/
/* Firmware Md Report command class structs */              
/************************************************************/
typedef struct _ZW_FIRMWARE_MD_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      manufacturerId1;              /* MSB */
    BYTE      manufacturerId2;              /* LSB */
    BYTE      firmwareId1;                  /* MSB */
    BYTE      firmwareId2;                  /* LSB */
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_MD_REPORT_FRAME;

/************************************************************/
/* Firmware Update Md Get command class structs */          
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfReports;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
} ZW_FIRMWARE_UPDATE_MD_GET_FRAME;

/************************************************************/
/* Firmware Update Md Report 1byte command class structs */ 
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        
} ZW_FIRMWARE_UPDATE_MD_REPORT_1BYTE_FRAME;

/************************************************************/
/* Firmware Update Md Report 2byte command class structs */ 
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REPORT_2BYTE_FRAME;

/************************************************************/
/* Firmware Update Md Report 3byte command class structs */ 
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REPORT_3BYTE_FRAME;

/************************************************************/
/* Firmware Update Md Report 4byte command class structs */ 
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        
    BYTE      data4;                        /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REPORT_4BYTE_FRAME;

/************************************************************/
/* Firmware Update Md Request Get command class structs */  
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      manufacturerId1;              /* MSB */
    BYTE      manufacturerId2;              /* LSB */
    BYTE      firmwareId1;                  /* MSB */
    BYTE      firmwareId2;                  /* LSB */
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_FRAME;

/************************************************************/
/* Firmware Update Md Request Report command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /**/
} ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_FRAME;

/************************************************************/
/* Firmware Update Md Status Report command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /**/
} ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_FRAME;

/************************************************************/
/* Firmware Md Get V2 command class structs */              
/************************************************************/
typedef struct _ZW_FIRMWARE_MD_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_FIRMWARE_MD_GET_V2_FRAME;

/************************************************************/
/* Firmware Md Report V2 command class structs */           
/************************************************************/
typedef struct _ZW_FIRMWARE_MD_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      manufacturerId1;              /* MSB */
    BYTE      manufacturerId2;              /* LSB */
    BYTE      firmwareId1;                  /* MSB */
    BYTE      firmwareId2;                  /* LSB */
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_MD_REPORT_V2_FRAME;

/************************************************************/
/* Firmware Update Md Get V2 command class structs */       
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfReports;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
} ZW_FIRMWARE_UPDATE_MD_GET_V2_FRAME;

/************************************************************/
/* Firmware Update Md Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Firmware Update Md Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        /* LSB */
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Firmware Update Md Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        /* LSB */
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Firmware Update Md Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      reportNumber2;                /**/
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        
    BYTE      data4;                        /* LSB */
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Firmware Update Md Request Get V2 command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      manufacturerId1;              /* MSB */
    BYTE      manufacturerId2;              /* LSB */
    BYTE      firmwareId1;                  /* MSB */
    BYTE      firmwareId2;                  /* LSB */
    BYTE      checksum1;                    /* MSB */
    BYTE      checksum2;                    /* LSB */
} ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V2_FRAME;

/************************************************************/
/* Firmware Update Md Request Report V2 command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /**/
} ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_V2_FRAME;

/************************************************************/
/* Firmware Update Md Status Report V2 command class structs */
/************************************************************/
typedef struct _ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /**/
} ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V2_FRAME;

/************************************************************/
/* Geographic Location Get command class structs */         
/************************************************************/
typedef struct _ZW_GEOGRAPHIC_LOCATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_GEOGRAPHIC_LOCATION_GET_FRAME;

/************************************************************/
/* Geographic Location Report command class structs */      
/************************************************************/
typedef struct _ZW_GEOGRAPHIC_LOCATION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      longitudeMinutes;             /**/
    BYTE      level2;                       /* masked byte */
    BYTE      latitudeMinutes;              /**/
} ZW_GEOGRAPHIC_LOCATION_REPORT_FRAME;

/************************************************************/
/* Geographic Location Set command class structs */         
/************************************************************/
typedef struct _ZW_GEOGRAPHIC_LOCATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      longitudeMinutes;             /**/
    BYTE      level2;                       /* masked byte */
    BYTE      latitudeMinutes;              /**/
} ZW_GEOGRAPHIC_LOCATION_SET_FRAME;

/************************************************************/
/* Grouping Name Get command class structs */               
/************************************************************/
typedef struct _ZW_GROUPING_NAME_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
} ZW_GROUPING_NAME_GET_FRAME;

/************************************************************/
/* Grouping Name Report command class structs */            
/************************************************************/
typedef struct _ZW_GROUPING_NAME_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      groupingName1;                /* MSB */
    BYTE      groupingName2;                
    BYTE      groupingName3;                
    BYTE      groupingName4;                
    BYTE      groupingName5;                
    BYTE      groupingName6;                
    BYTE      groupingName7;                
    BYTE      groupingName8;                
    BYTE      groupingName9;                
    BYTE      groupingName10;               
    BYTE      groupingName11;               
    BYTE      groupingName12;               
    BYTE      groupingName13;               
    BYTE      groupingName14;               
    BYTE      groupingName15;               
    BYTE      groupingName16;               /* LSB */
} ZW_GROUPING_NAME_REPORT_FRAME;

/************************************************************/
/* Grouping Name Set command class structs */               
/************************************************************/
typedef struct _ZW_GROUPING_NAME_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      groupingName1;                /* MSB */
    BYTE      groupingName2;                
    BYTE      groupingName3;                
    BYTE      groupingName4;                
    BYTE      groupingName5;                
    BYTE      groupingName6;                
    BYTE      groupingName7;                
    BYTE      groupingName8;                
    BYTE      groupingName9;                
    BYTE      groupingName10;               
    BYTE      groupingName11;               
    BYTE      groupingName12;               
    BYTE      groupingName13;               
    BYTE      groupingName14;               
    BYTE      groupingName15;               
    BYTE      groupingName16;               /* LSB */
} ZW_GROUPING_NAME_SET_FRAME;

/************************************************************/
/* Hail command class structs */                            
/************************************************************/
typedef struct _ZW_HAIL_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_HAIL_FRAME;

/************************************************************/
/* Hrv Control Bypass Get command class structs */          
/************************************************************/
typedef struct _ZW_HRV_CONTROL_BYPASS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_HRV_CONTROL_BYPASS_GET_FRAME;

/************************************************************/
/* Hrv Control Bypass Report command class structs */       
/************************************************************/
typedef struct _ZW_HRV_CONTROL_BYPASS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bypass;                       /**/
} ZW_HRV_CONTROL_BYPASS_REPORT_FRAME;

/************************************************************/
/* Hrv Control Bypass Set command class structs */          
/************************************************************/
typedef struct _ZW_HRV_CONTROL_BYPASS_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bypass;                       /**/
} ZW_HRV_CONTROL_BYPASS_SET_FRAME;

/************************************************************/
/* Hrv Control Mode Get command class structs */            
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_HRV_CONTROL_MODE_GET_FRAME;

/************************************************************/
/* Hrv Control Mode Report command class structs */         
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_HRV_CONTROL_MODE_REPORT_FRAME;

/************************************************************/
/* Hrv Control Mode Set command class structs */            
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_HRV_CONTROL_MODE_SET_FRAME;

/************************************************************/
/* Hrv Control Mode Supported Get command class structs */  
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_HRV_CONTROL_MODE_SUPPORTED_GET_FRAME;

/************************************************************/
/* Hrv Control Mode Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     
} ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Hrv Control Mode Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Hrv Control Mode Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Hrv Control Mode Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_HRV_CONTROL_MODE_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Hrv Control Ventilation Rate Get command class structs */
/************************************************************/
typedef struct _ZW_HRV_CONTROL_VENTILATION_RATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_HRV_CONTROL_VENTILATION_RATE_GET_FRAME;

/************************************************************/
/* Hrv Control Ventilation Rate Report command class structs */
/************************************************************/
typedef struct _ZW_HRV_CONTROL_VENTILATION_RATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      ventilationRate;              /**/
} ZW_HRV_CONTROL_VENTILATION_RATE_REPORT_FRAME;

/************************************************************/
/* Hrv Control Ventilation Rate Set command class structs */
/************************************************************/
typedef struct _ZW_HRV_CONTROL_VENTILATION_RATE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      ventilationRate;              /**/
} ZW_HRV_CONTROL_VENTILATION_RATE_SET_FRAME;

/************************************************************/
/* Hrv Status Get command class structs */                  
/************************************************************/
typedef struct _ZW_HRV_STATUS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      statusParameter;              /**/
} ZW_HRV_STATUS_GET_FRAME;

/************************************************************/
/* Hrv Status Report 1byte command class structs */         
/************************************************************/
typedef struct _ZW_HRV_STATUS_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      statusParameter;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_HRV_STATUS_REPORT_1BYTE_FRAME;

/************************************************************/
/* Hrv Status Report 2byte command class structs */         
/************************************************************/
typedef struct _ZW_HRV_STATUS_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      statusParameter;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_HRV_STATUS_REPORT_2BYTE_FRAME;

/************************************************************/
/* Hrv Status Report 3byte command class structs */         
/************************************************************/
typedef struct _ZW_HRV_STATUS_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      statusParameter;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_HRV_STATUS_REPORT_3BYTE_FRAME;

/************************************************************/
/* Hrv Status Report 4byte command class structs */         
/************************************************************/
typedef struct _ZW_HRV_STATUS_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      statusParameter;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_HRV_STATUS_REPORT_4BYTE_FRAME;

/************************************************************/
/* Hrv Status Supported Get command class structs */        
/************************************************************/
typedef struct _ZW_HRV_STATUS_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_HRV_STATUS_SUPPORTED_GET_FRAME;

/************************************************************/
/* Hrv Status Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_STATUS_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     
} ZW_HRV_STATUS_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Hrv Status Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_STATUS_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_HRV_STATUS_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Hrv Status Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_STATUS_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_HRV_STATUS_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Hrv Status Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_HRV_STATUS_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_HRV_STATUS_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Indicator Get command class structs */                   
/************************************************************/
typedef struct _ZW_INDICATOR_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_INDICATOR_GET_FRAME;

/************************************************************/
/* Indicator Report command class structs */                
/************************************************************/
typedef struct _ZW_INDICATOR_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_INDICATOR_REPORT_FRAME;

/************************************************************/
/* Indicator Set command class structs */                   
/************************************************************/
typedef struct _ZW_INDICATOR_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_INDICATOR_SET_FRAME;

/************************************************************/
/* Ip Configuration Get command class structs */            
/************************************************************/
typedef struct _ZW_IP_CONFIGURATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_IP_CONFIGURATION_GET_FRAME;

/************************************************************/
/* Ip Configuration Release command class structs */        
/************************************************************/
typedef struct _ZW_IP_CONFIGURATION_RELEASE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_IP_CONFIGURATION_RELEASE_FRAME;

/************************************************************/
/* Ip Configuration Renew command class structs */          
/************************************************************/
typedef struct _ZW_IP_CONFIGURATION_RENEW_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_IP_CONFIGURATION_RENEW_FRAME;

/************************************************************/
/* Ip Configuration Report command class structs */         
/************************************************************/
typedef struct _ZW_IP_CONFIGURATION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      ipAddress1;                   /* MSB */
    BYTE      ipAddress2;                   
    BYTE      ipAddress3;                   
    BYTE      ipAddress4;                   /* LSB */
    BYTE      subnetMask1;                  /* MSB */
    BYTE      subnetMask2;                  
    BYTE      subnetMask3;                  
    BYTE      subnetMask4;                  /* LSB */
    BYTE      gateway1;                     /* MSB */
    BYTE      gateway2;                     
    BYTE      gateway3;                     
    BYTE      gateway4;                     /* LSB */
    BYTE      dns11;                        /* MSB */
    BYTE      dns12;                        
    BYTE      dns13;                        
    BYTE      dns14;                        /* LSB */
    BYTE      dns21;                        /* MSB */
    BYTE      dns22;                        
    BYTE      dns23;                        
    BYTE      dns24;                        /* LSB */
    BYTE      leasetime1;                   /* MSB */
    BYTE      leasetime2;                   
    BYTE      leasetime3;                   
    BYTE      leasetime4;                   /* LSB */
} ZW_IP_CONFIGURATION_REPORT_FRAME;

/************************************************************/
/* Ip Configuration Set command class structs */            
/************************************************************/
typedef struct _ZW_IP_CONFIGURATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      ipAddress1;                   /* MSB */
    BYTE      ipAddress2;                   
    BYTE      ipAddress3;                   
    BYTE      ipAddress4;                   /* LSB */
    BYTE      subnetMask1;                  /* MSB */
    BYTE      subnetMask2;                  
    BYTE      subnetMask3;                  
    BYTE      subnetMask4;                  /* LSB */
    BYTE      gateway1;                     /* MSB */
    BYTE      gateway2;                     
    BYTE      gateway3;                     
    BYTE      gateway4;                     /* LSB */
    BYTE      dns11;                        /* MSB */
    BYTE      dns12;                        
    BYTE      dns13;                        
    BYTE      dns14;                        /* LSB */
    BYTE      dns21;                        /* MSB */
    BYTE      dns22;                        
    BYTE      dns23;                        
    BYTE      dns24;                        /* LSB */
} ZW_IP_CONFIGURATION_SET_FRAME;

/************************************************************/
/* Language Get command class structs */                    
/************************************************************/
typedef struct _ZW_LANGUAGE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_LANGUAGE_GET_FRAME;

/************************************************************/
/* Language Report command class structs */                 
/************************************************************/
typedef struct _ZW_LANGUAGE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      language1;                    /* MSB */
    BYTE      language2;                    
    BYTE      language3;                    /* LSB */
    BYTE      country1;                     /* MSB */
    BYTE      country2;                     /* LSB */
} ZW_LANGUAGE_REPORT_FRAME;

/************************************************************/
/* Language Set command class structs */                    
/************************************************************/
typedef struct _ZW_LANGUAGE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      language1;                    /* MSB */
    BYTE      language2;                    
    BYTE      language3;                    /* LSB */
    BYTE      country1;                     /* MSB */
    BYTE      country2;                     /* LSB */
} ZW_LANGUAGE_SET_FRAME;

/************************************************************/
/* Lock Get command class structs */                        
/************************************************************/
typedef struct _ZW_LOCK_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_LOCK_GET_FRAME;

/************************************************************/
/* Lock Report command class structs */                     
/************************************************************/
typedef struct _ZW_LOCK_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      lockState;                    /**/
} ZW_LOCK_REPORT_FRAME;

/************************************************************/
/* Lock Set command class structs */                        
/************************************************************/
typedef struct _ZW_LOCK_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      lockState;                    /**/
} ZW_LOCK_SET_FRAME;

/************************************************************/
/* Manufacturer Specific Get command class structs */       
/************************************************************/
typedef struct _ZW_MANUFACTURER_SPECIFIC_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_MANUFACTURER_SPECIFIC_GET_FRAME;

/************************************************************/
/* Manufacturer Specific Report command class structs */    
/************************************************************/
typedef struct _ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      manufacturerId1;              /* MSB */
    BYTE      manufacturerId2;              /* LSB */
    BYTE      productTypeId1;               /* MSB */
    BYTE      productTypeId2;               /* LSB */
    BYTE      productId1;                   /* MSB */
    BYTE      productId2;                   /* LSB */
} ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME;

/************************************************************/
/* Meter Pulse Get command class structs */                 
/************************************************************/
typedef struct _ZW_METER_PULSE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_PULSE_GET_FRAME;

/************************************************************/
/* Meter Pulse Report command class structs */              
/************************************************************/
typedef struct _ZW_METER_PULSE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      pulseCount1;                  /* MSB */
    BYTE      pulseCount2;                  
    BYTE      pulseCount3;                  
    BYTE      pulseCount4;                  /* LSB */
} ZW_METER_PULSE_REPORT_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Set 1byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;
} ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_1BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Set 2byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;/* MSB */
    BYTE      meterPointAdmNumberCharacter2;/* LSB */
} ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_2BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Set 3byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;/* MSB */
    BYTE      meterPointAdmNumberCharacter2;
    BYTE      meterPointAdmNumberCharacter3;/* LSB */
} ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_3BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Set 4byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;/* MSB */
    BYTE      meterPointAdmNumberCharacter2;
    BYTE      meterPointAdmNumberCharacter3;
    BYTE      meterPointAdmNumberCharacter4;/* LSB */
} ZW_METER_TBL_TABLE_POINT_ADM_NO_SET_4BYTE_FRAME;

/************************************************************/
/* Meter Tbl Status Report variant group structs */         
/************************************************************/
typedef struct _VG_METER_TBL_STATUS_REPORT_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
} VG_METER_TBL_STATUS_REPORT_VG;

/************************************************************/
/* Meter Tbl Status Report 1byte command class structs */   
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      currentOperatingStatus1;      /* MSB */
    BYTE      currentOperatingStatus2;      
    BYTE      currentOperatingStatus3;      /* LSB */
    VG_METER_TBL_STATUS_REPORT_VG variantgroup1;                /**/
} ZW_METER_TBL_STATUS_REPORT_1BYTE_FRAME;

/************************************************************/
/* Meter Tbl Status Report 2byte command class structs */   
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      currentOperatingStatus1;      /* MSB */
    BYTE      currentOperatingStatus2;      
    BYTE      currentOperatingStatus3;      /* LSB */
    VG_METER_TBL_STATUS_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_STATUS_REPORT_VG variantgroup2;                /**/
} ZW_METER_TBL_STATUS_REPORT_2BYTE_FRAME;

/************************************************************/
/* Meter Tbl Status Report 3byte command class structs */   
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      currentOperatingStatus1;      /* MSB */
    BYTE      currentOperatingStatus2;      
    BYTE      currentOperatingStatus3;      /* LSB */
    VG_METER_TBL_STATUS_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_STATUS_REPORT_VG variantgroup2;                /**/
    VG_METER_TBL_STATUS_REPORT_VG variantgroup3;                /**/
} ZW_METER_TBL_STATUS_REPORT_3BYTE_FRAME;

/************************************************************/
/* Meter Tbl Status Report 4byte command class structs */   
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      currentOperatingStatus1;      /* MSB */
    BYTE      currentOperatingStatus2;      
    BYTE      currentOperatingStatus3;      /* LSB */
    VG_METER_TBL_STATUS_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_STATUS_REPORT_VG variantgroup2;                /**/
    VG_METER_TBL_STATUS_REPORT_VG variantgroup3;                /**/
    VG_METER_TBL_STATUS_REPORT_VG variantgroup4;                /**/
} ZW_METER_TBL_STATUS_REPORT_4BYTE_FRAME;

/************************************************************/
/* Meter Tbl Status Date Get command class structs */       
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_DATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      maximumReports;               /**/
    BYTE      startYear1;                   /* MSB */
    BYTE      startYear2;                   /* LSB */
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      startSecondLocalTime;         /**/
    BYTE      stopYear1;                    /* MSB */
    BYTE      stopYear2;                    /* LSB */
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHourLocalTime;            /**/
    BYTE      stopMinuteLocalTime;          /**/
    BYTE      stopSecondLocalTime;          /**/
} ZW_METER_TBL_STATUS_DATE_GET_FRAME;

/************************************************************/
/* Meter Tbl Status Depth Get command class structs */      
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_DEPTH_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      statusEventLogDepth;          /**/
} ZW_METER_TBL_STATUS_DEPTH_GET_FRAME;

/************************************************************/
/* Meter Tbl Status Supported Get command class structs */  
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_TBL_STATUS_SUPPORTED_GET_FRAME;

/************************************************************/
/* Meter Tbl Status Supported Report command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_STATUS_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedOperatingStatus1;    /* MSB */
    BYTE      supportedOperatingStatus2;    
    BYTE      supportedOperatingStatus3;    /* LSB */
    BYTE      statusEventLogDepth;          /**/
} ZW_METER_TBL_STATUS_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Meter Tbl Current Data Get command class structs */      
/************************************************************/
typedef struct _ZW_METER_TBL_CURRENT_DATA_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      datasetRequested1;            /* MSB */
    BYTE      datasetRequested2;            
    BYTE      datasetRequested3;            /* LSB */
} ZW_METER_TBL_CURRENT_DATA_GET_FRAME;

/************************************************************/
/* Meter Tbl Current Data Report variant group structs */   
/************************************************************/
typedef struct _VG_METER_TBL_CURRENT_DATA_REPORT_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      currentValue1;                /* MSB */
    BYTE      currentValue2;                
    BYTE      currentValue3;                
    BYTE      currentValue4;                /* LSB */
} VG_METER_TBL_CURRENT_DATA_REPORT_VG;

/************************************************************/
/* Meter Tbl Current Data Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_CURRENT_DATA_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
} ZW_METER_TBL_CURRENT_DATA_REPORT_1BYTE_FRAME;

/************************************************************/
/* Meter Tbl Current Data Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_CURRENT_DATA_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup2;                /**/
} ZW_METER_TBL_CURRENT_DATA_REPORT_2BYTE_FRAME;

/************************************************************/
/* Meter Tbl Current Data Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_CURRENT_DATA_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup2;                /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup3;                /**/
} ZW_METER_TBL_CURRENT_DATA_REPORT_3BYTE_FRAME;

/************************************************************/
/* Meter Tbl Current Data Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_CURRENT_DATA_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup2;                /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup3;                /**/
    VG_METER_TBL_CURRENT_DATA_REPORT_VG variantgroup4;                /**/
} ZW_METER_TBL_CURRENT_DATA_REPORT_4BYTE_FRAME;

/************************************************************/
/* Meter Tbl Historical Data Get command class structs */   
/************************************************************/
typedef struct _ZW_METER_TBL_HISTORICAL_DATA_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      maximumReports;               /**/
    BYTE      historicalDatasetRequested1;  /* MSB */
    BYTE      historicalDatasetRequested2;  
    BYTE      historicalDatasetRequested3;  /* LSB */
    BYTE      startYear1;                   /* MSB */
    BYTE      startYear2;                   /* LSB */
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      startSecondLocalTime;         /**/
    BYTE      stopYear1;                    /* MSB */
    BYTE      stopYear2;                    /* LSB */
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHourLocalTime;            /**/
    BYTE      stopMinuteLocalTime;          /**/
    BYTE      stopSecondLocalTime;          /**/
} ZW_METER_TBL_HISTORICAL_DATA_GET_FRAME;

/************************************************************/
/* Meter Tbl Historical Data Report variant group structs */
/************************************************************/
typedef struct _VG_METER_TBL_HISTORICAL_DATA_REPORT_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      historicalValue1;             /* MSB */
    BYTE      historicalValue2;             
    BYTE      historicalValue3;             
    BYTE      historicalValue4;             /* LSB */
} VG_METER_TBL_HISTORICAL_DATA_REPORT_VG;

/************************************************************/
/* Meter Tbl Historical Data Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_HISTORICAL_DATA_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
} ZW_METER_TBL_HISTORICAL_DATA_REPORT_1BYTE_FRAME;

/************************************************************/
/* Meter Tbl Historical Data Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_HISTORICAL_DATA_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup2;                /**/
} ZW_METER_TBL_HISTORICAL_DATA_REPORT_2BYTE_FRAME;

/************************************************************/
/* Meter Tbl Historical Data Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_HISTORICAL_DATA_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup2;                /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup3;                /**/
} ZW_METER_TBL_HISTORICAL_DATA_REPORT_3BYTE_FRAME;

/************************************************************/
/* Meter Tbl Historical Data Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_HISTORICAL_DATA_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup2;                /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup3;                /**/
    VG_METER_TBL_HISTORICAL_DATA_REPORT_VG variantgroup4;                /**/
} ZW_METER_TBL_HISTORICAL_DATA_REPORT_4BYTE_FRAME;

/************************************************************/
/* Meter Tbl Report command class structs */                
/************************************************************/
typedef struct _ZW_METER_TBL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      datasetSupported1;            /* MSB */
    BYTE      datasetSupported2;            
    BYTE      datasetSupported3;            /* LSB */
    BYTE      datasetHistorySupported1;     /* MSB */
    BYTE      datasetHistorySupported2;     
    BYTE      datasetHistorySupported3;     /* LSB */
    BYTE      dataHistorySupported1;        /* MSB */
    BYTE      dataHistorySupported2;        
    BYTE      dataHistorySupported3;        /* LSB */
} ZW_METER_TBL_REPORT_FRAME;

/************************************************************/
/* Meter Tbl Table Capability Get command class structs */  
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_CAPABILITY_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_TBL_TABLE_CAPABILITY_GET_FRAME;

/************************************************************/
/* Meter Tbl Table Id Get command class structs */          
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_ID_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_TBL_TABLE_ID_GET_FRAME;

/************************************************************/
/* Meter Tbl Table Id Report 1byte command class structs */ 
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_ID_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterIdCharacter1;            
} ZW_METER_TBL_TABLE_ID_REPORT_1BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Id Report 2byte command class structs */ 
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_ID_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterIdCharacter1;            /* MSB */
    BYTE      meterIdCharacter2;            /* LSB */
} ZW_METER_TBL_TABLE_ID_REPORT_2BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Id Report 3byte command class structs */ 
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_ID_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterIdCharacter1;            /* MSB */
    BYTE      meterIdCharacter2;            
    BYTE      meterIdCharacter3;            /* LSB */
} ZW_METER_TBL_TABLE_ID_REPORT_3BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Id Report 4byte command class structs */ 
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_ID_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterIdCharacter1;            /* MSB */
    BYTE      meterIdCharacter2;            
    BYTE      meterIdCharacter3;            
    BYTE      meterIdCharacter4;            /* LSB */
} ZW_METER_TBL_TABLE_ID_REPORT_4BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Get command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_TBL_TABLE_POINT_ADM_NO_GET_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;
} ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_1BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;/* MSB */
    BYTE      meterPointAdmNumberCharacter2;/* LSB */
} ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_2BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;/* MSB */
    BYTE      meterPointAdmNumberCharacter2;
    BYTE      meterPointAdmNumberCharacter3;/* LSB */
} ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_3BYTE_FRAME;

/************************************************************/
/* Meter Tbl Table Point Adm No Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      meterPointAdmNumberCharacter1;/* MSB */
    BYTE      meterPointAdmNumberCharacter2;
    BYTE      meterPointAdmNumberCharacter3;
    BYTE      meterPointAdmNumberCharacter4;/* LSB */
} ZW_METER_TBL_TABLE_POINT_ADM_NO_REPORT_4BYTE_FRAME;

/************************************************************/
/* Meter Tbl Push Configuration Get command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_PUSH_CONFIGURATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_TBL_PUSH_CONFIGURATION_GET_FRAME;

/************************************************************/
/* Meter Tbl Push Configuration Report command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_PUSH_CONFIGURATION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      pushDataset1;                 /* MSB */
    BYTE      pushDataset2;                 
    BYTE      pushDataset3;                 /* LSB */
    BYTE      intervalMonths;               /**/
    BYTE      intervalDays;                 /**/
    BYTE      intervalHours;                /**/
    BYTE      intervalMinutes;              /**/
    BYTE      pushNodeId;                   /**/
} ZW_METER_TBL_PUSH_CONFIGURATION_REPORT_FRAME;

/************************************************************/
/* Meter Tbl Push Configuration Set command class structs */
/************************************************************/
typedef struct _ZW_METER_TBL_PUSH_CONFIGURATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      pushDataset1;                 /* MSB */
    BYTE      pushDataset2;                 
    BYTE      pushDataset3;                 /* LSB */
    BYTE      intervalMonths;               /**/
    BYTE      intervalDays;                 /**/
    BYTE      intervalHours;                /**/
    BYTE      intervalMinutes;              /**/
    BYTE      pushNodeId;                   /**/
} ZW_METER_TBL_PUSH_CONFIGURATION_SET_FRAME;

/************************************************************/
/* Meter Get command class structs */                       
/************************************************************/
typedef struct _ZW_METER_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_GET_FRAME;

/************************************************************/
/* Meter Report 1byte command class structs */              
/************************************************************/
typedef struct _ZW_METER_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      meterType;                    /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      meterValue1;                  
} ZW_METER_REPORT_1BYTE_FRAME;

/************************************************************/
/* Meter Report 2byte command class structs */              
/************************************************************/
typedef struct _ZW_METER_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      meterType;                    /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  /* LSB */
} ZW_METER_REPORT_2BYTE_FRAME;

/************************************************************/
/* Meter Report 3byte command class structs */              
/************************************************************/
typedef struct _ZW_METER_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      meterType;                    /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  
    BYTE      meterValue3;                  /* LSB */
} ZW_METER_REPORT_3BYTE_FRAME;

/************************************************************/
/* Meter Report 4byte command class structs */              
/************************************************************/
typedef struct _ZW_METER_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      meterType;                    /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  
    BYTE      meterValue3;                  
    BYTE      meterValue4;                  /* LSB */
} ZW_METER_REPORT_4BYTE_FRAME;

/************************************************************/
/* Meter Get V2 command class structs */                    
/************************************************************/
typedef struct _ZW_METER_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_METER_GET_V2_FRAME;

/************************************************************/
/* Meter Report 1byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          
} ZW_METER_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Meter Report 2byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  /* LSB */
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          /* MSB */
    BYTE      previousMeterValue2;          /* LSB */
} ZW_METER_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Meter Report 3byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  
    BYTE      meterValue3;                  /* LSB */
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          /* MSB */
    BYTE      previousMeterValue2;          
    BYTE      previousMeterValue3;          /* LSB */
} ZW_METER_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Meter Report 4byte V2 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  
    BYTE      meterValue3;                  
    BYTE      meterValue4;                  /* LSB */
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          /* MSB */
    BYTE      previousMeterValue2;          
    BYTE      previousMeterValue3;          
    BYTE      previousMeterValue4;          /* LSB */
} ZW_METER_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Meter Reset V2 command class structs */                  
/************************************************************/
typedef struct _ZW_METER_RESET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_RESET_V2_FRAME;

/************************************************************/
/* Meter Supported Get V2 command class structs */          
/************************************************************/
typedef struct _ZW_METER_SUPPORTED_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_SUPPORTED_GET_V2_FRAME;

/************************************************************/
/* Meter Supported Report V2 command class structs */       
/************************************************************/
typedef struct _ZW_METER_SUPPORTED_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
} ZW_METER_SUPPORTED_REPORT_V2_FRAME;

/************************************************************/
/* Meter Get V3 command class structs */                    
/************************************************************/
typedef struct _ZW_METER_GET_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_METER_GET_V3_FRAME;

/************************************************************/
/* Meter Report 1byte V3 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_1BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          
} ZW_METER_REPORT_1BYTE_V3_FRAME;

/************************************************************/
/* Meter Report 2byte V3 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_2BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  /* LSB */
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          /* MSB */
    BYTE      previousMeterValue2;          /* LSB */
} ZW_METER_REPORT_2BYTE_V3_FRAME;

/************************************************************/
/* Meter Report 3byte V3 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_3BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  
    BYTE      meterValue3;                  /* LSB */
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          /* MSB */
    BYTE      previousMeterValue2;          
    BYTE      previousMeterValue3;          /* LSB */
} ZW_METER_REPORT_3BYTE_V3_FRAME;

/************************************************************/
/* Meter Report 4byte V3 command class structs */           
/************************************************************/
typedef struct _ZW_METER_REPORT_4BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      meterValue1;                  /* MSB */
    BYTE      meterValue2;                  
    BYTE      meterValue3;                  
    BYTE      meterValue4;                  /* LSB */
    BYTE      deltaTime1;                   /* MSB */
    BYTE      deltaTime2;                   /* LSB */
    BYTE      previousMeterValue1;          /* MSB */
    BYTE      previousMeterValue2;          
    BYTE      previousMeterValue3;          
    BYTE      previousMeterValue4;          /* LSB */
} ZW_METER_REPORT_4BYTE_V3_FRAME;

/************************************************************/
/* Meter Reset V3 command class structs */                  
/************************************************************/
typedef struct _ZW_METER_RESET_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_RESET_V3_FRAME;

/************************************************************/
/* Meter Supported Get V3 command class structs */          
/************************************************************/
typedef struct _ZW_METER_SUPPORTED_GET_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_METER_SUPPORTED_GET_V3_FRAME;

/************************************************************/
/* Meter Supported Report V3 command class structs */       
/************************************************************/
typedef struct _ZW_METER_SUPPORTED_REPORT_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      scaleSupported;               /**/
} ZW_METER_SUPPORTED_REPORT_V3_FRAME;

/************************************************************/
/* Move To Position Get command class structs */            
/************************************************************/
typedef struct _ZW_MOVE_TO_POSITION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_MOVE_TO_POSITION_GET_FRAME;

/************************************************************/
/* Move To Position Report command class structs */         
/************************************************************/
typedef struct _ZW_MOVE_TO_POSITION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_MOVE_TO_POSITION_REPORT_FRAME;

/************************************************************/
/* Move To Position Set command class structs */            
/************************************************************/
typedef struct _ZW_MOVE_TO_POSITION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_MOVE_TO_POSITION_SET_FRAME;

/************************************************************/
/* Multi Channel Association Get V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_GET_V2_FRAME;

/************************************************************/
/* Multi Channel Association Groupings Get V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V2_FRAME;

/************************************************************/
/* Multi Channel Association Groupings Report V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedGroupings;           /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2_FRAME;

/************************************************************/
/* Multi Channel Association Remove V2 variant group structs */
/************************************************************/
typedef struct _VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG_
{
    BYTE      multiChannelNodeId;           /**/
    BYTE      properties1;                  /* masked byte */
} VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG;

/************************************************************/
/* Multi Channel Association Remove 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup1;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_1BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Remove 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup2;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_2BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Remove 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup3;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_3BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Remove 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup3;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REMOVE_V2_VG variantgroup4;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_4BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Report V2 variant group structs */
/************************************************************/
typedef struct _VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG_
{
    BYTE      multiChannelNodeId;           /**/
    BYTE      properties1;                  /* masked byte */
} VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG;

/************************************************************/
/* Multi Channel Association Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup1;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup2;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup3;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup3;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_REPORT_V2_VG variantgroup4;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Set V2 variant group structs */
/************************************************************/
typedef struct _VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG_
{
    BYTE      multiChannelNodeId;           /**/
    BYTE      properties1;                  /* masked byte */
} VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG;

/************************************************************/
/* Multi Channel Association Set 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup1;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Set 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_SET_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup2;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_SET_2BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Set 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_SET_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup3;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_SET_3BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Association Set 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_ASSOCIATION_SET_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup3;                /**/
    VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG variantgroup4;                /**/
} ZW_MULTI_CHANNEL_ASSOCIATION_SET_4BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Capability Get V2 command class structs */ 
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_CAPABILITY_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_MULTI_CHANNEL_CAPABILITY_GET_V2_FRAME;

/************************************************************/
/* Multi Channel Capability Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_CAPABILITY_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                
} ZW_MULTI_CHANNEL_CAPABILITY_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Capability Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_CAPABILITY_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                /* LSB */
} ZW_MULTI_CHANNEL_CAPABILITY_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Capability Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_CAPABILITY_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                
    BYTE      commandClass3;                /* LSB */
} ZW_MULTI_CHANNEL_CAPABILITY_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel Capability Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_CAPABILITY_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                
    BYTE      commandClass3;                
    BYTE      commandClass4;                /* LSB */
} ZW_MULTI_CHANNEL_CAPABILITY_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel End Point Find V2 command class structs */ 
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_END_POINT_FIND_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
} ZW_MULTI_CHANNEL_END_POINT_FIND_V2_FRAME;

/************************************************************/
/* Multi Channel End Point Find Report V2 variant group structs */
/************************************************************/
typedef struct _VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG_
{
    BYTE      properties1;                  /* masked byte */
} VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG;

/************************************************************/
/* Multi Channel End Point Find Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup1;                /**/
} ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel End Point Find Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup2;                /**/
} ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel End Point Find Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup3;                /**/
} ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel End Point Find Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup1;                /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup2;                /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup3;                /**/
    VG_MULTI_CHANNEL_END_POINT_FIND_REPORT_V2_VG variantgroup4;                /**/
} ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Multi Channel End Point Get V2 command class structs */  
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_END_POINT_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_MULTI_CHANNEL_END_POINT_GET_V2_FRAME;

/************************************************************/
/* Multi Channel End Point Report V2 command class structs */
/************************************************************/
typedef struct _ZW_MULTI_CHANNEL_END_POINT_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
} ZW_MULTI_CHANNEL_END_POINT_REPORT_V2_FRAME;

/************************************************************/
/* Multi Instance Get V2 command class structs */           
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      commandClass;                 /**/
} ZW_MULTI_INSTANCE_GET_V2_FRAME;

/************************************************************/
/* Multi Instance Report V2 command class structs */        
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      commandClass;                 /**/
    BYTE      properties1;                  /* masked byte */
} ZW_MULTI_INSTANCE_REPORT_V2_FRAME;

/************************************************************/
/* Multi Instance Association Get command class structs */  
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_GET_FRAME;

/************************************************************/
/* Multi Instance Association Groupings Get command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_GROUPINGS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_MULTI_INSTANCE_ASSOCIATION_GROUPINGS_GET_FRAME;

/************************************************************/
/* Multi Instance Association Groupings Report command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_GROUPINGS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedGroupings;           /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_GROUPINGS_REPORT_FRAME;

/************************************************************/
/* Multi Instance Association Remove variant group structs */
/************************************************************/
typedef struct _VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG_
{
    BYTE      multiInstanceNodeId;          /**/
    BYTE      instance;                     /**/
} VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG;

/************************************************************/
/* Multi Instance Association Remove 1byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup1;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_1BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Remove 2byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup2;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_2BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Remove 3byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup2;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup3;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_3BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Remove 4byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup2;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup3;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REMOVE_VG variantgroup4;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REMOVE_4BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Report variant group structs */
/************************************************************/
typedef struct _VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG_
{
    BYTE      multiInstanceNodeId;          /**/
    BYTE      instance;                     /**/
} VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG;

/************************************************************/
/* Multi Instance Association Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup1;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_1BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup2;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_2BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup2;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup3;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_3BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      maxNodesSupported;            /**/
    BYTE      reportsToFollow;              /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup2;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup3;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_REPORT_VG variantgroup4;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_REPORT_4BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Set variant group structs */  
/************************************************************/
typedef struct _VG_MULTI_INSTANCE_ASSOCIATION_SET_VG_
{
    BYTE      multiInstanceNodeId;          /**/
    BYTE      instance;                     /**/
} VG_MULTI_INSTANCE_ASSOCIATION_SET_VG;

/************************************************************/
/* Multi Instance Association Set 1byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup1;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_SET_1BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Set 2byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup2;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_SET_2BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Set 3byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup2;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup3;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_SET_3BYTE_FRAME;

/************************************************************/
/* Multi Instance Association Set 4byte command class structs */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_ASSOCIATION_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
    BYTE      nodeId1;                      /* MSB */
    BYTE      nodeId2;                      
    BYTE      nodeId3;                      
    BYTE      nodeId4;                      /* LSB */
    BYTE      marker;                       /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup1;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup2;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup3;                /**/
    VG_MULTI_INSTANCE_ASSOCIATION_SET_VG variantgroup4;                /**/
} ZW_MULTI_INSTANCE_ASSOCIATION_SET_4BYTE_FRAME;

/************************************************************/
/* Multi Instance Get command class structs */              
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      commandClass;                 /**/
} ZW_MULTI_INSTANCE_GET_FRAME;

/************************************************************/
/* Multi Instance Report command class structs */           
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      commandClass;                 /**/
    BYTE      instances;                    /**/
} ZW_MULTI_INSTANCE_REPORT_FRAME;

/************************************************************/
/* Node Info Get command class structs */                   
/************************************************************/
typedef struct _ZW_NODE_INFO_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
} ZW_NODE_INFO_GET_FRAME;

/************************************************************/
/* Node Info Report 1byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_INFO_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                
} ZW_NODE_INFO_REPORT_1BYTE_FRAME;

/************************************************************/
/* Node Info Report 2byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_INFO_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                /* LSB */
} ZW_NODE_INFO_REPORT_2BYTE_FRAME;

/************************************************************/
/* Node Info Report 3byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_INFO_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                
    BYTE      commandClass3;                /* LSB */
} ZW_NODE_INFO_REPORT_3BYTE_FRAME;

/************************************************************/
/* Node Info Report 4byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_INFO_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                
    BYTE      commandClass3;                
    BYTE      commandClass4;                /* LSB */
} ZW_NODE_INFO_REPORT_4BYTE_FRAME;

/************************************************************/
/* Node Info Reduced Get command class structs */           
/************************************************************/
typedef struct _ZW_NODE_INFO_REDUCED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      nodeid;                       /**/
} ZW_NODE_INFO_REDUCED_GET_FRAME;

/************************************************************/
/* Node Info Reduced Report 1byte command class structs */  
/************************************************************/
typedef struct _ZW_NODE_INFO_REDUCED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      nodeid;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      reserved;                     /**/
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                
} ZW_NODE_INFO_REDUCED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Node Info Reduced Report 2byte command class structs */  
/************************************************************/
typedef struct _ZW_NODE_INFO_REDUCED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      nodeid;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      reserved;                     /**/
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                /* LSB */
} ZW_NODE_INFO_REDUCED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Node Info Reduced Report 3byte command class structs */  
/************************************************************/
typedef struct _ZW_NODE_INFO_REDUCED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      nodeid;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      reserved;                     /**/
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                
    BYTE      commandClass3;                /* LSB */
} ZW_NODE_INFO_REDUCED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Node Info Reduced Report 4byte command class structs */  
/************************************************************/
typedef struct _ZW_NODE_INFO_REDUCED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      nodeid;                       /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      reserved;                     /**/
    BYTE      basicDeviceClass;             /**/
    BYTE      genericDeviceClass;           /**/
    BYTE      specificDeviceClass;          /**/
    BYTE      commandClass1;                /* MSB */
    BYTE      commandClass2;                
    BYTE      commandClass3;                
    BYTE      commandClass4;                /* LSB */
} ZW_NODE_INFO_REDUCED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Node List Get command class structs */                   
/************************************************************/
typedef struct _ZW_NODE_LIST_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
} ZW_NODE_LIST_GET_FRAME;

/************************************************************/
/* Node List Report 1byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_LIST_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      nodeListController;           /**/
    BYTE      nodelistdata1;                
} ZW_NODE_LIST_REPORT_1BYTE_FRAME;

/************************************************************/
/* Node List Report 2byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_LIST_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      nodeListController;           /**/
    BYTE      nodelistdata1;                /* MSB */
    BYTE      nodelistdata2;                /* LSB */
} ZW_NODE_LIST_REPORT_2BYTE_FRAME;

/************************************************************/
/* Node List Report 3byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_LIST_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      nodeListController;           /**/
    BYTE      nodelistdata1;                /* MSB */
    BYTE      nodelistdata2;                
    BYTE      nodelistdata3;                /* LSB */
} ZW_NODE_LIST_REPORT_3BYTE_FRAME;

/************************************************************/
/* Node List Report 4byte command class structs */          
/************************************************************/
typedef struct _ZW_NODE_LIST_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      nodeListController;           /**/
    BYTE      nodelistdata1;                /* MSB */
    BYTE      nodelistdata2;                
    BYTE      nodelistdata3;                
    BYTE      nodelistdata4;                /* LSB */
} ZW_NODE_LIST_REPORT_4BYTE_FRAME;

/************************************************************/
/* Learn Mode Set command class structs */                  
/************************************************************/
typedef struct _ZW_LEARN_MODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      mode;                         /**/
} ZW_LEARN_MODE_SET_FRAME;

/************************************************************/
/* Learn Mode Status command class structs */               
/************************************************************/
typedef struct _ZW_LEARN_MODE_STATUS_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
} ZW_LEARN_MODE_STATUS_FRAME;

/************************************************************/
/* Default Set command class structs */                     
/************************************************************/
typedef struct _ZW_DEFAULT_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
} ZW_DEFAULT_SET_FRAME;

/************************************************************/
/* Default Set Complete command class structs */            
/************************************************************/
typedef struct _ZW_DEFAULT_SET_COMPLETE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
} ZW_DEFAULT_SET_COMPLETE_FRAME;

/************************************************************/
/* Node Add command class structs */                        
/************************************************************/
typedef struct _ZW_NODE_ADD_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      mode;                         /**/
} ZW_NODE_ADD_FRAME;

/************************************************************/
/* Node Add Status 1byte command class structs */           
/************************************************************/
typedef struct _ZW_NODE_ADD_STATUS_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      newnodeid;                    /**/
    BYTE      nodeinfolength;               /**/
    BYTE      nodeinfo1;                    
} ZW_NODE_ADD_STATUS_1BYTE_FRAME;

/************************************************************/
/* Node Add Status 2byte command class structs */           
/************************************************************/
typedef struct _ZW_NODE_ADD_STATUS_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      newnodeid;                    /**/
    BYTE      nodeinfolength;               /**/
    BYTE      nodeinfo1;                    /* MSB */
    BYTE      nodeinfo2;                    /* LSB */
} ZW_NODE_ADD_STATUS_2BYTE_FRAME;

/************************************************************/
/* Node Add Status 3byte command class structs */           
/************************************************************/
typedef struct _ZW_NODE_ADD_STATUS_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      newnodeid;                    /**/
    BYTE      nodeinfolength;               /**/
    BYTE      nodeinfo1;                    /* MSB */
    BYTE      nodeinfo2;                    
    BYTE      nodeinfo3;                    /* LSB */
} ZW_NODE_ADD_STATUS_3BYTE_FRAME;

/************************************************************/
/* Node Add Status 4byte command class structs */           
/************************************************************/
typedef struct _ZW_NODE_ADD_STATUS_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      newnodeid;                    /**/
    BYTE      nodeinfolength;               /**/
    BYTE      nodeinfo1;                    /* MSB */
    BYTE      nodeinfo2;                    
    BYTE      nodeinfo3;                    
    BYTE      nodeinfo4;                    /* LSB */
} ZW_NODE_ADD_STATUS_4BYTE_FRAME;

/************************************************************/
/* Node Remove command class structs */                     
/************************************************************/
typedef struct _ZW_NODE_REMOVE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      mode;                         /**/
} ZW_NODE_REMOVE_FRAME;

/************************************************************/
/* Node Remove Status command class structs */              
/************************************************************/
typedef struct _ZW_NODE_REMOVE_STATUS_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seqNo;                        /**/
    BYTE      status;                       /**/
    BYTE      nodeid;                       /**/
} ZW_NODE_REMOVE_STATUS_FRAME;

/************************************************************/
/* Node Naming Node Location Report command class structs */
/************************************************************/
typedef struct _ZW_NODE_NAMING_NODE_LOCATION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      nodeLocationChar1;            /* MSB */
    BYTE      nodeLocationChar2;            
    BYTE      nodeLocationChar3;            
    BYTE      nodeLocationChar4;            
    BYTE      nodeLocationChar5;            
    BYTE      nodeLocationChar6;            
    BYTE      nodeLocationChar7;            
    BYTE      nodeLocationChar8;            
    BYTE      nodeLocationChar9;            
    BYTE      nodeLocationChar10;           
    BYTE      nodeLocationChar11;           
    BYTE      nodeLocationChar12;           
    BYTE      nodeLocationChar13;           
    BYTE      nodeLocationChar14;           
    BYTE      nodeLocationChar15;           
    BYTE      nodeLocationChar16;           /* LSB */
} ZW_NODE_NAMING_NODE_LOCATION_REPORT_FRAME;

/************************************************************/
/* Node Naming Node Location Set command class structs */   
/************************************************************/
typedef struct _ZW_NODE_NAMING_NODE_LOCATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      nodeLocationChar1;            /* MSB */
    BYTE      nodeLocationChar2;            
    BYTE      nodeLocationChar3;            
    BYTE      nodeLocationChar4;            
    BYTE      nodeLocationChar5;            
    BYTE      nodeLocationChar6;            
    BYTE      nodeLocationChar7;            
    BYTE      nodeLocationChar8;            
    BYTE      nodeLocationChar9;            
    BYTE      nodeLocationChar10;           
    BYTE      nodeLocationChar11;           
    BYTE      nodeLocationChar12;           
    BYTE      nodeLocationChar13;           
    BYTE      nodeLocationChar14;           
    BYTE      nodeLocationChar15;           
    BYTE      nodeLocationChar16;           /* LSB */
} ZW_NODE_NAMING_NODE_LOCATION_SET_FRAME;

/************************************************************/
/* Node Naming Node Location Get command class structs */   
/************************************************************/
typedef struct _ZW_NODE_NAMING_NODE_LOCATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_NODE_NAMING_NODE_LOCATION_GET_FRAME;

/************************************************************/
/* Node Naming Node Name Get command class structs */       
/************************************************************/
typedef struct _ZW_NODE_NAMING_NODE_NAME_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_NODE_NAMING_NODE_NAME_GET_FRAME;

/************************************************************/
/* Node Naming Node Name Report command class structs */    
/************************************************************/
typedef struct _ZW_NODE_NAMING_NODE_NAME_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      nodeNameChar1;                /* MSB */
    BYTE      nodeNameChar2;                
    BYTE      nodeNameChar3;                
    BYTE      nodeNameChar4;                
    BYTE      nodeNameChar5;                
    BYTE      nodeNameChar6;                
    BYTE      nodeNameChar7;                
    BYTE      nodeNameChar8;                
    BYTE      nodeNameChar9;                
    BYTE      nodeNameChar10;               
    BYTE      nodeNameChar11;               
    BYTE      nodeNameChar12;               
    BYTE      nodeNameChar13;               
    BYTE      nodeNameChar14;               
    BYTE      nodeNameChar15;               
    BYTE      nodeNameChar16;               /* LSB */
} ZW_NODE_NAMING_NODE_NAME_REPORT_FRAME;

/************************************************************/
/* Node Naming Node Name Set command class structs */       
/************************************************************/
typedef struct _ZW_NODE_NAMING_NODE_NAME_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      nodeNameChar1;                /* MSB */
    BYTE      nodeNameChar2;                
    BYTE      nodeNameChar3;                
    BYTE      nodeNameChar4;                
    BYTE      nodeNameChar5;                
    BYTE      nodeNameChar6;                
    BYTE      nodeNameChar7;                
    BYTE      nodeNameChar8;                
    BYTE      nodeNameChar9;                
    BYTE      nodeNameChar10;               
    BYTE      nodeNameChar11;               
    BYTE      nodeNameChar12;               
    BYTE      nodeNameChar13;               
    BYTE      nodeNameChar14;               
    BYTE      nodeNameChar15;               
    BYTE      nodeNameChar16;               /* LSB */
} ZW_NODE_NAMING_NODE_NAME_SET_FRAME;

/************************************************************/
/* Powerlevel Get command class structs */                  
/************************************************************/
typedef struct _ZW_POWERLEVEL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_POWERLEVEL_GET_FRAME;

/************************************************************/
/* Powerlevel Report command class structs */               
/************************************************************/
typedef struct _ZW_POWERLEVEL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      powerLevel;                   /**/
    BYTE      timeout;                      /**/
} ZW_POWERLEVEL_REPORT_FRAME;

/************************************************************/
/* Powerlevel Set command class structs */                  
/************************************************************/
typedef struct _ZW_POWERLEVEL_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      powerLevel;                   /**/
    BYTE      timeout;                      /**/
} ZW_POWERLEVEL_SET_FRAME;

/************************************************************/
/* Powerlevel Test Node Get command class structs */        
/************************************************************/
typedef struct _ZW_POWERLEVEL_TEST_NODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_POWERLEVEL_TEST_NODE_GET_FRAME;

/************************************************************/
/* Powerlevel Test Node Report command class structs */     
/************************************************************/
typedef struct _ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      testNodeid;                   /**/
    BYTE      statusOfOperation;            /**/
    BYTE      testFrameCount1;              /* MSB */
    BYTE      testFrameCount2;              /* LSB */
} ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME;

/************************************************************/
/* Powerlevel Test Node Set command class structs */        
/************************************************************/
typedef struct _ZW_POWERLEVEL_TEST_NODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      testNodeid;                   /**/
    BYTE      powerLevel;                   /**/
    BYTE      testFrameCount1;              /* MSB */
    BYTE      testFrameCount2;              /* LSB */
} ZW_POWERLEVEL_TEST_NODE_SET_FRAME;

/************************************************************/
/* Cmd Encapsulation 1byte command class structs */         
/************************************************************/
typedef struct _ZW_CMD_ENCAPSULATION_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        
} ZW_CMD_ENCAPSULATION_1BYTE_FRAME;

/************************************************************/
/* Cmd Encapsulation 2byte command class structs */         
/************************************************************/
typedef struct _ZW_CMD_ENCAPSULATION_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        /* LSB */
} ZW_CMD_ENCAPSULATION_2BYTE_FRAME;

/************************************************************/
/* Cmd Encapsulation 3byte command class structs */         
/************************************************************/
typedef struct _ZW_CMD_ENCAPSULATION_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        /* LSB */
} ZW_CMD_ENCAPSULATION_3BYTE_FRAME;

/************************************************************/
/* Cmd Encapsulation 4byte command class structs */         
/************************************************************/
typedef struct _ZW_CMD_ENCAPSULATION_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        
    BYTE      data4;                        /* LSB */
} ZW_CMD_ENCAPSULATION_4BYTE_FRAME;

/************************************************************/
/* Prepayment Balance Get command class structs */          
/************************************************************/
typedef struct _ZW_PREPAYMENT_BALANCE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_PREPAYMENT_BALANCE_GET_FRAME;

/************************************************************/
/* Prepayment Balance Report command class structs */       
/************************************************************/
typedef struct _ZW_PREPAYMENT_BALANCE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      balanceValue1;                /* MSB */
    BYTE      balanceValue2;                
    BYTE      balanceValue3;                
    BYTE      balanceValue4;                /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      debt1;                        /* MSB */
    BYTE      debt2;                        
    BYTE      debt3;                        
    BYTE      debt4;                        /* LSB */
    BYTE      properties4;                  /* masked byte */
    BYTE      emerCredit1;                  /* MSB */
    BYTE      emerCredit2;                  
    BYTE      emerCredit3;                  
    BYTE      emerCredit4;                  /* LSB */
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      debtRecoveryPercentage;       /**/
} ZW_PREPAYMENT_BALANCE_REPORT_FRAME;

/************************************************************/
/* Prepayment Supported Get command class structs */        
/************************************************************/
typedef struct _ZW_PREPAYMENT_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_PREPAYMENT_SUPPORTED_GET_FRAME;

/************************************************************/
/* Prepayment Supported Report command class structs */     
/************************************************************/
typedef struct _ZW_PREPAYMENT_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
} ZW_PREPAYMENT_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Proprietary Get 1byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_GET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        
} ZW_PROPRIETARY_GET_1BYTE_FRAME;

/************************************************************/
/* Proprietary Get 2byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_GET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        /* LSB */
} ZW_PROPRIETARY_GET_2BYTE_FRAME;

/************************************************************/
/* Proprietary Get 3byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_GET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        /* LSB */
} ZW_PROPRIETARY_GET_3BYTE_FRAME;

/************************************************************/
/* Proprietary Get 4byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_GET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        
    BYTE      data4;                        /* LSB */
} ZW_PROPRIETARY_GET_4BYTE_FRAME;

/************************************************************/
/* Proprietary Report 1byte command class structs */        
/************************************************************/
typedef struct _ZW_PROPRIETARY_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        
} ZW_PROPRIETARY_REPORT_1BYTE_FRAME;

/************************************************************/
/* Proprietary Report 2byte command class structs */        
/************************************************************/
typedef struct _ZW_PROPRIETARY_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        /* LSB */
} ZW_PROPRIETARY_REPORT_2BYTE_FRAME;

/************************************************************/
/* Proprietary Report 3byte command class structs */        
/************************************************************/
typedef struct _ZW_PROPRIETARY_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        /* LSB */
} ZW_PROPRIETARY_REPORT_3BYTE_FRAME;

/************************************************************/
/* Proprietary Report 4byte command class structs */        
/************************************************************/
typedef struct _ZW_PROPRIETARY_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        
    BYTE      data4;                        /* LSB */
} ZW_PROPRIETARY_REPORT_4BYTE_FRAME;

/************************************************************/
/* Proprietary Set 1byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        
} ZW_PROPRIETARY_SET_1BYTE_FRAME;

/************************************************************/
/* Proprietary Set 2byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        /* LSB */
} ZW_PROPRIETARY_SET_2BYTE_FRAME;

/************************************************************/
/* Proprietary Set 3byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        /* LSB */
} ZW_PROPRIETARY_SET_3BYTE_FRAME;

/************************************************************/
/* Proprietary Set 4byte command class structs */           
/************************************************************/
typedef struct _ZW_PROPRIETARY_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      data1;                        /* MSB */
    BYTE      data2;                        
    BYTE      data3;                        
    BYTE      data4;                        /* LSB */
} ZW_PROPRIETARY_SET_4BYTE_FRAME;

/************************************************************/
/* Protection Get command class structs */                  
/************************************************************/
typedef struct _ZW_PROTECTION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_PROTECTION_GET_FRAME;

/************************************************************/
/* Protection Report command class structs */               
/************************************************************/
typedef struct _ZW_PROTECTION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      protectionState;              /**/
} ZW_PROTECTION_REPORT_FRAME;

/************************************************************/
/* Protection Set command class structs */                  
/************************************************************/
typedef struct _ZW_PROTECTION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      protectionState;              /**/
} ZW_PROTECTION_SET_FRAME;

/************************************************************/
/* Protection Ec Get V2 command class structs */            
/************************************************************/
typedef struct _ZW_PROTECTION_EC_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_PROTECTION_EC_GET_V2_FRAME;

/************************************************************/
/* Protection Ec Report V2 command class structs */         
/************************************************************/
typedef struct _ZW_PROTECTION_EC_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      nodeId;                       /**/
} ZW_PROTECTION_EC_REPORT_V2_FRAME;

/************************************************************/
/* Protection Ec Set V2 command class structs */            
/************************************************************/
typedef struct _ZW_PROTECTION_EC_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      nodeId;                       /**/
} ZW_PROTECTION_EC_SET_V2_FRAME;

/************************************************************/
/* Protection Get V2 command class structs */               
/************************************************************/
typedef struct _ZW_PROTECTION_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_PROTECTION_GET_V2_FRAME;

/************************************************************/
/* Protection Report V2 command class structs */            
/************************************************************/
typedef struct _ZW_PROTECTION_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
} ZW_PROTECTION_REPORT_V2_FRAME;

/************************************************************/
/* Protection Set V2 command class structs */               
/************************************************************/
typedef struct _ZW_PROTECTION_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
} ZW_PROTECTION_SET_V2_FRAME;

/************************************************************/
/* Protection Supported Get V2 command class structs */     
/************************************************************/
typedef struct _ZW_PROTECTION_SUPPORTED_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_PROTECTION_SUPPORTED_GET_V2_FRAME;

/************************************************************/
/* Protection Supported Report V2 command class structs */  
/************************************************************/
typedef struct _ZW_PROTECTION_SUPPORTED_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      localProtectionState1;        /* MSB */
    BYTE      localProtectionState2;        /* LSB */
    BYTE      rfProtectionState1;           /* MSB */
    BYTE      rfProtectionState2;           /* LSB */
} ZW_PROTECTION_SUPPORTED_REPORT_V2_FRAME;

/************************************************************/
/* Protection Timeout Get V2 command class structs */       
/************************************************************/
typedef struct _ZW_PROTECTION_TIMEOUT_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_PROTECTION_TIMEOUT_GET_V2_FRAME;

/************************************************************/
/* Protection Timeout Report V2 command class structs */    
/************************************************************/
typedef struct _ZW_PROTECTION_TIMEOUT_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      timeout;                      /**/
} ZW_PROTECTION_TIMEOUT_REPORT_V2_FRAME;

/************************************************************/
/* Protection Timeout Set V2 command class structs */       
/************************************************************/
typedef struct _ZW_PROTECTION_TIMEOUT_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      timeout;                      /**/
} ZW_PROTECTION_TIMEOUT_SET_V2_FRAME;

/************************************************************/
/* Rate Tbl Remove 1byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REMOVE_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          
} ZW_RATE_TBL_REMOVE_1BYTE_FRAME;

/************************************************************/
/* Rate Tbl Remove 2byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REMOVE_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          /* MSB */
    BYTE      rateParameterSetId2;          /* LSB */
} ZW_RATE_TBL_REMOVE_2BYTE_FRAME;

/************************************************************/
/* Rate Tbl Remove 3byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REMOVE_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          /* MSB */
    BYTE      rateParameterSetId2;          
    BYTE      rateParameterSetId3;          /* LSB */
} ZW_RATE_TBL_REMOVE_3BYTE_FRAME;

/************************************************************/
/* Rate Tbl Remove 4byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REMOVE_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          /* MSB */
    BYTE      rateParameterSetId2;          
    BYTE      rateParameterSetId3;          
    BYTE      rateParameterSetId4;          /* LSB */
} ZW_RATE_TBL_REMOVE_4BYTE_FRAME;

/************************************************************/
/* Rate Tbl Set 1byte command class structs */              
/************************************************************/
typedef struct _ZW_RATE_TBL_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_SET_1BYTE_FRAME;

/************************************************************/
/* Rate Tbl Set 2byte command class structs */              
/************************************************************/
typedef struct _ZW_RATE_TBL_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               /* MSB */
    BYTE      rateCharacter2;               /* LSB */
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_SET_2BYTE_FRAME;

/************************************************************/
/* Rate Tbl Set 3byte command class structs */              
/************************************************************/
typedef struct _ZW_RATE_TBL_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               /* MSB */
    BYTE      rateCharacter2;               
    BYTE      rateCharacter3;               /* LSB */
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_SET_3BYTE_FRAME;

/************************************************************/
/* Rate Tbl Set 4byte command class structs */              
/************************************************************/
typedef struct _ZW_RATE_TBL_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               /* MSB */
    BYTE      rateCharacter2;               
    BYTE      rateCharacter3;               
    BYTE      rateCharacter4;               /* LSB */
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_SET_4BYTE_FRAME;

/************************************************************/
/* Rate Tbl Active Rate Get command class structs */        
/************************************************************/
typedef struct _ZW_RATE_TBL_ACTIVE_RATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_RATE_TBL_ACTIVE_RATE_GET_FRAME;

/************************************************************/
/* Rate Tbl Active Rate Report command class structs */     
/************************************************************/
typedef struct _ZW_RATE_TBL_ACTIVE_RATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
} ZW_RATE_TBL_ACTIVE_RATE_REPORT_FRAME;

/************************************************************/
/* Rate Tbl Current Data Get command class structs */       
/************************************************************/
typedef struct _ZW_RATE_TBL_CURRENT_DATA_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      datasetRequested1;            /* MSB */
    BYTE      datasetRequested2;            
    BYTE      datasetRequested3;            /* LSB */
} ZW_RATE_TBL_CURRENT_DATA_GET_FRAME;

/************************************************************/
/* Rate Tbl Current Data Report variant group structs */    
/************************************************************/
typedef struct _VG_RATE_TBL_CURRENT_DATA_REPORT_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      currentValue1;                /* MSB */
    BYTE      currentValue2;                
    BYTE      currentValue3;                
    BYTE      currentValue4;                /* LSB */
} VG_RATE_TBL_CURRENT_DATA_REPORT_VG;

/************************************************************/
/* Rate Tbl Current Data Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_CURRENT_DATA_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
} ZW_RATE_TBL_CURRENT_DATA_REPORT_1BYTE_FRAME;

/************************************************************/
/* Rate Tbl Current Data Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_CURRENT_DATA_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup2;                /**/
} ZW_RATE_TBL_CURRENT_DATA_REPORT_2BYTE_FRAME;

/************************************************************/
/* Rate Tbl Current Data Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_CURRENT_DATA_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup2;                /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup3;                /**/
} ZW_RATE_TBL_CURRENT_DATA_REPORT_3BYTE_FRAME;

/************************************************************/
/* Rate Tbl Current Data Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_CURRENT_DATA_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup1;                /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup2;                /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup3;                /**/
    VG_RATE_TBL_CURRENT_DATA_REPORT_VG variantgroup4;                /**/
} ZW_RATE_TBL_CURRENT_DATA_REPORT_4BYTE_FRAME;

/************************************************************/
/* Rate Tbl Get command class structs */                    
/************************************************************/
typedef struct _ZW_RATE_TBL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
} ZW_RATE_TBL_GET_FRAME;

/************************************************************/
/* Rate Tbl Historical Data Get command class structs */    
/************************************************************/
typedef struct _ZW_RATE_TBL_HISTORICAL_DATA_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      maximumReports;               /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      datasetRequested1;            /* MSB */
    BYTE      datasetRequested2;            
    BYTE      datasetRequested3;            /* LSB */
    BYTE      startYear1;                   /* MSB */
    BYTE      startYear2;                   /* LSB */
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      startSecondLocalTime;         /**/
    BYTE      stopYear1;                    /* MSB */
    BYTE      stopYear2;                    /* LSB */
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHourLocalTime;            /**/
    BYTE      stopMinuteLocalTime;          /**/
    BYTE      stopSecondLocalTime;          /**/
} ZW_RATE_TBL_HISTORICAL_DATA_GET_FRAME;

/************************************************************/
/* Rate Tbl Historical Data Report variant group structs */ 
/************************************************************/
typedef struct _VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      historicalValue1;             /* MSB */
    BYTE      historicalValue2;             
    BYTE      historicalValue3;             
    BYTE      historicalValue4;             /* LSB */
} VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG;

/************************************************************/
/* Rate Tbl Historical Data Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_HISTORICAL_DATA_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
} ZW_RATE_TBL_HISTORICAL_DATA_REPORT_1BYTE_FRAME;

/************************************************************/
/* Rate Tbl Historical Data Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_HISTORICAL_DATA_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup2;                /**/
} ZW_RATE_TBL_HISTORICAL_DATA_REPORT_2BYTE_FRAME;

/************************************************************/
/* Rate Tbl Historical Data Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_HISTORICAL_DATA_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup2;                /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup3;                /**/
} ZW_RATE_TBL_HISTORICAL_DATA_REPORT_3BYTE_FRAME;

/************************************************************/
/* Rate Tbl Historical Data Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_RATE_TBL_HISTORICAL_DATA_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      rateParameterSetId;           /**/
    BYTE      dataset1;                     /* MSB */
    BYTE      dataset2;                     
    BYTE      dataset3;                     /* LSB */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup1;                /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup2;                /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup3;                /**/
    VG_RATE_TBL_HISTORICAL_DATA_REPORT_VG variantgroup4;                /**/
} ZW_RATE_TBL_HISTORICAL_DATA_REPORT_4BYTE_FRAME;

/************************************************************/
/* Rate Tbl Report 1byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_REPORT_1BYTE_FRAME;

/************************************************************/
/* Rate Tbl Report 2byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               /* MSB */
    BYTE      rateCharacter2;               /* LSB */
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_REPORT_2BYTE_FRAME;

/************************************************************/
/* Rate Tbl Report 3byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               /* MSB */
    BYTE      rateCharacter2;               
    BYTE      rateCharacter3;               /* LSB */
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_REPORT_3BYTE_FRAME;

/************************************************************/
/* Rate Tbl Report 4byte command class structs */           
/************************************************************/
typedef struct _ZW_RATE_TBL_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      rateCharacter1;               /* MSB */
    BYTE      rateCharacter2;               
    BYTE      rateCharacter3;               
    BYTE      rateCharacter4;               /* LSB */
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      durationMinute1;              /* MSB */
    BYTE      durationMinute2;              /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      minConsumptionValue1;         /* MSB */
    BYTE      minConsumptionValue2;         
    BYTE      minConsumptionValue3;         
    BYTE      minConsumptionValue4;         /* LSB */
    BYTE      maxConsumptionValue1;         /* MSB */
    BYTE      maxConsumptionValue2;         
    BYTE      maxConsumptionValue3;         
    BYTE      maxConsumptionValue4;         /* LSB */
    BYTE      properties3;                  /* masked byte */
    BYTE      maxDemandValue1;              /* MSB */
    BYTE      maxDemandValue2;              
    BYTE      maxDemandValue3;              
    BYTE      maxDemandValue4;              /* LSB */
    BYTE      dcpRateId;                    /**/
} ZW_RATE_TBL_REPORT_4BYTE_FRAME;

/************************************************************/
/* Rate Tbl Supported Get command class structs */          
/************************************************************/
typedef struct _ZW_RATE_TBL_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_RATE_TBL_SUPPORTED_GET_FRAME;

/************************************************************/
/* Rate Tbl Supported Report command class structs */       
/************************************************************/
typedef struct _ZW_RATE_TBL_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      ratesSupported;               /**/
    BYTE      parameterSetSupportedBitMask1;/* MSB */
    BYTE      parameterSetSupportedBitMask2;/* LSB */
} ZW_RATE_TBL_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Remote Association Activate command class structs */     
/************************************************************/
typedef struct _ZW_REMOTE_ASSOCIATION_ACTIVATE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupingIdentifier;           /**/
} ZW_REMOTE_ASSOCIATION_ACTIVATE_FRAME;

/************************************************************/
/* Remote Association Configuration Get command class structs */
/************************************************************/
typedef struct _ZW_REMOTE_ASSOCIATION_CONFIGURATION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      localGroupingIdentifier;      /**/
} ZW_REMOTE_ASSOCIATION_CONFIGURATION_GET_FRAME;

/************************************************************/
/* Remote Association Configuration Report command class structs */
/************************************************************/
typedef struct _ZW_REMOTE_ASSOCIATION_CONFIGURATION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      localGroupingIdentifier;      /**/
    BYTE      remoteNodeid;                 /**/
    BYTE      remoteGroupingIdentifier;     /**/
} ZW_REMOTE_ASSOCIATION_CONFIGURATION_REPORT_FRAME;

/************************************************************/
/* Remote Association Configuration Set command class structs */
/************************************************************/
typedef struct _ZW_REMOTE_ASSOCIATION_CONFIGURATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      localGroupingIdentifier;      /**/
    BYTE      remoteNodeid;                 /**/
    BYTE      remoteGroupingIdentifier;     /**/
} ZW_REMOTE_ASSOCIATION_CONFIGURATION_SET_FRAME;

/************************************************************/
/* Scene Activation Set command class structs */            
/************************************************************/
typedef struct _ZW_SCENE_ACTIVATION_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sceneId;                      /**/
    BYTE      dimmingDuration;              /**/
} ZW_SCENE_ACTIVATION_SET_FRAME;

/************************************************************/
/* Scene Actuator Conf Get command class structs */         
/************************************************************/
typedef struct _ZW_SCENE_ACTUATOR_CONF_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sceneId;                      /**/
} ZW_SCENE_ACTUATOR_CONF_GET_FRAME;

/************************************************************/
/* Scene Actuator Conf Report command class structs */      
/************************************************************/
typedef struct _ZW_SCENE_ACTUATOR_CONF_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sceneId;                      /**/
    BYTE      level;                        /**/
    BYTE      dimmingDuration;              /**/
} ZW_SCENE_ACTUATOR_CONF_REPORT_FRAME;

/************************************************************/
/* Scene Actuator Conf Set command class structs */         
/************************************************************/
typedef struct _ZW_SCENE_ACTUATOR_CONF_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sceneId;                      /**/
    BYTE      dimmingDuration;              /**/
    BYTE      level2;                       /* masked byte */
    BYTE      level;                        /**/
} ZW_SCENE_ACTUATOR_CONF_SET_FRAME;

/************************************************************/
/* Scene Controller Conf Get command class structs */       
/************************************************************/
typedef struct _ZW_SCENE_CONTROLLER_CONF_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupId;                      /**/
} ZW_SCENE_CONTROLLER_CONF_GET_FRAME;

/************************************************************/
/* Scene Controller Conf Report command class structs */    
/************************************************************/
typedef struct _ZW_SCENE_CONTROLLER_CONF_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupId;                      /**/
    BYTE      sceneId;                      /**/
    BYTE      dimmingDuration;              /**/
} ZW_SCENE_CONTROLLER_CONF_REPORT_FRAME;

/************************************************************/
/* Scene Controller Conf Set command class structs */       
/************************************************************/
typedef struct _ZW_SCENE_CONTROLLER_CONF_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      groupId;                      /**/
    BYTE      sceneId;                      /**/
    BYTE      dimmingDuration;              /**/
} ZW_SCENE_CONTROLLER_CONF_SET_FRAME;

/************************************************************/
/* Schedule Entry Lock Enable All Set command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      enabled;                      /**/
} ZW_SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_FRAME;

/************************************************************/
/* Schedule Entry Lock Enable Set command class structs */  
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_ENABLE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      enabled;                      /**/
} ZW_SCHEDULE_ENTRY_LOCK_ENABLE_SET_FRAME;

/************************************************************/
/* Schedule Entry Lock Week Day Get command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
} ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_FRAME;

/************************************************************/
/* Schedule Entry Lock Week Day Report command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      dayOfWeek;                    /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_FRAME;

/************************************************************/
/* Schedule Entry Lock Week Day Set command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setAction;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      dayOfWeek;                    /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_FRAME;

/************************************************************/
/* Schedule Entry Lock Year Day Get command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
} ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_FRAME;

/************************************************************/
/* Schedule Entry Lock Year Day Report command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      startYear;                    /**/
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopYear;                     /**/
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_FRAME;

/************************************************************/
/* Schedule Entry Lock Year Day Set command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setAction;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      startYear;                    /**/
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopYear;                     /**/
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_FRAME;

/************************************************************/
/* Schedule Entry Type Supported Get command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_FRAME;

/************************************************************/
/* Schedule Entry Type Supported Report command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfSlotsWeekDay;         /**/
    BYTE      numberOfSlotsYearDay;         /**/
} ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Schedule Entry Lock Enable All Set V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      enabled;                      /**/
} ZW_SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Enable Set V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_ENABLE_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      enabled;                      /**/
} ZW_SCHEDULE_ENTRY_LOCK_ENABLE_SET_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Time Offset Get V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Time Offset Report V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      minuteTzo;                    /**/
    BYTE      level2;                       /* masked byte */
} ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Time Offset Set V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      minuteTzo;                    /**/
    BYTE      level2;                       /* masked byte */
} ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Week Day Get V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
} ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Week Day Report V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      dayOfWeek;                    /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Week Day Set V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setAction;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      dayOfWeek;                    /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Year Day Get V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
} ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Year Day Report V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      startYear;                    /**/
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopYear;                     /**/
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_V2_FRAME;

/************************************************************/
/* Schedule Entry Lock Year Day Set V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setAction;                    /**/
    BYTE      userIdentifier;               /**/
    BYTE      scheduleSlotId;               /**/
    BYTE      startYear;                    /**/
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHour;                    /**/
    BYTE      startMinute;                  /**/
    BYTE      stopYear;                     /**/
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHour;                     /**/
    BYTE      stopMinute;                   /**/
} ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_V2_FRAME;

/************************************************************/
/* Schedule Entry Type Supported Get V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_V2_FRAME;

/************************************************************/
/* Schedule Entry Type Supported Report V2 command class structs */
/************************************************************/
typedef struct _ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfSlotsWeekDay;         /**/
    BYTE      numberOfSlotsYearDay;         /**/
} ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_V2_FRAME;

/************************************************************/
/* Screen Attributes Get command class structs */           
/************************************************************/
typedef struct _ZW_SCREEN_ATTRIBUTES_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SCREEN_ATTRIBUTES_GET_FRAME;

/************************************************************/
/* Screen Attributes Report command class structs */        
/************************************************************/
typedef struct _ZW_SCREEN_ATTRIBUTES_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      numberOfCharactersPerLine;    /**/
    BYTE      sizeOfLineBuffer;             /**/
    BYTE      numericalPresentationOfACharacter;/**/
} ZW_SCREEN_ATTRIBUTES_REPORT_FRAME;

/************************************************************/
/* Screen Attributes Get V2 command class structs */        
/************************************************************/
typedef struct _ZW_SCREEN_ATTRIBUTES_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SCREEN_ATTRIBUTES_GET_V2_FRAME;

/************************************************************/
/* Screen Attributes Report V2 command class structs */     
/************************************************************/
typedef struct _ZW_SCREEN_ATTRIBUTES_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      numberOfCharactersPerLine;    /**/
    BYTE      sizeOfLineBuffer;             /**/
    BYTE      numericalPresentationOfACharacter;/**/
    BYTE      screenTimeout;                /**/
} ZW_SCREEN_ATTRIBUTES_REPORT_V2_FRAME;

/************************************************************/
/* Screen Md Get command class structs */                   
/************************************************************/
typedef struct _ZW_SCREEN_MD_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfReports;              /**/
    BYTE      nodeId;                       /**/
} ZW_SCREEN_MD_GET_FRAME;

/************************************************************/
/* Screen Md Report variant group structs */                
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_1BYTE_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   
} VG_SCREEN_MD_REPORT_1BYTE_VG;

/************************************************************/
/* Screen Md Report variant group structs */                
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_2BYTE_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   /* MSB */
    BYTE      character2;                   /* LSB */
} VG_SCREEN_MD_REPORT_2BYTE_VG;

/************************************************************/
/* Screen Md Report variant group structs */                
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_3BYTE_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   /* MSB */
    BYTE      character2;                   
    BYTE      character3;                   /* LSB */
} VG_SCREEN_MD_REPORT_3BYTE_VG;

/************************************************************/
/* Screen Md Report variant group structs */                
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_4BYTE_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   /* MSB */
    BYTE      character2;                   
    BYTE      character3;                   
    BYTE      character4;                   /* LSB */
} VG_SCREEN_MD_REPORT_4BYTE_VG;

/************************************************************/
/* Screen Md Report 1byte command class structs */          
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup1;                /**/
} ZW_SCREEN_MD_REPORT_1BYTE_FRAME;

/************************************************************/
/* Screen Md Report 2byte command class structs */          
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup1;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup2;                /**/
} ZW_SCREEN_MD_REPORT_2BYTE_FRAME;

/************************************************************/
/* Screen Md Report 3byte command class structs */          
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup1;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup2;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup3;                /**/
} ZW_SCREEN_MD_REPORT_3BYTE_FRAME;

/************************************************************/
/* Screen Md Report 4byte command class structs */          
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup1;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup2;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup3;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_VG variantgroup4;                /**/
} ZW_SCREEN_MD_REPORT_4BYTE_FRAME;

/************************************************************/
/* Screen Md Get V2 command class structs */                
/************************************************************/
typedef struct _ZW_SCREEN_MD_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfReports;              /**/
    BYTE      nodeId;                       /**/
} ZW_SCREEN_MD_GET_V2_FRAME;

/************************************************************/
/* Screen Md Report V2 variant group structs */             
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_1BYTE_V2_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   
} VG_SCREEN_MD_REPORT_1BYTE_V2_VG;

/************************************************************/
/* Screen Md Report V2 variant group structs */             
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_2BYTE_V2_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   /* MSB */
    BYTE      character2;                   /* LSB */
} VG_SCREEN_MD_REPORT_2BYTE_V2_VG;

/************************************************************/
/* Screen Md Report V2 variant group structs */             
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_3BYTE_V2_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   /* MSB */
    BYTE      character2;                   
    BYTE      character3;                   /* LSB */
} VG_SCREEN_MD_REPORT_3BYTE_V2_VG;

/************************************************************/
/* Screen Md Report V2 variant group structs */             
/************************************************************/
typedef struct _VG_SCREEN_MD_REPORT_4BYTE_V2_VG_
{
    BYTE      properties1;                  /* masked byte */
    BYTE      characterPosition;            /**/
    BYTE      numberOfCharacters;           /**/
    BYTE      character1;                   /* MSB */
    BYTE      character2;                   
    BYTE      character3;                   
    BYTE      character4;                   /* LSB */
} VG_SCREEN_MD_REPORT_4BYTE_V2_VG;

/************************************************************/
/* Screen Md Report 1byte V2 command class structs */       
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup1;                /**/
} ZW_SCREEN_MD_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Screen Md Report 2byte V2 command class structs */       
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup1;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup2;                /**/
} ZW_SCREEN_MD_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Screen Md Report 3byte V2 command class structs */       
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup1;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup2;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup3;                /**/
} ZW_SCREEN_MD_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Screen Md Report 4byte V2 command class structs */       
/************************************************************/
typedef struct _ZW_SCREEN_MD_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup1;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup2;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup3;                /**/
    VG_SCREEN_MD_REPORT_4BYTE_V2_VG variantgroup4;                /**/
} ZW_SCREEN_MD_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Security Panel Mode Get command class structs */         
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_MODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SECURITY_PANEL_MODE_GET_FRAME;

/************************************************************/
/* Security Panel Mode Report command class structs */      
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_MODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_SECURITY_PANEL_MODE_REPORT_FRAME;

/************************************************************/
/* Security Panel Mode Set command class structs */         
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_MODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_SECURITY_PANEL_MODE_SET_FRAME;

/************************************************************/
/* Security Panel Mode Supported Get command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_MODE_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SECURITY_PANEL_MODE_SUPPORTED_GET_FRAME;

/************************************************************/
/* Security Panel Mode Supported Report command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_MODE_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedModeBitMask1;        /* MSB */
    BYTE      supportedModeBitMask2;        /* LSB */
} ZW_SECURITY_PANEL_MODE_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Security Panel Zone Sensor Installed Report command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
    BYTE      numberOfSensors;              /**/
} ZW_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_REPORT_FRAME;

/************************************************************/
/* Security Panel  Zone Sensor Type Get command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL__ZONE_SENSOR_TYPE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
    BYTE      sensorNumber;                 /**/
} ZW_SECURITY_PANEL__ZONE_SENSOR_TYPE_GET_FRAME;

/************************************************************/
/* Security Panel  Zone Sensor Type Report command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL__ZONE_SENSOR_TYPE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
    BYTE      sensorNumber;                 /**/
    BYTE      zwaveAlarmType;               /**/
} ZW_SECURITY_PANEL__ZONE_SENSOR_TYPE_REPORT_FRAME;

/************************************************************/
/* Security Panel Zone Sensor Installed Get command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
} ZW_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_GET_FRAME;

/************************************************************/
/* Security Panel Zone Sensor State Get command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_SENSOR_STATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
    BYTE      sensorNumber;                 /**/
} ZW_SECURITY_PANEL_ZONE_SENSOR_STATE_GET_FRAME;

/************************************************************/
/* Security Panel Zone Sensor State Report command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_SENSOR_STATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
    BYTE      sensorNumber;                 /**/
    BYTE      zwaveAlarmType;               /**/
    BYTE      zwaveAlarmEvent;              /**/
    BYTE      eventParameters;              /**/
} ZW_SECURITY_PANEL_ZONE_SENSOR_STATE_REPORT_FRAME;

/************************************************************/
/* Security Panel Zone Number Supported Get command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_NUMBER_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SECURITY_PANEL_ZONE_NUMBER_SUPPORTED_GET_FRAME;

/************************************************************/
/* Security Panel Zone State Get command class structs */   
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_STATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
} ZW_SECURITY_PANEL_ZONE_STATE_GET_FRAME;

/************************************************************/
/* Security Panel Zone State Report command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_STATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
    BYTE      zoneState;                    /**/
} ZW_SECURITY_PANEL_ZONE_STATE_REPORT_FRAME;

/************************************************************/
/* Security Panel Zone Supported Report command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_SUPPORTED_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      parameters1;                  /* masked byte */
} ZW_SECURITY_PANEL_ZONE_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Security Panel Zone Type Get command class structs */    
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_TYPE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
} ZW_SECURITY_PANEL_ZONE_TYPE_GET_FRAME;

/************************************************************/
/* Security Panel Zone Type Report command class structs */ 
/************************************************************/
typedef struct _ZW_SECURITY_PANEL_ZONE_TYPE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zoneNumber;                   /**/
    BYTE      zoneType;                     /**/
} ZW_SECURITY_PANEL_ZONE_TYPE_REPORT_FRAME;

/************************************************************/
/* Network Key Set 1byte command class structs */           
/************************************************************/
typedef struct _ZW_NETWORK_KEY_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      networkKeyByte1;              
} ZW_NETWORK_KEY_SET_1BYTE_FRAME;

/************************************************************/
/* Network Key Set 2byte command class structs */           
/************************************************************/
typedef struct _ZW_NETWORK_KEY_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      networkKeyByte1;              /* MSB */
    BYTE      networkKeyByte2;              /* LSB */
} ZW_NETWORK_KEY_SET_2BYTE_FRAME;

/************************************************************/
/* Network Key Set 3byte command class structs */           
/************************************************************/
typedef struct _ZW_NETWORK_KEY_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      networkKeyByte1;              /* MSB */
    BYTE      networkKeyByte2;              
    BYTE      networkKeyByte3;              /* LSB */
} ZW_NETWORK_KEY_SET_3BYTE_FRAME;

/************************************************************/
/* Network Key Set 4byte command class structs */           
/************************************************************/
typedef struct _ZW_NETWORK_KEY_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      networkKeyByte1;              /* MSB */
    BYTE      networkKeyByte2;              
    BYTE      networkKeyByte3;              
    BYTE      networkKeyByte4;              /* LSB */
} ZW_NETWORK_KEY_SET_4BYTE_FRAME;

/************************************************************/
/* Network Key Verify command class structs */              
/************************************************************/
typedef struct _ZW_NETWORK_KEY_VERIFY_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_NETWORK_KEY_VERIFY_FRAME;

/************************************************************/
/* Security Commands Supported Get command class structs */ 
/************************************************************/
typedef struct _ZW_SECURITY_COMMANDS_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SECURITY_COMMANDS_SUPPORTED_GET_FRAME;

/************************************************************/
/* Security Commands Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      commandClassSupport1;         
    BYTE      command_class_mark;           /*The COMMAND_CLASS_MARK is used to indicate that all preceding command classes are supported and all following command classes are controlled.*/
    BYTE      commandClassControl1;         
} ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Security Commands Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      commandClassSupport1;         /* MSB */
    BYTE      commandClassSupport2;         /* LSB */
    BYTE      command_class_mark;           /*The COMMAND_CLASS_MARK is used to indicate that all preceding command classes are supported and all following command classes are controlled.*/
    BYTE      commandClassControl1;         /* MSB */
    BYTE      commandClassControl2;         /* LSB */
} ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Security Commands Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      commandClassSupport1;         /* MSB */
    BYTE      commandClassSupport2;         
    BYTE      commandClassSupport3;         /* LSB */
    BYTE      command_class_mark;           /*The COMMAND_CLASS_MARK is used to indicate that all preceding command classes are supported and all following command classes are controlled.*/
    BYTE      commandClassControl1;         /* MSB */
    BYTE      commandClassControl2;         
    BYTE      commandClassControl3;         /* LSB */
} ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Security Commands Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportsToFollow;              /**/
    BYTE      commandClassSupport1;         /* MSB */
    BYTE      commandClassSupport2;         
    BYTE      commandClassSupport3;         
    BYTE      commandClassSupport4;         /* LSB */
    BYTE      command_class_mark;           /*The COMMAND_CLASS_MARK is used to indicate that all preceding command classes are supported and all following command classes are controlled.*/
    BYTE      commandClassControl1;         /* MSB */
    BYTE      commandClassControl2;         
    BYTE      commandClassControl3;         
    BYTE      commandClassControl4;         /* LSB */
} ZW_SECURITY_COMMANDS_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation 1byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_1BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation 2byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 /* LSB */
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_2BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation 3byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 /* LSB */
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_3BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation 4byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 
    BYTE      commandByte4;                 /* LSB */
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_4BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation Nonce Get 1byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_1BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation Nonce Get 2byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 /* LSB */
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_2BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation Nonce Get 3byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 /* LSB */
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_3BYTE_FRAME;

/************************************************************/
/* Security Message Encapsulation Nonce Get 4byte command class structs */
/************************************************************/
typedef struct _ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      initializationVectorByte1;    /* MSB */
    BYTE      initializationVectorByte2;    
    BYTE      initializationVectorByte3;    
    BYTE      initializationVectorByte4;    
    BYTE      initializationVectorByte5;    
    BYTE      initializationVectorByte6;    
    BYTE      initializationVectorByte7;    
    BYTE      initializationVectorByte8;    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      commandClassIdentifier;       /**/
    BYTE      commandIdentifier;            /**/
    BYTE      commandByte1;                 /* MSB */
    BYTE      commandByte2;                 
    BYTE      commandByte3;                 
    BYTE      commandByte4;                 /* LSB */
    BYTE      receiversNonceIdentifier;     /**/
    BYTE      messageAuthenticationCodeByte1;/* MSB */
    BYTE      messageAuthenticationCodeByte2;
    BYTE      messageAuthenticationCodeByte3;
    BYTE      messageAuthenticationCodeByte4;
    BYTE      messageAuthenticationCodeByte5;
    BYTE      messageAuthenticationCodeByte6;
    BYTE      messageAuthenticationCodeByte7;
    BYTE      messageAuthenticationCodeByte8;/* LSB */
} ZW_SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_4BYTE_FRAME;

/************************************************************/
/* Security Nonce Get command class structs */              
/************************************************************/
typedef struct _ZW_SECURITY_NONCE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SECURITY_NONCE_GET_FRAME;

/************************************************************/
/* Security Nonce Report command class structs */           
/************************************************************/
typedef struct _ZW_SECURITY_NONCE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      nonceByte1;                   /* MSB */
    BYTE      nonceByte2;                   
    BYTE      nonceByte3;                   
    BYTE      nonceByte4;                   
    BYTE      nonceByte5;                   
    BYTE      nonceByte6;                   
    BYTE      nonceByte7;                   
    BYTE      nonceByte8;                   /* LSB */
} ZW_SECURITY_NONCE_REPORT_FRAME;

/************************************************************/
/* Security Scheme Get command class structs */             
/************************************************************/
typedef struct _ZW_SECURITY_SCHEME_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedSecuritySchemes;     /**/
} ZW_SECURITY_SCHEME_GET_FRAME;

/************************************************************/
/* Security Scheme Inherit command class structs */         
/************************************************************/
typedef struct _ZW_SECURITY_SCHEME_INHERIT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedSecuritySchemes;     /**/
} ZW_SECURITY_SCHEME_INHERIT_FRAME;

/************************************************************/
/* Security Scheme Report command class structs */          
/************************************************************/
typedef struct _ZW_SECURITY_SCHEME_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedSecuritySchemes;     /**/
} ZW_SECURITY_SCHEME_REPORT_FRAME;

/************************************************************/
/* Sensor Alarm Get command class structs */                
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
} ZW_SENSOR_ALARM_GET_FRAME;

/************************************************************/
/* Sensor Alarm Report command class structs */             
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sourceNodeId;                 /**/
    BYTE      sensorType;                   /**/
    BYTE      sensorState;                  /**/
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     /* LSB */
} ZW_SENSOR_ALARM_REPORT_FRAME;

/************************************************************/
/* Sensor Alarm Supported Get command class structs */      
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SENSOR_ALARM_SUPPORTED_GET_FRAME;

/************************************************************/
/* Sensor Alarm Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     
} ZW_SENSOR_ALARM_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Sensor Alarm Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_SENSOR_ALARM_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Sensor Alarm Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_SENSOR_ALARM_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Sensor Alarm Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_SENSOR_ALARM_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Sensor Binary Get command class structs */               
/************************************************************/
typedef struct _ZW_SENSOR_BINARY_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SENSOR_BINARY_GET_FRAME;

/************************************************************/
/* Sensor Binary Report command class structs */            
/************************************************************/
typedef struct _ZW_SENSOR_BINARY_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorValue;                  /**/
} ZW_SENSOR_BINARY_REPORT_FRAME;

/************************************************************/
/* Sensor Trigger Level Get command class structs */        
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SENSOR_TRIGGER_LEVEL_GET_FRAME;

/************************************************************/
/* Sensor Trigger Level Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      triggerValue1;                
} ZW_SENSOR_TRIGGER_LEVEL_REPORT_1BYTE_FRAME;

/************************************************************/
/* Sensor Trigger Level Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      triggerValue1;                /* MSB */
    BYTE      triggerValue2;                /* LSB */
} ZW_SENSOR_TRIGGER_LEVEL_REPORT_2BYTE_FRAME;

/************************************************************/
/* Sensor Trigger Level Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      triggerValue1;                /* MSB */
    BYTE      triggerValue2;                
    BYTE      triggerValue3;                /* LSB */
} ZW_SENSOR_TRIGGER_LEVEL_REPORT_3BYTE_FRAME;

/************************************************************/
/* Sensor Trigger Level Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      triggerValue1;                /* MSB */
    BYTE      triggerValue2;                
    BYTE      triggerValue3;                
    BYTE      triggerValue4;                /* LSB */
} ZW_SENSOR_TRIGGER_LEVEL_REPORT_4BYTE_FRAME;

/************************************************************/
/* Sensor Trigger Level Set 1byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      sensorType;                   /**/
    BYTE      properties2;                  /* masked byte */
    BYTE      triggerValue1;                
} ZW_SENSOR_TRIGGER_LEVEL_SET_1BYTE_FRAME;

/************************************************************/
/* Sensor Trigger Level Set 2byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      sensorType;                   /**/
    BYTE      properties2;                  /* masked byte */
    BYTE      triggerValue1;                /* MSB */
    BYTE      triggerValue2;                /* LSB */
} ZW_SENSOR_TRIGGER_LEVEL_SET_2BYTE_FRAME;

/************************************************************/
/* Sensor Trigger Level Set 3byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      sensorType;                   /**/
    BYTE      properties2;                  /* masked byte */
    BYTE      triggerValue1;                /* MSB */
    BYTE      triggerValue2;                
    BYTE      triggerValue3;                /* LSB */
} ZW_SENSOR_TRIGGER_LEVEL_SET_3BYTE_FRAME;

/************************************************************/
/* Sensor Trigger Level Set 4byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_TRIGGER_LEVEL_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      sensorType;                   /**/
    BYTE      properties2;                  /* masked byte */
    BYTE      triggerValue1;                /* MSB */
    BYTE      triggerValue2;                
    BYTE      triggerValue3;                
    BYTE      triggerValue4;                /* LSB */
} ZW_SENSOR_TRIGGER_LEVEL_SET_4BYTE_FRAME;

/************************************************************/
/* Sensor Multilevel Get command class structs */           
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SENSOR_MULTILEVEL_GET_FRAME;

/************************************************************/
/* Sensor Multilevel Report 1byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 
} ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_FRAME;

/************************************************************/
/* Sensor Multilevel Report 2byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_FRAME;

/************************************************************/
/* Sensor Multilevel Report 3byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 
    BYTE      sensorValue3;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_FRAME;

/************************************************************/
/* Sensor Multilevel Report 4byte command class structs */  
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 
    BYTE      sensorValue3;                 
    BYTE      sensorValue4;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_FRAME;

/************************************************************/
/* Sensor Multilevel Get V2 command class structs */        
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SENSOR_MULTILEVEL_GET_V2_FRAME;

/************************************************************/
/* Sensor Multilevel Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 
} ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Sensor Multilevel Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Sensor Multilevel Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 
    BYTE      sensorValue3;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Sensor Multilevel Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 
    BYTE      sensorValue3;                 
    BYTE      sensorValue4;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Sensor Multilevel Get V3 command class structs */        
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_GET_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SENSOR_MULTILEVEL_GET_V3_FRAME;

/************************************************************/
/* Sensor Multilevel Report 1byte V3 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 
} ZW_SENSOR_MULTILEVEL_REPORT_1BYTE_V3_FRAME;

/************************************************************/
/* Sensor Multilevel Report 2byte V3 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_2BYTE_V3_FRAME;

/************************************************************/
/* Sensor Multilevel Report 3byte V3 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 
    BYTE      sensorValue3;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_3BYTE_V3_FRAME;

/************************************************************/
/* Sensor Multilevel Report 4byte V3 command class structs */
/************************************************************/
typedef struct _ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sensorType;                   /**/
    BYTE      level;                        /* masked byte */
    BYTE      sensorValue1;                 /* MSB */
    BYTE      sensorValue2;                 
    BYTE      sensorValue3;                 
    BYTE      sensorValue4;                 /* LSB */
} ZW_SENSOR_MULTILEVEL_REPORT_4BYTE_V3_FRAME;

/************************************************************/
/* Sensor Alarm Set 1byte command class structs */          
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     /* LSB */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     
} ZW_SENSOR_ALARM_SET_1BYTE_FRAME;

/************************************************************/
/* Sensor Alarm Set 2byte command class structs */          
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     /* LSB */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_SENSOR_ALARM_SET_2BYTE_FRAME;

/************************************************************/
/* Sensor Alarm Set 3byte command class structs */          
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     /* LSB */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_SENSOR_ALARM_SET_3BYTE_FRAME;

/************************************************************/
/* Sensor Alarm Set 4byte command class structs */          
/************************************************************/
typedef struct _ZW_SENSOR_ALARM_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     /* LSB */
    BYTE      numberOfBitMasks;             /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_SENSOR_ALARM_SET_4BYTE_FRAME;

/************************************************************/
/* Simple Av Control Get command class structs */           
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SIMPLE_AV_CONTROL_GET_FRAME;

/************************************************************/
/* Simple Av Control Report command class structs */        
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      numberOfReports;              /**/
} ZW_SIMPLE_AV_CONTROL_REPORT_FRAME;

/************************************************************/
/* Simple Av Control Set variant group structs */           
/************************************************************/
typedef struct _VG_SIMPLE_AV_CONTROL_SET_VG_
{
    BYTE      command1;                     /* MSB */
    BYTE      command2;                     /* LSB */
} VG_SIMPLE_AV_CONTROL_SET_VG;

/************************************************************/
/* Simple Av Control Set 1byte command class structs */     
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      itemId1;                      /* MSB */
    BYTE      itemId2;                      /* LSB */
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup1;                /**/
} ZW_SIMPLE_AV_CONTROL_SET_1BYTE_FRAME;

/************************************************************/
/* Simple Av Control Set 2byte command class structs */     
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      itemId1;                      /* MSB */
    BYTE      itemId2;                      /* LSB */
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup1;                /**/
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup2;                /**/
} ZW_SIMPLE_AV_CONTROL_SET_2BYTE_FRAME;

/************************************************************/
/* Simple Av Control Set 3byte command class structs */     
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      itemId1;                      /* MSB */
    BYTE      itemId2;                      /* LSB */
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup1;                /**/
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup2;                /**/
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup3;                /**/
} ZW_SIMPLE_AV_CONTROL_SET_3BYTE_FRAME;

/************************************************************/
/* Simple Av Control Set 4byte command class structs */     
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      sequenceNumber;               /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      itemId1;                      /* MSB */
    BYTE      itemId2;                      /* LSB */
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup1;                /**/
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup2;                /**/
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup3;                /**/
    VG_SIMPLE_AV_CONTROL_SET_VG variantgroup4;                /**/
} ZW_SIMPLE_AV_CONTROL_SET_4BYTE_FRAME;

/************************************************************/
/* Simple Av Control Supported Get command class structs */ 
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportNo;                     /**/
} ZW_SIMPLE_AV_CONTROL_SUPPORTED_GET_FRAME;

/************************************************************/
/* Simple Av Control Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportNo;                     /**/
    BYTE      bitMask1;                     
} ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Simple Av Control Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportNo;                     /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Simple Av Control Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportNo;                     /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Simple Av Control Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      reportNo;                     /**/
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Switch All Get command class structs */                  
/************************************************************/
typedef struct _ZW_SWITCH_ALL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_ALL_GET_FRAME;

/************************************************************/
/* Switch All Off command class structs */                  
/************************************************************/
typedef struct _ZW_SWITCH_ALL_OFF_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_ALL_OFF_FRAME;

/************************************************************/
/* Switch All On command class structs */                   
/************************************************************/
typedef struct _ZW_SWITCH_ALL_ON_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_ALL_ON_FRAME;

/************************************************************/
/* Switch All Report command class structs */               
/************************************************************/
typedef struct _ZW_SWITCH_ALL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_SWITCH_ALL_REPORT_FRAME;

/************************************************************/
/* Switch All Set command class structs */                  
/************************************************************/
typedef struct _ZW_SWITCH_ALL_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_SWITCH_ALL_SET_FRAME;

/************************************************************/
/* Switch Binary Get command class structs */               
/************************************************************/
typedef struct _ZW_SWITCH_BINARY_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_BINARY_GET_FRAME;

/************************************************************/
/* Switch Binary Report command class structs */            
/************************************************************/
typedef struct _ZW_SWITCH_BINARY_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_SWITCH_BINARY_REPORT_FRAME;

/************************************************************/
/* Switch Binary Set command class structs */               
/************************************************************/
typedef struct _ZW_SWITCH_BINARY_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      switchValue;                  /**/
} ZW_SWITCH_BINARY_SET_FRAME;

/************************************************************/
/* Switch Multilevel Get command class structs */           
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_MULTILEVEL_GET_FRAME;

/************************************************************/
/* Switch Multilevel Report command class structs */        
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_SWITCH_MULTILEVEL_REPORT_FRAME;

/************************************************************/
/* Switch Multilevel Set command class structs */           
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_SWITCH_MULTILEVEL_SET_FRAME;

/************************************************************/
/* Switch Multilevel Start Level Change command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      startLevel;                   /**/
} ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_FRAME;

/************************************************************/
/* Switch Multilevel Stop Level Change command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_FRAME;

/************************************************************/
/* Switch Multilevel Get V2 command class structs */        
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_MULTILEVEL_GET_V2_FRAME;

/************************************************************/
/* Switch Multilevel Report V2 command class structs */     
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_SWITCH_MULTILEVEL_REPORT_V2_FRAME;

/************************************************************/
/* Switch Multilevel Set V2 command class structs */        
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
    BYTE      dimmingDuration;              /**/
} ZW_SWITCH_MULTILEVEL_SET_V2_FRAME;

/************************************************************/
/* Switch Multilevel Start Level Change V2 command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      startLevel;                   /**/
    BYTE      dimmingDuration;              /**/
} ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V2_FRAME;

/************************************************************/
/* Switch Multilevel Stop Level Change V2 command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V2_FRAME;

/************************************************************/
/* Switch Multilevel Get V3 command class structs */        
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_GET_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_MULTILEVEL_GET_V3_FRAME;

/************************************************************/
/* Switch Multilevel Report V3 command class structs */     
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_REPORT_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_SWITCH_MULTILEVEL_REPORT_V3_FRAME;

/************************************************************/
/* Switch Multilevel Set V3 command class structs */        
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_SET_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
    BYTE      dimmingDuration;              /**/
} ZW_SWITCH_MULTILEVEL_SET_V3_FRAME;

/************************************************************/
/* Switch Multilevel Start Level Change V3 command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      startLevel;                   /**/
    BYTE      dimmingDuration;              /**/
    BYTE      stepSize;                     /**/
} ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V3_FRAME;

/************************************************************/
/* Switch Multilevel Stop Level Change V3 command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V3_FRAME;

/************************************************************/
/* Switch Multilevel Supported Get V3 command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_SUPPORTED_GET_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_MULTILEVEL_SUPPORTED_GET_V3_FRAME;

/************************************************************/
/* Switch Multilevel Supported Report V3 command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
} ZW_SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3_FRAME;

/************************************************************/
/* Switch Toggle Binary Set command class structs */        
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_BINARY_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_TOGGLE_BINARY_SET_FRAME;

/************************************************************/
/* Switch Toggle Binary Get command class structs */        
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_BINARY_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_TOGGLE_BINARY_GET_FRAME;

/************************************************************/
/* Switch Toggle Binary Report command class structs */     
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_BINARY_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_SWITCH_TOGGLE_BINARY_REPORT_FRAME;

/************************************************************/
/* Switch Toggle Multilevel Set command class structs */    
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_MULTILEVEL_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_TOGGLE_MULTILEVEL_SET_FRAME;

/************************************************************/
/* Switch Toggle Multilevel Get command class structs */    
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_MULTILEVEL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_TOGGLE_MULTILEVEL_GET_FRAME;

/************************************************************/
/* Switch Toggle Multilevel Report command class structs */ 
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_MULTILEVEL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      value;                        /**/
} ZW_SWITCH_TOGGLE_MULTILEVEL_REPORT_FRAME;

/************************************************************/
/* Switch Toggle Multilevel Start Level Change command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      startLevel;                   /**/
} ZW_SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_FRAME;

/************************************************************/
/* Switch Toggle Multilevel Stop Level Change command class structs */
/************************************************************/
typedef struct _ZW_SWITCH_TOGGLE_MULTILEVEL_STOP_LEVEL_CHANGE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_SWITCH_TOGGLE_MULTILEVEL_STOP_LEVEL_CHANGE_FRAME;

/************************************************************/
/* Tariff Tbl Remove 1byte command class structs */         
/************************************************************/
typedef struct _ZW_TARIFF_TBL_REMOVE_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          
} ZW_TARIFF_TBL_REMOVE_1BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Remove 2byte command class structs */         
/************************************************************/
typedef struct _ZW_TARIFF_TBL_REMOVE_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          /* MSB */
    BYTE      rateParameterSetId2;          /* LSB */
} ZW_TARIFF_TBL_REMOVE_2BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Remove 3byte command class structs */         
/************************************************************/
typedef struct _ZW_TARIFF_TBL_REMOVE_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          /* MSB */
    BYTE      rateParameterSetId2;          
    BYTE      rateParameterSetId3;          /* LSB */
} ZW_TARIFF_TBL_REMOVE_3BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Remove 4byte command class structs */         
/************************************************************/
typedef struct _ZW_TARIFF_TBL_REMOVE_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      rateParameterSetId1;          /* MSB */
    BYTE      rateParameterSetId2;          
    BYTE      rateParameterSetId3;          
    BYTE      rateParameterSetId4;          /* LSB */
} ZW_TARIFF_TBL_REMOVE_4BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Set command class structs */                  
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      tariffValue1;                 /* MSB */
    BYTE      tariffValue2;                 
    BYTE      tariffValue3;                 
    BYTE      tariffValue4;                 /* LSB */
} ZW_TARIFF_TBL_SET_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Set 1byte command class structs */   
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           
} ZW_TARIFF_TBL_SUPPLIER_SET_1BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Set 2byte command class structs */   
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           /* MSB */
    BYTE      supplierCharacter2;           /* LSB */
} ZW_TARIFF_TBL_SUPPLIER_SET_2BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Set 3byte command class structs */   
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           /* MSB */
    BYTE      supplierCharacter2;           
    BYTE      supplierCharacter3;           /* LSB */
} ZW_TARIFF_TBL_SUPPLIER_SET_3BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Set 4byte command class structs */   
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           /* MSB */
    BYTE      supplierCharacter2;           
    BYTE      supplierCharacter3;           
    BYTE      supplierCharacter4;           /* LSB */
} ZW_TARIFF_TBL_SUPPLIER_SET_4BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Cost Get command class structs */             
/************************************************************/
typedef struct _ZW_TARIFF_TBL_COST_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      startYear1;                   /* MSB */
    BYTE      startYear2;                   /* LSB */
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      stopYear1;                    /* MSB */
    BYTE      stopYear2;                    /* LSB */
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHourLocalTime;            /**/
    BYTE      stopMinuteLocalTime;          /**/
} ZW_TARIFF_TBL_COST_GET_FRAME;

/************************************************************/
/* Tariff Tbl Cost Report command class structs */          
/************************************************************/
typedef struct _ZW_TARIFF_TBL_COST_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      startYear1;                   /* MSB */
    BYTE      startYear2;                   /* LSB */
    BYTE      startMonth;                   /**/
    BYTE      startDay;                     /**/
    BYTE      startHourLocalTime;           /**/
    BYTE      startMinuteLocalTime;         /**/
    BYTE      stopYear1;                    /* MSB */
    BYTE      stopYear2;                    /* LSB */
    BYTE      stopMonth;                    /**/
    BYTE      stopDay;                      /**/
    BYTE      stopHourLocalTime;            /**/
    BYTE      stopMinuteLocalTime;          /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      costValue1;                   /* MSB */
    BYTE      costValue2;                   
    BYTE      costValue3;                   
    BYTE      costValue4;                   /* LSB */
} ZW_TARIFF_TBL_COST_REPORT_FRAME;

/************************************************************/
/* Tariff Tbl Get command class structs */                  
/************************************************************/
typedef struct _ZW_TARIFF_TBL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
} ZW_TARIFF_TBL_GET_FRAME;

/************************************************************/
/* Tariff Tbl Report command class structs */               
/************************************************************/
typedef struct _ZW_TARIFF_TBL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      rateParameterSetId;           /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      tariffValue1;                 /* MSB */
    BYTE      tariffValue2;                 
    BYTE      tariffValue3;                 
    BYTE      tariffValue4;                 /* LSB */
} ZW_TARIFF_TBL_REPORT_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Get command class structs */         
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_TARIFF_TBL_SUPPLIER_GET_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           
} ZW_TARIFF_TBL_SUPPLIER_REPORT_1BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           /* MSB */
    BYTE      supplierCharacter2;           /* LSB */
} ZW_TARIFF_TBL_SUPPLIER_REPORT_2BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           /* MSB */
    BYTE      supplierCharacter2;           
    BYTE      supplierCharacter3;           /* LSB */
} ZW_TARIFF_TBL_SUPPLIER_REPORT_3BYTE_FRAME;

/************************************************************/
/* Tariff Tbl Supplier Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_TARIFF_TBL_SUPPLIER_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourLocalTime;                /**/
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
    BYTE      currency1;                    /* MSB */
    BYTE      currency2;                    
    BYTE      currency3;                    /* LSB */
    BYTE      properties1;                  /* masked byte */
    BYTE      standingChargeValue1;         /* MSB */
    BYTE      standingChargeValue2;         
    BYTE      standingChargeValue3;         
    BYTE      standingChargeValue4;         /* LSB */
    BYTE      properties2;                  /* masked byte */
    BYTE      supplierCharacter1;           /* MSB */
    BYTE      supplierCharacter2;           
    BYTE      supplierCharacter3;           
    BYTE      supplierCharacter4;           /* LSB */
} ZW_TARIFF_TBL_SUPPLIER_REPORT_4BYTE_FRAME;

/************************************************************/
/* Thermostat Fan Mode Get command class structs */         
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_FAN_MODE_GET_FRAME;

/************************************************************/
/* Thermostat Fan Mode Report command class structs */      
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_FAN_MODE_REPORT_FRAME;

/************************************************************/
/* Thermostat Fan Mode Set command class structs */         
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_FAN_MODE_SET_FRAME;

/************************************************************/
/* Thermostat Fan Mode Supported Get command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_FAN_MODE_SUPPORTED_GET_FRAME;

/************************************************************/
/* Thermostat Fan Mode Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     
} ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Thermostat Fan Mode Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Thermostat Fan Mode Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Thermostat Fan Mode Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Thermostat Fan State Get command class structs */        
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_STATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_FAN_STATE_GET_FRAME;

/************************************************************/
/* Thermostat Fan State Report command class structs */     
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_STATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_FAN_STATE_REPORT_FRAME;

/************************************************************/
/* Thermostat Heating Status Report command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_STATUS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /**/
} ZW_THERMOSTAT_HEATING_STATUS_REPORT_FRAME;

/************************************************************/
/* Thermostat Heating Mode Get command class structs */     
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_MODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_HEATING_MODE_GET_FRAME;

/************************************************************/
/* Thermostat Heating Mode Report command class structs */  
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_MODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_THERMOSTAT_HEATING_MODE_REPORT_FRAME;

/************************************************************/
/* Thermostat Heating Mode Set command class structs */     
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_MODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      mode;                         /**/
} ZW_THERMOSTAT_HEATING_MODE_SET_FRAME;

/************************************************************/
/* Thermostat Heating Relay Status Get command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_RELAY_STATUS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_HEATING_RELAY_STATUS_GET_FRAME;

/************************************************************/
/* Thermostat Heating Relay Status Report command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_RELAY_STATUS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      relayStatus;                  /**/
} ZW_THERMOSTAT_HEATING_RELAY_STATUS_REPORT_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Get command class structs */ 
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
} ZW_THERMOSTAT_HEATING_SETPOINT_GET_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_1BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_2BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_3BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_THERMOSTAT_HEATING_SETPOINT_REPORT_4BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Set 1byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       
} ZW_THERMOSTAT_HEATING_SETPOINT_SET_1BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Set 2byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_THERMOSTAT_HEATING_SETPOINT_SET_2BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Set 3byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_THERMOSTAT_HEATING_SETPOINT_SET_3BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Setpoint Set 4byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      setpointNr;                   /**/
    BYTE      properties1;                  /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_THERMOSTAT_HEATING_SETPOINT_SET_4BYTE_FRAME;

/************************************************************/
/* Thermostat Heating Status Get command class structs */   
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_STATUS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_HEATING_STATUS_GET_FRAME;

/************************************************************/
/* Thermostat Heating Status Set command class structs */   
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_STATUS_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      status;                       /**/
} ZW_THERMOSTAT_HEATING_STATUS_SET_FRAME;

/************************************************************/
/* Thermostat Heating Timed Off Set command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_TIMED_OFF_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      minutes;                      /**/
    BYTE      hours;                        /**/
} ZW_THERMOSTAT_HEATING_TIMED_OFF_SET_FRAME;

/************************************************************/
/* Thermostat Mode Get command class structs */             
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_MODE_GET_FRAME;

/************************************************************/
/* Thermostat Mode Report command class structs */          
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_MODE_REPORT_FRAME;

/************************************************************/
/* Thermostat Mode Set command class structs */             
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_MODE_SET_FRAME;

/************************************************************/
/* Thermostat Mode Supported Get command class structs */   
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_MODE_SUPPORTED_GET_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Thermostat Mode Get V2 command class structs */          
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_MODE_GET_V2_FRAME;

/************************************************************/
/* Thermostat Mode Report V2 command class structs */       
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_MODE_REPORT_V2_FRAME;

/************************************************************/
/* Thermostat Mode Set V2 command class structs */          
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_MODE_SET_V2_FRAME;

/************************************************************/
/* Thermostat Mode Supported Get V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_MODE_SUPPORTED_GET_V2_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Mode Supported Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_THERMOSTAT_MODE_SUPPORTED_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Operating State Get command class structs */  
/************************************************************/
typedef struct _ZW_THERMOSTAT_OPERATING_STATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_OPERATING_STATE_GET_FRAME;

/************************************************************/
/* Thermostat Operating State Report command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_OPERATING_STATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_OPERATING_STATE_REPORT_FRAME;

/************************************************************/
/* Thermostat Setback Get command class structs */          
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETBACK_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_SETBACK_GET_FRAME;

/************************************************************/
/* Thermostat Setback Report command class structs */       
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETBACK_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      setbackState;                 /**/
} ZW_THERMOSTAT_SETBACK_REPORT_FRAME;

/************************************************************/
/* Thermostat Setback Set command class structs */          
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETBACK_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      setbackState;                 /**/
} ZW_THERMOSTAT_SETBACK_SET_FRAME;

/************************************************************/
/* Thermostat Setpoint Get command class structs */         
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_SETPOINT_GET_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       
} ZW_THERMOSTAT_SETPOINT_REPORT_1BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_REPORT_2BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_REPORT_3BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_REPORT_4BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 1byte command class structs */   
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       
} ZW_THERMOSTAT_SETPOINT_SET_1BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 2byte command class structs */   
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_SET_2BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 3byte command class structs */   
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_SET_3BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 4byte command class structs */   
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_SET_4BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Get command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_GET_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 1byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_1BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 2byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_2BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 3byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_3BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 4byte command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_4BYTE_FRAME;

/************************************************************/
/* Thermostat Setpoint Get V2 command class structs */      
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
} ZW_THERMOSTAT_SETPOINT_GET_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       
} ZW_THERMOSTAT_SETPOINT_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       
} ZW_THERMOSTAT_SETPOINT_SET_1BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_SET_2BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_SET_3BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Set 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SET_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      level2;                       /* masked byte */
    BYTE      value1;                       /* MSB */
    BYTE      value2;                       
    BYTE      value3;                       
    BYTE      value4;                       /* LSB */
} ZW_THERMOSTAT_SETPOINT_SET_4BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Get V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_GET_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 1byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_1BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_1BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 2byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_2BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     /* LSB */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_2BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 3byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_3BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     /* LSB */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_3BYTE_V2_FRAME;

/************************************************************/
/* Thermostat Setpoint Supported Report 4byte V2 command class structs */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_4BYTE_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      bitMask1;                     /* MSB */
    BYTE      bitMask2;                     
    BYTE      bitMask3;                     
    BYTE      bitMask4;                     /* LSB */
} ZW_THERMOSTAT_SETPOINT_SUPPORTED_REPORT_4BYTE_V2_FRAME;

/************************************************************/
/* Time Parameters Get command class structs */             
/************************************************************/
typedef struct _ZW_TIME_PARAMETERS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_TIME_PARAMETERS_GET_FRAME;

/************************************************************/
/* Time Parameters Report command class structs */          
/************************************************************/
typedef struct _ZW_TIME_PARAMETERS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourUtc;                      /**/
    BYTE      minuteUtc;                    /**/
    BYTE      secondUtc;                    /**/
} ZW_TIME_PARAMETERS_REPORT_FRAME;

/************************************************************/
/* Time Parameters Set command class structs */             
/************************************************************/
typedef struct _ZW_TIME_PARAMETERS_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
    BYTE      hourUtc;                      /**/
    BYTE      minuteUtc;                    /**/
    BYTE      secondUtc;                    /**/
} ZW_TIME_PARAMETERS_SET_FRAME;

/************************************************************/
/* Date Get command class structs */                        
/************************************************************/
typedef struct _ZW_DATE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_DATE_GET_FRAME;

/************************************************************/
/* Date Report command class structs */                     
/************************************************************/
typedef struct _ZW_DATE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
} ZW_DATE_REPORT_FRAME;

/************************************************************/
/* Time Get command class structs */                        
/************************************************************/
typedef struct _ZW_TIME_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_TIME_GET_FRAME;

/************************************************************/
/* Time Report command class structs */                     
/************************************************************/
typedef struct _ZW_TIME_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      hourLocalTime;                /* masked byte */
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
} ZW_TIME_REPORT_FRAME;

/************************************************************/
/* Date Get V2 command class structs */                     
/************************************************************/
typedef struct _ZW_DATE_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_DATE_GET_V2_FRAME;

/************************************************************/
/* Date Report V2 command class structs */                  
/************************************************************/
typedef struct _ZW_DATE_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      year1;                        /* MSB */
    BYTE      year2;                        /* LSB */
    BYTE      month;                        /**/
    BYTE      day;                          /**/
} ZW_DATE_REPORT_V2_FRAME;

/************************************************************/
/* Time Get V2 command class structs */                     
/************************************************************/
typedef struct _ZW_TIME_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_TIME_GET_V2_FRAME;

/************************************************************/
/* Time Offset Get V2 command class structs */              
/************************************************************/
typedef struct _ZW_TIME_OFFSET_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_TIME_OFFSET_GET_V2_FRAME;

/************************************************************/
/* Time Offset Report V2 command class structs */           
/************************************************************/
typedef struct _ZW_TIME_OFFSET_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      minuteTzo;                    /**/
    BYTE      level2;                       /* masked byte */
    BYTE      monthStartDst;                /**/
    BYTE      dayStartDst;                  /**/
    BYTE      hourStartDst;                 /**/
    BYTE      monthEndDst;                  /**/
    BYTE      dayEndDst;                    /**/
    BYTE      hourEndDst;                   /**/
} ZW_TIME_OFFSET_REPORT_V2_FRAME;

/************************************************************/
/* Time Offset Set V2 command class structs */              
/************************************************************/
typedef struct _ZW_TIME_OFFSET_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      level;                        /* masked byte */
    BYTE      minuteTzo;                    /**/
    BYTE      level2;                       /* masked byte */
    BYTE      monthStartDst;                /**/
    BYTE      dayStartDst;                  /**/
    BYTE      hourStartDst;                 /**/
    BYTE      monthEndDst;                  /**/
    BYTE      dayEndDst;                    /**/
    BYTE      hourEndDst;                   /**/
} ZW_TIME_OFFSET_SET_V2_FRAME;

/************************************************************/
/* Time Report V2 command class structs */                  
/************************************************************/
typedef struct _ZW_TIME_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      hourLocalTime;                /* masked byte */
    BYTE      minuteLocalTime;              /**/
    BYTE      secondLocalTime;              /**/
} ZW_TIME_REPORT_V2_FRAME;

/************************************************************/
/* User Code Get command class structs */                   
/************************************************************/
typedef struct _ZW_USER_CODE_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
} ZW_USER_CODE_GET_FRAME;

/************************************************************/
/* User Code Report command class structs */                
/************************************************************/
typedef struct _ZW_USER_CODE_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      userIdStatus;                 /**/
    BYTE      user_code1;                   /* MSB */
    BYTE      user_code2;                   
    BYTE      user_code3;                   
    BYTE      user_code4;                   
    BYTE      user_code5;                   
    BYTE      user_code6;                   
    BYTE      user_code7;                   
    BYTE      user_code8;                   
    BYTE      user_code9;                   
    BYTE      user_code10;                  /* LSB */
} ZW_USER_CODE_REPORT_FRAME;

/************************************************************/
/* User Code Set command class structs */                   
/************************************************************/
typedef struct _ZW_USER_CODE_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      userIdentifier;               /**/
    BYTE      userIdStatus;                 /**/
    BYTE      user_code1;                   /* MSB */
    BYTE      user_code2;                   
    BYTE      user_code3;                   
    BYTE      user_code4;                   
    BYTE      user_code5;                   
    BYTE      user_code6;                   
    BYTE      user_code7;                   
    BYTE      user_code8;                   
    BYTE      user_code9;                   
    BYTE      user_code10;                  /* LSB */
} ZW_USER_CODE_SET_FRAME;

/************************************************************/
/* Users Number Get command class structs */                
/************************************************************/
typedef struct _ZW_USERS_NUMBER_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_USERS_NUMBER_GET_FRAME;

/************************************************************/
/* Users Number Report command class structs */             
/************************************************************/
typedef struct _ZW_USERS_NUMBER_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      supportedUsers;               /**/
} ZW_USERS_NUMBER_REPORT_FRAME;

/************************************************************/
/* Version Command Class Get command class structs */       
/************************************************************/
typedef struct _ZW_VERSION_COMMAND_CLASS_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      requestedCommandClass;        /**/
} ZW_VERSION_COMMAND_CLASS_GET_FRAME;

/************************************************************/
/* Version Command Class Report command class structs */    
/************************************************************/
typedef struct _ZW_VERSION_COMMAND_CLASS_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      requestedCommandClass;        /**/
    BYTE      commandClassVersion;          /**/
} ZW_VERSION_COMMAND_CLASS_REPORT_FRAME;

/************************************************************/
/* Version Get command class structs */                     
/************************************************************/
typedef struct _ZW_VERSION_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_VERSION_GET_FRAME;

/************************************************************/
/* Version Report command class structs */                  
/************************************************************/
typedef struct _ZW_VERSION_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      zWaveLibraryType;             /**/
    BYTE      zWaveProtocolVersion;         /**/
    BYTE      zWaveProtocolSubVersion;      /**/
    BYTE      applicationVersion;           /**/
    BYTE      applicationSubVersion;        /**/
} ZW_VERSION_REPORT_FRAME;

/************************************************************/
/* Wake Up Interval Get command class structs */            
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_GET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_WAKE_UP_INTERVAL_GET_FRAME;

/************************************************************/
/* Wake Up Interval Report command class structs */         
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_REPORT_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     
    BYTE      seconds3;                     /* LSB */
    BYTE      nodeid;                       /**/
} ZW_WAKE_UP_INTERVAL_REPORT_FRAME;

/************************************************************/
/* Wake Up Interval Set command class structs */            
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_SET_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     
    BYTE      seconds3;                     /* LSB */
    BYTE      nodeid;                       /**/
} ZW_WAKE_UP_INTERVAL_SET_FRAME;

/************************************************************/
/* Wake Up No More Information command class structs */     
/************************************************************/
typedef struct _ZW_WAKE_UP_NO_MORE_INFORMATION_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_WAKE_UP_NO_MORE_INFORMATION_FRAME;

/************************************************************/
/* Wake Up Notification command class structs */            
/************************************************************/
typedef struct _ZW_WAKE_UP_NOTIFICATION_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_WAKE_UP_NOTIFICATION_FRAME;

/************************************************************/
/* Wake Up Interval Capabilities Get V2 command class structs */
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_CAPABILITIES_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_WAKE_UP_INTERVAL_CAPABILITIES_GET_V2_FRAME;

/************************************************************/
/* Wake Up Interval Capabilities Report V2 command class structs */
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      minimumWakeUpIntervalSeconds1;/* MSB */
    BYTE      minimumWakeUpIntervalSeconds2;
    BYTE      minimumWakeUpIntervalSeconds3;/* LSB */
    BYTE      maximumWakeUpIntervalSeconds1;/* MSB */
    BYTE      maximumWakeUpIntervalSeconds2;
    BYTE      maximumWakeUpIntervalSeconds3;/* LSB */
    BYTE      defaultWakeUpIntervalSeconds1;/* MSB */
    BYTE      defaultWakeUpIntervalSeconds2;
    BYTE      defaultWakeUpIntervalSeconds3;/* LSB */
    BYTE      wakeUpIntervalStepSeconds1;   /* MSB */
    BYTE      wakeUpIntervalStepSeconds2;   
    BYTE      wakeUpIntervalStepSeconds3;   /* LSB */
} ZW_WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2_FRAME;

/************************************************************/
/* Wake Up Interval Get V2 command class structs */         
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_GET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_WAKE_UP_INTERVAL_GET_V2_FRAME;

/************************************************************/
/* Wake Up Interval Report V2 command class structs */      
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_REPORT_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     
    BYTE      seconds3;                     /* LSB */
    BYTE      nodeid;                       /**/
} ZW_WAKE_UP_INTERVAL_REPORT_V2_FRAME;

/************************************************************/
/* Wake Up Interval Set V2 command class structs */         
/************************************************************/
typedef struct _ZW_WAKE_UP_INTERVAL_SET_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      seconds1;                     /* MSB */
    BYTE      seconds2;                     
    BYTE      seconds3;                     /* LSB */
    BYTE      nodeid;                       /**/
} ZW_WAKE_UP_INTERVAL_SET_V2_FRAME;

/************************************************************/
/* Wake Up No More Information V2 command class structs */  
/************************************************************/
typedef struct _ZW_WAKE_UP_NO_MORE_INFORMATION_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_WAKE_UP_NO_MORE_INFORMATION_V2_FRAME;

/************************************************************/
/* Wake Up Notification V2 command class structs */         
/************************************************************/
typedef struct _ZW_WAKE_UP_NOTIFICATION_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
} ZW_WAKE_UP_NOTIFICATION_V2_FRAME;

/************************************************************/
/* Lowpan First Fragment 1byte command class structs */     
/************************************************************/
typedef struct _ZW_LOWPAN_FIRST_FRAGMENT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      payload1;                     
} ZW_LOWPAN_FIRST_FRAGMENT_1BYTE_FRAME;

/************************************************************/
/* Lowpan First Fragment 2byte command class structs */     
/************************************************************/
typedef struct _ZW_LOWPAN_FIRST_FRAGMENT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      payload1;                     /* MSB */
    BYTE      payload2;                     /* LSB */
} ZW_LOWPAN_FIRST_FRAGMENT_2BYTE_FRAME;

/************************************************************/
/* Lowpan First Fragment 3byte command class structs */     
/************************************************************/
typedef struct _ZW_LOWPAN_FIRST_FRAGMENT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      payload1;                     /* MSB */
    BYTE      payload2;                     
    BYTE      payload3;                     /* LSB */
} ZW_LOWPAN_FIRST_FRAGMENT_3BYTE_FRAME;

/************************************************************/
/* Lowpan First Fragment 4byte command class structs */     
/************************************************************/
typedef struct _ZW_LOWPAN_FIRST_FRAGMENT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      payload1;                     /* MSB */
    BYTE      payload2;                     
    BYTE      payload3;                     
    BYTE      payload4;                     /* LSB */
} ZW_LOWPAN_FIRST_FRAGMENT_4BYTE_FRAME;

/************************************************************/
/* Lowpan Subsequent Fragment 1byte command class structs */
/************************************************************/
typedef struct _ZW_LOWPAN_SUBSEQUENT_FRAGMENT_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      datagramOffset;               /**/
    BYTE      payload1;                     
} ZW_LOWPAN_SUBSEQUENT_FRAGMENT_1BYTE_FRAME;

/************************************************************/
/* Lowpan Subsequent Fragment 2byte command class structs */
/************************************************************/
typedef struct _ZW_LOWPAN_SUBSEQUENT_FRAGMENT_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      datagramOffset;               /**/
    BYTE      payload1;                     /* MSB */
    BYTE      payload2;                     /* LSB */
} ZW_LOWPAN_SUBSEQUENT_FRAGMENT_2BYTE_FRAME;

/************************************************************/
/* Lowpan Subsequent Fragment 3byte command class structs */
/************************************************************/
typedef struct _ZW_LOWPAN_SUBSEQUENT_FRAGMENT_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      datagramOffset;               /**/
    BYTE      payload1;                     /* MSB */
    BYTE      payload2;                     
    BYTE      payload3;                     /* LSB */
} ZW_LOWPAN_SUBSEQUENT_FRAGMENT_3BYTE_FRAME;

/************************************************************/
/* Lowpan Subsequent Fragment 4byte command class structs */
/************************************************************/
typedef struct _ZW_LOWPAN_SUBSEQUENT_FRAGMENT_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd_Properties1;              /* The command + parameter Properties1 */
    BYTE      datagramSize2;                /**/
    BYTE      datagramTag;                  /**/
    BYTE      datagramOffset;               /**/
    BYTE      payload1;                     /* MSB */
    BYTE      payload2;                     
    BYTE      payload3;                     
    BYTE      payload4;                     /* LSB */
} ZW_LOWPAN_SUBSEQUENT_FRAGMENT_4BYTE_FRAME;

/************************************************************/
/* Command Zip Packet 1byte command class structs */        
/************************************************************/
typedef struct _ZW_COMMAND_ZIP_PACKET_1BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      seqNo;                        /**/
    BYTE      zWaveHomeId1;                 /* MSB */
    BYTE      zWaveHomeId2;                 
    BYTE      zWaveHomeId3;                 
    BYTE      zWaveHomeId4;                 /* LSB */
    BYTE      zWaveSourceNodeId;            /**/
    BYTE      zWaveDestinationNodeId;       /**/
    BYTE      zIpGatewayMacAddress1;        /* MSB */
    BYTE      zIpGatewayMacAddress2;        
    BYTE      zIpGatewayMacAddress3;        
    BYTE      zIpGatewayMacAddress4;        
    BYTE      zIpGatewayMacAddress5;        
    BYTE      zIpGatewayMacAddress6;        /* LSB */
    BYTE      customerId1;                  /* MSB */
    BYTE      customerId2;                  
    BYTE      customerId3;                  
    BYTE      customerId4;                  
    BYTE      customerId5;                  
    BYTE      customerId6;                  
    BYTE      customerId7;                  
    BYTE      customerId8;                  /* LSB */
    BYTE      webKeepaliveDelay;            /**/
    BYTE      headerExtensionSize;          /**/
    BYTE      headerExtensionVersion;       /**/
    BYTE      headerExtension1;             
    BYTE      zWaveCommand1;                
} ZW_COMMAND_ZIP_PACKET_1BYTE_FRAME;

/************************************************************/
/* Command Zip Packet 2byte command class structs */        
/************************************************************/
typedef struct _ZW_COMMAND_ZIP_PACKET_2BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      seqNo;                        /**/
    BYTE      zWaveHomeId1;                 /* MSB */
    BYTE      zWaveHomeId2;                 
    BYTE      zWaveHomeId3;                 
    BYTE      zWaveHomeId4;                 /* LSB */
    BYTE      zWaveSourceNodeId;            /**/
    BYTE      zWaveDestinationNodeId;       /**/
    BYTE      zIpGatewayMacAddress1;        /* MSB */
    BYTE      zIpGatewayMacAddress2;        
    BYTE      zIpGatewayMacAddress3;        
    BYTE      zIpGatewayMacAddress4;        
    BYTE      zIpGatewayMacAddress5;        
    BYTE      zIpGatewayMacAddress6;        /* LSB */
    BYTE      customerId1;                  /* MSB */
    BYTE      customerId2;                  
    BYTE      customerId3;                  
    BYTE      customerId4;                  
    BYTE      customerId5;                  
    BYTE      customerId6;                  
    BYTE      customerId7;                  
    BYTE      customerId8;                  /* LSB */
    BYTE      webKeepaliveDelay;            /**/
    BYTE      headerExtensionSize;          /**/
    BYTE      headerExtensionVersion;       /**/
    BYTE      headerExtension1;             /* MSB */
    BYTE      headerExtension2;             /* LSB */
    BYTE      zWaveCommand1;                /* MSB */
    BYTE      zWaveCommand2;                /* LSB */
} ZW_COMMAND_ZIP_PACKET_2BYTE_FRAME;

/************************************************************/
/* Command Zip Packet 3byte command class structs */        
/************************************************************/
typedef struct _ZW_COMMAND_ZIP_PACKET_3BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      seqNo;                        /**/
    BYTE      zWaveHomeId1;                 /* MSB */
    BYTE      zWaveHomeId2;                 
    BYTE      zWaveHomeId3;                 
    BYTE      zWaveHomeId4;                 /* LSB */
    BYTE      zWaveSourceNodeId;            /**/
    BYTE      zWaveDestinationNodeId;       /**/
    BYTE      zIpGatewayMacAddress1;        /* MSB */
    BYTE      zIpGatewayMacAddress2;        
    BYTE      zIpGatewayMacAddress3;        
    BYTE      zIpGatewayMacAddress4;        
    BYTE      zIpGatewayMacAddress5;        
    BYTE      zIpGatewayMacAddress6;        /* LSB */
    BYTE      customerId1;                  /* MSB */
    BYTE      customerId2;                  
    BYTE      customerId3;                  
    BYTE      customerId4;                  
    BYTE      customerId5;                  
    BYTE      customerId6;                  
    BYTE      customerId7;                  
    BYTE      customerId8;                  /* LSB */
    BYTE      webKeepaliveDelay;            /**/
    BYTE      headerExtensionSize;          /**/
    BYTE      headerExtensionVersion;       /**/
    BYTE      headerExtension1;             /* MSB */
    BYTE      headerExtension2;             
    BYTE      headerExtension3;             /* LSB */
    BYTE      zWaveCommand1;                /* MSB */
    BYTE      zWaveCommand2;                
    BYTE      zWaveCommand3;                /* LSB */
} ZW_COMMAND_ZIP_PACKET_3BYTE_FRAME;

/************************************************************/
/* Command Zip Packet 4byte command class structs */        
/************************************************************/
typedef struct _ZW_COMMAND_ZIP_PACKET_4BYTE_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
    BYTE      seqNo;                        /**/
    BYTE      zWaveHomeId1;                 /* MSB */
    BYTE      zWaveHomeId2;                 
    BYTE      zWaveHomeId3;                 
    BYTE      zWaveHomeId4;                 /* LSB */
    BYTE      zWaveSourceNodeId;            /**/
    BYTE      zWaveDestinationNodeId;       /**/
    BYTE      zIpGatewayMacAddress1;        /* MSB */
    BYTE      zIpGatewayMacAddress2;        
    BYTE      zIpGatewayMacAddress3;        
    BYTE      zIpGatewayMacAddress4;        
    BYTE      zIpGatewayMacAddress5;        
    BYTE      zIpGatewayMacAddress6;        /* LSB */
    BYTE      customerId1;                  /* MSB */
    BYTE      customerId2;                  
    BYTE      customerId3;                  
    BYTE      customerId4;                  
    BYTE      customerId5;                  
    BYTE      customerId6;                  
    BYTE      customerId7;                  
    BYTE      customerId8;                  /* LSB */
    BYTE      webKeepaliveDelay;            /**/
    BYTE      headerExtensionSize;          /**/
    BYTE      headerExtensionVersion;       /**/
    BYTE      headerExtension1;             /* MSB */
    BYTE      headerExtension2;             
    BYTE      headerExtension3;             
    BYTE      headerExtension4;             /* LSB */
    BYTE      zWaveCommand1;                /* MSB */
    BYTE      zWaveCommand2;                
    BYTE      zWaveCommand3;                
    BYTE      zWaveCommand4;                /* LSB */
} ZW_COMMAND_ZIP_PACKET_4BYTE_FRAME;



/**********************************************************************/
/* Command class structs use to encapsulating other commands          */
/* Do not define these commands in ZW_FRAME_COLLECTION_MACRO          */
/* Do not include commands defined in ZW_FRAME_COLLECTION_MACRO below */
/**********************************************************************/
typedef union _ALL_EXCEPT_ENCAP
{
  ZW_FRAME_COLLECTION_MACRO
} ALL_EXCEPT_ENCAP;

typedef struct _ZW_MULTI_COMMAND_ENCAP_FRAME_
{
  BYTE        cmdClass;            /* The command class */
  BYTE        cmd;                 /* The command */
  BYTE        numberOfCommands;
  BYTE        commandLength;
  ALL_EXCEPT_ENCAP     encapFrame;
} ZW_MULTI_COMMAND_ENCAP_FRAME;

typedef struct _ZW_COMPOSITE_CMD_ENCAP_FRAME_
{
  BYTE        cmdClass;            /* The command class */
  BYTE        cmd;                 /* The command */
  BYTE        endPointMask1;       /* End point mask 1 */
  BYTE        endPointMask2;       /* End point mask 2 */
  ALL_EXCEPT_ENCAP     encapFrame;
} ZW_COMPOSITE_CMD_ENCAP_FRAME;

typedef struct _ZW_COMPOSITE_REPLY_ENCAP_FRAME_
{
  BYTE        cmdClass;            /* The command class */
  BYTE        cmd;                 /* The command */
  BYTE        endPoint;            /* Bit7-5=Reserved and Bit0-4=End Point */
  ALL_EXCEPT_ENCAP     encapFrame;
} ZW_COMPOSITE_REPLY_ENCAP_FRAME;

typedef struct _ZW_MULTI_INSTANCE_CMD_ENCAP_FRAME_
{
  BYTE        cmdClass;            /* The command class */
  BYTE        cmd;                 /* The command */
  BYTE        instance;            /* The instance to access */  
  ALL_EXCEPT_ENCAP     encapFrame;
} ZW_MULTI_INSTANCE_CMD_ENCAP_FRAME;

typedef struct _ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME_
{
    BYTE      cmdClass;                     /* The command class */
    BYTE      cmd;                          /* The command */
    BYTE      properties1;                  /* masked byte */
    BYTE      properties2;                  /* masked byte */
  ALL_EXCEPT_ENCAP     encapFrame;
} ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME;

typedef struct _ZW_SECURITY_MESSAGE_ENCAP_FRAME_
{
  BYTE        cmdClass;            /* The command class */
  BYTE        cmd;                 /* The command */
  BYTE        initVectorByte1;     /* The initialization vector byte 1 (MSB) */
  BYTE        initVectorByte2;     /* The initialization vector byte 2  */
  BYTE        initVectorByte3;     /* The initialization vector byte 3 */
  BYTE        initVectorByte4;     /* The initialization vector byte 4 */
  BYTE        initVectorByte5;     /* The initialization vector byte 5 */
  BYTE        initVectorByte6;     /* The initialization vector byte 6 */
  BYTE        initVectorByte7;     /* The initialization vector byte 7 */
  BYTE        initVectorByte8;     /* The initialization vector byte 8 (LSB) */
  BYTE        securityEncapMessage[29];
  BYTE        receiverNonceIdent;
  BYTE        messageAuthenticationCodeByte1; /* The Authentication code byte 1 (MSB) */
  BYTE        messageAuthenticationCodeByte2; /* The Authentication code byte 2 */
  BYTE        messageAuthenticationCodeByte3; /* The Authentication code byte 3 */
  BYTE        messageAuthenticationCodeByte4; /* The Authentication code byte 4 */
  BYTE        messageAuthenticationCodeByte5; /* The Authentication code byte 5 */
  BYTE        messageAuthenticationCodeByte6; /* The Authentication code byte 6 */
  BYTE        messageAuthenticationCodeByte7; /* The Authentication code byte 7 */
  BYTE        messageAuthenticationCodeByte8; /* The Authentication code byte 8 (LSB) */
} ZW_SECURITY_MESSAGE_ENCAP_FRAME;

/************************************************************/
/* Union of all command classes                             */
/************************************************************/
typedef union _ZW_APPLICATION_TX_BUFFER_
{
  ZW_MULTI_COMMAND_ENCAP_FRAME           ZW_MultiCommandEncapFrame;
  ZW_COMPOSITE_CMD_ENCAP_FRAME           ZW_CompositeCmdEncapFrame;
  ZW_COMPOSITE_REPLY_ENCAP_FRAME         ZW_CompositeReplyEncapFrame;
  ZW_MULTI_INSTANCE_CMD_ENCAP_FRAME      ZW_MultiInstanceCmdEncapFrame;
  ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME    ZW_MultiChannelCmdEncapV2Frame;
  ZW_FRAME_COLLECTION_MACRO
} ZW_APPLICATION_TX_BUFFER;

/************************************************************/
/* Union of all command classes with room for a full        */
/* meta data frame                                          */
/************************************************************/
typedef union _ZW_APPLICATION_META_TX_BUFFER_
{
  ZW_MULTI_COMMAND_ENCAP_FRAME           ZW_MultiCommandEncapFrame;
  ZW_COMPOSITE_CMD_ENCAP_FRAME           ZW_CompositeCmdEncapFrame;
  ZW_COMPOSITE_REPLY_ENCAP_FRAME         ZW_CompositeReplyEncapFrame;
  ZW_MULTI_INSTANCE_CMD_ENCAP_FRAME      ZW_MultiInstanceCmdEncapFrame;
  ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME    ZW_MultiChannelCmdEncapV2Frame;
  ZW_FRAME_COLLECTION_MACRO
  BYTE                                    bPadding[META_DATA_MAX_DATA_SIZE];
} ZW_APPLICATION_META_TX_BUFFER;

//================== in file "ZW_classcmd.h" end


void *process_exe_callback(void);

void* process_timer(void);
void* process_zwave_rx(void);
void* process_zwave_apipoll(void);

unsigned long atoh(char *s);
void dormir(unsigned long microsecs);
unsigned long htime(void);

void setcom(void);
int openCom(short value);
short writecom(char *text, unsigned int length);
short Uart_tx_cmd(int length, char *at_cmdstring);

void Dispatch(DONGLE_INFO *p, BYTE *pData );
void SerialAPI_Poll( void );

BYTE ZW_Version(HCMD p, BYTE *pBuf);
BYTE ZW_SerialAPI_GetInitData(HCMD p, BYTE *ver, BYTE *capabilities, BYTE *len, BYTE *nodesList );
void ZW_GetNodeProtocolInfo(HCMD p, WORD bNodeID, NODEINFO *nodeInfo );
BYTE ZW_GetControllerCapabilities(HCMD p);
void ZW_MemoryGetID( HCMD p, BYTE *pHomeID, BYTE *pNodeID );
BYTE ZW_GetSUCNodeID( HCMD p );


void ZW_AddNodeToNetwork(HCMD p, BYTE bMode, VOID_CALLBACKFUNC(completedFunc)(auto LEARN_INFO*));
void ZW_RemoveNodeFromNetwork(HCMD p, BYTE bMode, VOID_CALLBACKFUNC(completedFunc)(auto LEARN_INFO*));
BYTE ZW_SendData(HCMD p, BYTE  nodeID, BYTE *pData, BYTE  dataLength, BYTE  txOptions, VOID_CALLBACKFUNC(completedFunc)(BYTE));

BOOL ZW_RequestNodeInfo(HCMD p, WORD nodeID, VOID_CALLBACKFUNC(completedFunc)(auto BYTE));
BYTE ZW_SendDataMulti(HCMD p, BYTE *pNodeIDList, BYTE numberNodes, BYTE *pData, BYTE dataLength, BYTE txOptions, VOID_CALLBACKFUNC ( completedFunc ) ( auto BYTE txStatus ) );

void ZW_SetDefault(HCMD p, VOID_CALLBACKFUNC(completedFunc)(void));
void ZW_SoftReset(HCMD p);

BYTE ZW_RFPowerLevelGet(HCMD p);
BYTE ZW_RFPowerLevelSet(HCMD p, BYTE powerLevel) ;

BYTE ZW_isFailedNode(HCMD p, BYTE nodeID);
BYTE ZW_RemoveFailedNode(HCMD p, BYTE NodeID, VOID_CALLBACKFUNC(completedFunc)(BYTE));

BYTE ZW_ExploreRequestInclusion(HCMD p);


void NodeMaskSetBit(BYTE *pMask, BYTE bNodeID);
void ResetDefaultCompleted(void);

void LearnModeCompleted(LEARN_INFO *learnNodeInfo);
void LearnModeCompleted_org(LEARN_INFO *learnNodeInfo);

void RemoveNodeCompleted(LEARN_INFO *learnNodeInfo);
void RemoveNodeCompleted_org(LEARN_INFO *learnNodeInfo);

void RemoveNodeFromGroup(BYTE bNodeID);  //IN NodeID to remove
void SendCompleted(BYTE txStatus);
void RemoveFailCompleted(BYTE txStatus);

BYTE Analysis_DeviceType(NODEINFO *nodeInfo);

void PrepareBinaryGet ( ZW_APPLICATION_TX_BUFFER *pBuf );
void PrepareBinarySet ( ZW_APPLICATION_TX_BUFFER *pBuf , BYTE value );

void PrepareMultilevelStop ( ZW_APPLICATION_TX_BUFFER *pBuf);
void PrepareMultilevelGet ( ZW_APPLICATION_TX_BUFFER *pBuf );
void PrepareMultilevelSet ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE value );
void PrepareMultilevelStartLevelChange ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE dir, BYTE startlevel);

void PrepareBasicGet ( ZW_APPLICATION_TX_BUFFER *pBuf );
void PrepareBasicSet ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE status );

void PrepareMeterSupport ( ZW_APPLICATION_TX_BUFFER *pBuf );
void PrepareMeterGet ( ZW_APPLICATION_TX_BUFFER *pBuf , BYTE value);
void PrepareMultiInstanceGet ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE cmdclass );
//void PrepareMultiInstanceEncap ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE instance );
void PrepareMultiInstanceEncap ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE instance, BYTE seqnum, int cmd);

void PrepareBatteryGet ( ZW_APPLICATION_TX_BUFFER *pBuf );
void PrepareWakeUpIntervaleGet ( ZW_APPLICATION_TX_BUFFER *pBuf );
void PrepareWakeUpIntervaleSet ( ZW_APPLICATION_TX_BUFFER *pBuf , BYTE node, int sec);
void PrepareWakeUpNoMoreInformation ( ZW_APPLICATION_TX_BUFFER *pBuf );

void PrepareAssociationGet( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE groupnum);
void PrepareAssociationRemove( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE groupnum, BYTE node);
void PrepareAssociationSet( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE groupnum, BYTE node);


void SendDataAndWaitForResponse( BYTE *buffer, BYTE length, BYTE byFunc, BYTE *out, BYTE *byLen );
BYTE SendData(BYTE *buffer, BYTE length);
void TransmitData(BYTE *buffer, BYTE length);
BYTE CalculateChecksum( BYTE *pData, int nLength );
BOOL WaitForAck();

void TimerInit(void);
void TimerAction( void );
BYTE TimerCancel(BYTE btimerHandle);
BYTE TimerStart( VOID_CALLBACKFUNC(func)(), BYTE btimerTicks, BYTE brepeats);
static void TimerSave(BYTE btimerHandle, VOID_CALLBACKFUNC(func)(), BYTE btimerTicks, BYTE brepeats);

void ReceiveAckIndicateTimeOut( void );
int ReceiveData( BYTE *buffer );
BYTE bySerialCheck( void );
BYTE bySerialGetByte( void );
void WaitForResponse( BYTE byFunc, BYTE *buffer, BYTE *byLen );
void WaitForResponseIndicateTimeOut( void );

BOOL SerialAPI_InitSW(void ( *funcApplCmdHandler ) (BYTE, BYTE, ZW_APPLICATION_TX_BUFFER *, BYTE),void ( *funcAppConUpdate ) (BYTE, BYTE, BYTE*, BYTE, BYTE ));
//BOOL SerialAPI_InitSW(void ( *funcApplCmdHandler ) (BYTE, BYTE, ZW_APPLICATION_TX_BUFFER *, BYTE), void ( *funcAppConUpdate ) (BYTE, BYTE, BYTE*, BYTE ));
BOOL API_SerialAPI_InitCBFunction(void ( *funcGetNIF )(WORD, BYTE), void ( *funcIRSensorStatus )(WORD , BYTE), void ( *funcGetWUN)(WORD));


void ApplicationCommandHandler(DONGLE_INFO *p, BYTE rxStatus, BYTE sourceNode, ZW_APPLICATION_TX_BUFFER *pCmd, BYTE  cmdLength);
void ApplicationControllerUpdate(DONGLE_INFO *p, BYTE bStatus, BYTE bNodeID, BYTE *pCmd, BYTE bLen, BYTE type);


BYTE Choice_get_type(BYTE type, ZW_APPLICATION_TX_BUFFER *pBuf);

BYTE API_Initial(DONGLE_INFO *p);

BYTE API_SetBinarySwitch(WORD node, BYTE value);
BYTE API_GetBinarySwitchStatus(WORD node, EACH_NODE_INFO* p);
BYTE API_SetAndGetBinarySwitch(WORD node, BYTE value, EACH_NODE_INFO* p);

BYTE API_SetDimmer(WORD node, BYTE value);
BYTE API_GetDimmerStatus(WORD node, EACH_NODE_INFO* p);
BYTE API_SetAndGetDimmer(WORD node, BYTE value, EACH_NODE_INFO* p);

BYTE API_SetCurtain(WORD node, BYTE value);

BYTE API_GetMeter(WORD node, EACH_NODE_INFO* p);

BYTE API_ResetToDefault(void);


//BYTE API_StartAddNodeIntoNetwork(void);
BYTE API_StartAddNodeIntoNetwork(unsigned int *flag, unsigned int* type);
BYTE API_StopAddNodeIntoNetwork(unsigned int *flag, unsigned int* type);

BYTE API_StartRemoveNodeFromNetwork(unsigned int *flag, unsigned int* type);
BYTE API_StopRemoveNodeFromNetwork(unsigned int *flag, unsigned int* type);

BYTE GetStatusSelectDevicetype(BYTE type, ZW_APPLICATION_TX_BUFFER *pBuf);
BYTE GetStatusAnalysis(BYTE node, BYTE type, EACH_NODE_INFO* p, BYTE status);


BYTE API_StartAddNodeIntoNetwork_cb(VOID_CALLBACKFUNC(completedFunc)(BYTE state, ADDREMOVE_NODE_INFO* p));
void AP_callback_removeadd(BYTE state, ADDREMOVE_NODE_INFO* p);
BYTE API_StartRemoveNodeFromNetwork_cb(VOID_CALLBACKFUNC(completedFunc)(BYTE state, ADDREMOVE_NODE_INFO* p));


BYTE API_GetMultiInstanceNum(WORD node, BYTE cmdclass, EACH_NODE_INFO* p);
BYTE API_SetAVDevice(WORD node, int cmd);

BYTE API_AssociationSetandRemoveandGet(BYTE type, WORD prinode, WORD secnode);
BYTE API_RequestNodeInfo(WORD node);

void AP_callback_getIRSensorStatus(WORD node, BYTE status);
void AP_callback_getNIF(WORD node, BYTE type);
void AP_callback_getWakeUpNotification(WORD node);

BYTE API_GetBatteryLevel(WORD node, EACH_NODE_INFO* p);
BYTE API_CheckisFailNode(WORD node);
//======


#endif
