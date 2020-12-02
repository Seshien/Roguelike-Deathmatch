#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
class Client
{
public:

	void initClient(int clientId, SOCKET clientSocket, struct addrinfo clientInfo)
	{
		this->playerId = clientId;
		this->clientSocket = clientSocket;
		this->clientInfo = clientInfo;
		this->timeoutTimer = 0;
	}
	int playerId;
	SOCKET clientSocket;
	struct addrinfo clientInfo;
	int timeoutTimer;
private:

};

