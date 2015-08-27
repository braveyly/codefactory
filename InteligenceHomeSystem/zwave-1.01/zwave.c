/*
 *Auther : Haas Tsai
 *	
 * Revision : 1.01, 2011-02-18 
 * 	(1) API for z-wave initial and control.
 * Revision : 1.02, 2011-02-24
 * 	(1) Add software reset function.
 *  (2) Add power meter function.
 *  (3) Modify ZW_SendData() form SendData to SendDataAndWaitForResponse.
 * Revision : 1.02, 2011-03-02	
 * 	(1) Add test parallel sending command in case 96.
 * Revision : 1.03, 2011-03-14
 *	(1) Add ZW_SendDataMulti() API.
 * Revision : 1.03, 2011-03-15
 *	(1) Modify all API into local variable.
 * Revision : 1.03, 2011-03-16, 17, 18
 *	(1) Set initial information and node status information into one structure. 
 *	(2) Add one flag indicate the transmission condition.
 *	(3) Add binary switch and curtain status define.
 * 	(4) Add stop curtain API.
 *	(5) Modify cmd_exe_status.
 *	(6) Add switch all on/off API.
 * Revision : 1.04, 2011-03-25
 *	(1) Modify cmd_exe_status back to idle when ZW_SendData full.
 *	(2) Add "p->node_info_table[sourceNode].id = sourceNode" in ApplicationCommandHandler.
 *	(3) Remove "cmd_exe_status = STATUS_IDLE" in ApplicationCommandHandler.
 *	(4) Modify the cmd_exe_status in ZW_RequestNodeInfo().
 * Revision : 1.05, 2011-03-28, 29, 30, 31
 *	(1) Add case 19, 20 for binary switch set/get
 *	(2) Modify Basic/Multilevel/Meter Get, modify printf() length.
 *	(3) Add get status flag for time out.
 *	(4) Modify LearnModeCompleted, for get node status normal.
 *	(5) Add case 21, meter support get.
 *	(6) Modify meter value analysis.
 *	(7) Add case 22,23 for check RF powerlevel(no use).
 *	(8) Add case 24, battery level get API.
 *	(9) Add case 25, 26, 27, the wake up relative API.
 * Revision : 1.06, 2011-04-01
 *	(1)Add case 28, 29, set fail node API, remove failed node API. 
 * Revision : 1.07, 2011-04-13, 2011-04-22, 2011-04-25
 *	(1) Modify the remove node also remove nodeID issue. 	 
 *	(2) Add case 30, 31, 32, association set, association get, association remove APIs.
 *	(3) Add "zw_senddata_status" flag to indicate API transmit command status.
 *	(4) Add case 40, 41, package the "set status", "get status" process to Nick.
 *	(5) Add return value of function "SendData", if no ACK, we don't need to call waitForResponse.
 *	(6) Package curtain controller/power meter API for Nick.
 * Revision : 1.08, 2011-04-26, 2011-04-27, 2011-05-02, 2011-0504, 2011-05-09
 *	(1) Package reset to default "API_ResetToDefault"	
 *	(2) Removed the global variable "txBuf".
 *	(3) Modify Add/remove node API.
 * 	(4) Add test multi-instance api.
 *	(5) Modify GetMeter API.
 * Revision : 1.09, 2011-05-11, 2011-05-12, 2011-05-13
 *	(1) Modify node format for IR controller and add IR control API.
 *	(2) Add callback function for AP, when get NIF and get IR sensor status.
 *			NIF callback function for getting device status
 *      IR sensor status callback function for telling AP sensor status.
 *  (3) Modify API request node information.
 *	(4) Add association API for setting IR sensor device.
 *	(5) Modify Add/Remove Node function.
 *	(6) Add get battery level API. Fix initial fail issue.
 *	(7) Add get "wake up notification" frame callback function for AP.
 *	(8) Add check is fail node API.
 *	(9) Add one process to execute AP_callback function. To prevent Dispatch block issue.
 * Revision : 1.10, 2011-05-20,2011-05-24
 *	(1) Fix bug Add/Remove node stop, flag can't go back to normal issue.
 *	(2) Handle buffer full condition, reinitial the buffer index.
 *	(3) Add prevent blocked status machnism.
 *	(4) Modify curtain control command, and add ir sensor return value.
 *
 *
 *
 */
 
 
/**********************************************************/
// INCLUDES                                                                  
/**********************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "zwave.h"


/**********************************************************/
// DEFINE OR VARIABLES OR FUNCTION PROTOTYPE                                          
/**********************************************************/
//#define DEBUG_USB_RX	1



//====== Z-wave USB interface
#define NUM_COM_DEVICE_NAME     20
#define MAX_SERIAL_STRING       2048
#define MAX_READ_USB_DEV_ERR 	100  //10 sec

struct termio stbuf;  			// termios 
int is_zwave_open = 0;
int comfd = 0; 				// Communication file descriptor.  Defaults to stdin.
char device[NUM_COM_DEVICE_NAME]; 	// Comm device. 
unsigned long hstart, hset;
long senddelay = 0; 			// 0/100th second character delay for sending 
int zwave_speed = B0; 			// Set to B110, B150, B300,..., B38400
int clocal = 0, parity = 0, bits = CS8, stopbits = 0;


//====== Z-wave initial and control
//static BYTE idx;
//static BYTE byLen;
//static BYTE byCompletedFunc;
//BYTE buffer[ BUF_SIZE ];	//74 bytes

BYTE pCmd[ BUF_SIZE ];

// Buffer for incoming frames.
BYTE RecvBuffer[ BUF_SIZE ];

// Buffer for outgoing frames.
// Is primarily used to remember outgoing telegrams in case a retransmission is needed.
BYTE TransmitBuffer[ BUF_SIZE ];

// Holds the length of the frame currently in the \ref TransmitBuffer.
volatile static BYTE TransmitBufferLength = 0;

static BOOL bWaitForResponseRunning = FALSE;
static BYTE workbuf[BUF_SIZE_RX];        // Used for frames received from remote side,74 bytes
static BYTE compl_workbuf[BUF_SIZE_TX];  // Used for frames send to remote side, 60 bytes

// Indicate whether acknowledgment is expected or not.  
static BOOL waitForAck;

static BOOL receiveDataTimeOut;
BOOL  receiveAskTimeOut;
static BOOL bReceiveDataRunning = FALSE;

// Receive and transmit buffer
volatile BYTE rxQueue[UART_RX_BUFSIZE]; // 128 bytes
volatile BYTE txQueue[UART_TX_BUFSIZE];	// 128 bytes

volatile WORD rxInPtr, rxOutPtr;
volatile WORD rxCount;

volatile WORD txInPtr, txOutPtr;
volatile WORD txCount;

// Used for storing the result of \ref ReceiveData. see SAUSC
static BYTE zw_result = 0;

// Flags used by the I/O functions
typedef struct __IOFLAGS
{
  unsigned rx:1;
  unsigned tx:1;
  unsigned err:1;
}IOFLAGS;
IOFLAGS	ioflags;

BYTE  timerReceiveTimer;

// timer elements
static struct timerElement
{
  WORD  tickCounts;
  WORD  timeoutValue;
  void (*func)(void);
  BYTE  repeats;
  BYTE  enable;
} timerArray[TIMER_MAX];

// Incremented every 10msec for ZW010x and every 2msec for ZW020x and this 
// done by the timer interrupt function and decremented by the timer action function
volatile BYTE timerCount; 
int timerctr2;
                                                       
static BOOL waitForResponseTimeOut;                                 

BYTE myNodeID = 0;
BYTE myHomeID[4];

LEARN_INFO learnNodeInfo;

static void ( *funcApplicationCommandHandler ) ( DONGLE_INFO *, BYTE, BYTE, ZW_APPLICATION_TX_BUFFER *, BYTE );
static void ( *funcApplicationControllerUpdate ) ( DONGLE_INFO *, BYTE,  BYTE, BYTE*, BYTE , BYTE);
static void ( *cbFuncZWSendData ) ( BYTE txStatus );
static void ( *cbFuncZWSendDataMulti ) ( BYTE txStatus );
static void ( *cbFuncZWSendNodeInformation ) ( BYTE txStatus );
static void ( *cbFuncMemoryPutBuffer ) ( void );
static void ( *cbFuncZWSetDefault ) ( void );
static void ( *cbFuncZWNewController ) ( LEARN_INFO* );
static void ( *cbFuncZWReplicationSendData ) ( BYTE txStatus );
static void ( *cbFuncZWAssignReturnRoute ) ( BYTE bStatus );
static void ( *cbFuncZWAssignSUCReturnRoute ) ( BYTE bStatus );
static void ( *cbFuncZWDeleteSUCReturnRoute ) ( BYTE bStatus );
static void ( *cbFuncZWDeleteReturnRoute ) ( BYTE bStatus );
static void ( *cbFuncZWSetLearnMode ) (LEARN_INFO*);  //BYTE bStatus, BYTE nodeID
static void ( *cbFuncZWRequestNodeNodeNeighborUpdate ) ( BYTE bStatus );
static void ( *cbFuncZWSetSUCNodeID ) ( BYTE bSstatus );
static void ( *cbFuncZWRequestNetworkUpdate ) ( BYTE txStatus );
static void ( *cbFuncZWRemoveFailNodeID ) ( BYTE txStatus );	// haas added


BYTE bRequestTimeoutHandle = 0xFF;

// Application can use this flag to check if learn mode is active                             
//BOOL learnInProgress = FALSE;           
BYTE get_nodeID;

//ZW_APPLICATION_TX_BUFFER txBuf, txBuf2;

static BYTE groupMask[MAX_NODEMASK_LENGTH]; // Used for sending nodelist in a multicast
static BYTE groupNumber[ZW_MAX_NODES];
static BYTE test_number = 0;


//========== 2011-03-02 test parallel sending command
SCMD test_cmd1, test_cmd2;

void init_buffer(HCMD p)
{
	memset(p, '\0', sizeof(SCMD));
}
//========== end


DONGLE_INFO device_info; 
BYTE cmd_exe_status = STATUS_IDLE;	

//unsigned short get_status_flag_ctr = 0;


static int zw_senddata_status = STATUS_NONE;
BYTE get_status_flag = GET_STATUS_OFF;    					
BYTE get_info_flag = GET_STATUS_OFF;

static int device_type;

unsigned int* pairing_status = NULL;
unsigned int* add_remove_type = NULL;


BYTE addtimer_flag = 0;
int addtimer = 0;
BYTE removetimer_flag = 0;
int removetimer = 0;

ADDREMOVE_NODE_INFO addremovenode_info;
static void ( *cbFuncAddNode ) (BYTE state, ADDREMOVE_NODE_INFO* p);	// haas added

BYTE av_sequence_num = 0x00;

//int test_speed_flag = 0;
//int test_speed_ctr = 0;
BYTE flag_prevnet_lock = 0;
BYTE prevent_lock_ctr = 0;


static void ( *cbFuncGetNIF ) (WORD nodeID, BYTE type);
static void ( *cbFuncGetIRSensorStatus ) (WORD nodeID, BYTE status);
static void ( *cbFuncGetWUN ) (WORD nodeID);


BYTE initial_flag = 0;

BYTE flag_remove_n_add = 0;
BYTE flag_getNIF = 0;
BYTE flag_getNIF_ID = 0;
BYTE flag_getNIF_TYPE = 0;
BYTE flag_getIRSensorStatus = 0;
BYTE flag_getIRSensorStatus_ID = 0;
BYTE flag_getIRSensorStatus_status = 0;
BYTE flag_getWUN = 0;
BYTE flag_getWUN_ID = 0;



/*======================= API_GetBatteryLevel ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_GetBatteryLevel(WORD node, EACH_NODE_INFO* p)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
		
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;
		get_status_flag = GET_STATUS_ON;
														
		PrepareBatteryGet(&txBuf);			
					
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
						
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_BatteryGetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		//if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_BatteryGetFrame), (TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)			
		{
			printf("Queue full\n");
			
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy	
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			//p->exe_status = zw_senddata_status;
			
			return zw_senddata_status;
		}
		
		//printf("test test\n");
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)
			{	
				get_status_flag = GET_STATUS_OFF;	
						
				//p->exe_status = zw_senddata_status;					
						
				//p->id = device_info.node_info_table[node].id;
      	//p->status = device_info.node_info_table[node].status;
      	p->powerlevel = device_info.node_info_table[node].powerlevel;
      	
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		//p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		//p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;
	}
}


/*================== AP_callback_getWakeUpNotification ====================
 * Function:  for AP testing his callback function  
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
void AP_callback_getWakeUpNotification(WORD node)
{	
	int value, value2;	
	EACH_NODE_INFO ap_node_info;
	
	//printf("Node %d get WUN\n", node);
	
	value2 = API_GetBatteryLevel(node, &ap_node_info);
	if(value2 == STATUS_OK)
	{
		printf("device battery: %d\n", ap_node_info.powerlevel);
	}	
	else
		printf("get error %d\n", value2);
}


/*======================= AP_callback_getIRSensorStatus ========================
 * Function:  for AP testing his callback function  
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
void AP_callback_getIRSensorStatus(WORD node, BYTE status)
{
	if(status == SENSOR_DETECTED || status == SENSOR_DETECTED2)
		printf("Get sensor ID %d, sensor detected\n", node);
	else if(status == SENSOR_NORMAL)
		printf("Get sensor ID %d, sensor normal\n", node);
}


/*======================= AP_callback_getNIF ========================
 * Function:  for AP testing his callback function  
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
void AP_callback_getNIF(WORD node, BYTE type)
{
	int value, value2;	
	EACH_NODE_INFO ap_node_info;
	//printf("Get node %d NIF\n", node);	
	
	if(type == DEVICE_BINARY_SWITCH)
	{	
		value2 = API_GetBinarySwitchStatus(node, &ap_node_info);
		if(value2 == STATUS_OK)
		{
			printf("device status: %d\n", ap_node_info.status);
		}	
		else
			printf("get error %d\n", value2);
	}
	else if(type == DEVICE_DIMMER)
	{
		sleep(2);			
		value2 = API_GetDimmerStatus(node, &ap_node_info);
		if(value2 == STATUS_OK)
		{
			printf("device status: %d\n", ap_node_info.status);
		}	
		else
			printf("get error %d\n", value2);	
	}			
}



/*======================= API_CheckisFailNode ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_CheckisFailNode(WORD node)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;
												
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
			
		init_buffer(&cmd_buf);			
		if(ZW_isFailedNode(&cmd_buf, id) == TRUE )
			zw_senddata_status = STATUS_NODE_FAIL;
		else
			zw_senddata_status = STATUS_NODE_WELL;
		
		return zw_senddata_status;						
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*======================= API_RequestNodeInfo ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_RequestNodeInfo(WORD node)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;
		get_info_flag = GET_STATUS_ON;
												
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
			
		init_buffer(&cmd_buf);			
		if(ZW_RequestNodeInfo(&cmd_buf, id, ApplicationControllerUpdate) == FALSE)
		{
			printf("RequestNodeInfo busy\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{
				get_info_flag = GET_STATUS_OFF;				
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		get_info_flag = GET_STATUS_OFF;
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ		
		zw_senddata_status = STATUS_NOREQUEST;		
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*=================== API_AssociationSetandRemove ====================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_AssociationSetandRemoveandGet(BYTE type, WORD prinode, WORD secnode)
{
	int ctr;
	BYTE pri_id, pri_ch, sec_id, sec_ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;	
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;
							
		pri_id = 0x00FF & prinode;	// get the real node id	
		pri_ch = (0xFF00 & prinode)>>8;
		sec_id = 0x00FF & secnode;	// get the real node id	
		sec_ch = (0xFF00 & secnode)>>8;
		
		if(type == ASSOCIATIONSET)
		{	
			PrepareAssociationSet(&txBuf, 0x01, sec_id);
			
			init_buffer(&cmd_buf);			
			if(ZW_SendData(&cmd_buf, pri_id, (BYTE*)&txBuf, sizeof(txBuf.ZW_AssociationSet1byteV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
			{
				printf("Queue full\n");
				cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
				zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
				return zw_senddata_status;
			}
		}
		else if(type == ASSOCIATIONREMOVE)
		{
			PrepareAssociationRemove(&txBuf, 0x01, sec_id);
				
			init_buffer(&cmd_buf);	
      if(ZW_SendData(&cmd_buf, pri_id, (BYTE*)&txBuf, sizeof(txBuf.ZW_AssociationRemove1byteV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
      {
				printf("Queue full\n");
				cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
				zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
				return zw_senddata_status;
			}
		}
		else if(type == ASSOCIATIONGET)
		{
			PrepareAssociationGet(&txBuf, 0x01);
			
			init_buffer(&cmd_buf);	
      if(ZW_SendData(&cmd_buf, pri_id, (BYTE*)&txBuf, sizeof(txBuf.ZW_AssociationGetV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
      {
				printf("Queue full\n");
				cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
				zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
				return zw_senddata_status;
			}
		}		
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{				
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ		
		zw_senddata_status = STATUS_NOREQUEST;		
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*======================= API_SetAVDevice ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_SetAVDevice(WORD node, int cmd)
{
	int ctr, num;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;
		av_sequence_num++;
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
							
		PrepareMultiInstanceEncap( &txBuf, ch, av_sequence_num, cmd);
		
		num = 4 + sizeof(txBuf.ZW_SimpleAvControlSet1byteFrame);
		printf("size:%d\n", num);
					
		init_buffer(&cmd_buf);			
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, num, (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{				
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ		
		zw_senddata_status = STATUS_NOREQUEST;		
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*======================= API_GetMultiInstanceNum ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_GetMultiInstanceNum(WORD node, BYTE cmdclass, EACH_NODE_INFO* p)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
		
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;
		get_status_flag = GET_STATUS_ON;
				
		PrepareMultiInstanceGet(&txBuf, cmdclass);																				
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_MultiInstanceGetV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			//p->exe_status = zw_senddata_status;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)
			{	
				get_status_flag = GET_STATUS_OFF;	
						
				//p->exe_status = zw_senddata_status;					
						
				p->id = device_info.node_info_table[node].id;
      	p->status = device_info.node_info_table[node].status;
      	
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		//p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		//p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;
	}
}


/*======================= AP_callback ========================
 * Function:  for AP testing his callback function  
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
void AP_callback_removeadd(BYTE state, ADDREMOVE_NODE_INFO* p)
{
	int i,value,value2;
	EACH_NODE_INFO ap_node_info;
	
	if(state == STATUS_ADDNODEDONE || state == STATUS_REMOVENODEDONE)
	{	
		if(p->dev_num != 1)
		{
			for(i=0;i<(p->dev_num);i++)
				printf("state:%d, id:%x, type:%x\n", state, p->id[i], p->type[i]);
		}
		else
		{	
			printf("state:%d, id:%x, type:%x\n", state, p->id[0], p->type[0]);
			
			if(state == STATUS_ADDNODEDONE)
			{	
				if(p->type[0] == DEVICE_BINARY_SWITCH)
				{	
					value2 = API_GetBinarySwitchStatus(p->id[0], &ap_node_info);
					if(value2 == STATUS_OK)
					{
						printf("device status: %d\n", ap_node_info.status);
					}	
					else
						printf("get error %d\n", value2);
				}
				else if(p->type[0] == DEVICE_DIMMER)
				{
					
				}
				else if(p->type[0] == DEVICE_IR_SENSOR)
				{					
					value2 = API_AssociationSetandRemoveandGet(ASSOCIATIONSET, p->id[0], 0x01);				
					if(value2 == STATUS_OK)
					{
						printf("Associate OK\n");
					}	
					else
						printf("get error %d\n", value2);
						
					value2 = API_GetBinarySwitchStatus(p->id[0], &ap_node_info);
					if(value2 == STATUS_OK)
					{
						printf("device status: %d\n", ap_node_info.status);
					}	
					else
						printf("get error %d\n", value2);	
				}				
			}
			else if(state == STATUS_REMOVENODEDONE)
			{

			}			
		}	
	}
	else if(state == STATUS_ADDNODEFAIL || state == STATUS_REMOVENODEFAIL)
	{	
		printf("ap callback, state:%d\n", state);
	}		
}


/*======================= API_StartRemoveNodeFromNetwork ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_StartRemoveNodeFromNetwork_cb(VOID_CALLBACKFUNC(completedFunc)(BYTE state, ADDREMOVE_NODE_INFO* p))
{ 
	int ctr;	
	SCMD cmd_buf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;											
		//*flag = &zw_senddata_status;
		//*type = &device_type;
		cbFuncAddNode = completedFunc;
		removetimer_flag = 1;
		removetimer = 0;
		addremovenode_info.timeout = 0;
		 			
		//init_buffer(&cmd_buf);		
		//ZW_RemoveNodeFromNetwork(&cmd_buf, REMOVE_NODE_STOP, NULL);
		//usleep(1000);
		init_buffer(&cmd_buf);
		ZW_RemoveNodeFromNetwork(&cmd_buf, REMOVE_NODE_ANY, RemoveNodeCompleted);	
				
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{									
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                           	
}


/*================ API_StartAddNodeIntoNetwork_cb ===================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_StartAddNodeIntoNetwork_cb(VOID_CALLBACKFUNC(completedFunc)(BYTE state, ADDREMOVE_NODE_INFO* p))
{ 
	int ctr;	
	SCMD cmd_buf;
	//unsigned int *tmp = flag;
	
	if(cmd_exe_status == STATUS_IDLE)
	{			
		zw_senddata_status = STATUS_NONE;
		//printf("flag %d\n", flag);														
		//*flag = &zw_senddata_status;
		//*type = &device_type;
		cbFuncAddNode = completedFunc;
		addtimer_flag = 1;
		addtimer = 0;
		addremovenode_info.timeout = 0;
		
		//printf("flag2 %d %d %d %d\n", *flag, flag, zw_senddata_status, &zw_senddata_status);
		//usleep(1000);
		//printf("flag3  %d %d %d\n", *tmp, tmp, &tmp);
		 			
		//init_buffer(&cmd_buf);
		//ZW_AddNodeToNetwork(&cmd_buf, ADD_NODE_STOP, NULL);
		//usleep(1000);
		init_buffer(&cmd_buf);
		ZW_AddNodeToNetwork(&cmd_buf, ADD_NODE_ANY, LearnModeCompleted);	// state waiting for add
					
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{									
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                           	
}


/*======================= API_SetandGetBinarySwitch ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_SetAndGetDimmer(WORD node, BYTE value, EACH_NODE_INFO* p)
{
	int ctr;	
	BYTE id, ch;
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;							
		PrepareMultilevelSet( &txBuf, value );
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
					
		init_buffer(&cmd_buf);			
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_SwitchMultilevelSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{				
				break;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		sleep(2);
		
		zw_senddata_status = STATUS_NONE;				
		get_status_flag = GET_STATUS_ON;
														
		PrepareMultilevelGet(&txBuf);					
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_SwitchMultilevelGetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)
			{	
				get_status_flag = GET_STATUS_OFF;	
									
				p->id = device_info.node_info_table[node].id;
      	p->status = device_info.node_info_table[node].status;
      	
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*======================= API_SetandGetBinarySwitch ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_SetAndGetBinarySwitch(WORD node, BYTE value, EACH_NODE_INFO* p)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;
							
		if(value == 0)
			PrepareBasicSet(&txBuf, BASIC_OFF);
		else if(value == 1)
			PrepareBasicSet(&txBuf, BASIC_ON);
		else
		{
			printf("error input\n");	
		}
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
					
		init_buffer(&cmd_buf);			
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_BasicSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{				
				break;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		usleep(200000);
		//sleep(1);
		
		zw_senddata_status = STATUS_NONE;				
		get_status_flag = GET_STATUS_ON;
														
		PrepareBasicGet(&txBuf);				
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_BasicGetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)
			{	
				get_status_flag = GET_STATUS_OFF;	
				
				//printf("get %d %d %d %d\n", zw_senddata_status, node, device_info.node_info_table[node].id, device_info.node_info_table[node].status);					
				p->id = device_info.node_info_table[node].id;
      	p->status = device_info.node_info_table[node].status;
      	
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*======================= GetStatusSelectDevicetype ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE GetStatusAnalysis(BYTE node, BYTE type, EACH_NODE_INFO* p, BYTE status)
{
	BYTE cmd_size;
	
	switch(type) 
	{
		case DEVICE_BINARY_SWITCH:
			p->exe_status = status;
			p->id = device_info.node_info_table[node].id;
      p->status = device_info.node_info_table[node].status;		
		break;
		
		case DEVICE_DIMMER:
			p->exe_status = status;
			p->id = device_info.node_info_table[node].id;
      p->status = device_info.node_info_table[node].status;	
		break;
		
		case DEVICE_CURTAIN_CONTROLLER:
			p->exe_status = status;
			p->id = device_info.node_info_table[node].id;
      p->status = device_info.node_info_table[node].status;	
		break;
		
		case	DEVICE_POWER_METER: 
			
		break;
		
		default:
			printf("not the type\n");
			return -1;
		break;
	}	
	
	return cmd_size;
}


/*======================= GetStatusSelectDevicetype ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE GetStatusSelectDevicetype(BYTE type, ZW_APPLICATION_TX_BUFFER *pBuf)
{
	BYTE cmd_size;
	
	switch(type) 
	{
		case DEVICE_BINARY_SWITCH:
			pBuf->ZW_BasicGetFrame.cmdClass = COMMAND_CLASS_BASIC;
  		pBuf->ZW_BasicGetFrame.cmd = BASIC_GET;
  		cmd_size = sizeof(pBuf->ZW_BasicSetFrame);		
		break;
		
		case DEVICE_DIMMER:
			pBuf->ZW_SwitchMultilevelGetFrame.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL;
  		pBuf->ZW_SwitchMultilevelGetFrame.cmd = SWITCH_MULTILEVEL_GET;
  		cmd_size = sizeof(pBuf->ZW_SwitchMultilevelGetFrame);
		break;
		
		case DEVICE_CURTAIN_CONTROLLER:
			pBuf->ZW_SwitchMultilevelGetFrame.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL;
  		pBuf->ZW_SwitchMultilevelGetFrame.cmd = SWITCH_MULTILEVEL_GET;
  		cmd_size = sizeof(pBuf->ZW_SwitchMultilevelGetFrame);
		break;
		
		case	DEVICE_POWER_METER: 
			//pBuf->ZW_MeterGetV3Frame.cmdClass = COMMAND_CLASS_METER;
  		//pBuf->ZW_MeterGetV3Frame.cmd = METER_GET_V3;
  		//pBuf->ZW_MeterGetV3Frame.properties1 = value;
  		//cmd_size = sizeof(pBuf->ZW_MeterGetV3Frame);
		break;
		
		default:
			printf("not the type\n");
			return -1;
		break;
	}	
	
	return cmd_size;
}


/*======================= GetBinarySwitchStatus ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_GetDeviceStatus(BYTE node, BYTE type, EACH_NODE_INFO* p)
{
	int ctr;
	BYTE cmdsize;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
		
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;
		get_status_flag = GET_STATUS_ON;
		
		cmdsize = GetStatusSelectDevicetype(type, &txBuf);
		if(cmdsize == -1)
			return STATUS_INPUT_ERROR;
																							
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, node, (BYTE*)&txBuf, cmdsize, (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			
			p->exe_status = zw_senddata_status;
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 100)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)
			{	
				get_status_flag = GET_STATUS_OFF;			
				
				GetStatusAnalysis(node, p, type, zw_senddata_status);
      	//printf("check %d, %d\n", p->id, p->status);
      	
				return zw_senddata_status;
			}
								
			usleep(20000);
			ctr++;			
		}
		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;
	}
}


/*======================= API_StopRemoveNodeFromNetwork ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_StopRemoveNodeFromNetwork(unsigned int *flag, unsigned int* type)
{
	int ctr;	
	SCMD cmd_buf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;											
		*flag = NULL;
		*type = NULL;
		 			
		init_buffer(&cmd_buf);
		ZW_RemoveNodeFromNetwork(&cmd_buf, REMOVE_NODE_STOP, NULL);
		cmd_exe_status = STATUS_IDLE;
		
		if(zw_senddata_status != STATUS_NORESPONSE)	
			zw_senddata_status = STATUS_REMOVENODESTOP;
		
		printf("state %d\n", cmd_exe_status);
		return zw_senddata_status;
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                 
}


/*======================= API_StartRemoveNodeFromNetwork ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_StartRemoveNodeFromNetwork(unsigned int *flag, unsigned int* type)
{ 
	int ctr;	
	SCMD cmd_buf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;											
		*flag = &zw_senddata_status;
		*type = &device_type;
		 			
		init_buffer(&cmd_buf);		
		ZW_RemoveNodeFromNetwork(&cmd_buf, REMOVE_NODE_STOP, NULL);
		usleep(1000);
		init_buffer(&cmd_buf);
		ZW_RemoveNodeFromNetwork(&cmd_buf, REMOVE_NODE_ANY, RemoveNodeCompleted);	
				
		ctr = 0;
		while(ctr < 100)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{									
				return zw_senddata_status;
			}
								
			usleep(20000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                           	
}


/*======================= API_StopAddNodeIntoNetwork ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_StopAddNodeIntoNetwork(unsigned int *flag, unsigned int* type)
{
	int ctr;	
	SCMD cmd_buf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{			
		zw_senddata_status = STATUS_NONE;											
		*flag = NULL;
		*type = NULL;
		 			
		init_buffer(&cmd_buf);
		ZW_AddNodeToNetwork(&cmd_buf, ADD_NODE_STOP, NULL);	
		cmd_exe_status = STATUS_IDLE;
		
		if(zw_senddata_status != STATUS_NORESPONSE)	
			zw_senddata_status = STATUS_ADDNODESTOP;
		
		printf("state %d\n", cmd_exe_status);
		return zw_senddata_status;
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                 
}


/*======================= API_AddNodeIntoNetwork ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_StartAddNodeIntoNetwork(unsigned int* flag, unsigned int* type)
{ 
	int ctr;	
	SCMD cmd_buf;
	//unsigned int *tmp = flag;
	
	if(cmd_exe_status == STATUS_IDLE)
	{			
		zw_senddata_status = STATUS_NONE;
		//printf("flag %d\n", flag);														
		*flag = &zw_senddata_status;
		*type = &device_type;
		
		//printf("flag2 %d %d %d %d\n", *flag, flag, zw_senddata_status, &zw_senddata_status);
		//usleep(1000);
		//printf("flag3  %d %d %d\n", *tmp, tmp, &tmp);
		 			
		init_buffer(&cmd_buf);
		ZW_AddNodeToNetwork(&cmd_buf, ADD_NODE_STOP, NULL);
		usleep(1000);
		init_buffer(&cmd_buf);
		ZW_AddNodeToNetwork(&cmd_buf, ADD_NODE_ANY, LearnModeCompleted);	// state waiting for add
					
		ctr = 0;
		while(ctr < 100)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{									
				return zw_senddata_status;
			}
								
			usleep(20000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                           	
}


/*======================= API_ResetToDefault ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_ResetToDefault(void)
{ 
	int ctr;	
	SCMD cmd_buf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;											
		 			
		init_buffer(&cmd_buf);					
		ZW_SetDefault(&cmd_buf, ResetDefaultCompleted);
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{									
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                           	
}


/*======================= API_GetMeter ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_GetMeter(WORD node, EACH_NODE_INFO* p)
{ 
	int ctr;	
	BYTE id, ch;
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;											
		get_status_flag = GET_STATUS_ON;	

							
		//if(value == GET_POWER)
  	//	value = 0x10;
  	//else if(value == GET_VOLTAGE)
  	//	value = 0x20;
  	//else if(value == GET_CURRENT)
  	//	value = 0x28;
  	//else
  	//	printf("error input\n");
  	
  	//============================== get power
  	PrepareMeterGet(&txBuf, 0x10);
  	
  	id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;		
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_MeterGetV3Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{	
				get_status_flag = GET_STATUS_OFF;
				
				//p->voltage = device_info.node_info_table[node].voltage;
      	//p->current = device_info.node_info_table[node].current;
      	p->power = device_info.node_info_table[node].power;
      	
      	printf("ctr %d\n", ctr);
				//return zw_senddata_status;
				break;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		usleep(1000);
		//printf("AAA\n");
		//============================== get voltage
		zw_senddata_status = STATUS_NONE;
		get_status_flag = GET_STATUS_ON;	
		PrepareMeterGet(&txBuf, 0x20);
  			
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_MeterGetV3Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{	
				get_status_flag = GET_STATUS_OFF;
				
				p->voltage = device_info.node_info_table[node].voltage;
      	//p->current = device_info.node_info_table[node].current;
      	//p->power = device_info.node_info_table[node].power;
      	
				//return zw_senddata_status;
				printf("ctr %d\n", ctr);
				break;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		usleep(1000);
		//printf("BBB\n");
		//============================== get current
		zw_senddata_status = STATUS_NONE;
		get_status_flag = GET_STATUS_ON;
		PrepareMeterGet(&txBuf, 0x28);
  			
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_MeterGetV3Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{	
				get_status_flag = GET_STATUS_OFF;
				
				//p->voltage = device_info.node_info_table[node].voltage;
      	p->current = device_info.node_info_table[node].current;
      	//p->power = device_info.node_info_table[node].power;
      	
      	printf("ctr %d\n", ctr);
				return zw_senddata_status;
				//break;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		//printf("CCC\n");		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}		                     		                           	
}
        	
        	
/*======================= API_SetCurtain ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_SetCurtain(WORD node, BYTE value)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;						
		
		if(value == CURTAIN_OFF)
			//PrepareMultilevelSet( &txBuf, value );
			PrepareMultilevelStartLevelChange ( &txBuf, value, 0x00);
		else if(value == CURTAIN_ON)
			//PrepareMultilevelSet( &txBuf, value );				
			PrepareMultilevelStartLevelChange ( &txBuf, value, 0x00);
		else if(value == CURTAIN_STOP)	
			PrepareMultilevelStop(&txBuf);
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
					
		init_buffer(&cmd_buf);			
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_SwitchMultilevelSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ			
		zw_senddata_status = STATUS_NOREQUEST;		
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*======================= API_GetDimmerStatus ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_GetDimmerStatus(WORD node, EACH_NODE_INFO* p)
{
	int ctr;	
	BYTE id, ch;
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;
											
		get_status_flag = GET_STATUS_ON;				
		PrepareMultilevelGet(&txBuf);
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
					
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_SwitchMultilevelGetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)			
			{	
				get_status_flag = GET_STATUS_OFF;			
				p->id = device_info.node_info_table[node].id;
      	p->status = device_info.node_info_table[node].status;
      	
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		
		return zw_senddata_status;
	}
}


/*======================= API_SetDimmer_and_GetStatus ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_SetDimmer(WORD node, BYTE value)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;						
		PrepareMultilevelSet( &txBuf, value );				
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
					
		init_buffer(&cmd_buf);			
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_SwitchMultilevelSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ			
		zw_senddata_status = STATUS_NOREQUEST;		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}


/*======================= API_GetBinarySwitchStatus ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_GetBinarySwitchStatus(WORD node, EACH_NODE_INFO* p)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
		
	if(cmd_exe_status == STATUS_IDLE)
	{	
		zw_senddata_status = STATUS_NONE;
		get_status_flag = GET_STATUS_ON;
														
		PrepareBasicGet(&txBuf);
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
						
		init_buffer(&cmd_buf);					
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_BasicGetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
		{
			printf("Queue full\n");
			
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy	
			zw_senddata_status = STATUS_GET_DONGLEBUSY;			
			//p->exe_status = zw_senddata_status;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)
			{	
				get_status_flag = GET_STATUS_OFF;	
						
				//p->exe_status = zw_senddata_status;					
						
				p->id = device_info.node_info_table[node].id;
      	p->status = device_info.node_info_table[node].status;
      	
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}
		
		get_status_flag = GET_STATUS_OFF;
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ	
		zw_senddata_status = STATUS_NOREQUEST;
		//p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		zw_senddata_status = STATUS_APIPROCESSING;	
		//p->exe_status = zw_senddata_status;
		
		return zw_senddata_status;
	}
}


/*======================= API_SetBinarySwitch ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      
 *
 * External:
 -------------------------------------------------------------------*/
