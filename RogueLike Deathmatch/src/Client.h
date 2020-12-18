#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Constants.h"
class Client
{
public:

	void initClient(int clientId, SOCKET clientSocket, struct addrinfo clientInfo)
	{
		this->playerId = clientId;
		this->clientSocket = clientSocket;
		this->clientInfo = clientInfo;
		this->timeoutTimer = 0;
		this->msgNumberToSend = 0;
	}
	int playerId;
	SOCKET clientSocket;
	struct addrinfo clientInfo;
	int timeoutTimer;
	char bufferInput[Constants::bufferLength];
	char bufferOutput[Constants::bufferLength];
	int bufferInputCounter;
	int bufferOutputCounter;
	int msgNumberToSend;
private:

};



