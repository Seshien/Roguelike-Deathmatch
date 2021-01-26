#include "Contact.h"

void Contact::initClient(int clientId, SOCKET clientSocket, sockaddr_in clientInfo)
{
	this->playerId = clientId;
	this->clientSocket = clientSocket;
	this->clientInfo = clientInfo;
	this->timeoutTimer = 0;
	this->msgExpectedLenght = -1;
}