BYTE API_SetBinarySwitch(WORD node, BYTE value)
{
	int ctr;
	BYTE id, ch;	
	SCMD cmd_buf;
	ZW_APPLICATION_TX_BUFFER txBuf;
	
	if(cmd_exe_status == STATUS_IDLE)
	{
		zw_senddata_status = STATUS_NONE;
						
		if(value == 0)
			PrepareBasicSet(&txBuf, BASIC_OFF);
		else if(value == 1)
			PrepareBasicSet(&txBuf, BASIC_ON);
		else
		{
			printf("error input\n");	
		}
		
		id = 0x00FF & node;	// get the real node id	
		ch = (0xFF00 & node)>>8;
			
		init_buffer(&cmd_buf);			
		if(ZW_SendData(&cmd_buf, id, (BYTE*)&txBuf, sizeof(txBuf.ZW_BasicSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
		{
			printf("Queue full\n");
			cmd_exe_status = STATUS_IDLE;	// set status back to idle, if dongle is busy
			zw_senddata_status = STATUS_SET_DONGLEBUSY;
			
			return zw_senddata_status;
		}
		
		ctr = 0;
		while(ctr < 1000)
		{
			if(cmd_exe_status == STATUS_IDLE && zw_senddata_status != STATUS_NONE)						
			{				
				return zw_senddata_status;
			}
								
			usleep(1000);
			ctr++;			
		}		
		
		cmd_exe_status = STATUS_IDLE;	// set status back to idle, if we don't get any REQ		
		zw_senddata_status = STATUS_NOREQUEST;		
		
		return zw_senddata_status;								
	}
	else
	{	
		printf("API processing now\n");
		
		zw_senddata_status = STATUS_APIPROCESSING;			
		
		return zw_senddata_status;
	}
}

    					
/*======================= Analysis_DeviceType ========================
 * Function:    
 *
 * Parameters:  
 *              
 *
 * Return:      void
 *
 * External:
 -------------------------------------------------------------------*/
//BYTE Analysis_DeviceType(NODEINFO *nodeInfo, BYTE *cmdclass)
BYTE Analysis_DeviceType(NODEINFO *nodeInfo)
{
	BYTE reval=0;
	
	switch(nodeInfo->nodeType.basic)
	{
		case BASIC_TYPE_CONTROLLER:	// 1
			if(nodeInfo->nodeType.generic == GENERIC_TYPE_GENERIC_CONTROLLER)// 1
			{
				if((nodeInfo->nodeType.specific == SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER) || (nodeInfo->nodeType.specific == SPECIFIC_TYPE_PORTABLE_SCENE_CONTROLLER))	// 1
					reval = DEVICE_PORTABLE_CONTROLLER;
				else
					reval = DEVICE_UNKNOW;
			}
			else 
				reval = DEVICE_UNKNOW;
		break;
		
		case BASIC_TYPE_STATIC_CONTROLLER:	// 2
			if(nodeInfo->nodeType.generic == GENERIC_TYPE_STATIC_CONTROLLER)	// 2
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_PC_CONTROLLER)	// 1
					reval = DEVICE_STATIC_CONTROLLER;
				else
					reval = DEVICE_UNKNOW;
			}
			else 
				reval = DEVICE_UNKNOW;
		break;
		
		case BASIC_TYPE_SLAVE:	// 3
			if(nodeInfo->nodeType.generic == GENERIC_TYPE_SWITCH_BINARY)	// 10
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_POWER_SWITCH_BINARY || nodeInfo->nodeType.specific == SPECIFIC_TYPE_SCENE_SWITCH_BINARY)	// 1 or 3
					reval = DEVICE_BINARY_SWITCH;
				else
					reval = DEVICE_UNKNOW;
			}
			else if(nodeInfo->nodeType.generic == GENERIC_TYPE_SWITCH_MULTILEVEL)	// 11
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL)	// 1
					reval = DEVICE_DIMMER;
				else
					reval = DEVICE_UNKNOW;				
			}	
			else 
				reval = DEVICE_UNKNOW;
		break;
		
		case BASIC_TYPE_ROUTING_SLAVE:	// 4
			if(nodeInfo->nodeType.generic == GENERIC_TYPE_SWITCH_BINARY)	// 10
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_POWER_SWITCH_BINARY || nodeInfo->nodeType.specific == SPECIFIC_TYPE_SCENE_SWITCH_BINARY)	// 1 or 3
					reval = DEVICE_BINARY_SWITCH;
				else
					reval = DEVICE_UNKNOW;
			}
			else if(nodeInfo->nodeType.generic == GENERIC_TYPE_SWITCH_MULTILEVEL)	// 11
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL)	// 1
					reval = DEVICE_DIMMER;
				else if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_MOTOR_MULTIPOSITION)	// 3
					reval = DEVICE_CURTAIN_CONTROLLER;
				else
					reval = DEVICE_UNKNOW;				
			}	
			else if(nodeInfo->nodeType.generic == GENERIC_TYPE_SENSOR_BINARY)	// 20
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_ROUTING_SENSOR_BINARY)	// 1
					reval = DEVICE_IR_SENSOR;				
				else
					reval = DEVICE_UNKNOW;
				
			}
			else if(nodeInfo->nodeType.generic == GENERIC_TYPE_SENSOR_MULTILEVEL)	// 21
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL)	// 1
					reval = DEVICE_POWER_METER;				
				else
					reval = DEVICE_UNKNOW;				
			}
			else if(nodeInfo->nodeType.generic == GENERIC_TYPE_AV_CONTROL_POINT)	// 3	
			{
				if(nodeInfo->nodeType.specific == SPECIFIC_TYPE_NOT_USED)	// 0
					reval = DEVICE_IR_CONTROLLER;				
				else
					reval = DEVICE_UNKNOW;				
			}
			else 
				reval = DEVICE_UNKNOW;
		break;
	} 
  
	return reval;
}


/*======================= ZW_NodeMaskSetBit ========================
 * Function:    Set the node bit in a node bitmask
 *
 * Parameters:  pMask   Nodemask
 *              bNodeID Node ID that should be set in the mask
 *
 * Return:      void
 *
 * External:
 -------------------------------------------------------------------*/
void NodeMaskSetBit(BYTE *pMask, BYTE bNodeID)
{
  bNodeID--;
  *(pMask+(bNodeID>>3)) |= (0x1 << (bNodeID & 0x7));
}


/*=======================   ZW_SendDataMulti   ========================
  * \ingroup ZWCMD
  * Transmit data buffer to a list of Z-Wave Nodes (multicast frame).
  * \param[in] pNodeIDList    List of destination node ID's
  * \param[in] numberNodes    Number of Nodes
  * \param[in] pData          Data buffer pointer
  * \param[in] dataLength     Data buffer length
  * \param[in] txOptions      Transmit option flags
  * \param[in] completedFunc  Transmit completed call back function
  * \return FALSE if transmitter queue overflow
  -------------------------------------------------------------------*/
BYTE ZW_SendDataMulti(HCMD p, BYTE *pNodeIDList, BYTE numberNodes, BYTE *pData, BYTE dataLength, BYTE txOptions, VOID_CALLBACKFUNC ( completedFunc ) ( auto BYTE txStatus ) )
{
  int i;
  cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->byCompletedFunc = ( completedFunc == NULL ? 0 : completedFunc );
	p->idx = 0;

  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_SEND_DATA_MULTI;
  numberNodes = ( numberNodes <= ZW_MAX_NODES ) ? numberNodes : ZW_MAX_NODES;
  p->buffer[ p->idx++ ] = numberNodes;

  for ( i = 0; i < MAX_NODEMASK_LENGTH; i++ )
  {
      p->buffer[ p->idx++ ] = pNodeIDList[ i ];
  }

  p->buffer[ p->idx++ ] = dataLength;

  for ( i = 0; i < dataLength; i++ )
  {
      p->buffer[ p->idx++ ] = pData[ i ];
  }

  p->buffer[ p->idx++ ] = txOptions;
  p->buffer[ p->idx++ ] = p->byCompletedFunc;	// Func id for CompletedFunc
  p->buffer[ 0 ] = p->idx;	// length
  p->byLen = 0;
  
  cbFuncZWSendDataMulti = completedFunc;
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_ZW_SEND_DATA_MULTI, &(p->buffer), &(p->byLen) );
		
  return p->buffer[ IDX_DATA ];
}


/*=====================   ApplicationControllerUpdate   =====================
**    Inform the static controller of node information update done through
**   the network managment.
**  void                                // RET  Nothing                
**  ApplicationControllerUpdate(
**  BYTE bStatus,                     // IN   Status of learn mode        
**  BYTE bNodeID,                     // IN   Node id of node sending nodeinfo
**  BYTE *pCmd,                       // IN   Pointer to appl. node info      
**  BYTE bLen)                        // IN   Node info length                
**--------------------------------------------------------------------------*/
void ApplicationControllerUpdate(DONGLE_INFO *p, BYTE bStatus, BYTE bNodeID, BYTE *pCmd, BYTE bLen, BYTE type)                     
{	
	BYTE i;
	
	//printf("ApplicationControllerUpdate\n");

	
	switch(bStatus)
	{
		case UPDATE_STATE_NODE_INFO_RECEIVED:
  		for (i = 3; i < bLen; i++)
  		{
  			*(pCmd + i) = *(pCmd + i)-1;
    		p->node_info_table[bNodeID].cmdclass_bitmask[(*(pCmd + i) >> 3)] |= (0x01 << (*(pCmd + i) & 0x7));
  		}
  		
  		if(get_info_flag == GET_STATUS_ON)
  		{	
  			zw_senddata_status = STATUS_OK;  
  			cmd_exe_status = STATUS_IDLE;
  			printf("update ok, status %d\n", cmd_exe_status);
  		}
  		else
  		{
  			flag_getNIF = 1;
  			flag_getNIF_ID = bNodeID;
  			flag_getNIF_TYPE = type;
  			//if(cbFuncGetNIF != NULL)
				//   cbFuncGetNIF(bNodeID);
  		}	
  	break;
  	
  	case UPDATE_STATE_NODE_INFO_REQ_FAILED:
  		if(get_info_flag == GET_STATUS_ON)
  		{
  			zw_senddata_status = STATUS_GETINFO_FAIL;	
  			cmd_exe_status = STATUS_IDLE;
  			//printf("update fail, status %d\n", cmd_exe_status);	// why this would affect the REQ ??
  		}	  		
  	break;
  	
  	case UPDATE_STATE_ADD_DONE:
  	case UPDATE_STATE_DELETE_DONE: 
  		if(get_info_flag == GET_STATUS_ON)
  		{ 
  			zw_senddata_status = STATUS_OK;
  			cmd_exe_status = STATUS_IDLE;
  			printf("A node has add/remove done, status %d\n", cmd_exe_status);
  		}	  		  		
  	break; 	  	
	}
}


