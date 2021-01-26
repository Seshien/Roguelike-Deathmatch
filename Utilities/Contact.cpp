#include "Contact.h"

void Contact::initContact(int clientId, SOCKET clientSocket, sockaddr clientInfo)
{
	this->playerId = clientId;
	this->clientSocket = clientSocket;
	this->clientInfo = clientInfo;
	this->timeoutTimer = 0;
	this->msgExpectedLenght = -1;
}

std::string Contact::info()
{
	char ip[INET6_ADDRSTRLEN] = { 0 };

	switch (clientInfo.sa_family) {
	case AF_INET: {
		// use of reinterpret_cast preferred to C style cast
		sockaddr_in *sin = reinterpret_cast<sockaddr_in*>(&clientInfo);
		inet_ntop(AF_INET, &sin->sin_addr, ip, INET6_ADDRSTRLEN);
		break;
	}
	case AF_INET6: {
		sockaddr_in6 *sin = reinterpret_cast<sockaddr_in6*>(&clientInfo);
		// inet_ntoa should be considered deprecated
		inet_ntop(AF_INET6, &sin->sin6_addr, ip, INET6_ADDRSTRLEN);
		break;
	}
	default:
		return std::string();
	}
	return std::string(ip);
}
