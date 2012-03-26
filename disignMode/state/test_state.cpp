#include "TCPConnection.h"
#include "TCPEstablished.h"
#include "TCPListen.h"
#include "TCPClosed.h"


int main()
{
	TCPConnection* pconnect = new TCPConnection();
	pconnect->active_open();
	pconnect->active_open();
	pconnect->active_open();
	pconnect->active_open();
	pconnect->active_open();
}
