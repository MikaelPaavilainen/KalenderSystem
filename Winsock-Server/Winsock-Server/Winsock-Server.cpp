///This file will start and restart command after everytime the command has been entered.
///Because it uses packets and binds from the user it will use it as a indentifier and makes server multiclient-able

#undef UNICODE
#define BOOST_BIND_ENABLE_STDCALL
#define WIN32_LEAN_AND_MEAN



#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>



#include "mysql_connection.h"
#include <cppconn/resultset.h>




// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512 //Lenght Of Messages that can be send, make sure client and server uses same once
#define DEFAULT_PORT "1234" //Default port of server, make sure client uses same.



//Global Variables
WSADATA wsaData;
int iResult;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;

struct addrinfo *result = NULL;
struct addrinfo hints;

int iSendResult;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

int sendError() {
	char eList[256] = "Server:\nNot A Valid Command!\nType Help For Commands"; //This will be sent when user types in command !help.
	printf("Server: Error\n");
	//Sends buffer to client
	int commandList = send(ClientSocket, eList, sizeof(eList) - 1, NULL);
	//check if valid
	if (commandList == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	//DEBUG
	/*printf("Bytes sent: %d\n", commandList);
	printf("string sent: %s\n", eList);*/
}

//Mysql const sql::SQLString& hostName, const sql::SQLString& userName, const sql::SQLString& password
void mysqlSetup(sql::SQLString& hostName, const sql::SQLString& userName, const sql::SQLString& password) {
	try {
		sql::Driver *driver; //Select Mysql as driver
		sql::Connection *con; //connection to database
		sql::Statement *stmt; //statement to send to database
		sql::ResultSet *res; //result from database

		/* Create a connection */
		//std::string ip = "tcp://" + ip;
		//printf("IP: %s", hostName);
		//printf("IP: %s", userName);

		driver = get_driver_instance();
		con = driver->connect(hostName, userName, password);

		/* Connect to the MySQL database */
		con->setSchema("test");//Database name

		stmt = con->createStatement(); //init statement
		res = stmt->executeQuery("show tables"); //request string
		std::cout << "\n\n\t... ~MySQL Loaded~ ...\n";
		std::cout << "\t______________________\n\t______________________\n\n\t-Tables-\n\t______________________\n";
		while (res->next()) { //looped result, all results will be displayed
			/* Access column fata by numeric offset, 1 is the first column */
			std::cout << "\t.....[" <<res->getString("Tables_in_test") << "]....." << std::endl;
		}
		std::cout << "\t______________________\n";

		delete res;
		delete stmt;
		delete con;

	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line »" << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}

}

int serverSetup() {

	std::string username;
	std::string password;

	std::string line;
	std::ifstream myfile("config.ini");
	if (myfile.is_open())
	{
		for (int lineno = 0; getline(myfile, line) && lineno < 7; lineno++) 
		{
			if (lineno == 0) //Username
			{
				username = line;
				//std::cout << line << std::endl;
			}
			if (lineno == 1) //Password
			{
				password = line;

				//std::cout << line << std::endl;
			}
			/*Not needed
			if (lineno == 2) //server port
			{
				password = line;
			}
			if (lineno == 3) //Mysql IP+port
			{
				password = line;
			}
			if (lineno == 4) //Mysql userName
			{
				password = line;
			}
			if (lineno == 5) //Mysql userName
			{
				password = line;
			}*/
		}
		myfile.close();
	}
	printf("Please Enter Username:");
	std::string enteredUsername;
	std::string enteredPassword;

	
	std::cin >> enteredUsername;
	if (enteredUsername == username)
	{
		printf("Please Enter Password:");
		std::cin >> enteredPassword;
		if (enteredPassword == password)
		{
			//new username
			char username; //temp hold name
			std::ofstream myfile;
			myfile.open("config.ini");
			printf("\nEnter New Username:");
			std::cin >> &username;
			std::cout << &username << std::endl;
			myfile.write(&username, (unsigned)strlen(&username));
			myfile.close();
			//Spacer
			myfile.open("config.ini", std::ofstream::app);
			myfile.write("\n", 1);
			myfile.close();
			//new password
			char password; //temp hold name
			myfile.open("config.ini", std::ofstream::app);
			printf("\nEnter New Password:");
			std::cin >> &password;
			std::cout << &password << std::endl;
			myfile.write(&password, (unsigned)strlen(&password));
			myfile.close();
			//Spacer
			myfile.open("config.ini", std::ofstream::app);
			myfile.write("\n", 1);
			myfile.close();
			//Server Setup
			printf("Would You Like To Run Server-Setup?\n(y/n)\n");
			char answer; //char hold choice 1
			char port; //temp hold port
			std::cin >> answer;
			if (answer == 'yes' || answer == 'y' || answer == 'YES' || answer == 'Y')
			{
				std::ofstream myfile;
				myfile.open("config.ini", std::ofstream::app);
				printf("\nEnter Port(ex 1234):");
				std::cin >> &port;
				std::cout << &port << std::endl;
				

				myfile.write(&port, (unsigned)strlen(&port));
				myfile.close(); 
				printf("Would You Like To Run Mysql-Setup?\n(y/n)\n");
				
				char answer2; //char hold choice 1
				char tempip; //temp hold ip + port
				char tempuserName; //temp hold userName
				char temppassword; //temp hold password
				std::cin >> answer2;
				if (answer2 == 'yes' || answer2 == 'y' || answer2 == 'YES' || answer2 == 'Y')
				{
					//Spacer
					myfile.open("config.ini", std::ofstream::app);
					myfile.write("\n", 1);
					myfile.close();
					//mysql ip
					myfile.open("config.ini", std::ofstream::app);
					printf("\nEnter Mysql IP/Url + port (ex 127.0.0.1:3306):");
					std::cin >> &tempip;
					std::cout << &tempip << std::endl;
					std::cout << (unsigned)strlen(&tempip)+1 << std::endl;
					myfile.write(&tempip, (unsigned)strlen(&tempip));
					myfile.close();
					//Spacer
					myfile.open("config.ini", std::ofstream::app);
					myfile.write("\n", 1);
					myfile.close();
					//mysql username
					myfile.open("config.ini", std::ofstream::app);
					printf("Enter Mysql login userName:");
					std::cin >> &tempuserName;
					std::cout << &tempuserName << std::endl;
					std::cout << (unsigned)strlen(&tempuserName) << std::endl;
					myfile.write(&tempuserName, (unsigned)strlen(&tempuserName));
					myfile.close();
					//Spacer
					myfile.open("config.ini", std::ofstream::app);
					myfile.write("\n", 1);
					myfile.close();
					//mysql password
					myfile.open("config.ini", std::ofstream::app);
					printf("Enter Mysql login password:");
					std::cin >> &temppassword;
					std::cout << &temppassword << std::endl;
					std::cout << (unsigned)strlen(&temppassword) << std::endl;
					myfile.write(&temppassword, (unsigned)strlen(&temppassword)); 
					myfile.close();
					//Spacer
					myfile.open("config.ini", std::ofstream::app);
					myfile.write("\n", 1);
					myfile.close();

					
					
					printf("\nRestart Server");
					std::cin;


				}
				else if (answer2 == 'no' || answer2 == 'n' || answer2 == 'NO' || answer2 == 'N') {
					printf("no");
				}
			}
			else {
				printf("Mkaj, goodbye!");
			}

		}
		else {
			printf("Wrong Password!!");

		}
	}
	else {
		printf("Wrong Username!!");
	}

	
	return 0;
}
//Checks if conf file exist, if not create one
bool checkFile() {

	if (std::ifstream("config.ini"))
	{
		//cout << "File already exists" << endl;
		return false;
	}
	else {

		return true;
	}
}
//Send command list to user
int sendCommandList() {
	char cList[256] = "CommandList\n!help\n!reset"; //This will be sent when user types in command !help.
	printf("Command List\n");
	int commandList = send(ClientSocket, cList, sizeof(cList) - 1, NULL);
	if (commandList == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	//DEBUG
	/*printf("Bytes sent: %d\n", commandList);
	printf("string sent: %s\n", cList);*/
}

int sendReset() {
	char cReset[256] = "To Reset Please Enter Server Password After Reset!\n!reset:password"; //This will be sent when user types in command !help.
	printf("Reset\n");
	int cResetChat = send(ClientSocket, cReset, sizeof(cReset) - 1, NULL);
	if (cResetChat == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	
}

//StartServer Method
int startServer() {
	
	//server settings
	std::string port;

	PCSTR pPort = "1234";

	//user settings
	std::string username;
	std::string password;

	//read config file
	std::string line;
	std::ifstream myfile("config.ini");
	if (myfile.is_open())
	{
		for (int lineno = 0; getline(myfile, line) && lineno < 7; lineno++)
		{
			if (lineno == 0) //Username
			{
				username = line;
				//std::cout << line << std::endl;
			}
			if (lineno == 1) //Password
			{
				password = line;
				//std::cout << line << std::endl;
			}
			
			if (lineno == 2) //server port
			{
				port = line;
				pPort = port.c_str();
				//std::cout << line << std::endl;
				//std::cout << "pPort: " <<pPort << std::endl;
			}
		}
		myfile.close();
	}
	
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	else {
		printf("\n\nStarted Server With PORT:%s", pPort);
		printf("\n");
	}
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, pPort, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	//iResult = bind(boost::asio);


	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket

	closesocket(ListenSocket);
	
	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		//printf("Bytes received: %s\n", recvbuf);
		
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			printf("Bytes received:%s\n", recvbuf);
			for (size_t i = 0; i < sizeof(recvbuf) - 4; i++)
			{
				if (recvbuf[0] == '!') //Check if buffer starts with a !, else do nothing
				{
					if (recvbuf[i + 1] == 'h' && recvbuf[i + 2] == 'e' && recvbuf[i + 3] == 'l' && recvbuf[i + 4] == 'p') //!help
					{
						sendCommandList();
					}
					if (recvbuf[i + 1] == 'r' && recvbuf[i + 2] == 'e' && recvbuf[i + 3] == 's' && recvbuf[i + 4] == 'e' && recvbuf[i+5] == 't')
					{
						
						if (recvbuf[i + 7] == password.at(i))
						{
							std::cout << "Same password" << std::endl;
							if (remove("config.ini") != 0)
								perror("Error deleting file");
								char cReset[256] = "Reset Failed!\nPlease Contact My Maker"; //This will be sent when user types in command !help.	
								int cResetChat = send(ClientSocket, cReset, sizeof(cReset) - 1, NULL);
								if (cResetChat == SOCKET_ERROR) {
									printf("send failed with error: %d\n", WSAGetLastError());
									closesocket(ClientSocket);
									WSACleanup();
									return 1;
								}
							else
								puts("File successfully deleted");
							char cRr[256] = "Server Reseted!\nServer Will Restart!\nSo Shall The Client\nServer Needs Direct Input"; //This will be sent when user types in command !help.	
							int cRrc = send(ClientSocket, cReset, sizeof(cRr) - 1, NULL);
							if (cRrc == SOCKET_ERROR) {
								printf("send failed with error: %d\n", WSAGetLastError());
								closesocket(ClientSocket);
								WSACleanup();
								return 1;
							}
						}
						else {
							sendReset();
						}
					}
				}
				else { //no !
					i = sizeof(recvbuf);
					sendError();
				}
			}
			
				
				// Echo the buffer back to the sender
			/*char motd[256] = "Welcome! This is message of the day";

			iSendResult = send(ClientSocket, motd, sizeof(motd), NULL);
			//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				
			}
			
				printf("Bytes sent: %d\n", iSendResult);
				printf("string sent: %s\n", motd);*/
			printf("--------------------------\n");

		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	else {
		startServer();
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

int __cdecl main(void)
{
	
	//mysqlSetup("127.0.0.1", "root", "");
	if (checkFile())
	{
		//Read and write from config file
		printf("Couldnt Find Config, Creates one!\nPlease Wait\n");
		std::ofstream myfile;
		myfile.open("config.ini");
		myfile << "root\nroot\n"; //hardcoded username and password for reseted purposes
		myfile.close();
		Sleep(3000);
		serverSetup(); //Add all info to server
	}
	else {
		printf("Loading Config!\n");
		//mysql settings
		std::string mysqlIP;
		std::string mysqlName;
		std::string mysqlPw;

		//read config file
		std::string line;
		std::ifstream myfile("config.ini");
		if (myfile.is_open())
		{
			for (int lineno = 0; getline(myfile, line) && lineno < 7; lineno++)
			{
				
				if (lineno == 3) //Mysql IP+port
				{
					mysqlIP = line;
					std::cout <<"MysqlIP:"<<line << std::endl;
				}
				if (lineno == 4) //Mysql userName
				{
					mysqlName = line;
					std::cout << "MysqlName:" << line << std::endl;
				}
				if (lineno == 5) //Mysql password
				{
					mysqlPw = line;
					std::cout << "Mysqlpw:" << line << std::endl;
				}
				else { //No password on mysql
					//std::cout << "No Password For Mysql" << std::endl;
					mysqlPw = "";
				}
			}
			myfile.close();
		}
		//start connection for database
		mysqlSetup(sql::SQLString(mysqlIP), sql::SQLString(mysqlName), sql::SQLString(mysqlPw));

		startServer();
	}
	
	return 0;
}
