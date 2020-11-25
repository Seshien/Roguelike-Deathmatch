#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "Logger.h"
#include "Client.h"
class Network
{
public:
	Network()
	{

	}
	int startNetwork(std::string port)
	{
		this->clientAmount = 0;
		this->port = port;

		WSADATA wsaData;
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			Logger::log("WSAStartup failed", iResult);
			return 1;
		}

	}
private:
	Client clientList[16];
	SOCKET listenSocket;
	std::string port;
	pollfd descrList[17];
	int clientAmount;
	int createServerSocket()
	{
		this->listenSocket = INVALID_SOCKET;

		struct addrinfo *result = NULL;
		struct addrinfo hints;
		int iResult;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
		if (iResult != 0) {
			Logger::log("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Create a SOCKET for connecting to server
		listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (listenSocket == INVALID_SOCKET) {
			Logger::log("Socket failed", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}


		// Setup the TCP listening socket
		iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			Logger::log("Bind failed", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);

		iResult = listen(listenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			Logger::log("Listen failed", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		//Everything worked
		return 0;
	}
	void updateDescrList()
	{
		this->descrList->fd = listenSocket;
		this->descrList->events = POLLIN;
		int i = 1;
		for (auto & client : this->clientList)
		{
			this->descrList[i].fd = client.clientSocket;
			this->descrList[i].events = POLLIN;
			i++;
		}
	}
	void loopNetwork()
	{
		updateDescrList();
		int result = WSAPoll(this->descrList, this->clientAmount + 1, 0);
		if (result == -1) {
			Logger::log("Poll failed");
		}
		else if (result > 0) this->manageEvents(result);
	}
	void manageEvents(int events)
	{
		for (int i = 0; i <= this->clientAmount && events > 0; i++) {
			if (this->descrList[i].revents) {
				if (this->descrList[i].fd == this->listenSocket)
					manageServerEvent();
				else
					manageClientEvent(i);
				events--;
			}
		}

	}
	void manageServerEvent();
	void manageClientEvent(int index);
};