/*==================   ApplicationCommandHandler   ==================
**    Handling of a received application commands and requests
**
**    This is an application function example
**  void                              //RET Nothing  
**  ApplicationCommandHandler(
**  BYTE  rxStatus,                 // IN Frame header info 
**  BYTE  sourceNode,               // IN Command sender Node ID 
**  ZW_APPLICATION_TX_BUFFER *pCmd,///////BYTE *pCmd, // IN Payload from the received frame, the union
**                                  //    should be used to access the fields
**  BYTE   cmdLength)               // IN Number of command bytes including the command 
**------------------------------------------------------------------*/
void ApplicationCommandHandler(DONGLE_INFO *p, BYTE rxStatus, BYTE sourceNode, ZW_APPLICATION_TX_BUFFER *pCmd, BYTE  cmdLength)
{ 
	BYTE precision = 0, scale = 0, size = 0, i;		   
  BYTE classcmd = pCmd->ZW_Common.cmdClass;
  BYTE cmd = pCmd->ZW_Common.cmd;
  
  //printf("Application Command Handler\n");
   
  switch(classcmd)
  {
    case COMMAND_CLASS_SWITCH_MULTILEVEL:
      if (cmd == SWITCH_MULTILEVEL_REPORT)
      {
      	if(cmdLength == 3)
      	{	
      		p->node_info_table[sourceNode].id = sourceNode;
      		p->node_info_table[sourceNode].status = *((BYTE*)pCmd + OFFSET_PARAM_1);
      		//printf("multilevel report:%d\n", p->node_info_table[sourceNode].status);
      	}
      	else
      		printf("get error\n");      	
/*        
				groupLevel = pCmdTMP->ZW_SwitchMultilevelReportFrame.value;
        StoreGroupLevel(groupLevel);
        StopDimEndTimer();
        if (!receivedReport)
        {
          SetGroupDimLevel();
        }
        receivedReport = TRUE;
*/        
      }
    break;
    
    case COMMAND_CLASS_SWITCH_BINARY:
    	if (cmd == SWITCH_BINARY_REPORT)
    	{   		
    		if(cmdLength == 3)
    		{
    			if(*((BYTE*)pCmd + OFFSET_PARAM_1) == 0x00)
    			{
    				p->node_info_table[sourceNode].id = sourceNode;
    				p->node_info_table[sourceNode].status = STATUS_BINARY_SWITCH_OFF;    				
    			}
    			else if(*((BYTE*)pCmd + OFFSET_PARAM_1) == 0xFF)
    			{
    				p->node_info_table[sourceNode].id = sourceNode;	
    				p->node_info_table[sourceNode].status = STATUS_BINARY_SWITCH_ON;    				
    			}
    			else
    				printf("status error\n");
    			    					
      		//printf("switch binary report:%d\n", p->node_info_table[sourceNode].status);
      	}
      	else
      		printf("get error\n");    		 
    	}		
    break;

    case COMMAND_CLASS_BASIC:
    	if (cmd == BASIC_REPORT)
    	{
    		//printf("test0\n");
    		if(cmdLength == 3)
    		{
    			if(*((BYTE*)pCmd + OFFSET_PARAM_1) == 0x00)
    			{
    				p->node_info_table[sourceNode].id = sourceNode;	
    				p->node_info_table[sourceNode].status = STATUS_BINARY_SWITCH_OFF;
    				//printf("test1,%d %d\n",p->node_info_table[sourceNode].id, p->node_info_table[sourceNode].status);
    			}
    			else if(*((BYTE*)pCmd + OFFSET_PARAM_1) == 0xFF)
    			{
    				p->node_info_table[sourceNode].id = sourceNode;	
    				p->node_info_table[sourceNode].status = STATUS_BINARY_SWITCH_ON;
    				//printf("test2,%d %d\n",p->node_info_table[sourceNode].id, p->node_info_table[sourceNode].status);	
    			}
    			else
    				printf("status error\n");	
    			
    			//printf("speed %d\n",test_speed_ctr);
      		//printf("basic report:%d\n", p->node_info_table[sourceNode].status);
      	}
      	else
      		printf("get error\n");    		
    	}
    	else if(cmd == BASIC_SET)
    	{
    		if(cmdLength == 3)
    		{
    			//if(*((BYTE*)pCmd + OFFSET_PARAM_1) == 0x00)	
    			//	printf("node %d alarm off\n", sourceNode);
    			//else if(*((BYTE*)pCmd + OFFSET_PARAM_1) == 0xFF)
    			//	printf("node %d alarm on\n", sourceNode);
    			
    			flag_getIRSensorStatus = 1;
    			flag_getIRSensorStatus_ID = sourceNode;	
    			flag_getIRSensorStatus_status = *((BYTE*)pCmd + OFFSET_PARAM_1);
    			
    			//if(cbFuncGetIRSensorStatus != NULL)
    			//	cbFuncGetIRSensorStatus(sourceNode, *((BYTE*)pCmd + OFFSET_PARAM_1));
    		}
    		else
    			printf("get error\n");
    	}
    break;
    
    case COMMAND_CLASS_METER:
    	if (cmd == METER_REPORT_V3)
    	{     		    		   	
    		precision = (*((BYTE*)pCmd + OFFSET_PARAM_2) & 0xE0)>>5;
				scale = ((*((BYTE*)pCmd + OFFSET_PARAM_2) & 0x18)>>3)|((*((BYTE*)pCmd + OFFSET_PARAM_1) & 0x80)>>5);
				size = (*((BYTE*)pCmd + OFFSET_PARAM_2) & 0x07);
				
				p->node_info_table[sourceNode].precision = precision;
				
    		printf("precision:%d, scale:%d, size:%d\n", p->node_info_table[sourceNode].precision, scale, size);				
	
				if(scale == 0x04)	//Voltage
				{
					p->node_info_table[sourceNode].id = sourceNode;
					
					p->node_info_table[sourceNode].voltage = 0;
					for(i=0; i<size; i++)
						 p->node_info_table[sourceNode].voltage += *((BYTE*)pCmd + OFFSET_PARAM_3 + i)<<((size-1-i)<<3);					
					
					//printf("now voltage is:%d, ", p->node_info_table[sourceNode].voltage);
					//p->node_info_table[sourceNode].sec = (*((BYTE*)pCmd + OFFSET_PARAM_3 + size) << 8) + *((BYTE*)pCmd + OFFSET_PARAM_3 + size + 1);
										
					//p->node_info_table[sourceNode].voltage = 0;
					//for(i=0; i<size; i++)
					//	p->node_info_table[sourceNode].voltage += *((BYTE*)pCmd + OFFSET_PARAM_3 + size + 2 + i) << ((size-1-i)<<3);
					
					//printf("%d sec ago, voltage is: %d\n", p->node_info_table[sourceNode].sec, p->node_info_table[sourceNode].voltage);						
  			}
  			else if(scale == 0x05)	//current
  			{
  				p->node_info_table[sourceNode].id = sourceNode;
  					
  				p->node_info_table[sourceNode].current = 0;
  				for(i=0; i<size; i++)
						 p->node_info_table[sourceNode].current += *((BYTE*)pCmd + OFFSET_PARAM_3 + i)<<((size-1-i)<<3);					
					
					//printf("now current is:%d, ", p->node_info_table[sourceNode].current);
					
					//p->node_info_table[sourceNode].sec = (*((BYTE*)pCmd + OFFSET_PARAM_3 + size) << 8) + *((BYTE*)pCmd + OFFSET_PARAM_3 + size + 1);
										
					//p->node_info_table[sourceNode].current = 0;
					//for(i=0; i<size; i++)
					//	p->node_info_table[sourceNode].current += *((BYTE*)pCmd + OFFSET_PARAM_3 + size + 2 + i) << ((size-1-i)<<3);
					
					//printf("%d sec ago, current is: %d\n", p->node_info_table[sourceNode].sec, p->node_info_table[sourceNode].current);											  					
  			}
  			else if(scale == 0x02)	//power
  			{
  				p->node_info_table[sourceNode].id = sourceNode;
  				p->node_info_table[sourceNode].power = 0;	
  				for(i=0; i<size; i++)
						 p->node_info_table[sourceNode].power += *((BYTE*)pCmd + OFFSET_PARAM_3 + i)<<((size-1-i)<<3);					
					
					//printf("now power is:%d, ", p->node_info_table[sourceNode].power);
					
					//p->node_info_table[sourceNode].sec = (*((BYTE*)pCmd + OFFSET_PARAM_3 + size) << 8) + *((BYTE*)pCmd + OFFSET_PARAM_3 + size + 1);
										
					//p->node_info_table[sourceNode].power = 0;
					//for(i=0; i<size; i++)
					//	p->node_info_table[sourceNode].power += *((BYTE*)pCmd + OFFSET_PARAM_3 + size + 2 + i) << ((size-1-i)<<3);
					
					//printf("%d sec ago, power is: %d\n", p->node_info_table[sourceNode].sec, p->node_info_table[sourceNode].power);										
  			}
  			else
  				printf("unknown type\n");
  						
    	}
    	else if (cmd == METER_SUPPORTED_REPORT_V3)
    	{
    		p->node_info_table[sourceNode].id = sourceNode; 
    	}
    break;
    
    case COMMAND_CLASS_MULTI_CHANNEL_V2:
    	if (cmd == MULTI_INSTANCE_CMD_ENCAP_V2)
    	{
    		if(*((BYTE*)pCmd + OFFSET_PARAM_2) == COMMAND_CLASS_SENSOR_MULTILEVEL_V3)
    		{
    			if(*((BYTE*)pCmd + OFFSET_PARAM_3) == SENSOR_MULTILEVEL_REPORT_V3)
    			{
    				precision = (*((BYTE*)pCmd + OFFSET_PARAM_5) & 0xE0)>>5;
						scale = (*((BYTE*)pCmd + OFFSET_PARAM_5) & 0x18)>>3;
						size = (*((BYTE*)pCmd + OFFSET_PARAM_5) & 0x07);
	
    				printf("precision:%d, scale:%d, size:%d\n", precision, scale, size);
	
						if(*((BYTE*)pCmd + OFFSET_PARAM_4) == SENSOR_TYPE_POWER)
						{
							p->node_info_table[sourceNode].id = sourceNode;
							
							p->node_info_table[sourceNode].power = 0;	
  						for(i=0; i<size; i++)
						 		p->node_info_table[sourceNode].power += *((BYTE*)pCmd + OFFSET_PARAM_6 + i)<<((size-1-i)<<3);					
					
							printf("now power is:%d, ", p->node_info_table[sourceNode].power);
					
							p->node_info_table[sourceNode].sec = (*((BYTE*)pCmd + OFFSET_PARAM_6 + size) << 8) + *((BYTE*)pCmd + OFFSET_PARAM_6 + size + 1);
										
							p->node_info_table[sourceNode].power = 0;
							for(i=0; i<size; i++)
								p->node_info_table[sourceNode].power += *((BYTE*)pCmd + OFFSET_PARAM_6 + size + 2 + i) << ((size-1-i)<<3);
					
							printf("%d sec ago, power is: %d\n", p->node_info_table[sourceNode].sec, p->node_info_table[sourceNode].power);										
  					}
  					else if(*((BYTE*)pCmd + OFFSET_PARAM_4) == SENSOR_TYPE_VOLTAGE)
  					{
  						p->node_info_table[sourceNode].id = sourceNode;
  							
  						p->node_info_table[sourceNode].voltage = 0;
							for(i=0; i<size; i++)
						 		p->node_info_table[sourceNode].voltage += *((BYTE*)pCmd + OFFSET_PARAM_6 + i)<<((size-1-i)<<3);					
					
							printf("now voltage is:%d, ", p->node_info_table[sourceNode].voltage);
							p->node_info_table[sourceNode].sec = (*((BYTE*)pCmd + OFFSET_PARAM_6 + size) << 8) + *((BYTE*)pCmd + OFFSET_PARAM_6 + size + 1);
										
							p->node_info_table[sourceNode].voltage = 0;
							for(i=0; i<size; i++)
								p->node_info_table[sourceNode].voltage += *((BYTE*)pCmd + OFFSET_PARAM_6 + size + 2 + i) << ((size-1-i)<<3);
					
							printf("%d sec ago, voltage is: %d\n", p->node_info_table[sourceNode].sec, p->node_info_table[sourceNode].voltage);  					
  					}
  					else if(*((BYTE*)pCmd + OFFSET_PARAM_4) == SENSOR_TYPE_CURRENT)
  					{
  						p->node_info_table[sourceNode].id = sourceNode;	
  						
  						p->node_info_table[sourceNode].current = 0;
  						for(i=0; i<size; i++)
						 		p->node_info_table[sourceNode].current += *((BYTE*)pCmd + OFFSET_PARAM_6 + i)<<((size-1-i)<<3);					
					
							printf("now current is:%d, ", p->node_info_table[sourceNode].current);
					
							p->node_info_table[sourceNode].sec = (*((BYTE*)pCmd + OFFSET_PARAM_6 + size) << 8) + *((BYTE*)pCmd + OFFSET_PARAM_6 + size + 1);
										
							p->node_info_table[sourceNode].current = 0;
							for(i=0; i<size; i++)
								p->node_info_table[sourceNode].current += *((BYTE*)pCmd + OFFSET_PARAM_6 + size + 2 + i) << ((size-1-i)<<3);
					
							printf("%d sec ago, current is: %d\n", p->node_info_table[sourceNode].sec, p->node_info_table[sourceNode].current);											  					
  					}
  					else
  						printf("unknown type\n");	
    			}	
    		}	
    	}
    	else if(cmd == MULTI_INSTANCE_REPORT_V2)	// report how many instance this command have
    	{
    		p->node_info_table[sourceNode].id = sourceNode; 
    		p->node_info_table[sourceNode].cmdclass[*((BYTE*)pCmd + OFFSET_PARAM_1)].instance = *((BYTE*)pCmd + OFFSET_PARAM_2);   		
      	printf("instance number report:%d\n", p->node_info_table[sourceNode].cmdclass[*((BYTE*)pCmd + OFFSET_PARAM_1)].instance);
    	}	
    break;
    
    case COMMAND_CLASS_BATTERY:
    	if(cmd == BATTERY_REPORT)
    	{
    		p->node_info_table[sourceNode].id = sourceNode; 
    		p->node_info_table[sourceNode].powerlevel = *((BYTE*)pCmd + OFFSET_PARAM_1);
    		printf("power level:%d\n", p->node_info_table[sourceNode].powerlevel);
    	}
    break;
    
    case COMMAND_CLASS_WAKE_UP_V2:
    	if(cmd == WAKE_UP_INTERVAL_REPORT_V2)
    	{
    		p->node_info_table[sourceNode].id = sourceNode;
    		   		
    		p->node_info_table[sourceNode].wakeupinterval = (*((BYTE*)pCmd + OFFSET_PARAM_1)<<16) + (*((BYTE*)pCmd + OFFSET_PARAM_2)<<8) + *((BYTE*)pCmd + OFFSET_PARAM_3);
    		printf("wakeup interval:%d\n", p->node_info_table[sourceNode].wakeupinterval);
    	}
    	else if(cmd == WAKE_UP_NOTIFICATION_V2)
    	{
    		p->node_info_table[sourceNode].id = sourceNode;
    		printf("Get ID %d WUN, we can sent queued cmd\n", p->node_info_table[sourceNode].id);
				
				flag_getWUN = 1;
				flag_getWUN_ID = sourceNode;
				
				//if(cbFuncGetWUN != NULL)
				//   cbFuncGetWUN(sourceNode);								
    	}	
    break;
    
    case COMMAND_CLASS_ASSOCIATION_V2:
    	if(cmd == ASSOCIATION_REPORT_V2)
    	{
    		printf("Group Num:%d, Max nodes support:%d\n", *((BYTE*)pCmd + OFFSET_PARAM_1), *((BYTE*)pCmd + OFFSET_PARAM_2));
    		printf("AssociationNodeID:");
    		for(i=5; i<cmdLength; i++)
    			printf(" %d", *((BYTE*)pCmd + i));
    		printf("\n");
    	}	    		
    break;
    
    case COMMAND_CLASS_SENSOR_BINARY:
    	if(cmd == SENSOR_BINARY_REPORT)
    	{
    		flag_getIRSensorStatus = 1;
    		flag_getIRSensorStatus_ID = sourceNode;	
    		flag_getIRSensorStatus_status = *((BYTE*)pCmd + OFFSET_PARAM_1);
    		
    		//if(cbFuncGetIRSensorStatus != NULL)
    		//		cbFuncGetIRSensorStatus(sourceNode, *((BYTE*)pCmd + OFFSET_PARAM_1));
    	}	
    break;
/*
    case COMMAND_CLASS_CONTROLLER_REPLICATION:
      // Handle replication frames 
      if (receivingRemote)
      {
        // We are only interested in group replication here 
        // but all other replication can be handled in the same way. 
        // In this application we ignore everything else and just 
        // send the command complete reply 
        if (pCmdTMP->ZW_Common.cmd == CTRL_REPLICATION_TRANSFER_GROUP)
        {
          // We are only interested in group 1 
          if (pCmdTMP->ZW_CtrlReplicationTransferGroupFrame.groupId == 1)
          {
            // Add the node ID to the group 
            AddNodeToGroup(pCmdTMP->ZW_CtrlReplicationTransferGroupFrame.nodeId);
          }
        }
        // Send command complete to other controller 
        // NOTE: This call should be done when ready to receive 
        // the next replication frame 
        ZW_REPLICATION_COMMAND_COMPLETE();
      }
    break;
    
    case COMMAND_CLASS_VERSION:
      if (pCmdTMP->ZW_Common.cmd == VERSION_GET)
      {
        txBuffer.ZW_VersionReportFrame.cmdClass = COMMAND_CLASS_VERSION;
        txBuffer.ZW_VersionReportFrame.cmd = VERSION_REPORT;
        txBuffer.ZW_VersionReportFrame.zWaveLibraryType = ZW_TYPE_LIBRARY();
        txBuffer.ZW_VersionReportFrame.zWaveProtocolVersion = ZW_VERSION_MAJOR;
        txBuffer.ZW_VersionReportFrame.zWaveProtocolSubVersion = ZW_VERSION_MINOR;
        txBuffer.ZW_VersionReportFrame.applicationVersion = PORTABLE_CTRL_VERSION;
        txBuffer.ZW_VersionReportFrame.applicationSubVersion = PORTABLE_CTRL_REVISION;
        ZW_SendData(sourceNode, (BYTE *)&txBuffer, sizeof(txBuffer.ZW_VersionReportFrame), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, NULL);
      }
    break;
*/

/*    
    case COMMAND_CLASS_SECURITY:           
      if (statusNodeIn(sourceNode))
      {
        switch(pCmdTMP->ZW_Common.cmd)
        {
          case SECURITY_SCHEME_REPORT:
            StopSecurityTimeOut();
            ZW_DEBUG_SEND_BYTE('1');
            nodeSecureIncl = 1;
            
          if (nodeSecure)
          {
            if (SECURITY_SCHEME==SECURITY_SCHEME_0)
            {
              if (pCmdTMP->ZW_SecuritySchemeReportFrame.supportedSecuritySchemes == SECURITY_SCHEME_0)
              {
                StopSecurityTimeOut();
                ZW_DEBUG_SEND_BYTE('2');
                enNodeID = 0xff;
                txBuffer.ZW_NetworkKeySetFrame.cmdClass                       = COMMAND_CLASS_SECURITY;
                txBuffer.ZW_NetworkKeySetFrame.cmd                            = NETWORK_KEY_SET;
                memset(inclusionKey, 0x0, 16);
                OutKeyNewController = 1;
                memcpy((BYTE *)txBuffer.ZW_NetworkKeySetFrame.networkKey,networkKey,16);
                NodeS();
                nodeInWork = sourceNode;
                StartSecurityTimeOut(TIME_SECURITY_LIFE);
                ZW_SendDataSecure(sourceNode, (BYTE *)&txBuffer, sizeof(txBuffer.ZW_NetworkKeySetFrame), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, 0);
              }
              else OutKeyNewController = 0xff;
            }
          }
          break;
  
          case SECURITY_NONCE_REPORT:
            enNodeID = sourceNode;
            StopSecuritySendTimeOut();
            RegisterENOut(sourceNode,&pCmdBuf[2]);
          break;
  
        case SECURITY_SCHEME_GET:
          StopSecurityTimeOut();
  ZW_DEBUG_SEND_BYTE('3');
          if (nodeSecure)
          {
            if ((BYTE)pCmdBuf[2]!=SECURITY_SCHEME_0)
            {
              nodeSecure = 0;
//              ZW_MEM_PUT_BYTE(EEOFFSET_NETWORK_SECURITY, 0x0);
              setCtrlNoneSecure();
              /////TO#02438 Secure dev. ctrl ignores invalid scheme
              return;
            }
            MemoryGetID(bufHomeID,&nodeID);
            if (SECURITY_SCHEME==SECURITY_SCHEME_0)
            {
              if (pCmdBuf[2]==SECURITY_SCHEME_0)
              {
                memset(networkKey,0x00,16);
                LoadKeys();
              }
            }
            setCtrlSecure();
            txBuf.ZW_SecuritySchemeReportFrame.cmdClass = COMMAND_CLASS_SECURITY; // The command class 
            txBuf.ZW_SecuritySchemeReportFrame.cmd = SECURITY_SCHEME_REPORT; // The command 
            txBuf.ZW_SecuritySchemeReportFrame.supportedSecuritySchemes = SECURITY_SCHEME;
            ZW_SendData(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_SecuritySchemeReportFrame), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, NULL);
            nodeSecureIncl = 1;
            NodeS();
            nodeInWork = sourceNode;
            StartSecurityTimeOut(TIME_SECURITY_LIFE);
          }
          break;
  
        default:
          pBuf = (BYTE *)pCmd + 1;
          
          if (!nodeSecure) break;
          
          cmdLength = ProcessIncomingSecure(sourceNode, pBuf, TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE,cmdLength - 1);
          enNodeID = 0xff;
          if (cmdLength && (cmdLength < 255))
          {
            pCmdTMP = (ZW_APPLICATION_TX_BUFFER_NEW *)(pBuf + 9 + 1);
            switch(pCmdTMP->ZW_Common.cmdClass)
            {
              case COMMAND_CLASS_SWITCH_MULTILEVEL:
                if (pCmdTMP->ZW_Common.cmd == SWITCH_MULTILEVEL_REPORT)
                {
                  groupLevel = pCmdTMP->ZW_SwitchMultilevelReportFrame.value;
                  StoreGroupLevel(groupLevel);
                  StopDimEndTimer();
                  if (!receivedReport)
                  {
                    SetGroupDimLevel();
                  }
                  receivedReport = TRUE;
                }
                break;
              case COMMAND_CLASS_VERSION:
                if (pCmdTMP->ZW_Common.cmd == VERSION_GET)
                {
                  txBuffer.ZW_VersionReportFrame.cmdClass = COMMAND_CLASS_VERSION;
                  txBuffer.ZW_VersionReportFrame.cmd = VERSION_REPORT;
                  txBuffer.ZW_VersionReportFrame.zWaveLibraryType = ZW_TYPE_LIBRARY();
                  txBuffer.ZW_VersionReportFrame.zWaveProtocolVersion = ZW_VERSION_MAJOR;
                  txBuffer.ZW_VersionReportFrame.zWaveProtocolSubVersion = ZW_VERSION_MINOR;
                  txBuffer.ZW_VersionReportFrame.applicationVersion = PORTABLE_CTRL_VERSION;
                  txBuffer.ZW_VersionReportFrame.applicationSubVersion = PORTABLE_CTRL_REVISION;
                  ZW_SendDataSecure(sourceNode, (BYTE *)&txBuffer, sizeof(txBuf.ZW_VersionReportFrame), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, securityCommandsTimer);
                }
                break;
              case COMMAND_CLASS_SECURITY:
                if (pCmdTMP->ZW_Common.cmd == NETWORK_KEY_VERIFY)
                {
                  StopSecurityTimeOut();
                  ZW_DEBUG_SEND_BYTE('4');
                  if (nodeSecure)
                  {
                    AddSecuritySlave(sourceNode,TRUE);
                    if (addController)
                    {
                      enNodeID = 0xff;
                      txBuf.ZW_SecuritySchemeInheritFrame.cmdClass = COMMAND_CLASS_SECURITY; // The command class 
                      txBuf.ZW_SecuritySchemeInheritFrame.cmd = SECURITY_SCHEME_INHERIT; // The command 
                      txBuf.ZW_SecuritySchemeInheritFrame.supportedSecuritySchemes = SECURITY_SCHEME;

                      ZW_SendDataSecure(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_SecuritySchemeInheritFrame), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, securityCommandsTimer);
                      NodeS();
                      nodeInWork = sourceNode;
                      StartSecurityTimeOut(TIME_SECURITY_LIFE);
                    }
                  }
                  else if (IS_PB_HELD(glKey))
                  {
                    // User wants to include a new slave node to the network. Handle it 
                    StartLED0Blink(BUSY_BLINK_RATE);
                    mainState = INCLUDE_NODE_STATE;
                    NodeS();
                    setCtrlIncluding();
                    ZW_ADD_NODE_TO_NETWORK((ADD_NODE_ANY | ADD_NODE_OPTION_HIGH_POWER | ADD_NODE_OPTION_NETWORK_WIDE), NewControllerSendStarted);
                  }
                }
                else if (pCmdTMP->ZW_Common.cmd == SECURITY_SCHEME_REPORT)
                {
                  StopSecurityTimeOut();
                  ZW_DEBUG_SEND_BYTE('5');
                  if (IS_PB_HELD(glKey))
                  {
                      //User wants to include a new slave node to the network. Handle it 
                      StartLED0Blink(BUSY_BLINK_RATE);
                      mainState = INCLUDE_NODE_STATE;
                      NodeS();
                      setCtrlIncluding();
                      ZW_ADD_NODE_TO_NETWORK((ADD_NODE_ANY | ADD_NODE_OPTION_HIGH_POWER | ADD_NODE_OPTION_NETWORK_WIDE), NewControllerSendStarted);
                  }
                }
                else if (pCmdTMP->ZW_Common.cmd ==  SECURITY_SCHEME_INHERIT)
                {
                  StopSecurityTimeOut();
                  ZW_DEBUG_SEND_BYTE('6');
                  if(nodeSecureIncl==2)
                  if (nodeSecure)
                  {
                    enNodeID = 0xff;
                    if (pCmdTMP->ZW_SecuritySchemeInheritFrame.supportedSecuritySchemes == SECURITY_SCHEME_0)
                    {
                      SECURITY_SCHEME = pCmdTMP->ZW_SecuritySchemeInheritFrame.supportedSecuritySchemes;
                    }
                    else
                    {
                      /////TO#02438 Secure dev. ctrl ignores invalid scheme 
                      nodeSecure = 0;
                      setCtrlNoneSecure();
//                      ZW_MEM_PUT_BYTE(EEOFFSET_NETWORK_SECURITY, 0x0);                     
                      return;                    
                    }
                    
                    txBuf.ZW_SecuritySchemeReportFrame.cmdClass = COMMAND_CLASS_SECURITY; // The command class 
                    txBuf.ZW_SecuritySchemeReportFrame.cmd = SECURITY_SCHEME_REPORT; // The command 
                    txBuf.ZW_SecuritySchemeReportFrame.supportedSecuritySchemes = SECURITY_SCHEME;
                    ZW_SendDataSecure(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_SecuritySchemeReportFrame), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, securityEnd);
                  }
                }
                else if (pCmdTMP->ZW_Common.cmd == SECURITY_COMMANDS_SUPPORTED_GET)
                {
                  enNodeID = 0xff;
                  txBuf.ZW_SecurityCommandsSupportedReportFrame.cmdClass = COMMAND_CLASS_SECURITY;
                  txBuf.ZW_SecurityCommandsSupportedReportFrame.cmd = SECURITY_COMMANDS_SUPPORTED_REPORT;
                  securityFrameCount = CLASS_REPORT_COUNT/(sizeof(txBuf.ZW_SecurityCommandsSupportedReportFrame)-3);
                  if (securityFrameCount)
                  {
                    memcpy( &txBuf.ZW_SecurityCommandsSupportedReportFrame.cmdClassSupported[1], &nodeInfoFrame, (sizeof(txBuf.ZW_SecurityCommandsSupportedReportFrame)-3));
                    securityFramePos = nodeInfoFrame;
                    securityFramePos+=(sizeof(txBuf.ZW_SecurityCommandsSupportedReportFrame)-3);
                    txBuf.ZW_SecurityCommandsSupportedReportFrame.cmdClassSupported[0] = securityFrameCount;
                    reportNode = sourceNode;
                    ZW_SendDataSecure(sourceNode, (BYTE *)&txBuf, 3 + (sizeof(txBuf.ZW_SecurityCommandsSupportedReportFrame)-2), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, securityCommandsSupported);
                  }
                  else
                  {
                    memcpy( &txBuf.ZW_SecurityCommandsSupportedReportFrame.cmdClassSupported[1], &nodeInfoFrame, CLASS_REPORT_COUNT);
                    securityFramePos = nodeInfoFrame;
                    securityFramePos+=CLASS_REPORT_COUNT;
                    txBuf.ZW_SecurityCommandsSupportedReportFrame.cmdClassSupported[0] = securityFrameCount;
                    reportNode = sourceNode;
                    ZW_SendDataSecure(sourceNode, (BYTE *)&txBuf, 3 + CLASS_REPORT_COUNT, TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, securityCommandsSupported);
                  }
                }
                else if (pCmdTMP->ZW_Common.cmd == SECURITY_COMMANDS_SUPPORTED_REPORT)
                {
                  if(statAddNodeToGroup)
                  {
                    statAddNodeToGroup = FALSE;
                    AddSecuritySlave(sourceNode,TRUE);
                  }
                  enNodeID = 0xff;
                  ControlGroup(DIM_STOP,DimStopped);
                }
                else if (pCmdTMP->ZW_Common.cmd==NETWORK_KEY_SET)
                {
                  StopSecurityTimeOut();
                  ZW_DEBUG_SEND_BYTE('7');
                  if(nodeSecureIncl)
                  if(nodeSecure)
                  {
                    memcpy(networkKey,pCmdTMP->ZW_NetworkKeySetFrame.networkKey,16);
                    // Now network Key should contain the new networkKey 
                    // Save the new networkKey in NV RAM 
                    // Go and make the Authentication and the encryption keys 
                    LoadKeys();
                    enNodeID = 0xff;
                    setCtrlSecure();
                    txBuf.ZW_NetworkKeyVerifyFrame.cmdClass = COMMAND_CLASS_SECURITY;
                    txBuf.ZW_NetworkKeyVerifyFrame.cmd = NETWORK_KEY_VERIFY;
                    ZW_SendDataSecure(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_NetworkKeyVerifyFrame), TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE, NULL);
                    NodeS();
                    nodeSecureIncl = 2;
                    nodeInWork = sourceNode;
                    StartSecurityTimeOut(TIME_SECURITY_LIFE);
                  }
                }
  
                break;
              default:
                break;
            }
          }
        break;
        }
      }
    break;
*/
		//zw_senddata_status = STATUS_OK;			    
  }
}


