#ifndef _ZDEMO_H_
#define _ZDEMO_H_

pthread_t gzwave_rx_thread,gzwave_apipoll_thread, gzwave_timer_thread, gzwave_rxdata_thread;
#define UPDATE_ALL_NODE_ID -100
#define TEST_RESET_TO_DEFAULT 1001

tcmRet startRecvbySelect();
tcmRet initZwave(int usbComNum);
tcmRet APResetToDefault();
tcmRet updateZunitStatus();
tcmRet APStartAddNodeToNetwork();
tcmRet APStopAddNodeToNetwork();
tcmRet APStartRemoveNodeFromNetwork();
tcmRet APStopRemoveNodeFromNetwork();
void deleteZwaveApp();
tcmRet APResetToDefault();
int gaddDeviceProfileID;

#endif
