#pragma once
#include <winsock2.h>
#include <Windows.h>

//make a class for easier use of Winsock
class WinClient
{

public:

	static int sendMessage(SOCKET curSocket, char * message, int messageSize); //Send
	static int receiveMessage(SOCKET curSocket, char * buffer, int bufSize); // Recieve

};