/*=========================   ZW_Initial   ====================
**    Initalize the z-wave dongle.
**
**    Side effects:
**
**------------------------------------------------------------*/
BYTE API_Initial(DONGLE_INFO *p)
{
	BYTE i, j, device, temp, node_ID, ver, cap, nodes[MAX_NODEMASK_LENGTH]={0};
	BYTE len, chip_type, chip_version;
	SCMD cmd;
	
	initial_flag = 1;
	
	TimerInit();
	
	API_SerialAPI_InitCBFunction(AP_callback_getNIF, AP_callback_getIRSensorStatus, AP_callback_getWakeUpNotification);		
	//SerialAPI_InitCBFunction(AP_callback_getNIF, NULL, NULL);		
	SerialAPI_InitSW(ApplicationCommandHandler, ApplicationControllerUpdate);
	
	
	for(i=0;i<MAX_NODEMASK_LENGTH;i++)	// Clear all the node bit mask value
		p->device_bit_mask[i] = 0;				
	
	//====== 100-05-24
	for(i=0; i<ZW_MAX_NODES; i++)	// Clear all the node bit mask value
	{
		p->node_info_table[i].id = 0;
		p->node_info_table[i].type = 0;
	}
	//======
									
	temp = ZW_Version(&cmd, (BYTE *)compl_workbuf);
	if(temp<0x08 && temp>0x00)
		printf("*Library type:%x, %s*\n\n", temp, compl_workbuf);
	else
	{
		printf("ZW-version error\n");
		return INIT_FAIL;
	}
	
	init_buffer(&cmd);	
	if( ZW_SerialAPI_GetInitData(&cmd, &ver, &cap, &len, nodes) == -1 )
	{
		printf("Get Info error\n");
		return INIT_FAIL;
	}
	else
	{
		printf("*API version:%x, Cap:%x*\n\n", ver, cap);
		
		memcpy(&(p->device_bit_mask[0]), nodes, MAX_NODEMASK_LENGTH);
		
		j=0;
		for(temp=0; temp<MAX_NODEMASK_LENGTH; temp++)
		{
			for(i=0; i<8; i++)	// 8 bit each BYTE
			{
				if(p->device_bit_mask[temp] & (0x01<<i))
				{	
					init_buffer(&cmd);
					node_ID = temp*8 + (i+1);
					ZW_GetNodeProtocolInfo(&cmd, node_ID, &nodeInfo);
					printf("*NodeID:%d, Cap:%x, Secu:%x*\n", node_ID, nodeInfo.capability, nodeInfo.security);
					printf("*Basic:%x, generic:%x, specific:%x*\n", nodeInfo.nodeType.basic, nodeInfo.nodeType.generic, nodeInfo.nodeType.specific);
					
					device_type = Analysis_DeviceType(&nodeInfo);	// Get device type
					p->node_info_table[node_ID].id = node_ID;
					p->node_info_table[node_ID].type = device_type;
					printf("*ID:%d, Type:%d*\n\n", p->node_info_table[node_ID].id, p->node_info_table[node_ID].type);					
					j++;
				}
			}
		}
		p->total_device_number = j-1;
	}
	
	init_buffer(&cmd);
	temp = ZW_GetControllerCapabilities(&cmd);
	printf("*Controller Type:%x*\n\n", temp);
		
	init_buffer(&cmd);	
  ZW_MemoryGetID(&cmd, myHomeID, &myNodeID);	// Check if we have a node id
	printf("*HomeID:%x %x %x %x, NodeID:%x*\n\n", myHomeID[0], myHomeID[1], myHomeID[2], myHomeID[3], myNodeID);
	
	init_buffer(&cmd);
	temp = ZW_GetSUCNodeID(&cmd);
	printf("*Get SUC nodeID:%x*\n\n", temp);
	
	initial_flag = 0;
	
	return INIT_OK;		
}


/*=========================   PrepareAssociationSet   ====================
**    Prepares the transmit buffer with data for a association set frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareAssociationSet( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE groupnum, BYTE node)
{   
  pBuf->ZW_AssociationSet1byteV2Frame.cmdClass = COMMAND_CLASS_ASSOCIATION_V2;
  pBuf->ZW_AssociationSet1byteV2Frame.cmd = ASSOCIATION_SET_V2; 
  pBuf->ZW_AssociationSet1byteV2Frame.groupingIdentifier = groupnum; 
  pBuf->ZW_AssociationSet1byteV2Frame.nodeId1 = node;  	
}


/*=========================   PrepareAssociationRemove   ====================
**    Prepares the transmit buffer with data for a association remove frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareAssociationRemove( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE groupnum, BYTE node)
{   
  pBuf->ZW_AssociationRemove1byteV2Frame.cmdClass = COMMAND_CLASS_ASSOCIATION_V2;
  pBuf->ZW_AssociationRemove1byteV2Frame.cmd = ASSOCIATION_REMOVE_V2; 
  pBuf->ZW_AssociationRemove1byteV2Frame.groupingIdentifier = groupnum; 
  pBuf->ZW_AssociationRemove1byteV2Frame.nodeId1 = node;  	
}


/*=========================   PrepareAssociationGet   ====================
**    Prepares the transmit buffer with data for a association get frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareAssociationGet( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE groupnum)
{   
  pBuf->ZW_AssociationGetV2Frame.cmdClass = COMMAND_CLASS_ASSOCIATION_V2;
  pBuf->ZW_AssociationGetV2Frame.cmd = ASSOCIATION_GET_V2; 
  pBuf->ZW_AssociationGetV2Frame.groupingIdentifier = groupnum;   	
}


/*=========================   PrepareWakeUpNoMoreInformation   ====================
**    Prepares the transmit buffer with data for a wake up no more information frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareWakeUpNoMoreInformation ( ZW_APPLICATION_TX_BUFFER *pBuf )
{   
  pBuf->ZW_WakeUpNoMoreInformationV2Frame.cmdClass = COMMAND_CLASS_WAKE_UP_V2;
  pBuf->ZW_WakeUpNoMoreInformationV2Frame.cmd = WAKE_UP_NO_MORE_INFORMATION_V2;    	
}


/*=========================   PrepareWakeUpIntervaleSet   ====================
**    Prepares the transmit buffer with data for a wake up interval Set frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareWakeUpIntervaleSet ( ZW_APPLICATION_TX_BUFFER *pBuf , BYTE node, int sec)
{ 	  
  pBuf->ZW_WakeUpIntervalSetV2Frame.cmdClass = COMMAND_CLASS_WAKE_UP_V2;
  pBuf->ZW_WakeUpIntervalSetV2Frame.cmd = WAKE_UP_INTERVAL_SET_V2; 
  pBuf->ZW_WakeUpIntervalSetV2Frame.seconds1 = (sec & 0xFF0000) >> 16;
  pBuf->ZW_WakeUpIntervalSetV2Frame.seconds2 = (sec & 0x00FF00) >> 8;
  pBuf->ZW_WakeUpIntervalSetV2Frame.seconds3 = sec & 0x0000FF;
  pBuf->ZW_WakeUpIntervalSetV2Frame.nodeid = node;  	
}


/*=========================   PrepareWakeUpIntervaleGet   ====================
**    Prepares the transmit buffer with data for a wake up interval Get frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareWakeUpIntervaleGet ( ZW_APPLICATION_TX_BUFFER *pBuf )
{   
  pBuf->ZW_WakeUpIntervalGetV2Frame.cmdClass = COMMAND_CLASS_WAKE_UP_V2;
  pBuf->ZW_WakeUpIntervalGetV2Frame.cmd = WAKE_UP_INTERVAL_GET_V2;    	
}


/*=========================   PrepareBatteryGet   ====================
**    Prepares the transmit buffer with data for a Battery Get frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareBatteryGet ( ZW_APPLICATION_TX_BUFFER *pBuf )
{   
  pBuf->ZW_BatteryGetFrame.cmdClass = COMMAND_CLASS_BATTERY;
  pBuf->ZW_BatteryGetFrame.cmd = BATTERY_GET;   	
}


/*=========================   PrepareMultiInstanceEncap   ====================
**   date: 1000224
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMultiInstanceEncap ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE endpoint, BYTE seqnum, int cmd)
{
  pBuf->ZW_MultiChannelCmdEncapV2Frame.cmdClass = COMMAND_CLASS_MULTI_CHANNEL_V2;
  //pBuf->ZW_MultiInstanceCmdEncapFrame.cmd = MULTI_INSTANCE_CMD_ENCAP_V2;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.cmd = MULTI_CHANNEL_CMD_ENCAP_V2;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.properties1 = 0x00;	// for homesecnario
  pBuf->ZW_MultiChannelCmdEncapV2Frame.properties2 = endpoint;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.cmdClass = COMMAND_CLASS_SIMPLE_AV_CONTROL;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.cmd = SIMPLE_AV_CONTROL_SET;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.sequenceNumber = seqnum;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.properties1 = KEY_DOWN;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.itemId1 = ITEMID_NONE_MSB;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.itemId2 = ITEMID_NONE_LSB;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.variantgroup1.command1 = (cmd & 0xFF00)>>8;
  pBuf->ZW_MultiChannelCmdEncapV2Frame.encapFrame.ZW_SimpleAvControlSet1byteFrame.variantgroup1.command2 = cmd & 0x00FF;
}


/*=========================   PrepareMultiInstanceGet   ====================
**   date: 1000224
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMultiInstanceGet ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE cmdclass )
{
  pBuf->ZW_MultiInstanceGetV2Frame.cmdClass = COMMAND_CLASS_MULTI_CHANNEL_V2;
  pBuf->ZW_MultiInstanceGetV2Frame.cmd = MULTI_INSTANCE_GET_V2;
  pBuf->ZW_MultiInstanceGetV2Frame.commandClass = cmdclass;
}


/*=========================   PrepareBinaryGet   ====================
**    Prepares the transmit buffer with data for a Binary Get frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareBinaryGet ( ZW_APPLICATION_TX_BUFFER *pBuf )
{   
  pBuf->ZW_SwitchBinaryGetFrame.cmdClass = COMMAND_CLASS_SWITCH_BINARY;
  pBuf->ZW_SwitchBinaryGetFrame.cmd = SWITCH_BINARY_GET;   	
}


/*=========================   PrepareBinarySet   ====================
**    Prepares the transmit buffer with data for a Binary Set frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareBinarySet ( ZW_APPLICATION_TX_BUFFER *pBuf , BYTE value )
{   
  pBuf->ZW_SwitchBinarySetFrame.cmdClass = COMMAND_CLASS_SWITCH_BINARY;
  pBuf->ZW_SwitchBinarySetFrame.cmd = SWITCH_BINARY_SET; 
  pBuf->ZW_SwitchBinarySetFrame.switchValue = value;  	
}


/*=========================   PrepareBasicGet   ====================
**    Prepares the transmit buffer with data for a Basic Get frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareBasicGet ( ZW_APPLICATION_TX_BUFFER *pBuf )
{   
  pBuf->ZW_BasicGetFrame.cmdClass = COMMAND_CLASS_BASIC;
  pBuf->ZW_BasicGetFrame.cmd = BASIC_GET;   	
}


/*=========================   PrepareMeterSupport   ====================
**    Prepares the transmit buffer with data for a Meter Support frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMeterSupport ( ZW_APPLICATION_TX_BUFFER *pBuf )
{
  pBuf->ZW_MeterSupportedGetV3Frame.cmdClass = COMMAND_CLASS_METER;
  pBuf->ZW_MeterSupportedGetV3Frame.cmd = METER_SUPPORTED_GET_V3;
}


/*=========================   PrepareMeterGet   ====================
**    Prepares the transmit buffer with data for a Meter Get frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMeterGet ( ZW_APPLICATION_TX_BUFFER *pBuf , BYTE value )
{
  pBuf->ZW_MeterGetV3Frame.cmdClass = COMMAND_CLASS_METER;
  pBuf->ZW_MeterGetV3Frame.cmd = METER_GET_V3;
  pBuf->ZW_MeterGetV3Frame.properties1 = value;
}


/*=========================   PrepareMultilevelGet   ====================
**    Prepares the transmit buffer with data for a Multilevel Get frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMultilevelGet ( ZW_APPLICATION_TX_BUFFER *pBuf )
{
  pBuf->ZW_SwitchMultilevelGetFrame.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL;
  pBuf->ZW_SwitchMultilevelGetFrame.cmd = SWITCH_MULTILEVEL_GET;
}


/*===================   PrepareMultilevelStartLevelChange   ==================
**    Prepares the transmit buffer with data for a Multilevel Set frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMultilevelStartLevelChange ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE dir, BYTE startlevel)
{   
  pBuf->ZW_SwitchMultilevelStartLevelChangeFrame.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL;
  pBuf->ZW_SwitchMultilevelStartLevelChangeFrame.cmd = SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V3;  
  pBuf->ZW_SwitchMultilevelStartLevelChangeFrame.level = 0x20|(dir<<6);  	
  pBuf->ZW_SwitchMultilevelStartLevelChangeFrame.startLevel = startlevel;  	
}


/*=========================   PrepareMultilevelStop   ====================
**    Prepares the transmit buffer with data for a Multilevel Set frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMultilevelStop ( ZW_APPLICATION_TX_BUFFER *pBuf)
{   
  pBuf->ZW_SwitchMultilevelSetFrame.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL;
  pBuf->ZW_SwitchMultilevelSetFrame.cmd = SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE;  	
}


/*=========================   PrepareMultilevelSet   ====================
**    Prepares the transmit buffer with data for a Multilevel Set frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareMultilevelSet ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE value )
{   
  pBuf->ZW_SwitchMultilevelSetFrame.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL;
  pBuf->ZW_SwitchMultilevelSetFrame.cmd = SWITCH_MULTILEVEL_SET;
  pBuf->ZW_SwitchMultilevelSetFrame.value = value;   	
}


/*============================   SendCompleted   ======================
**    Callback for association sends...
**
**    Side effects:
**
**-------------------------------------------------------------------*/
void SendCompleted(BYTE txStatus)       // IN   Transmission result           
{
	switch(txStatus)
	{
		case TRANSMIT_COMPLETE_OK:
		{
			cmd_exe_status = STATUS_IDLE;
			
			if(get_status_flag == GET_STATUS_OFF)	// only when set operation will report
				zw_senddata_status = STATUS_OK;
  		
  		printf("send ok, next, state %d\n", cmd_exe_status);
		}
		break;
		
		case TRANSMIT_COMPLETE_NO_ACK:
		{
			cmd_exe_status = STATUS_IDLE;	// ?? should we send again ?
			zw_senddata_status = STATUS_NOACK;
			printf("send no ack, state %d\n", cmd_exe_status);
		}
		break;
		
		case TRANSMIT_COMPLETE_FAIL:
		{
			cmd_exe_status = STATUS_IDLE;
			zw_senddata_status = STATUS_NETWORKBUSY;
			printf("send fail, again, state %d\n", cmd_exe_status);
		}
		break;
	}  
}


/*=========================   PrepareBasicSet   ====================
**    Prepares the transmit buffer with data for a Basic Set frame.
**
**    Side effects:
**
**---------------------------------------------------------------*/
void PrepareBasicSet ( ZW_APPLICATION_TX_BUFFER *pBuf, BYTE status )
{   
  pBuf->ZW_BasicSetFrame.cmdClass = COMMAND_CLASS_BASIC;
  pBuf->ZW_BasicSetFrame.cmd = BASIC_SET;
  pBuf->ZW_BasicSetFrame.value = status;    	
}


//===============================   ZW_SendData   ===========================
//**    Transmit data buffer to a single ZW-node or all ZW-nodes (broadcast).
//**
//**    txOptions:
//**          TRANSMIT_OPTION_LOW_POWER     transmit at low output power level
//**                                        (1/3 of normal RF range).
//**          TRANSMIT_OPTION_ACK           the multicast frame will be followed
//**                                        by a  singlecast frame to each of
//**                                        the destination nodes
//**                                        and request acknowledge from each
//**                                        destination node.
//**          TRANSMIT_OPTION_AUTO_ROUTE    request retransmission via repeater
//**                                        nodes at normal output power level).
//**
//** extern BYTE            RET  FALSE if transmitter queue overflow
//** ZW_SendData(
//** BYTE  nodeID,          IN  Destination node ID (0xFF == broadcast)
//** BYTE *pData,           IN  Data buffer pointer
//** BYTE  dataLength,      IN  Data buffer length
//** BYTE  txOptions,       IN  Transmit option flags
//** void (*completedFunc)( IN  Transmit completed call back function
//**        BYTE txStatus));    IN Transmit status
//**--------------------------------------------------------------------------*/
BYTE ZW_SendData(HCMD p, BYTE  nodeID, BYTE *pData, BYTE  dataLength, BYTE  txOptions, VOID_CALLBACKFUNC(completedFunc)(BYTE))
{
  int i;
	
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
//	test_speed_flag = 1;
//	test_speed_ctr = 0;

	flag_prevnet_lock = 1;
  prevent_lock_ctr = 0;
		
  p->idx = 0;
  p->byLen = 0;  
  
  p->byCompletedFunc = (completedFunc == NULL ? 0 : completedFunc);
  p->idx++;
  p->buffer[p->idx++] = REQUEST;
  p->buffer[p->idx++] = FUNC_ID_ZW_SEND_DATA;
  p->buffer[p->idx++] = nodeID;
  p->buffer[p->idx++] = dataLength;
  
  for (i = 0; i < dataLength; i++)
  {
    p->buffer[p->idx++] = pData[i];
  }
  
  p->buffer[p->idx++] = txOptions;
  p->buffer[p->idx++] = p->byCompletedFunc;      // Func id for CompletedFunc
  p->buffer[0] = p->idx;                      // length
  
  cbFuncZWSendData = completedFunc; 
  SendDataAndWaitForResponse(&(p->buffer), p->idx, FUNC_ID_ZW_SEND_DATA, &(p->buffer), &(p->byLen));    
  
  if(p->buffer[IDX_DATA] == FALSE)
  	cmd_exe_status = STATUS_IDLE;
  
  //printf("speed %d\n",test_speed_ctr);
  	
  return p->buffer[IDX_DATA];
}


//============== 100-03-02 test
BYTE ZW_SendData_test(HCMD p, BYTE  nodeID, BYTE *pData, BYTE  dataLength, BYTE  txOptions, VOID_CALLBACKFUNC(completedFunc)(BYTE))
{
  int i;

  p->idx = 0;
  p->byLen = 0;

  printf("ZW sendData test\n");

  p->byCompletedFunc = (completedFunc == NULL ? 0 : completedFunc);
  p->idx++;
  p->buffer[p->idx++] = REQUEST;
  p->buffer[p->idx++] = FUNC_ID_ZW_SEND_DATA;
  p->buffer[p->idx++] = nodeID;
  p->buffer[p->idx++] = dataLength;

  for (i = 0; i < dataLength; i++)
  {
    p->buffer[p->idx++] = pData[i];
  }

  p->buffer[p->idx++] = txOptions;
  p->buffer[p->idx++] = p->byCompletedFunc;   // Func id for CompletedFunc
  p->buffer[0] = p->idx;                      // length
	
  cbFuncZWSendData = completedFunc;
  SendDataAndWaitForResponse(&(p->buffer), p->idx, FUNC_ID_ZW_SEND_DATA, &(p->buffer), &(p->byLen));

  //SendData( buffer, idx );

  return p->buffer[IDX_DATA];
}
//====================== end


/*============================   ResetDefaultCompleted   ======================
**    Function description
**      reset to default.
**    
**      
**--------------------------------------------------------------------------*/
void ResetDefaultCompleted(void)  
{
	zw_senddata_status = STATUS_OK;
	cmd_exe_status = STATUS_IDLE;
	
  printf("reset to default OK, status %d\n", cmd_exe_status);
}


/*============================   RemoveNodeFromGroup   ======================
**    Function description
**      Removes a node from a group.
**    Side effects:
**      updates the groupArray
**--------------------------------------------------------------------------*/
void RemoveNodeFromGroup(BYTE bNodeID)  //IN NodeID to remove
{
  //StoreNodeBitInGroup(bNodeID, 0);
  printf("removed node %x from group\n", bNodeID);
}


/*=========================   RemoveNodeCompleted_org   ========================
**    Function description
**        Callback function, used when removing switches from the network.
**          bStatus BYTE,     IN  Status of learn process
**          bSource BYTE,     IN  Node id of the node that send node info
**          BYTE* pCmd,       IN  Pointer to Node information
**          BYTE bLen));      IN  Node info length
**
**    Side effects: Shuts down RF receive mode when completed and removes the
**                  node from the group.
**--------------------------------------------------------------------------*/
void RemoveNodeCompleted_org(LEARN_INFO *learnNodeInfo)
{
	SCMD cmd;
	
  if ((*learnNodeInfo).bStatus == REMOVE_NODE_STATUS_NODE_FOUND)
  {
    printf("remove node found\n");
  }
  else if (((*learnNodeInfo).bStatus == REMOVE_NODE_STATUS_REMOVING_SLAVE)||
           ((*learnNodeInfo).bStatus == REMOVE_NODE_STATUS_REMOVING_CONTROLLER))
  {
    
  }
  else if (learnNodeInfo->bStatus == REMOVE_NODE_STATUS_FAILED)
  {
    ZW_RemoveNodeFromNetwork(&cmd, REMOVE_NODE_STOP, NULL);
    printf("removed node fail, try again\n");
    ZW_RemoveNodeFromNetwork(&cmd, REMOVE_NODE_ANY, RemoveNodeCompleted);
  }	 
  else if (learnNodeInfo->bStatus == REMOVE_NODE_STATUS_DONE)
  {
    ZW_RemoveNodeFromNetwork(&cmd, REMOVE_NODE_STOP, NULL);
    cmd_exe_status = STATUS_IDLE;
    printf("removed node done, status %d\n", cmd_exe_status);
  }
}


/*=========================   RemoveNodeCompleted   ========================
**    Function description
**        Callback function, used when removing switches from the network.
**          bStatus BYTE,     IN  Status of learn process
**          bSource BYTE,     IN  Node id of the node that send node info
**          BYTE* pCmd,       IN  Pointer to Node information
**          BYTE bLen));      IN  Node info length
**
**    Side effects: Shuts down RF receive mode when completed and removes the
**                  node from the group.
**--------------------------------------------------------------------------*/
void RemoveNodeCompleted(LEARN_INFO *learnNodeInfo)
{
	SCMD cmd;
	
  if ((*learnNodeInfo).bStatus == REMOVE_NODE_STATUS_NODE_FOUND)
  {
  	cmd_exe_status = STATUS_IDLE;
  	zw_senddata_status = STATUS_REMOVENODENOW;
    printf("remove node found, status %d\n", cmd_exe_status);
  }
  else if (((*learnNodeInfo).bStatus == REMOVE_NODE_STATUS_REMOVING_SLAVE)||
           ((*learnNodeInfo).bStatus == REMOVE_NODE_STATUS_REMOVING_CONTROLLER))
  {
  	cmd_exe_status = STATUS_IDLE;
  	zw_senddata_status = STATUS_REMOVENODENOW;
    printf("removing node now, status %d\n", cmd_exe_status);
  }
  else if (learnNodeInfo->bStatus == REMOVE_NODE_STATUS_FAILED)
  {
  	ZW_RemoveNodeFromNetwork(&cmd, REMOVE_NODE_STOP, NULL);    
  	cmd_exe_status = STATUS_IDLE;
  	zw_senddata_status = STATUS_REMOVENODEFAIL;
  	removetimer_flag = 0;
		removetimer = 0;
		
    printf("removed node fail, status %d\n", cmd_exe_status);
  }	 
  else if (learnNodeInfo->bStatus == REMOVE_NODE_STATUS_DONE)
  {
  	ZW_RemoveNodeFromNetwork(&cmd, REMOVE_NODE_STOP, NULL);
    cmd_exe_status = STATUS_IDLE;
    zw_senddata_status = STATUS_REMOVENODEDONE;
    removetimer_flag = 0;
		removetimer = 0;
		
    printf("removed node done, status %d\n", cmd_exe_status);
  }
  else if (learnNodeInfo->bStatus == REMOVE_NODE_STATUS_LEARN_READY)
  {
  	cmd_exe_status = STATUS_IDLE;
  	zw_senddata_status = STATUS_REMOVENODEREADY;
    printf("removed node ready, status %d\n", cmd_exe_status);
  }	
  
  flag_remove_n_add = 1;	
  //if(cbFuncAddNode != NULL)  	
    //cbFuncAddNode(zw_senddata_status, &addremovenode_info); 
}


