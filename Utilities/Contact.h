#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Config.h"
class Contact
{
public:

	void initClient(int clientId, SOCKET clientSocket, sockaddr_in clientInfo)
	{
		this->playerId = clientId;
		this->clientSocket = clientSocket;
		this->clientInfo = clientInfo;
		this->timeoutTimer = 0;
		this->msgExpectedLenght=-1;
	}
	int playerId;
	SOCKET clientSocket;
	sockaddr_in clientInfo;
	int timeoutTimer;
	std::string bufferInput;
	std::string bufferOutput;
	//char bufferInput[Constants::bufferLength];
	//char bufferOutput[Constants::bufferLength];
	int bufferInputCounter;
	int bufferOutputCounter;
	int msgExpectedLenght;

private:

};



