#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Config.h"
class Contact
{
public:

	void initContact(int clientId, SOCKET clientSocket, sockaddr clientInfo);
	std::string info();
	
	int playerId;
	SOCKET clientSocket;
	sockaddr clientInfo;
	int timeoutTimer;
	std::string bufferInput;
	std::string bufferOutput;
	//char bufferInput[Constants::bufferLength];
	//char bufferOutput[Constants::bufferLength];
	int bufferInputCounter;
	int bufferOutputCounter;
	size_t msgExpectedLenght;

private:

};