/*=========================   LearnModeCompleted_org =======================
**    Function description
**        Callback function used when adding devices to the controller
**          bStatus BYTE,     IN  Status of learn process
**          bSource BYTE,     IN  Node id of the node that send node info
**          BYTE* pCmd,       IN  Pointer to Node information
**          BYTE bLen));      IN  Node info length
**
**    Side effects: Shuts down RF receive mode when completed
**--------------------------------------------------------------------------*/
void LearnModeCompleted_org(LEARN_INFO *learnNodeInfo)
{
	SCMD cmd;
	
  if (learnNodeInfo->bStatus == ADD_NODE_STATUS_DONE)
  {    
    ZW_AddNodeToNetwork(&cmd, ADD_NODE_STOP, NULL);	//We have added a new device to the controller
    cmd_exe_status = STATUS_IDLE;
    printf("add node done, status %d\n", cmd_exe_status);
  }
  else if(learnNodeInfo->bStatus == ADD_NODE_STATUS_NODE_FOUND)
  { 
    printf("add node found\n");
  }	
  else if(learnNodeInfo->bStatus == ADD_NODE_STATUS_PROTOCOL_DONE)
  {
    //ZW_AddNodeToNetwork(&cmd, ADD_NODE_STATUS_NODE_FOUND|ADD_NODE_STATUS_ADDING_SLAVE, NULL);
    ZW_AddNodeToNetwork(&cmd, ADD_NODE_STOP, NULL);  
    cmd_exe_status = STATUS_IDLE;
    printf("add node protocol done, status %d\n", cmd_exe_status);
  }	
  else if (learnNodeInfo->bStatus == ADD_NODE_STATUS_FAILED)
  {
    //We failed to add a new device to the controller
    ZW_AddNodeToNetwork(&cmd, ADD_NODE_STOP, NULL);
    printf("add node failed, try again\n");
    ZW_AddNodeToNetwork(&cmd, ADD_NODE_ANY, LearnModeCompleted);	
  }
  else if (learnNodeInfo->bStatus == ADD_NODE_STATUS_LEARN_READY)
  {
    printf("add node learn ready\n");
  }	
  else
  {    
    if ((learnNodeInfo->bStatus == ADD_NODE_STATUS_ADDING_CONTROLLER) ||
        (learnNodeInfo->bStatus == ADD_NODE_STATUS_ADDING_SLAVE))
    { 
      //printf("adding node now\n");
    }
    //bMainState = STATE_EXECUTING;
  }
}


/*=========================   LearnModeCompleted =======================
**    Function description
**        Callback function used when adding devices to the controller
**          bStatus BYTE,     IN  Status of learn process
**          bSource BYTE,     IN  Node id of the node that send node info
**          BYTE* pCmd,       IN  Pointer to Node information
**          BYTE bLen));      IN  Node info length
**
**    Side effects: Shuts down RF receive mode when completed
**--------------------------------------------------------------------------*/
void LearnModeCompleted(LEARN_INFO *learnNodeInfo)
{
	SCMD cmd;
	
  if (learnNodeInfo->bStatus == ADD_NODE_STATUS_DONE)
  { 
  	ZW_AddNodeToNetwork(&cmd, ADD_NODE_STOP, NULL);   
    cmd_exe_status = STATUS_IDLE;
    zw_senddata_status = STATUS_ADDNODEDONE;
    addtimer_flag = 0;
		addtimer = 0;	
    printf("add node done, status %d\n", cmd_exe_status);
  }
  else if(learnNodeInfo->bStatus == ADD_NODE_STATUS_NODE_FOUND)
  {
  	cmd_exe_status = STATUS_IDLE;
  	zw_senddata_status = STATUS_ADDNODENOW; 
    printf("add node found, status %d\n", cmd_exe_status);
  }	
  else if(learnNodeInfo->bStatus == ADD_NODE_STATUS_PROTOCOL_DONE)
  {
  	ZW_AddNodeToNetwork(&cmd, ADD_NODE_STOP, NULL);
    cmd_exe_status = STATUS_IDLE;
    zw_senddata_status = STATUS_ADDNODEDONE;       
    addtimer_flag = 0;
		addtimer = 0;	
    printf("add node protocol done, status %d\n", cmd_exe_status);
  }	
  else if (learnNodeInfo->bStatus == ADD_NODE_STATUS_FAILED)	//We failed to add a new device to the controller
  { 
  	ZW_AddNodeToNetwork(&cmd, ADD_NODE_STOP, NULL);   
    cmd_exe_status = STATUS_IDLE;
    zw_senddata_status = STATUS_ADDNODEFAIL;
    addtimer_flag = 0;
		addtimer = 0; 
		   
    printf("add node failed, status %d\n", cmd_exe_status);
  }
  else if (learnNodeInfo->bStatus == ADD_NODE_STATUS_LEARN_READY)
  {
    cmd_exe_status = STATUS_IDLE;
    zw_senddata_status = STATUS_ADDNODEREADY;
    printf("add node ready, status %d\n", cmd_exe_status);
  }	
  else
  {    
    if ((learnNodeInfo->bStatus == ADD_NODE_STATUS_ADDING_CONTROLLER) ||
        (learnNodeInfo->bStatus == ADD_NODE_STATUS_ADDING_SLAVE))
    { 
    	cmd_exe_status = STATUS_IDLE;
    	zw_senddata_status = STATUS_ADDNODENOW;
      printf("adding node now, status %d\n", cmd_exe_status);
    }
  }
	
  flag_remove_n_add = 1;
  //if(cbFuncAddNode != NULL)		
    //cbFuncAddNode(zw_senddata_status, &addremovenode_info);  	
}


/**********************************************************/
//  * \ingroup SerialAPI
//  * Execute a callback based on the received frame.
//  * NOTE: Must only be called locally in the Serial API.
//  * \param[in] pData    Pointer to data frame (without SOF)
//  * \param[in] byLen    Length of data frame
/**********************************************************/
void Dispatch(DONGLE_INFO *p, BYTE *pData )
{
  int i;
  //BYTE device;

  i=0;
  if (pData[1]==1) 	//pData[0]:length, pData[1]:request(0)/response(1)
  	return;

  switch ( pData[ IDX_CMD ] )	// command
  {
    case FUNC_ID_ZW_APPLICATION_CONTROLLER_UPDATE:
    	{
    	  printf("cb_controller_update \n");      
    		
//    		get_status_flag = STATUS_IDLE;
//				get_status_flag_ctr = 0;
				
    	  if (pData[ IDX_DATA + 2 ]!=0x0)	// We get REQ data(len =\= 0)
    	  {
    	  	printf("Node: %d, ", pData[ IDX_DATA + 1 ]);
    	    for ( i = 0;i < pData[ IDX_DATA + 2 ];i++ )
    	 		{
    	 			if(i==0)
							printf("device class: ");     
						
						pCmd[ i ] = pData[ IDX_DATA + 3 + i ];
						printf("%x ", pCmd[i]);
						
						if(i==2)
							printf(",cmd class: ");
					}				
    			printf("get end\n");
    			
    			nodeInfo.nodeType.basic = pCmd[0];
  				nodeInfo.nodeType.generic = pCmd[1];
  				nodeInfo.nodeType.specific = pCmd[2];
  
          device_type = Analysis_DeviceType(&nodeInfo);	// Get device type
          printf("Device type %d\n", device_type);
          
    	    if ( funcApplicationControllerUpdate )
    	    {
    	      learnNodeInfo.bStatus = pData[ IDX_DATA ];
    	      learnNodeInfo.bSource = pData[ IDX_DATA + 1 ];
    	      learnNodeInfo.pCmd = pCmd;
    	      learnNodeInfo.bLen = pData[ IDX_DATA + 2 ];
    	      funcApplicationControllerUpdate(&device_info, learnNodeInfo.bStatus,
    	                                      learnNodeInfo.bSource,
    	                                      learnNodeInfo.pCmd,
    	                                      learnNodeInfo.bLen, device_type);    	                                          	                                    
    	   	}
    	   	else
						printf("cb controller update null\n");
    	  }
    	  else
    	  {
    	  	cmd_exe_status = STATUS_IDLE;		// Not get node information condition, re-ask again
  				printf("update fail, state %d\n", cmd_exe_status);
    	  }	      
    	}      
    break;

    case FUNC_ID_APPLICATION_COMMAND_HANDLER:    	    	
      {
      	printf("cb_cmd_handler \n");                
				
				for ( i = 0;i < pData[ IDX_DATA + 2 ];i++ )
        {
          pCmd[ i ] = pData[ IDX_DATA + 3 + i ];
          printf("%x ", pCmd[ i ]);
        }        
				printf("get end\n");								
				
//				get_status_flag_ctr = 0;
//				get_status_flag = STATUS_IDLE;
				
				if(funcApplicationCommandHandler != NULL)
				{
					if(pData[ IDX_DATA ] == 0x01)
						printf("route busy\n");
					else						
        		funcApplicationCommandHandler( &device_info, pData[ IDX_DATA ], pData[ IDX_DATA + 1 ], (ZW_APPLICATION_TX_BUFFER*)pCmd, pData[ IDX_DATA + 2 ] );
				}
				else
					printf("cb cmdhandler null\n");
				
				zw_senddata_status = STATUS_OK;	
      }      
    break;

    // The rest are callback functions
    case FUNC_ID_ZW_SEND_NODE_INFORMATION:
    	printf("cb_node_inform \n");
    	      
    break;

    case FUNC_ID_ZW_SEND_DATA:    	  	
      {
      	printf("cb_send_data \n"); 
				
				flag_prevnet_lock = 0;
  			prevent_lock_ctr = 0;
  			
        if ( cbFuncZWSendData != NULL )
        {
          	cbFuncZWSendData( pData[ IDX_DATA + 1 ] );
        } 
      }      
    break;

    case FUNC_ID_ZW_SEND_DATA_MULTI:
    	printf("cb_send_data_multi \n");
    	
      if ( cbFuncZWSendDataMulti != NULL )
      {
        cbFuncZWSendDataMulti( pData[ IDX_DATA + 1 ] );
      }            
    break;

    case FUNC_ID_MEMORY_PUT_BUFFER:
    	printf("cb_mem_put_buf \n");
      
    break;

    case FUNC_ID_ZW_SET_DEFAULT:
    	printf("cb_zw_set_default \n");
    	
      if ( cbFuncZWSetDefault != NULL )
      {
        cbFuncZWSetDefault();
      }
      
    break;

    case FUNC_ID_ZW_CONTROLLER_CHANGE:
    case FUNC_ID_ZW_CREATE_NEW_PRIMARY:
    case FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK:
      if ( cbFuncZWNewController != NULL )
      {
      	//printf("cb_remove_node status:%x\n", pData[ IDX_DATA + 1 ]);      	
      	// slave or controller node has been removed from the network 
        if ((pData[ IDX_DATA + 1 ]==0x03) || (pData[ IDX_DATA + 1 ]==0x04) || (pData[ IDX_DATA + 1 ]==0x06))
        {
	  			printf("Rmv get:");
          for ( i = 0;i < pData[ IDX_DATA + 3 ];i++ )
          {
              pCmd[ i ] = pData[ IDX_DATA + 4 + i ];
              printf("%x ", pCmd[ i ]);
          }
          printf("end\n");
          
          learnNodeInfo.bSource = pData[ IDX_DATA + 2 ];
          learnNodeInfo.pCmd = pCmd;
          learnNodeInfo.bLen = pData[ IDX_DATA + 3 ];                  
        }
        
        learnNodeInfo.bStatus = pData[ IDX_DATA + 1 ];
				
				//printf("bStatus:%x, RemoveNodeID:%d\n", learnNodeInfo.bStatus, learnNodeInfo.bSource);        
        if(learnNodeInfo.bStatus == REMOVE_NODE_STATUS_DONE)
        {
        	nodeInfo.nodeType.basic = pCmd[0];
  				nodeInfo.nodeType.generic = pCmd[1];
  				nodeInfo.nodeType.specific = pCmd[2];
  
          device_type = Analysis_DeviceType(&nodeInfo);
          
        	p->node_info_table[pData[ IDX_DATA + 2 ]].id = pData[ IDX_DATA + 2 ];
					p->node_info_table[pData[ IDX_DATA + 2 ]].type = device_type;
					p->device_bit_mask[(pData[ IDX_DATA + 2 ]-1)>>3] &= ~(0x1 << ((pData[ IDX_DATA + 2 ]-1) & 0x7)); 					
        	
        	if(device_type == DEVICE_IR_CONTROLLER)
					{	
						addremovenode_info.dev_num = MULTI_CHANNEL_NUM;
						for(i=0;i<MULTI_CHANNEL_NUM;i++)
						{
							addremovenode_info.id[i] = ((i+1)<<8)| pData[ IDX_DATA + 2 ];
							if(i==0 || i==4)
								addremovenode_info.type[i] = DEVICE_TV; 
							else if(i==1 || i==5)
								addremovenode_info.type[i] = DEVICE_DVD;
							else if(i==2 || i==6)
								addremovenode_info.type[i] = DEVICE_SAT;
							else if(i==3)
								addremovenode_info.type[i] = DEVICE_AUX;
							else if(i==7)
								addremovenode_info.type[i] = DEVICE_ACCONDITION;
										       			
        			printf("**rmvID:%x, Type:%x**\n", addremovenode_info.id[i], addremovenode_info.type[i]);
        		}	
        	}
        	else
        	{
        		addremovenode_info.dev_num = 1;
        		addremovenode_info.id[0] = pData[ IDX_DATA + 2 ];
        		addremovenode_info.type[0] = device_type;
        		printf("**rmvID:%d, Type:%d**\n", addremovenode_info.id[0], addremovenode_info.type[0]);
        	}	
        	       	
        	//printf("**rmvID:%d, Type:%d**\n\n", p->node_info_table[pData[ IDX_DATA + 2 ]].id, p->node_info_table[pData[ IDX_DATA + 2 ]].type);
        }	
        
        cbFuncZWNewController( &learnNodeInfo );        
      }
      else
				printf("cb_remove_node null\n");	
    break;
    
    case FUNC_ID_ZW_ADD_NODE_TO_NETWORK:
      if ( cbFuncZWNewController != NULL )
      {
      	//printf("cb_add_node status:%x\n", pData[ IDX_DATA + 1 ]);      	
      	// slave or controller node has been added to the network 
        if ((pData[ IDX_DATA + 1 ]==0x03) || (pData[ IDX_DATA + 1 ]==0x04) || (pData[ IDX_DATA + 1 ]==0x05))
        {
          printf("Add get:");
          for ( i = 0;i < pData[ IDX_DATA + 3 ];i++ )
          {
              pCmd[ i ] = pData[ IDX_DATA + 4 + i ];
              printf("%x.", pCmd[ i ]);
          }
          printf("end\n");
          
          learnNodeInfo.bSource = pData[ IDX_DATA + 2 ];
          learnNodeInfo.pCmd = pCmd;
          learnNodeInfo.bLen = pData[ IDX_DATA + 3 ];                    										
        }
        
        learnNodeInfo.bStatus = pData[ IDX_DATA + 1 ];
				
				//printf("bStatus:%x, AddNodeID:%d\n", learnNodeInfo.bStatus, learnNodeInfo.bSource);
				
				if(learnNodeInfo.bStatus == ADD_NODE_STATUS_PROTOCOL_DONE)
				{
					printf("Get added NodeID:%d\n", pData[ IDX_DATA + 2 ]);
					nodeInfo.nodeType.basic = pCmd[0];
  				nodeInfo.nodeType.generic = pCmd[1];
  				nodeInfo.nodeType.specific = pCmd[2];
  
          device_type = Analysis_DeviceType(&nodeInfo);	// Get device type
					p->node_info_table[pData[ IDX_DATA + 2 ]].id = pData[ IDX_DATA + 2 ];
					p->node_info_table[pData[ IDX_DATA + 2 ]].type = device_type;
					
					if(device_type == DEVICE_IR_CONTROLLER)
					{	
						addremovenode_info.dev_num = MULTI_CHANNEL_NUM;
						for(i=0;i<MULTI_CHANNEL_NUM;i++)
						{
							addremovenode_info.id[i] = ((i+1)<<8)| pData[ IDX_DATA + 2 ];
							if(i==0 || i==4)
								addremovenode_info.type[i] = DEVICE_TV; 
							else if(i==1 || i==5)
								addremovenode_info.type[i] = DEVICE_DVD;
							else if(i==2 || i==6)
								addremovenode_info.type[i] = DEVICE_SAT;
							else if(i==3)
								addremovenode_info.type[i] = DEVICE_AUX;
							else if(i==7)
								addremovenode_info.type[i] = DEVICE_ACCONDITION;
										       			
        			printf("**ID:%x, Type:%x**\n", addremovenode_info.id[i], addremovenode_info.type[i]);
        		}	
        	}
        	else
        	{
        		addremovenode_info.dev_num = 1;
        		addremovenode_info.id[0] = pData[ IDX_DATA + 2 ];
        		addremovenode_info.type[0] = device_type;
        		printf("**ID:%d, Type:%d**\n", addremovenode_info.id[0], addremovenode_info.type[0]);
        	}	
					//printf("**ID:%d, Type:%d**\n\n", p->node_info_table[pData[ IDX_DATA + 2 ]].id, p->node_info_table[pData[ IDX_DATA + 2 ]].type);
				}
					
        cbFuncZWNewController( &learnNodeInfo );
      }
      else
				printf("cb_add_node null\n");
    break;

    case FUNC_ID_ZW_REPLICATION_SEND_DATA:
    	printf("cb_replication_send_data\n");
/*    	
      if ( cbFuncZWReplicationSendData != NULL )
      {
        cbFuncZWReplicationSendData( pData[ IDX_DATA + 1 ] );
      }
*/      
    break;

    case FUNC_ID_ZW_ASSIGN_RETURN_ROUTE:
    	printf("cb_assign_return_route\n");
/*    	
      if ( cbFuncZWAssignReturnRoute != NULL )
      {
        cbFuncZWAssignReturnRoute( pData[ IDX_DATA + 1 ] );
      }
*/      
    break;

    case FUNC_ID_ZW_DELETE_RETURN_ROUTE:
    	printf("cb_delete_return_route \n");
/*    	
      if ( cbFuncZWDeleteReturnRoute != NULL )
      {
        cbFuncZWDeleteReturnRoute( pData[ IDX_DATA + 1 ] );
      }
*/      
    break;

    case FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE:
    	printf("cb_assign_SUC_return_route \n");
/*    	
      if ( cbFuncZWAssignSUCReturnRoute != NULL )
      {
        cbFuncZWAssignSUCReturnRoute( pData[ IDX_DATA + 1 ] );
      }
*/      
    break;

    case FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE:
    	printf("cb_delete_SUC_return_route \n");
/*    	
      if ( cbFuncZWDeleteSUCReturnRoute != NULL )
      {
        cbFuncZWDeleteSUCReturnRoute( pData[ IDX_DATA + 1 ] );
      }
*/      
    break;

    case FUNC_ID_ZW_SET_LEARN_MODE:
    	printf("cb_set_learn_mode \n");
/*    	
      if ( cbFuncZWSetLearnMode != NULL )
      {
        learnNodeInfo.bStatus = pData[ IDX_DATA + 1 ];
        learnNodeInfo.bSource = pData[ IDX_DATA + 2 ];

        for ( i = 0;i < pData[ IDX_DATA + 3 ];i++ )
        {
            pCmd[ i ] = pData[ IDX_DATA + 4 + i ];
        }
//        learnNodeInfo.bSource = pData[ IDX_DATA + 2 ];
        learnNodeInfo.pCmd = pCmd;
        learnNodeInfo.bLen = pData[ IDX_DATA + 3 ];

//        learnNodeInfo.bStatus = pData[ IDX_DATA + 1 ];

        cbFuncZWSetLearnMode( &learnNodeInfo );//learnNodeInfo.bStatus, learnNodeInfo.bSource
      }
*/      
    break;

    case FUNC_ID_ZW_SET_SUC_NODE_ID:
    	printf("cb_set_SUC_node_ID \n");
/*    	
      if ( cbFuncZWSetSUCNodeID != NULL )
      {
        cbFuncZWSetSUCNodeID( pData[ IDX_DATA + 1 ] );
      }
*/      
    break;

    case FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE:
    	printf("cb_node_neighber_update \n");
/*    	
      if ( cbFuncZWRequestNodeNodeNeighborUpdate != NULL )
      {
        cbFuncZWRequestNodeNodeNeighborUpdate( pData[ IDX_DATA + 1 ] );
      }
*/      
    break;

    case FUNC_ID_ZW_REQUEST_NETWORK_UPDATE:
    	printf("cb_network_update \n");
/*    	
      if ( cbFuncZWRequestNetworkUpdate != NULL )
      {
        cbFuncZWRequestNetworkUpdate( pData[ IDX_DATA + 1 ] );
      }
*/
    break;
    
    case FUNC_ID_ZW_REMOVE_FAILED_NODE_ID:
    	printf("cb_remove_fail_node_ID \n");
    	
//    	get_status_flag = STATUS_IDLE;
//			get_status_flag_ctr = 0;
				
    	if ( cbFuncZWRemoveFailNodeID != NULL )
      {
        cbFuncZWRemoveFailNodeID( pData[ IDX_DATA + 1 ] );
      }
    break;
    
    default:
			printf("Unused mode\n");
    break;
  }
}


/****************************    SerialAPI_Poll    **************************/
/**
  * \ingroup SerialAPI
  * Evaluates \ref result and calls \ref ReceiveData.
  * Must be called regularly.
  */
/****************************************************************************/
void SerialAPI_Poll( void )
{
  switch ( zw_result )
  {
    case STATUS_FRAMERECEIVED:
      Dispatch(&device_info, RecvBuffer);
    break;

    case STATUS_FRAMESENT:
    break;

    case STATUS_FRAMEERROR:
    break;

    default:
    break;
  }
	
	//if(waitForAck == FALSE && bWaitForResponseRunning == FALSE)
  //if(waitForAck == FALSE && bWaitForResponseRunning == FALSE && bReceiveDataRunning == FALSE)
  if(waitForAck == FALSE && bWaitForResponseRunning == FALSE && initial_flag == 0)
  	zw_result = ReceiveData(RecvBuffer);	
}


BOOL API_SerialAPI_InitCBFunction(void ( *funcGetNIF )(WORD, BYTE), void ( *funcIRSensorStatus )(WORD , BYTE), void ( *funcGetWUN)(WORD))
{
  cbFuncAddNode = NULL;	// Haas add
  
  cbFuncGetNIF = funcGetNIF; // add for AP get alarm status
  cbFuncGetIRSensorStatus = funcIRSensorStatus;
  cbFuncGetWUN = funcGetWUN;
  
  return TRUE;
}


/****************************************************************************/
//  * \ingroup SerialAPI
//  * Initialize callbacks and transmission variables.
//  * \see SACB waitForAck waitForResponseTimeOut receiveDataTimeOut
//  * \param[in] funcApplCmdHandler   Pointer to callback for ApplicationCommandHandler
//  * \param[in] funcAppConUpdate     Pointer to callback for ApplicationControllerUpdate
//  * \return = TRUE if initialization was succesful
/****************************************************************************/
BOOL SerialAPI_InitSW(void ( *funcApplCmdHandler ) (BYTE, BYTE, ZW_APPLICATION_TX_BUFFER *, BYTE), void ( *funcAppConUpdate ) (BYTE, BYTE, BYTE*, BYTE, BYTE ))
{
  cbFuncZWSendData = NULL;
  cbFuncZWSendDataMulti = NULL;
  cbFuncZWSendNodeInformation = NULL;
  cbFuncMemoryPutBuffer = NULL;
  cbFuncZWSetDefault = NULL;
  cbFuncZWNewController = NULL;
  cbFuncZWReplicationSendData = NULL;
  cbFuncZWAssignReturnRoute = NULL;
  cbFuncZWAssignSUCReturnRoute = NULL;
  cbFuncZWDeleteSUCReturnRoute = NULL;
  cbFuncZWDeleteReturnRoute = NULL;
  cbFuncZWSetLearnMode = NULL;
  cbFuncZWRequestNodeNodeNeighborUpdate = NULL;
  cbFuncZWSetSUCNodeID = NULL;
  cbFuncZWRequestNetworkUpdate = NULL;
	cbFuncZWRemoveFailNodeID = NULL;	// Haas added
	
  waitForAck = FALSE;
  receiveDataTimeOut = FALSE;
  waitForResponseTimeOut = FALSE;

  funcApplicationCommandHandler = funcApplCmdHandler;
  funcApplicationControllerUpdate = funcAppConUpdate;

  //printf("ApplCmdHandler add:%x", funcApplicationCommandHandler);
  cbFuncAddNode = NULL;	// Haas add
  
  //cbFuncGetNIF = funcGetNIF; // add for AP get alarm status
  //cbFuncGetIRSensorStatus = funcIRSensorStatus;
  //cbFuncGetWUN = funcGetWUN;
  
  return TRUE;
}


/**********************************************************/
//  * \ingroup SerialAPI
//  * Callback for \ref WaitForResponse.
/**********************************************************/
void WaitForResponseIndicateTimeOut( void )
{
  waitForResponseTimeOut = TRUE;
}


