#include "wServices.h"

int WinClient::sendMessage(SOCKET curSocket, char * message, int messageSize)
{
	return send(curSocket, message, messageSize, 0);
}

int WinClient::receiveMessage(SOCKET curSocket, char * buffer, int bufSize)
{
	return recv(curSocket, buffer, bufSize, 0);
}