/**********************************************************/
//  * \ingroup SerialAPI
//  * Wait for response.
//  * NOTE: Must only be called locally in the Serial API.
//  * \see SendDataAndWaitForResponse
//  * \param[in] byFunc   Function number expected in response.
//  * \param[out] out     Buffer to store response in.
//  * \param[out] byLen   Length of response.
/**********************************************************/
void WaitForResponse( BYTE byFunc, BYTE *buffer, BYTE *byLen )
{
  BYTE bFound;
  BYTE retry;
  static BYTE byTimerHandle;

  bFound = FALSE;
  bWaitForResponseRunning = TRUE;		// now is waiting for response

  waitForResponseTimeOut = FALSE;		// now is just start to waiting for response
  retry = MAX_SERIAL_RETRY - 1;			// retry 2 times
	
  // This timer is waiting for Response(maybe time out, 250 times)
  byTimerHandle = TimerStart( WaitForResponseIndicateTimeOut, TIMEOUT_TIME, TIMER_FOREVER );
  //printf("WaitRES Tstart%d\n", byTimerHandle);

  //printf("WaitForResponse1\n");
	
  while ( byTimerHandle != INVALID_TIMER_HANDLE )
  {	
    TimerAction();
  		
    zw_result = ReceiveData(RecvBuffer );

    if ( zw_result == STATUS_FRAMERECEIVED )	// We received data
    {
      if ( waitForResponseTimeOut == FALSE && ( RecvBuffer[ 1 ] == RESPONSE ) && ( RecvBuffer[ 2 ] == byFunc ) )	// we get response and data
      {
          bFound = TRUE;	// we found data frame
          TimerCancel(byTimerHandle);
          //printf("WaitRES Tcancel%d\n", byTimerHandle);

          byTimerHandle = INVALID_TIMER_HANDLE;
          //printf("We get response\n");	
      }
      else
      {
          waitForResponseTimeOut = TRUE;
      }
    }
    else	// We don't received data
    {
      if ( waitForResponseTimeOut == TRUE )
      {
          waitForResponseTimeOut = FALSE;
          if ( retry != 0 )
          {
            printf("We retry\n");	
            retry--;
            if(retry>MAX_SERIAL_RETRY)	// stop retry
            {
              bFound = TRUE;
              TimerCancel(byTimerHandle);
              byTimerHandle = INVALID_TIMER_HANDLE;
            }
            else
              SendData(& TransmitBuffer[ 1 ], TransmitBufferLength );	// send frame again
          }
          else	// stop retry
          {
              TimerCancel(byTimerHandle);
              byTimerHandle = INVALID_TIMER_HANDLE;
          }
      }

      if(retry>MAX_SERIAL_RETRY)
      {
        bFound = TRUE;
        TimerCancel(byTimerHandle);
        byTimerHandle = INVALID_TIMER_HANDLE;
        
        zw_senddata_status = STATUS_NORESPONSE;	// set status to no response
        cmd_exe_status = STATUS_IDLE;	// set status back to idle, prevent lock in processing status
        
        printf("retry timeout\n");
      }
    }
  }

  //printf("WaitForResponse2\n");
	
  TimerCancel(byTimerHandle);
  byTimerHandle = INVALID_TIMER_HANDLE;

  if ( !bFound )
  {
    if ( byLen != NULL )
      * byLen = 0;
  }
  else
  {
    if ( buffer != NULL )
    {
      memcpy( buffer, RecvBuffer, RecvBuffer[ 0 ] );
    }

    if ( byLen != NULL )
    {
      *byLen = RecvBuffer[ 0 ];
    }
  }

  bWaitForResponseRunning = FALSE;
}


/**********************************************************/
// Function Name : bySerialGetByte()                                                
// Discription : 
//               	
// Input	:               
// Output	:                                
/**********************************************************/
BYTE bySerialGetByte( void )
{
  BYTE retVal;
  
  retVal = rxQueue[ rxOutPtr ];

  if ( rxCount > 0 )     // Make sure...
  {
      rxCount--;
  }

  if ( ++rxOutPtr >= UART_RX_BUFSIZE )
  {
      rxOutPtr = 0;
  }
  
  return retVal;
}


/**********************************************************/
// Function Name : bySerialCheck()                                                
// Discription : get the rxCount value
//               	
// Input	:                                
// Output	:                                
/**********************************************************/
BYTE bySerialCheck( void )
{
  return rxCount;
}


/**********************************************************/
//  * \ingroup SerialAPI
//  * Collects acknowledgments and frames from the serial hardware.
//  * NOTE: Must only be called locally in the Serial API.
//  * \param[in] buffer   Buffer in which to store received frame
//  * \return \see SAUSC
/**********************************************************/
int ReceiveData( BYTE *buffer )
{
  int i;
  BYTE ch;
  BYTE bLen;
  char send_string[1]={0};
	
  i = 0;
  ch = 0;
  bLen = 255;
  
  if ( bReceiveDataRunning == TRUE )
  {
  	printf("ReceiveData called multiple times!\n");
  	return STATUS_FRAMEERROR;	// 1000211, add
  }
	
  //	printf("ReceivedData1\n");
	
  bReceiveDataRunning = TRUE;
  zw_result = 0;
  receiveDataTimeOut = FALSE;
	
  // Endless loop, just like while loop, waiting for "SOF" to break out loop
  for ( ;ch != SOF; )	
  {	
  	TimerAction();
  		
    // this part is for get ACK, the "WaitForAck" is the counter.
    if ( bySerialCheck() == 0 )	// rxCount is 0, no data, we are waiting for data...
    {
      bReceiveDataRunning = FALSE;
      
      //	printf("ReceivedDataTimeOut1\n");	//if open, keep print 
      
      return STATUS_RXTIMEOUT;	// now we don't have data, so return
    }

    ch = bySerialGetByte();			// get one byte from queue

#ifndef DEBUG_USB_RX
		printf("%x,", ch);
#endif
		
    if( waitForAck == TRUE )	// right now is waiting for ACK
    {
    	//printf("waitforack_true %x\n", ch);
      //waitForAck = FALSE;	// 1000211, remove

      if ( ch == ACK )	// Got ACK, senting process is finished
      {
        bReceiveDataRunning = FALSE;
        waitForAck = FALSE;	// 1000211, add
        // If frame is sent, then we will go next to waitforresponse status
        // And waitforresponse status will call ReceivedData again
        // But now we are not waiting for ACK
        //printf("Frame sent\n");
        	
        return STATUS_FRAMESENT;
      }

      if ( ch == NAK )	// Got NAK, senting process is error 
      {
        bReceiveDataRunning = FALSE;
        waitForAck = FALSE;	// 1000211, add
        
        printf("Frame error\n");
        
        return STATUS_FRAMEERROR;
      }

      bReceiveDataRunning = FALSE;
      
      return STATUS_FRAMEERROR;
    }

    if ( ch == SOF )	// Get SOF, break this loop, go to next for loop
    {
    	//	printf("Get SOF\n"); 
    	 	
      break;
    }
		
    bReceiveDataRunning = FALSE;

    return STATUS_FRAMEERROR;
  }
	
	//	printf("ReceivedData2\n");
	
  for (i = 0; i < (WORD)bLen + 1; i++)
  {
    timerReceiveTimer = 100;
    
    for ( ;!bySerialCheck(); )	// We have nothing in rxQueue buffer, so we wait
    {
    	TimerAction();
    				
      if ( receiveDataTimeOut == TRUE )
      {
          bReceiveDataRunning = FALSE;
          printf("Rx Data TimeOut 2\n");
          
          return STATUS_RXTIMEOUT;
      }
    }
    
    // We have frame in rxQueue buffer, and we get it out, store it into buffer.
    if ( i == 0 )
    {
      bLen = buffer[ i ] = bySerialGetByte(); // bLen is length of frame WITHOUT SOF and Cksum
      printf("%x,", buffer[ i ]);
    }
    else
    {
      buffer[ i ] = bySerialGetByte();
      printf("%x,", buffer[ i ]);
    }
  }
  
  //	printf("ReceivedData3\n");
  
  bReceiveDataRunning = FALSE;

  if (0 != CalculateChecksum(buffer, i))
  {
//    vSerialPutByte( NAK );	// Tell the sender something went wrong
	send_string[0] = NAK;
	writecom(send_string, 0x01);
		
    bReceiveDataRunning = FALSE;
    printf("CKsum error\n");
    
    return STATUS_FRAMEERROR;
  } 
  
  printf(" RxOK\n");
  
  buffer[ i - 1 ] = 0;
//  vSerialPutByte( ACK );	// Tell the world we are happy...
  send_string[0] = ACK;
  writecom(send_string, 0x01);
		
  bReceiveDataRunning = FALSE;

  return STATUS_FRAMERECEIVED;
}


/**********************************************************/
// Function Name : ReceiveAckIndicateTimeOut()                                                
// Discription :  
//               	
// Input	:                                                                                        
// Output	:                                
/**********************************************************/
void ReceiveAckIndicateTimeOut( void )
{
  receiveAskTimeOut = TRUE;
}


/*================================   TimerInit   =============================
**    Initialize Timers.
**
**--------------------------------------------------------------------------*/
void TimerInit(void)
{
  BYTE timerHandle;
  
  for (timerHandle = 0; timerHandle < TIMER_MAX; timerHandle++)
  {
    // Free timer element
    timerArray[timerHandle].tickCounts = 0;
    //timerArray[timerHandle].repeats = 0;
  	//timerArray[timerHandle].func = NULL;
  	//timerArray[timerHandle].enable = 0;
  }
}


/*================================   TimerAction   ===========================
**    Walk through the timer elements and calls the timeout functions.
**
**--------------------------------------------------------------------------*/
void TimerAction( void )       
{
  BYTE timerHandle;
  
  while(timerCount)
  {
    timerCount--;
    
    // walk through the timer array 
    for (timerHandle = 0; timerHandle < TIMER_MAX; timerHandle++)	
    {
      if (timerArray[timerHandle].tickCounts)	// check the started timer
      {
        if (--timerArray[timerHandle].tickCounts == 0) // count change from 1 to 0
        { 
           //printf("tickCount%d\n", timerCount);

          if (timerArray[timerHandle].repeats != 0)
          { // repeat timer action 
            if (timerArray[timerHandle].repeats != TIMER_FOREVER)
            { // repeat limit 
              timerArray[timerHandle].repeats--;
            }
          }
          
          if (timerArray[timerHandle].repeats)	// re-count
          {
            timerArray[timerHandle].tickCounts = timerArray[timerHandle].timeoutValue;
          }
          
          // call the timeout function 
          if(&timerArray[timerHandle].func != NULL) 
          	timerArray[timerHandle].func();
        }
      }
    }
  }
}


/*============================   TimerSave   =========================
**    Save timer specifications and start the timer
**
**    Side effects:
**	btimerHandle 		 // IN Index into the timer table
**  VOID_CALLBACKFUNC(func)()  // IN Timeout function adddress      
**	BYTE btimerTicks,          // IN Timeout value (value * 10 msec.)
**	BYTE brepeats              // IN Number of function calls, -1 = forever
**
**--------------------------------------------------------------------------*/
static void TimerSave(BYTE btimerHandle, VOID_CALLBACKFUNC(func)(), BYTE btimerTicks, BYTE brepeats)              
{
  register BYTE tmp;

  tmp = btimerTicks;
  if (!tmp)
  {
    tmp++; // at least 10 msec.
  }
  
  // create new active timer element(sett the timer : timeout value, repeat times, functions)
  timerArray[btimerHandle].tickCounts = timerArray[btimerHandle].timeoutValue = tmp;
  timerArray[btimerHandle].repeats = brepeats;
  timerArray[btimerHandle].func = func;
  //timerArray[btimerHandle].eanble = 1;
  
  //printf("Timer%d start\n", btimerHandle);
}


/*================================   TimerStart   ============================
**    Register a function that is called when the specified time has elapsed.
**    The function is called "repeats" times before the timer is then stopped.
**
**	RET Timer handle 
**	VOID_CALLBACKFUNC(func)(),  // IN Timeout function adddress         
**  BYTE btimerTicks,            //IN  Timeout value (value * 10 msec.)  
**  BYTE brepeats)               //IN  Number of function calls, -1 = forever  
**
**--------------------------------------------------------------------------*/
BYTE TimerStart(VOID_CALLBACKFUNC(func)(), BYTE btimerTicks, BYTE brepeats)              
{
  BYTE retCode;
  BYTE timerHandle;

  retCode = (BYTE)-1;
  for (timerHandle = 0; timerHandle < TIMER_MAX; timerHandle++)	// we have 10 timers
  {  		
    // find first free timer element, and use it
    if (timerArray[timerHandle].tickCounts == 0)
    //if(timerArray[timerHandle].enable == 0)
    {
    	TimerSave(timerHandle, func, btimerTicks, brepeats);
    	retCode = timerHandle + 1;	// Next time we use next no used timer
   	
	//printf("Start Timer:%d\n", timerHandle); 
    	return (retCode);
    }
  }
  
  printf("Timer all used\n");
  return(retCode);
}


/*================================   TimerCancel   ===========================
**    Stop the specified timer.
**      0 and 0xFF indicates no timer running.. This is acceptable
**	//RET TRUE if timer cancelled  
**--------------------------------------------------------------------------*/
BYTE TimerCancel(BYTE btimerHandle)       /* IN Timer number to stop      */
{
  BYTE retCode;

  retCode = FALSE;
  btimerHandle--;  // Index 0...n, release this timer
  
  if (btimerHandle < TIMER_MAX)
  {
    // valid timer element number

    timerArray[btimerHandle].tickCounts = timerArray[btimerHandle].timeoutValue = 0; // stop the timer
    //timerArray[timerHandle].repeats = 0;
  	//timerArray[timerHandle].func = NULL;
  	//timerArray[timerHandle].enable = 0;
  	
    //printf("Cancel Timer:%d\n", btimerHandle);
    retCode = TRUE;
  }
  
  return(retCode);
}


/**********************************************************/
// Function Name : WaitForAck()                                                
// Discription :  
//               	
// Input	:                                                                                        
// Output	:                                
/**********************************************************/
BOOL WaitForAck()
{
  //int i=0;
  static BYTE ReceiveTimerHandle;
  
  //printf("WaitForAck1\n");
  
  receiveAskTimeOut = FALSE;// right now is not time out, just start to wait ACK
  
  // This timer is waiting for ACK(maybe time outl, 200 ticks)
  ReceiveTimerHandle = TimerStart( ReceiveAckIndicateTimeOut, 200, TIMER_FOREVER);	
  //printf("WaitACK Tstart%d\n", ReceiveTimerHandle);

  while( !bySerialCheck() )		// we still have data in buffer 
  { 
	//i++;
	//if(i>) 
  	TimerAction();
  	      
    if ( receiveAskTimeOut == TRUE )	// but time's up
    {
      TimerCancel(ReceiveTimerHandle);	// time's up, so cancel this timer
      ReceiveTimerHandle = INVALID_TIMER_HANDLE;
      
      printf("RX Ack TimeOut\n");
      
      return STATUS_RXTIMEOUT;
    }    
  }
    
  TimerCancel(ReceiveTimerHandle);
  //printf("WaitACK Tcancel%d\n", ReceiveTimerHandle);

  ReceiveTimerHandle = INVALID_TIMER_HANDLE;
  
  //printf("WaitForAck, call ReceivedData\n");
  
  return  ReceiveData(RecvBuffer);
}


/**********************************************************/
//  * \ingroup SerialAPI
//  * Calculate checksum for given buffer.
//  * NOTE: Should only be called locally in the Serial API.
//  * \param[in] pData    Pointer to data frame (without SOF)
//  * \param[in] nLength  Length of data frame
//  * \return Checksum
/**********************************************************/
BYTE CalculateChecksum( BYTE *pData, int nLength )
{
  BYTE byChecksum;
  byChecksum = 0xff;	// checksum start from 0xFF
  
  for ( ; nLength; nLength--)
  {
    byChecksum ^= *pData++;
  }
  
  return byChecksum;
}


/**********************************************************/
// Function Name : TransmitData(BYTE *buffer, BYTE length)                                             
// Discription :  
//               	
// Input	:                                                                                        
// Output	:                                
/**********************************************************/
void TransmitData(BYTE *buffer, BYTE length)
{
  BYTE byChk; 
  
  ioflags.tx = 1;	// set the tx flag
  
  rxCount = 0;		// reset all the rxCount, rxInPtr, rxOutPtr number
  rxInPtr = 0;
  rxOutPtr = 0;
  
  //printf("Send data\n");
  
  Uart_tx_cmd(length, buffer+1);      
  
  ioflags.tx = 0;	// clean the tx flag
}


/**********************************************************/
//  * \ingroup SerialAPI
//  * Send request to remote side.
//  * NOTE: Must only be called locally in the Serial API.
//  * \param[in] buffer   Pointer to data frame.
//  * \param[in] length   Length of data.
/**********************************************************/
BYTE SendData(BYTE *buffer, BYTE length)
{
  BYTE byChk;

  if(ioflags.tx)
  {
  	printf("Sending data now\n");	 
  	return 0;
  }
	
  //printf("SendData\n");
	
  if(buffer != &TransmitBuffer[1])
  {
  	//printf("I don't know it\n");
    waitForAck = TRUE;
    TransmitBuffer[1] = SOF;
    
    for (byChk = 0; byChk < length; byChk++)
    {
      TransmitBuffer[2 + byChk] = buffer[byChk];
    }
    
    TransmitBuffer[2 + length] = CalculateChecksum(buffer, length);	
    TransmitBufferLength = length;
    TransmitBuffer[0] = length + 2;  // SOH + data + chksum
  }

  for (byChk = 0; byChk < 3; byChk++)	// transmit three times
  {
    TransmitData(TransmitBuffer, TransmitBuffer[0]);
    
    if(WaitForAck() == STATUS_FRAMESENT)// send the data out then waiting for ACK 
    {
    	//printf("We get ACK\n");
    	//BufTX = 0xAA; 
    	
    	return 1;
    }
    
    printf("No ACK, Send again%d\n", byChk+1);
  }
  
  return 0;
}


/**********************************************************/
//  * \ingroup SerialAPI
//  * Send request to remote side and wait for response.
//  * NOTE: Must only be called locally in the Serial API.
//  * \param[in] buffer   Pointer to data frame.
//  * \param[in] length   Length of data.
//  * \param[in] byFunc   Function number expected in response.
//  * \param[out] out     Buffer to store response in.
//  * \param[out] byLen   Length of response.
/**********************************************************/
void SendDataAndWaitForResponse( BYTE *buffer, BYTE length, BYTE byFunc, BYTE *out, BYTE *byLen )
{
	BYTE tmp;
	
  if ( bWaitForResponseRunning == TRUE )	// right now is waiting for response
  {
    ;
  }
  
  //printf("SendDataAndWaitForResponse\n");
  
  waitForAck = TRUE;										 // we are waiting for ACK now
  
  tmp = SendData( buffer, length );						// sending the data out, and wait for ACK 
  if(tmp)
  	WaitForResponse( byFunc, out, byLen );// waiting for response
  else
  {
  	zw_senddata_status = STATUS_NORESPONSE;	// set status to no response
    cmd_exe_status = STATUS_IDLE;	// set status back to idle, prevent lock in processing status        
  }
}


/*============================   RemoveFailCompleted   ======================
**    Callback for association sends...
**
**    Side effects:
**
**-------------------------------------------------------------------*/
void RemoveFailCompleted(BYTE txStatus)       // IN   Transmission result           
{
	switch(txStatus)
	{
		case ZW_NODE_OK:
		{
			cmd_exe_status = STATUS_IDLE;
  		printf("node work well no removed this node, state %d\n", cmd_exe_status);
		}
		break;
		
		case ZW_FAILED_NODE_REMOVED:
		{
			cmd_exe_status = STATUS_IDLE;	
			printf("remove failnode done, state %d\n", cmd_exe_status);
		}
		break;
		
		case ZW_FAILED_NODE_NOT_REMOVED:
		{
			cmd_exe_status = STATUS_FAIL;
			printf("remove failnode fail, again, state %d\n", cmd_exe_status);
		}
		break;
	}  
}


/*===========================   ZW_ExploreRequestInclusion   =============================
**   
** Note: Controller not support this.
**--------------------------------------------------------------------------*/
BYTE ZW_ExploreRequestInclusion(HCMD p)
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byLen = 0;
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_EXPLORE_REQUEST_INCLUSION;
  p->buffer[ 0 ] = p->idx;  // length

  SendDataAndWaitForResponse(&(p->buffer), p->idx, FUNC_ID_ZW_EXPLORE_REQUEST_INCLUSION, &(p->buffer), &(p->byLen));
  
  cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
	
	return p->buffer[ IDX_DATA ];
}


/*==========================   ZW_RemoveFailedNode   ===============================
**
**    remove a node from the failed node list, if it already exist.
**    A call back function should be provided otherwise the function will return
**    without removing the node.
**    If the removing process started successfully then the function will return
**    ZW_FAILED_NODE_REMOVE_STARTED        The removing process started
**
**    If the removing process can not be started then the API function will return
**    on or more of the following flags
**    ZW_NOT_PRIMARY_CONTROLLER             The removing process was aborted because the controller is not the primaray one
**    ZW_NO_CALLBACK_FUNCTION              The removing process was aborted because no call back function is used
**    ZW_FAILED_NODE_NOT_FOUND             The removing process aborted because the node was node found
**    ZW_FAILED_NODE_REMOVE_PROCESS_BUSY   The removing process is busy
**
**    The call back function parameter value is:
**
**    ZW_NODE_OK                     The node is working proppely (removed from the failed nodes list )
**    ZW_FAILED_NODE_REMOVED         The failed node was removed from the failed nodes list
**    ZW_FAILED_NODE_NOT_REMOVED     The failed node was not
**
**    IN the failed nodeID
**    IN callback function to be called
**    Side effects:
**--------------------------------------------------------------------------*/
BYTE ZW_RemoveFailedNode(HCMD p, BYTE NodeID, VOID_CALLBACKFUNC(completedFunc)(BYTE))
{
	int i;
	
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
			
  p->idx = 0;
  p->byLen = 0;  
  
  p->byCompletedFunc = (completedFunc == NULL ? 0 : completedFunc);
  p->idx++;
  p->buffer[p->idx++] = REQUEST;
  p->buffer[p->idx++] = FUNC_ID_ZW_REMOVE_FAILED_NODE_ID;
  p->buffer[p->idx++] = NodeID;
  p->buffer[p->idx++] = p->byCompletedFunc;
  
  p->buffer[0] = p->idx;                      // length
  
  cbFuncZWRemoveFailNodeID = completedFunc; 
  SendDataAndWaitForResponse(&(p->buffer), p->idx, FUNC_ID_ZW_REMOVE_FAILED_NODE_ID, &(p->buffer), &(p->byLen));    
  
  if(p->buffer[IDX_DATA] != ZW_FAILED_NODE_REMOVE_STARTED)
  	cmd_exe_status = STATUS_IDLE;
  	
  return p->buffer[IDX_DATA];
} 
    

/*==========================   ZW_GetFailedNode   ===============================
**
**    Check if a node failed is in the failed nodes table
**    Side effects:
**	// RET true if node in failed node table, else false 
**  // IN the failed node ID
**--------------------------------------------------------------------------*/
BYTE ZW_isFailedNode(HCMD p, BYTE nodeID)
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byLen = 0;
  p->buffer[p->idx++] = 0;
  p->buffer[p->idx++] = REQUEST;
  p->buffer[p->idx++] = FUNC_ID_ZW_IS_FAILED_NODE_ID;
  p->buffer[p->idx++] = nodeID;
  p->buffer[0] = p->idx;      // length
  
  SendDataAndWaitForResponse(&(p->buffer), p->idx, FUNC_ID_ZW_IS_FAILED_NODE_ID, &(p->buffer), &(p->byLen));
  
  cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
  	
  return p->buffer[ IDX_DATA ];
}   
  
  
/*==========================   ZW_RFPowerLevelGet  ==========================
**    Get the current powerlevel used in RF transmitting.
**    Possible powerlevel return values are :
**
**       normalPower : Max power possible
**       minus2dBm    - normalPower - 2dBm
**       minus4dBm    - normalPower - 4dBm
**       minus6dBm    - normalPower - 6dBm
**       minus8dBm    - normalPower - 8dBm
**       minus10dBm   - normalPower - 10dBm
**       minus12dBm   - normalPower - 12dBm
**       minus14dBm   - normalPower - 14dBm
**       minus16dBm   - normalPower - 16dBm
**       minus18dBm   - normalPower - 18dBm
**	return value : RET The current powerlevel
**  IN Nothing
**--------------------------------------------------------------------------*/
BYTE ZW_RFPowerLevelGet(HCMD p)            
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byLen = 0;
  p->buffer[p->idx++] = 0;
  p->buffer[p->idx++] = REQUEST;
  p->buffer[p->idx++] = FUNC_ID_ZW_RF_POWER_LEVEL_GET;
  p->buffer[0] = p->idx;      // length
  
  SendDataAndWaitForResponse(&(p->buffer), p->idx, FUNC_ID_ZW_RF_POWER_LEVEL_SET, &(p->buffer), &(p->byLen));
  
  cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  return p->buffer[ IDX_DATA ];
}

/*==========================   ZW_RFPowerLevelSet  ==========================
**    Set the powerlevel used in RF transmitting.
**    Valid powerlevel values are :
**
**       normalPower : Max power possible
**       minus2dBm    - normalPower - 2dBm
**       minus4dBm    - normalPower - 4dBm
**       minus6dBm    - normalPower - 6dBm
**       minus8dBm    - normalPower - 8dBm
**       minus10dBm   - normalPower - 10dBm
**       minus12dBm   - normalPower - 12dBm
**       minus14dBm   - normalPower - 14dBm
**       minus16dBm   - normalPower - 16dBm
**       minus18dBm   - normalPower - 18dBm
**		RET The powerlevel set
**    IN Powerlevel to set
**--------------------------------------------------------------------------*/
BYTE ZW_RFPowerLevelSet(HCMD p, BYTE powerLevel)      
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byLen = 0;
  p->buffer[p->idx++] = 0;
  p->buffer[p->idx++] = REQUEST;
  p->buffer[p->idx++] = FUNC_ID_ZW_RF_POWER_LEVEL_SET;
  p->buffer[p->idx++] = powerLevel;
  p->buffer[0] = p->idx;      // length
  
  SendDataAndWaitForResponse(&(p->buffer), p->idx, FUNC_ID_ZW_RF_POWER_LEVEL_SET, &(p->buffer), &(p->byLen));
  
  cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  return p->buffer[ IDX_DATA ];
}


/*===========================   ZW_SoftReset   =============================
**   
** 
**--------------------------------------------------------------------------*/
void ZW_SoftReset(HCMD p)
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_SERIAL_API_SOFT_RESET;
  p->buffer[ 0 ] = p->idx;  // length

  SendData( &(p->buffer), p->idx );
  
  cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
}


/*===========================   ZW_SetDefault   =============================
**    Remove all Nodes and timers from the EEPROM memory.
** // IN Command completed call back function 
**--------------------------------------------------------------------------*/
void ZW_SetDefault(HCMD p, VOID_CALLBACKFUNC(completedFunc)(void))
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byCompletedFunc = ( completedFunc == NULL ? 0 : completedFunc );
  	
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_SET_DEFAULT;
  p->buffer[ p->idx++ ] = p->byCompletedFunc;
  p->buffer[ 0 ] = p->idx;	// length
  
  cbFuncZWSetDefault = completedFunc;
  
  SendData( &(p->buffer), p->idx );
}


/*============================   ZW_RequestNodeInfo   ======================
**    Function description.
**     Request a node to send it's node information.
**     Function return TRUE if the request is send, else it return FALSE.
**     FUNC is a callback function, which is called with the status of the
**     Request nodeinformation frame transmission.
**     If a node sends its node info, ApplicationControllerUpdate will be called
**     with UPDATE_STATE_NODE_INFO_RECEIVED as status together with the received
**     nodeinformation.
**
**    Side effects:
**	//RET FALSE if transmitter busy 
**  BYTE nodeID, //IN: node id of the node to request node info from it.
**  VOID_CALLBACKFUNC, // IN Callback function 
**--------------------------------------------------------------------------*/
BOOL ZW_RequestNodeInfo(HCMD p, WORD nodeID, VOID_CALLBACKFUNC(completedFunc)(auto BYTE)) 
{
	BYTE id, ch;
	
	id = 0x00FF & nodeID;	// get the real node id	
	ch = (0xFF00 & nodeID)>>8;
	
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byLen = 0;
  p->idx++;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_REQUEST_NODE_INFO;
  p->buffer[ p->idx++ ] = id;
  p->buffer[ 0 ] = p->idx;	// length
  
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_ZW_REQUEST_NODE_INFO, &(p->buffer), &(p->byLen) );
	
  //cmd_exe_status = STATUS_IDLE;
  //printf("state %d\n", cmd_exe_status);
  
  if(p->buffer[IDX_DATA] == FALSE)
  	cmd_exe_status = STATUS_IDLE;
  		
  return p->buffer[ IDX_DATA ];
}


/*==========================   ZW_RemoveNodeFromNetwork   ========================
**
**    Remove any type of node from the network
**
**    The modes are:
**
**    REMOVE_NODE_ANY            Remove any node from the network
**    REMOVE_NODE_CONTROLLER     Remove a controller from the network
**    REMOVE_NODE_SLAVE          Remove a slaev node from the network
**
**    REMOVE_NODE_STOP           Stop learn mode without reporting an error.
**
**    ADD_NODE_OPTION_HIGH_POWER    Set this flag in bMode for High Power exclusion.
**
**    Side effects:
**
**--------------------------------------------------------------------------*/
void ZW_RemoveNodeFromNetwork(HCMD p, BYTE bMode, VOID_CALLBACKFUNC(completedFunc)(auto LEARN_INFO*))
{
	BYTE tmp;
	
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;   
  p->byCompletedFunc = ( completedFunc == NULL ? 0 : completedFunc );
  	
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK;
  p->buffer[ p->idx++ ] = bMode;
  p->buffer[ p->idx++ ] = p->byCompletedFunc;
  p->buffer[ 0 ] = p->idx;	// length
  
  cbFuncZWNewController = completedFunc;
  
  tmp =	SendData( &(p->buffer), p->idx );
  if(!tmp)
  {
  	zw_senddata_status = STATUS_NORESPONSE;	// set status to no response
    cmd_exe_status = STATUS_IDLE;	// set status back to idle, prevent lock in processing status        
  	printf("Remove node no ack\n");
  }
}


/*==========================   ZW_AddNodeToNetwork   ========================
**
**    Add any type of node to the network
**
**    The modes are:
**
**    ADD_NODE_ANY            Add any node to the network
**    ADD_NODE_CONTROLLER     Add a controller to the network
**    ADD_NODE_SLAVE          Add a slaev node to the network
**    ADD_NODE_STOP           Stop learn mode without reporting an error.
**    ADD_NODE_STOP_FAILED    Stop learn mode and report an error to the
**                            new controller.
**
**    ADD_NODE_OPTION_HIGH_POWER    Set this flag in bMode for High Power inclusion.
**
**    Side effects:
**
**--------------------------------------------------------------------------*/
void ZW_AddNodeToNetwork(HCMD p, BYTE bMode, VOID_CALLBACKFUNC(completedFunc)(auto LEARN_INFO*))
{
	BYTE tmp;
	
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0; 
  p->byCompletedFunc = ( completedFunc == NULL ? 0 : completedFunc );
  	
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_ADD_NODE_TO_NETWORK;
  p->buffer[ p->idx++ ] = bMode;
  p->buffer[ p->idx++ ] = p->byCompletedFunc;
  p->buffer[ 0 ] = p->idx;	// length
  
  cbFuncZWNewController = completedFunc;
	
  tmp =	SendData( &(p->buffer), p->idx );
  if(!tmp)
  {
  	zw_senddata_status = STATUS_NORESPONSE;	// set status to no response
    cmd_exe_status = STATUS_IDLE;	// set status back to idle, prevent lock in processing status        
  	printf("Add node no ack\n");
  }
}


/**********************************************************/
//  * \ingroup ZWCMD
//  * Get the currently registered SUC node ID.
//  * \return SUC node ID, ZERO if no SUC available
/**********************************************************/
BYTE ZW_GetSUCNodeID( HCMD p )
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_GET_SUC_NODE_ID;
  p->buffer[ 0 ] = p->idx;	// length
  p->byLen = 0;
  
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_ZW_GET_SUC_NODE_ID, &(p->buffer), &(p->byLen) );
  
  cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
  return p->buffer[ IDX_DATA ];
}


/**********************************************************/
//  * \ingroup ZWCMD
//  * Copy the Home-ID and Node-ID to the specified RAM addresses.
//  * \param[out] homeID  Home-ID pointer
//  * \param[out] nodeID  Node-ID pointer
/**********************************************************/
void ZW_MemoryGetID( HCMD p, BYTE *pHomeID, BYTE *pNodeID )
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_MEMORY_GET_ID;
  p->buffer[ 0 ] = p->idx;	// length
  p->byLen = 0;
  
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_MEMORY_GET_ID, &(p->buffer), &(p->byLen) );

  pHomeID[ 0 ] = p->buffer[ IDX_DATA ];
  pHomeID[ 1 ] = p->buffer[ IDX_DATA + 1 ];
  pHomeID[ 2 ] = p->buffer[ IDX_DATA + 2 ];
  pHomeID[ 3 ] = p->buffer[ IDX_DATA + 3 ];
  pNodeID[ 0 ] = p->buffer[ IDX_DATA + 4 ];
  
  cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
}


/**********************************************************/
//  * \ingroup ZWCMD
//  * Get capabilities of a controller.
//  * \return
//  *   = CONTROLLER_IS_SECONDARY :
//  *      If bit is set then the controller is a secondary controller \n
//  *   = CONTROLLER_ON_OTHER_NETWORK :
//  *      If this bit is set then this controller is not using its build-in home ID \n
//  *   = CONTROLLER_IS_SUC :
//  *      If this bit is set then this controller is a SUC \n
//  *   = CONTROLLER_NODEID_SERVER_PRESENT :
//  *      If this bit is set then there is a SUC ID server (SIS)
//  *      in the network and this controller can therefore
//  *      include/exclude nodes in the network (inclusion controller). \n
/**********************************************************/
BYTE ZW_GetControllerCapabilities( HCMD p )
{
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byLen = 0;
  p->idx++;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES;
  p->buffer[ 0 ] = p->idx;	// length
  
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES, &(p->buffer), &(p->byLen) );

	cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
	
  return p->buffer[ IDX_DATA ];
}


/**********************************************************/
//  * \ingroup ZWCMD
//  * Copy the Node's current protocol information from the non-volatile memory.
//  * \param[in] nodeID      Node ID
//  * \param[out] nodeInfo   Node info buffer
/**********************************************************/
void ZW_GetNodeProtocolInfo(HCMD p, WORD bNodeID, NODEINFO *nodeInfo )
{
	BYTE id, ch;
	
	id = 0x00FF & bNodeID;	// get the real node id	
	ch = (0xFF00 & bNodeID)>>8;
		
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO;
  p->buffer[ p->idx++ ] = id;
  p->buffer[ 0 ] = p->idx;	// length
  p->byLen = 0;
  
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO, &(p->buffer), &(p->byLen) );

  nodeInfo->capability = p->buffer[ IDX_DATA ];
  nodeInfo->security = p->buffer[ IDX_DATA + 1 ];
  nodeInfo->reserved = p->buffer[ IDX_DATA + 2 ];
  
#ifdef NEW_NODEINFO		// right now use
  nodeInfo->nodeType.basic = p->buffer[ IDX_DATA + 3 ];
  nodeInfo->nodeType.generic = p->buffer[ IDX_DATA + 4 ];
  nodeInfo->nodeType.specific = p->buffer[ IDX_DATA + 5 ];
#else
  nodeInfo->nodeType = p->buffer[ IDX_DATA + 3 ];
#endif
	cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
}


/**********************************************************/
//  * \ingroup ZWCMD
//  * Get Serial API initialization data from remote side (Enhanced Z-Wave module).
//  * \param[out]   ver            Remote sides Serial API version
//  * \param[out]   capabilities   Capabilities flag (GET_INIT_DATA_FLAG_xxx)
//  *   Capabilities flag: \n
//  *      Bit 0: 0 = Controller API; 1 = Slave API \n
//  *      Bit 1: 0 = Timer functions not supported; 1 = Timer functions supported. \n
//  *      Bit 2: 0 = Primary Controller; 1 = Secondary Controller \n
//  *      Bit 3: 0 = Not SUC; 1 = This node is SUC (static controller only) \n
//  *      Bit 3-7: Reserved \n
//  *   Timer functions are: TimerStart, TimerRestart and TimerCancel.
//  * \param[out]   len            Number of bytes in nodesList
//  * \param[out]   nodesList      Bitmask list with nodes known by Z-Wave module
/**********************************************************/
BYTE ZW_SerialAPI_GetInitData(HCMD p, BYTE *ver, BYTE *capabilities, BYTE *len, BYTE *nodesList )
{
  int i;
  
  cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_SERIAL_API_GET_INIT_DATA;
  p->buffer[ 0 ] = p->idx;			// length
  p->byLen = 0;
  *ver = 0;
  *capabilities = 0;
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_SERIAL_API_GET_INIT_DATA, &(p->buffer), &(p->byLen) );
  *ver = p->buffer[ IDX_DATA ];
  
  if(*ver == 0)
  {
  	cmd_exe_status = STATUS_IDLE;
		printf("state %d\n", cmd_exe_status);	
		return -1;
	}
	
  //controller api eller slave api
  *capabilities = p->buffer[ IDX_DATA + 1 ];
  *len = p->buffer[ IDX_DATA + 2 ];

  for ( i = 0; i < p->buffer[ IDX_DATA + 2 ]; i++ )
    nodesList[ i ] = p->buffer[ IDX_DATA + 3 + i ];

  // Bit 2 tells if it is Primary Controller (FALSE) or Secondary Controller (TRUE).
  if ( ( *capabilities ) & GET_INIT_DATA_FLAG_SECONDARY_CTRL )
		printf("Is Secondary Controller\n");
  else
		printf("Is Primary Controller\n");
	
	cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
  return 0;
}


/**********************************************************/
//  * \ingroup ZWCMD
//  * Get the Z-Wave library basis version.
//  * \param[out]   pBuf
//  *   Pointer to buffer where version string will be
//  *   copied to. Buffer must be at least 14 bytes long.
/**********************************************************/
BYTE ZW_Version(HCMD p, BYTE *pBuf )
{
  BYTE retVal;
	
	cmd_exe_status = STATUS_PROCESSING;
	printf("state %d\n", cmd_exe_status);
	
  p->idx = 0;
  p->byLen = 0;
  retVal = 0;
  memset( &(p->buffer), 0, sizeof( p->buffer ) );	// Clear the sending buffer data
  
  p->buffer[ p->idx++ ] = 0;
  p->buffer[ p->idx++ ] = REQUEST;
  p->buffer[ p->idx++ ] = FUNC_ID_ZW_GET_VERSION;
  p->buffer[ 0 ] = p->idx;	// length
  SendDataAndWaitForResponse( &(p->buffer), p->idx, FUNC_ID_ZW_GET_VERSION, &(p->buffer), &(p->byLen) );

  if ( memcmp( (void *)&(p->buffer[ IDX_DATA ]), (void *)"Z-Wave", 6 ) == 0 )
  {
    retVal = p->buffer[ IDX_DATA + 12 ];	// return value is library type
    memcpy(pBuf,(void *)&(p->buffer[ IDX_DATA ]), 12);// Store the "Z-wave x.yy retVal" into buffer
    pBuf[ 12 ] = 0;
    //printf("Get Z-wave init\n");
  }
	
	cmd_exe_status = STATUS_IDLE;
	printf("state %d\n", cmd_exe_status);
	
  return retVal;
}


/**********************************************************/
// Function Name : setcom()                                                
// Discription :  
//               	
// Input	: None                                                                                       
// Output	: None                                
/**********************************************************/
void setcom(void) 
{
  stbuf.c_cflag &= ~(CBAUD | CSIZE | CSTOPB | CLOCAL | PARENB);
  stbuf.c_cflag |= (zwave_speed | bits | CREAD | clocal | parity | stopbits );
  
  printf("C_FLAG:%x\n", stbuf.c_cflag);
  
  if(ioctl(comfd, TCSETA, &stbuf) < 0) 
    printf("Can't ioctl set device");
}


/**********************************************************/
// Function Name : writecom(char *text, unsigned int len)                                                
// Discription : Write a null-terminated string to communication device  
//               	
// Input	:                                                                                        
// Output	:                                 
/**********************************************************/
short writecom(char *text, unsigned int len) 
{
  int res;
  unsigned int a, b, errnum;
  char ch;

  a = 0;
  b = len;
  errnum = 0;
	
  //printf("Tx:");
  while(a < b) 
  {
    ch = text[a];            
    res = write(comfd, &ch, 1);     

    printf("%x ", 0xFF & text[a]);
		
    //if(senddelay) 
    //	dormir(senddelay);
    
    if(res != 1) 
    {
        printf("can not write \"%c\"to %s\n", text[a], device);
	errnum++;
	  	
	if (errnum>=5)
            return -1;
    }
    else 
    {
	a++;
	errnum=0;
    }
  }
  
  printf(",Txend\n");
  return 1;
}


/**********************************************************/
// Function Name : Uart_tx_cmd()                                                
// Discription :  
//               	
// Input	:                                                                                        
// Output	:                                 
/**********************************************************/
short Uart_tx_cmd(int length, char *at_cmdstring)
{
  //printf("send len=%d\n", length);

  if(writecom(at_cmdstring, length) == 1)
    return 1;
  else
  {
    printf("Write Com port error while sending %s. \n", at_cmdstring);
    return -1;
  }
}


/**********************************************************/
// Function Name : htime()                                                
// Discription : Returns hundreds of seconds 
//               	
// Input	: None                                                                                       
// Output	: None                                
/**********************************************************/
unsigned long htime(void) 
{
  struct timeval timenow;
  
  gettimeofday(&timenow, NULL);
  
  return(100L*(timenow.tv_sec-hstart)+(timenow.tv_usec)/10000L - hset);
}


/**********************************************************/
// Function Name : dormir()                                                
// Discription : I use select() 'cause CX/UX 6.2 doesn't have usleep(). 
//               On Linux, usleep() uses select() anyway.	
// Input	: microsecs                                                                                       
// Output	: None                                
/**********************************************************/
void dormir(unsigned long microsecs) 
{
  struct timeval timeout;
  
  timeout.tv_sec = microsecs/1000000L;
  timeout.tv_usec = microsecs-(timeout.tv_sec*1000000L);
  select(1,0,0,0,&timeout);
}


/**********************************************************/
// Function Name : openCom()                                                
// Discription :  
//               	
// Input	: value(select which ttyUSB to open)                                                                                       
// Output	: None                                
/**********************************************************/
int openCom(short value) 
{
	if(value == 0)
		strcpy(device, ZWAVE_DEV0);
	else if(value == 1)
		strcpy(device, ZWAVE_DEV1);
	else if(value == 2)
		strcpy(device, ZWAVE_DEV2);
				
	if(is_zwave_open)  
	{
		printf("%s is opened now\n", device);
		return 0;
	}

  //init com
  comfd = 0;
  hstart = time(0);
  hset = htime();
  senddelay = 5001;
  zwave_speed = B115200;

  if ((comfd = open(device, O_RDWR|O_EXCL|O_NONBLOCK|O_NOCTTY)) <0) 
  { 
    printf("Can't open GlobeTrotter %s.\n", device);
		is_zwave_open = 0;
    return -1;
  }

  if (ioctl(comfd, TCGETA, &stbuf) < 0) 
  {
		printf("Can't control %s, please try again.\n", device);
	  is_zwave_open = 0;
		return -1;
	}

  bits = stbuf.c_cflag & CSIZE;
  clocal = stbuf.c_cflag & CLOCAL;
  stopbits = stbuf.c_cflag & CSTOPB;  
  parity = stbuf.c_cflag & (PARENB | PARODD);
  
  stbuf.c_iflag &= ~(IGNCR | ICRNL | IUCLC | INPCK | IXON | IXANY | IGNPAR );
  stbuf.c_oflag &= ~(OPOST | OLCUC | OCRNL | ONLCR | ONLRET);
  stbuf.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  stbuf.c_cc[VMIN] = 1;
  stbuf.c_cc[VTIME] = 0;
  stbuf.c_cc[VEOF] = 1;
    
  setcom();
/*  stbuf.c_cflag &= ~(CBAUD | CSIZE | CSTOPB | CLOCAL | PARENB);
  stbuf.c_cflag |= (zwave_speed | bits | CREAD | clocal | parity | stopbits );
  
  if(ioctl(comfd, TCSETA, &stbuf) < 0) 
    printf("Can't ioctl set device");
*/    
  dormir(200000); // Wait a bit (DTR raise) 
  printf("Open %s as FD %d\n", device, comfd);

	is_zwave_open = 1;

	return 0;
}


/**********************************************************/
// Function Name : atoh()                                                
// Discription :                
// Input	: string 	                                                                                       
// Output	: None                                
/**********************************************************/
unsigned long atoh(char *s)
{
  unsigned long k = 0;
                                                                                    
  k = 0;
  
  while (*s != '\0')
  {
    if(*s >= '0' && *s <= '9')
    {
    	k = 16 * k + (*s - '0');
        s++;
    }
    else if(*s >= 'a' && *s <= 'f')
    {
      k= 16 * k + (*s - 'a' + 10);
      s++;
    }
    else if(*s >= 'A' && *s <= 'F')
    {
      k = 16 * k + (*s -'A' + 10);
      s++;
    }
    else
    {
	printf("Invalid alphabet\n");		
	return 0;
    }
  }
  
  return k;
}


/**********************************************************/
// Function Name : process_exe_callback()                                                
// Discription :  
//               	
// Input	: None                                                                                       
// Output	: None                                
/**********************************************************/
void *process_exe_callback(void)
{
	printf("process_exe_callback begin\n");
	
	while(1)
	{
		//if(waitForAck == FALSE && bWaitForResponseRunning == FALSE && initial_flag == 0)
  	//	zw_result = ReceiveData(RecvBuffer);
  		
  	if(flag_remove_n_add)
  	{
  		if(cbFuncAddNode != NULL)		
  			cbFuncAddNode(zw_senddata_status, &addremovenode_info);
  		flag_remove_n_add = 0;
  	}
  	
  	if(flag_getNIF)
  	{
  		if(cbFuncGetNIF != NULL)
				cbFuncGetNIF(flag_getNIF_ID, flag_getNIF_TYPE);
			flag_getNIF = 0;
			flag_getNIF_ID = 0;
			flag_getNIF_TYPE = 0;	
  	}
  	
  	if(flag_getIRSensorStatus)
  	{
  		if(cbFuncGetIRSensorStatus != NULL)
    		cbFuncGetIRSensorStatus(flag_getIRSensorStatus_ID, flag_getIRSensorStatus_status);
    		
    	flag_getIRSensorStatus = 0;
    	flag_getIRSensorStatus_ID = 0;
    	flag_getIRSensorStatus_status = 0;
  	}
  	
  	if(flag_getWUN)
  	{
  		if(cbFuncGetWUN != NULL)
				cbFuncGetWUN(flag_getWUN_ID);
				
			flag_getWUN = 0;
			flag_getWUN_ID = 0;
  	}
  				
  	usleep(20000);	
	}
}


/**********************************************************/
// Function Name : process_timer()                                                
// Discription :  
//               	
// Input	: None                                                                                       
// Output	: None                                
/**********************************************************/
void* process_timer(void)
{
	unsigned int last_pairing_status = STATUS_NONE;
	SCMD cmd;
	
	printf("process_timer begin\n");
	
	while(1)
	{
		//TimerAction();
		
		timerCount++;		
		usleep(10000);	// each 10ms count once
		//usleep(50000);	// each 50ms count once
		
/*		if(get_status_flag == STATUS_PROCESSING)
		{
			get_status_flag_ctr++;
			if(get_status_flag_ctr >= 80)
			{
				get_status_flag_ctr = 0;
				get_status_flag = STATUS_IDLE;
				printf("get status timeout\n");
			}		
		}
*/	
		
		
		if(addtimer_flag)
		{
			addtimer++;
			if(addtimer > 12000)
			{	
				ZW_AddNodeToNetwork(&cmd, ADD_NODE_STOP, NULL);
				addtimer_flag = 0;
				addtimer = 0;
				addremovenode_info.timeout = 1;
				cmd_exe_status = STATUS_IDLE;
				printf("Add node time out\n");
			}	
		}	
		
		if(removetimer_flag)
		{
			removetimer++;
			if(removetimer > 12000)
			{	
				ZW_RemoveNodeFromNetwork(&cmd, REMOVE_NODE_STOP, NULL);
				removetimer_flag = 0;
				removetimer = 0;
				addremovenode_info.timeout = 1;
				cmd_exe_status = STATUS_IDLE;
				printf("Remove node time out\n");
			}	
		}	
		
/*		
		if(test_speed_flag)
		{
			test_speed_ctr++;					
		}	
*/

		if(flag_prevnet_lock)
		{
			prevent_lock_ctr++;
			if(prevent_lock_ctr >= 50 && cmd_exe_status == STATUS_PROCESSING)
			{
				printf("Locked happened\n");
				flag_prevnet_lock = 0;
  			prevent_lock_ctr = 0;
  			cmd_exe_status = STATUS_IDLE;
			}	
		}
		
		timerctr2++;
		//if(timerctr2%200 == 0)
		//	printf("s:%d\n", cmd_exe_status);				
		
		//timerctr2++;
		//if(pairing_status != NULL)
		//{									
		//	if(timerctr2%100 == 0)
		//	{	
		//		printf("pairing status %d\n", *pairing_status);
		//		if(*pairing_status == STATUS_ADDNODEDONE || *pairing_status == STATUS_REMOVENODEDONE)
		//			printf("device type:%d\n", *add_remove_type);
		//	}							
		//}
										
	}
	
	printf("process_timer dead\n");
}



/**********************************************************/
// Function Name : process_zwave_apipoll()                                                
// Discription :  
//               	
// Input	: None                                                                                       
// Output	: None                                
/**********************************************************/
void* process_zwave_apipoll(void)
{
	printf("process_zwave_apipoll begin\n");
	
	while(1)
	{
		SerialAPI_Poll();
		usleep(1000);
		//usleep(10000);		
	}
	
	printf("process_zwave_apipoll dead\n");
}


/**********************************************************/
// Function Name : process_zwave_rx()                                                
// Discription :  
//               	
// Input	: None                                                                                       
// Output	: None                                
/**********************************************************/
void* process_zwave_rx(void)
{
	fd_set rfds;
	int res=0, i;
	struct timeval timeout;
	char serialstring[MAX_SERIAL_STRING] = {0};
	int read_usb_err_count = 0;
	
	printf("process_zwave_rx begin\n");
	
	while (1)
  	{
  		//SerialAPI_Poll();
  			
    		if(is_zwave_open) // zwave dongle lives
    		{  
      			// read comfd
	    		timeout.tv_sec=0L;
	    		timeout.tv_usec=100000;    
	    		FD_ZERO(&rfds);
	    		FD_SET(comfd, &rfds);

	    		res = select(comfd+1, &rfds, NULL, NULL, &timeout);
			
			if(res < 0)
				printf("error\n");
			else if(res == 0)
			{
				//printf("timeout\n");
			}	
			else
			{	
      				if(FD_ISSET(comfd, &rfds))
	    			{
	    				res = read(comfd, serialstring, MAX_SERIAL_STRING);
	    	  
					//printf("Rx_num:%d\n", res);
					for(i=0; i<res; i++)
					{
#ifdef DEBUG_USB_RX				
						printf("%x %d %d\n", 0xFF & serialstring[i], rxInPtr, rxOutPtr);
#endif						
						//====== Z-wave rx buffer
						if(rxCount < UART_RX_BUFSIZE)
		    				{
      							rxQueue[ rxInPtr++ ] = serialstring[i]; // Get the byte
      							rxCount++;
    						}
    						else
    						{	
    							printf("rx buffer full\n");
    							rxCount = 0;
    							rxInPtr = 0;
    							rxOutPtr = 0;
    						}
    						
    						if(rxInPtr >= UART_RX_BUFSIZE)
    						{
      							rxInPtr = 0;
    						}
    						//======
					}	
					//printf(", %d Bytes\n", rxCount);
					
      	  				if((res == (-1)) || (res == 0))
      	  				{
      	    					read_usb_err_count++;
      	    					printf("read serial port %s error.\n", device);
      	  				}
	    	  			else
      	  				{
      	   					 read_usb_err_count=0;
      	    					//UM_getcomfd(serialstring);
		  	  		}
		  	  
      	  				// USB HUB Error Detection
      	  				if (read_usb_err_count >= MAX_READ_USB_DEV_ERR)
						printf("Error exceed\n");
			    	}
			}			
		}
		else 
		{
			printf("close\n");			      
		}
		
  	}
  
  	printf("process_zwave_rx dead\n");
}


/**********************************************************/
// Function Name : main()                                                
// Discription :                
// Input	: None 	                                                                                       
// Output	: None                                
/**********************************************************/
int main(void)
{
	if( access("/config/zwave", R_OK) != 0 )
	{
		demo_main();
		return -1;
	}
		
	int i, j, n, choice;
	WORD value, value2, value3, value4;
	int thread_zwave_flag = 0, thread_zwave_apipoll_flag = 0, thread_zwave_timer_flag = 0, thread_zwave_execallback_flag = 0;
	char input, tmp;			
	BYTE AT_Request[100] = {0}, Buffer[256]={0}, val[6]={0};
	SCMD cmd_buf;	
	EACH_NODE_INFO node_info;
	

	
	pthread_t zwave_rx_thread, zwave_apipoll_thread, zwave_timer_thread;	
	
	pthread_t zwave_rxdata_thread;
	
	printf("Time 2011-05-24 18:00, Version 1.10\n\n");
	   
	while(1)
	{		
		printf(" 1: Open USB Com, 2: Run Z-Wave Rx, 3: Byte/Byte Tx, 4: Initial\n");		
		printf(" 5: Include Node, 6: Exclude Node\n");
		printf(" 7: Set Binary Switch,  8: Get Binary Switch status, 9: Set and Get Binary Switch\n");							
		printf("10: Set Dimmer,        11: Get Dimmer status        12: Set and Get Dimmer\n"); 
		printf("13: Get MeterValue\n");		
		printf("14: Get node information, 15: Request Node Information Frame\n");	
		printf("16: Get Battery level\n");
		printf("17: Set Curtain\n");
		
//		printf("18: All Switch(not finished)\n");		
//		printf("19: BinarySet,   20: BinaryGet,    21: MeterSupportGet\n");		
//		printf("22: GetRF Power(controller not support),23: SetRF Power(controller not support)\n");
			
//		printf("16: SendDataMulti(not finished)\n");
//		printf("24: BatteryLevelGet\n");
//		printf("25: WakeUpIntervalSet,  26: WakeUpIntervalGet, 27: WakeUpNoMoreInformation\n");
		printf("28: CheckisFailNode,  29: RemoveFailNode \n");
		printf("30: AssociationSet/Remove/Get\n\n");
		
		
//		printf("40: StartAddNodeIntoNetwork,    41: StopAddNodeIntoNetwork\n");
//		printf("42: StartRemoveNodeFromNetwork, 43: StopRemoveNodeFromNetwork\n\n");
		
//		printf("49: Test set dimmer and get dimmer status\n");
//		printf("50: Test return status to an structure\n");
		
//		printf("51: start add node,    52: stop add node\n");
//		printf("53: start remove node, 54: stop remove node\n");
		
		printf("55: Callback add node, 56: Callback remove node\n");
		
		printf("60: Get AV device channel num,   61: Set AV Device\n");		
		
		printf("\n93: Test Send ExploreFrame(Controller not support)\n");
		printf("94: Test Control TV continuously\n");			
		printf("95: Test Send cmd interval\n");
		printf("96: Test Parallel send command\n");
		printf("97: Soft Reset,  98: Reset to Default\n");		
		printf("99: Terminate thread\n");
		printf("Input your choice: ");
		fscanf(stdin, "%s", &input);
		fflush(stdin);	
		choice = atoi(&input);
	
		switch(choice)
		{		
			case 1:
				printf("Select USB number 0,1,2:");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);	
				
				if(value == 0)
				{
					if(openCom(value) != 0)
    				printf("Com0 is turned off now\n");
				}
				else if(value == 1)
				{
					if(openCom(value) != 0)
    				printf("Com1 is turned off now\n");
				}
				else if(value == 2)
				{
					if(openCom(value) != 0)
    				printf("Com2 is turned off now\n");
				}
				else
					printf("error input\n");						
			break;		
			
			case 2:				    
				pthread_create(&zwave_rx_thread, NULL, process_zwave_rx, NULL);
				thread_zwave_flag = 1;
								
#ifndef DEBUG_USB_RX
				pthread_create(&zwave_apipoll_thread, NULL, process_zwave_apipoll, NULL);				
				thread_zwave_apipoll_flag = 1;
#endif

				pthread_create(&zwave_timer_thread, NULL, process_timer, NULL);
				thread_zwave_timer_flag = 1;
				
				pthread_create(&zwave_rxdata_thread, NULL, process_exe_callback, NULL);
				thread_zwave_execallback_flag = 1;
			break;
			
			case 3:		
				printf("Input the hex value(Ex:61,06,00enter):");
				fscanf(stdin, "%s", &Buffer);
				fflush(stdin);				
				j = strlen(Buffer);
				
				i=0;
				n=0;
					
				do
				{	
					strncpy(val, &Buffer[i], 2);
					val[2]=NULL;
					tmp = (unsigned char)atoh(val);
					AT_Request[n] = (unsigned char)tmp;
					//printf("Get val:%x\n", (unsigned char)tmp);
					i+=3;
					n++;
				}while(i<j);
				
				Uart_tx_cmd(n, AT_Request);
			break;
			
			case 4:
				if(API_Initial(&device_info) == INIT_FAIL)
					printf("Initial fail\n");
				printf("Total Node Num:%d\n\n", device_info.total_device_number);	
				
/*				//====== add for check device support cmd class, and fail or not
				for(i=0; i<MAX_NODEMASK_LENGTH; i++)
				{
					for(j=0; j<8; j++)	// 8 bit each BYTE
					{
						if(device_info.device_bit_mask[i] & (0x01<<j))
						{	
							n = i*8 + (j+1);							
							//device_info.node_info_table[n].id;
							//device_info.node_info_table[n].type;
							//printf("*ID:%d, Type:%d*\n\n", device_info.node_info_table[n].id, device_info.node_info_table[n].type);
							
							if(device_info.node_info_table[n].id != 1)
							{
								//printf("*ID:%d, Type:%d*\n\n", device_info.node_info_table[n].id, device_info.node_info_table[n].type);
								
								value2 = API_RequestNodeInfo(device_info.node_info_table[n].id);
								if(value2 == STATUS_OK)
  							{
  								printf("OK\n");
  							}
  							else
  								printf("return value:%d\n", value2);  								  							
  						}
  						//usleep(100000);													
							//j++;
						}
					}
				}
*/				//======	
			break;
				
			case 5:
				init_buffer(&cmd_buf);
				ZW_AddNodeToNetwork(&cmd_buf, ADD_NODE_ANY, LearnModeCompleted_org);	// state waiting for add
			break;
			
			case 6:
				init_buffer(&cmd_buf);
				ZW_RemoveNodeFromNetwork(&cmd_buf, REMOVE_NODE_ANY, RemoveNodeCompleted_org);					
			break;
			
			case 7:			
				printf("Set on(1),set off(0):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);	
			
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value2 = atoi(&input);
								
				value3 = API_SetBinarySwitch(value2, value);
				printf("return value:%d\n", value3);
			break;
			
			case 8:
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				value2 = API_GetBinarySwitchStatus(value, &node_info);
				if(value2 == STATUS_OK)
				{
					printf("device status: %d\n", node_info.status);
				}	
				else
					printf("get error %d\n", value2);
			break;
			
			case 9:
				printf("Set on(1),set off(0):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);	
			
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value2 = atoi(&input);
				
				value3 = API_SetAndGetBinarySwitch(value2, value, &node_info);
				if(value3 == STATUS_OK)
				{
					printf("device status: %d\n", node_info.status);
				}	
				else
					printf("get error %d\n", value3);
			break;
			
			case 10:
				printf("Dimming value(dec 0~99):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);	
			
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value2 = atoi(&input);
				
				value3 = API_SetDimmer(value2, value);
				printf("return value:%d\n", value3);
			break;								
			
			case 11:
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				value2 = API_GetDimmerStatus(value, &node_info);
				if(value2 == STATUS_OK)
				{
					printf("device status: %d\n", node_info.status);
				}	
				else
					printf("get error %d\n", value2);
			break;
			
			case 12:
				printf("Dimming value(dec 0~99):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);	
			
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value2 = atoi(&input);
				
				value3 = API_SetAndGetDimmer(value2, value, &node_info);
				if(value3 == STATUS_OK)
				{
					printf("device status: %d\n", node_info.status);
				}	
				else
					printf("get error %d\n", value3);
			break;
			
			case 13:
				printf("NodeID(dec):");
  			fscanf(stdin, "%s", &input);
  			fflush(stdin);
  			value = atoi(&input);
  			
				//printf("Type(0:walt,1:voltage,2:current):");
  			//fscanf(stdin, "%s", &input);
  			//fflush(stdin);
  			//value2 = atoi(&input);
  			
  			value3 = API_GetMeter(value, &node_info);
  			if(value3 == STATUS_OK)
  			{
  				printf("device power:%d, voltage:%d, current:%d\n", node_info.power, node_info.voltage, node_info.current);
  			}
  			else
  				printf("return value:%d\n", value3); 							
			break;

			case 14:				
				printf("Input NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				ZW_GetNodeProtocolInfo(&cmd_buf, value, &nodeInfo);
				printf("*NodeID_%d_Capab:%x, Secu:%x*\n", value, nodeInfo.capability, nodeInfo.security);
				printf("*Basic:%x, generic:%x, specific:%x*\n", nodeInfo.nodeType.basic, nodeInfo.nodeType.generic, nodeInfo.nodeType.specific);	
			break;			
			
			case 15:
				printf("Input NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				value2 = API_RequestNodeInfo(value);
				if(value2 == STATUS_OK)
  			{
  				printf("OK\n");
  			}
  			else
  				printf("return value:%d\n", value2);
			break;						
			
			case 16:
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				value2 = API_GetBatteryLevel(value, &node_info);
				if(value2 == STATUS_OK)
				{
					printf("power:%d\n", node_info.powerlevel);
				}	
				else
					printf("get error %d\n", value2);				
			break;
			
/*			case 16:
				printf("Select Basic Set(on:1, off:0):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				if(value == 1)
					PrepareBasicSet(&txBuf, BASIC_ON);
				else if(value == 0)
					PrepareBasicSet(&txBuf, BASIC_OFF);
				else
				{	
					printf("error input\n");
					break;
				}
					
				printf("Input Node number(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
							
				for(test_number=0; test_number < value; test_number++)
				{
					printf("Input NodeID(dec):");					
					fscanf(stdin, "%s", &input);
					fflush(stdin);	
					value2 = atoi(&input);
					groupNumber[test_number] = value2;				
					printf("%dth NodeID:%d, next\n", test_number, value2);
				}
				
				// clear the destination node mask
        for (i = 0; i < MAX_NODEMASK_LENGTH; i++)
          groupMask[i] = 0;

        // Set the destination node mask bits
        for (i = 0; i < value && i < ZW_MAX_NODES; i++)
          NodeMaskSetBit(groupMask, groupNumber[i]);
          
        init_buffer(&cmd_buf);    
				if( ZW_SendDataMulti(&cmd_buf, groupMask, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_BasicSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted ) == FALSE)
					printf("Queue full\n");
			break;
*/			
			case 17:
				printf("open(0),close(1),stop(2):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value2 = atoi(&input);
									
				value3 = API_SetCurtain(value2, value);
				printf("return value:%d\n", value3);
			break;
			
			case 18:
/*				printf("Input NodeID(dec):");
        fscanf(stdin, "%s", &input);
        fflush(stdin);
        value = atoi(&input);
        
*/			break;
			
			case 19:
/*				printf("Set on(1),set off(0):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);	
				
				if(value == 0)
					PrepareBinarySet(&txBuf, BASIC_OFF);
				else if(value == 1)
					PrepareBinarySet(&txBuf, BASIC_ON);
				
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
					
				init_buffer(&cmd_buf);
				if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_SwitchBinarySetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
					printf("Queue full\n");
*/			break;
			
			case 20:
/*				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				PrepareBinaryGet(&txBuf);
				
				init_buffer(&cmd_buf);
//				get_status_flag = STATUS_PROCESSING;
//				get_status_flag_ctr = 0;		
				if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_SwitchBinaryGetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
					printf("Queue full\n");
*/			break;
			
			case 21:
/*				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				PrepareMeterSupport(&txBuf);
				
				init_buffer(&cmd_buf);
//				get_status_flag = STATUS_PROCESSING;
//				get_status_flag_ctr = 0;
				if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_MeterSupportedGetV3Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
					printf("Queue full\n");
*/			break;
			
			case 22:
/*				init_buffer(&cmd_buf);
				value = ZW_RFPowerLevelGet(&cmd_buf);
				printf("PowerLevel is:%d\n", value);
*/			break;
			
			case 23:
/*				printf("Input power level(dec max:0~min:9):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				if(value > 9)
					printf("error input\n");
					
				init_buffer(&cmd_buf);
				ZW_RFPowerLevelSet(&cmd_buf, value);
*/			break;
			
			case 24:
/*				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				PrepareBatteryGet(&txBuf);
				
				init_buffer(&cmd_buf);

				if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_BatteryGetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
					printf("Queue full\n");				
*/			break;
			
			case 25:
/*				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				printf("Sec(dec 1~1677215):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value2 = atoi(&input);
				
				PrepareWakeUpIntervaleSet(&txBuf, value, value2);
				
				init_buffer(&cmd_buf);
				if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_WakeUpIntervalSetV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
					printf("Queue full\n");
*/			break;
			
			case 26:
/*				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				PrepareWakeUpIntervaleGet(&txBuf);
				
				init_buffer(&cmd_buf);
				if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_WakeUpIntervalGetV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
					printf("Queue full\n");
*/			break;
			
			case 27:
/*				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				PrepareWakeUpNoMoreInformation(&txBuf);
				
				init_buffer(&cmd_buf);
				if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_WakeUpNoMoreInformationV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)	
					printf("Queue full\n");
*/			break;
			
			case 28:
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				value2 = API_CheckisFailNode(value);
				if(value2 == STATUS_NODE_WELL)
					printf("node well\n");
				else if(value2 == STATUS_NODE_FAIL)
					printf("node fail\n");
				else
					printf("get error %d\n", value2);				
			break;
			
			case 29:
				printf("NodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value = atoi(&input);
				
				init_buffer(&cmd_buf);	
				value2 = ZW_RemoveFailedNode(&cmd_buf, value, RemoveFailCompleted);		
				
				printf("return value:%d\n", value2);
			break;
			
			case 30:
				printf("SensorNodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);
				value = atoi(&input);
				
				printf("AddNodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);
				value2 = atoi(&input);
				
				printf("0:AssociationSet   1:AssociationRemove   2:AssociationGet\n");
				printf("Select operation:");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value3 = atoi(&input);
				
				if(value3 == 0)
					value3 = ASSOCIATIONSET;
				else if(value3 == 1)
					value3 = ASSOCIATIONREMOVE;
				else if(value3 == 2)
					value3 = ASSOCIATIONGET;
					
				value4 = API_AssociationSetandRemoveandGet(value3, value, value2);				
				if(value4 == STATUS_OK)
				{
					printf("Associate OK\n");
				}	
				else
					printf("get error %d\n", value4);
			break;
			
			case 31:								
/*				printf("SensorNodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);
				value = atoi(&input);
				
				printf("GroupNum(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);
				value2 = atoi(&input);
				
				PrepareAssociationGet(&txBuf, value2);				
				
				init_buffer(&cmd_buf);
//				get_status_flag = STATUS_PROCESSING;
//				get_status_flag_ctr = 0;	
        if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_AssociationGetV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
        	printf("Queue full\n");
*/			break;
			
			case 32:
/*				printf("SensorNodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);
				value = atoi(&input);
				
				printf("GroupNum(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);
				value2 = atoi(&input);
				
				printf("RemoveNodeID(dec):");
				fscanf(stdin, "%s", &input);
				fflush(stdin);
				value3 = atoi(&input);
				
				PrepareAssociationRemove(&txBuf, value2, value3);
				
				init_buffer(&cmd_buf);	
        if(ZW_SendData(&cmd_buf, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_AssociationRemove1byteV2Frame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
        	printf("Queue full\n");
*/			break;			

			case 40:
					//value = API_StartAddNodeIntoNetwork(&pairing_status);
					//printf("return value:%d\n", value);
			break;
				
			case 41:
					//value = API_StopAddNodeIntoNetwork(&pairing_status);
					//printf("return value:%d\n", value);
			break;
				
			case 42:
					//value = API_StartRemoveNodeFromNetwork();
					//printf("return value:%d\n", value);
			break;
				
			case 43:
					//value = API_StopRemoveNodeFromNetwork();
					//printf("return value:%d\n", value);
			break;
				
			case 49:
/*					printf("Dimming value(dec 0~99):");
					fscanf(stdin, "%s", &input);
					fflush(stdin);	
					value = atoi(&input);	
			
					printf("NodeID(dec):");
					fscanf(stdin, "%s", &input);
					fflush(stdin);	
					value2 = atoi(&input);
					
					usleep(50000);
					
					value3 = API_SetDimmer(value2, value);		
					printf("return value:%d\n", value3);								

					value3 = API_GetDimmerStatus(value2, &node_info);
					if(value3 == STATUS_OK)
					{
						printf("device status: %d\n", node_info.status);
					}	
					else
						printf("get error %d\n", value3);
*/			break;
				
			case 50:										
/*					printf("NodeID(dec):");
					fscanf(stdin, "%s", &input);
					fflush(stdin);	
					value = atoi(&input);
					
					printf("Device type(3:binary,5:dimmer,6:curtain,7:powermeter):");
					fscanf(stdin, "%s", &input);
					fflush(stdin);	
					value2 = atoi(&input);
					
					value3 = API_GetDeviceStatus(value, value2, &node_info);
					if(value3 == STATUS_OK)
					{
						printf("device status: %d\n", node_info.status);
					}	
					else
						printf("get error %d\n", value3);
*/			break;
				
			case 51:
/*					//printf("pairing :%d %d\n", pairing_status, *pairing_status);
																				
					value = API_StartAddNodeIntoNetwork(&pairing_status, &add_remove_type);
										
					if(value == STATUS_ADDNODEREADY)
					{	
						printf("ready pairing value:%d %d\n", *pairing_status, pairing_status);
					}
					else
						printf("return value:%d\n", value);				
*/			break;
				
			case 52:
/*					value = API_StopAddNodeIntoNetwork(&pairing_status, &add_remove_type);
					
					if(value == STATUS_ADDNODESTOP)
						printf("stop add node\n");
					else
						printf("return value:%d\n", value);
*/			break;
				
			case 53:
/*					value = API_StartRemoveNodeFromNetwork(&pairing_status, &add_remove_type);
										
					if(value == STATUS_REMOVENODEREADY)
					{	
						printf("ready pairing value:%d %d\n", *pairing_status, pairing_status);
					}
					else
						printf("return value:%d\n", value);
*/			break;
				
			case 54:
/*					value = API_StopRemoveNodeFromNetwork(&pairing_status, &add_remove_type);
					
					if(value == STATUS_REMOVENODESTOP)
						printf("stop remove node\n");
					else
						printf("return value:%d\n", value);
*/			break;

			case 55:
					value = API_StartAddNodeIntoNetwork_cb(AP_callback_removeadd);
					if(value == STATUS_ADDNODEREADY)
					{	
						printf("ready pairing\n");
					}
					else
						printf("return value:%d\n", value);	
			break;
				
			case 56:
					value = API_StartRemoveNodeFromNetwork_cb(AP_callback_removeadd);
					if(value == STATUS_REMOVENODEREADY)
					{	
						printf("ready pairing\n");
					}
					else
						printf("return value:%d\n", value);
			break;

				
			case 60:
				printf("Input NodeID(dec):");
        fscanf(stdin, "%s", &input);
        fflush(stdin);
        value = atoi(&input);
				
				printf("Input CmdClass(hex):");
        fscanf(stdin, "%s", &input);
        fflush(stdin);
        value2 = atoh(&input);
        
        value3 = API_GetMultiInstanceNum(value, value2, &node_info);
        if(value3 == STATUS_OK)
				{
					//printf("device status: %d\n", node_info.status);
					printf("OK\n");
				}	
				else
					printf("get error %d\n", value3);
										
			break;
			
			case 61:
				printf("Input NodeID(dec):");
        fscanf(stdin, "%s", &input);
        fflush(stdin);
        value = atoi(&input);
				
				printf(" 1:TV power,        2:TV channel up,   3:TV channel down\n");
				printf(" 4:TV volume up,    5:TV volume down,  6:TV mute\n");
				printf(" 7:DVD power,       8:DVD play,        9:DVD stop\n");		
				printf("10:DVD pause,      11:DVD eject,      12:DVD volume up\n");
				printf("13:DVD volume down,    \n");
				printf("Select operation:");
				fscanf(stdin, "%s", &input);
				fflush(stdin);	
				value3 = atoi(&input);
				                               
				switch(value3)
				{
					case 1:
						value3 = TV_POWER;
					break;
					
					case 2:
						value3 = TV_CHANNELUP;
					break;
					
					case 3:
						value3 = TV_CHANNELDOWN;
					break;
					
					case 4:
						value3 = TV_VOLUMEUP;
					break;
					
					case 5:
						value3 = TV_VOLUMEDOWN;
					break;
					
					case 6:
						value3 = TV_MUTE;
					break;
					
					case 7:
						value3 = DVD_POWER;
					break;
					
					case 8:
						value3 = DVD_PLAY;
					break;
					
					case 9:
						value3 = DVD_STOP;
					break;
					
					case 10:
						value3 = DVD_PAUSE;
					break;
					
					case 11:
						value3 = DVD_EJECT;
					break;
					
					case 12:
						value3 = DVD_VOLUMEUP;
					break;
					
					case 13:
						value3 = DVD_VOLUMEDOWN;
					break;
				}                                 
				
				value4 = API_SetAVDevice(value, value3);
				if(value4 == STATUS_OK)
				{
					printf("OK\n");
				}	
				else
					printf("get error %d\n", value4);
			break;
			
			case 93:
				init_buffer(&cmd_buf);
				value = ZW_ExploreRequestInclusion(&cmd_buf);
				printf("ret:%d\n", value);
			break;
			
			case 94:
				for(i=0; i<9; i++)
				{
					value = 0x102;
					
					if(i<7)
						value3 = TV_VOLUMEUP;
					else
						value3 = TV_VOLUMEDOWN;
						
					value4 = API_SetAVDevice(value, value3);
					if(value4 == STATUS_OK)
					{
						printf("OK\n");
					}	
					else
						printf("get error %d\n", value4);
					
					sleep(1);
					//usleep(500000);
				}	
			break;
				
			case 95:
/*				value = 1;
				value2 = 2;					
				value3 = API_SetBinarySwitch(value2, value);
				printf("return value:%d\n", value3);
				
				usleep(100000);
				
				value = 0;
				value2 = 2;					
				value3 = API_SetBinarySwitch(value2, value);
				printf("return value:%d\n", value3);
				
				usleep(100000);
				
				value = 1;
				value2 = 2;					
				value3 = API_SetBinarySwitch(value2, value);
				printf("return value:%d\n", value3);
				
				usleep(100000);
				
				value = 0;
				value2 = 2;					
				value3 = API_SetBinarySwitch(value2, value);
				printf("return value:%d\n", value3);
*/			break;
			
			case 96:
/*				value = 4;
				PrepareBasicSet(&txBuf, BASIC_ON);

				value2 = 3;
				PrepareMultilevelSet( &txBuf2, 99 );

				if(ZW_SendData_test(&test_cmd1, value, (BYTE*)&txBuf, sizeof(txBuf.ZW_BasicSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
					printf("Queue full\n");
				
				if(ZW_SendData_test(&test_cmd2, value2, (BYTE*)&txBuf2, sizeof(txBuf2.ZW_SwitchMultilevelSetFrame), (TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE), SendCompleted) == FALSE)
					printf("Queue full2\n");
*/			break;
	
			case 97:
				init_buffer(&cmd_buf);
				ZW_SoftReset(&cmd_buf);
			break;
	
			case 98:
				value = API_ResetToDefault();
				printf("return value:%d\n", value);
			break;
			
			case 99:
				printf("Terminate all existing thread\n");
				
				if(thread_zwave_flag)
				{						
					pthread_cancel(zwave_rx_thread);
					close(comfd);
					is_zwave_open = 0;
				}
				
				if(thread_zwave_apipoll_flag)
					pthread_cancel(zwave_apipoll_thread);

				if(thread_zwave_timer_flag)
					pthread_cancel(zwave_timer_thread);
				
				if(thread_zwave_execallback_flag)
					pthread_cancel(zwave_rxdata_thread);
						
				thread_zwave_apipoll_flag = 0;				
				thread_zwave_flag = 0;
				thread_zwave_timer_flag = 0;
				thread_zwave_execallback_flag = 0;				
			break;
			
			default:										
				printf("error input\n");
			break;	
		}
		
		usleep(20000);		
	}			
}


