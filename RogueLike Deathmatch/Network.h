#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <stdio.h>

#include "Logger.h"
#include "Client.h"

class Network
{
public:
	Network()
	{

	}
	~Network()
	{
		WSACleanup();
	}
	int startNetwork(std::string port)
	{
		this->clientAmount = 0;
		this->port = port;
		playerID = 0;

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
	//Client clientList[16];
	std::vector <Client> clientList;
	SOCKET listenSocket;
	std::string port;
	pollfd descrList[17];

	//decrease or increase, depending on connected clients
	int clientAmount;

	//player id, only increases so two players never have the same id
	int playerID;

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
	void manageServerEvent()
	{
		auto revent = descrList[0].revents;
		if (revent & POLLIN) 
		{
			acceptClient();
		}

	}
	void acceptClient()
	{
		struct addrinfo result;
		auto client = Client();

		auto clientFd = accept(this->listenSocket, (sockaddr*)&result.ai_addr, (int *)&result.ai_addrlen);
		unsigned long mode = 1;
		ioctlsocket(clientFd, FIONBIO, &mode);

		if (clientFd == -1)
		{
			Logger::log("Accepting new client failed");
			return;
		}

		descrList[this->clientAmount].fd = clientFd;
		descrList[this->clientAmount].events = POLLIN;
		this->clientAmount++;

		client.initClient(playerID++, clientFd, result);

		auto address = (sockaddr_in*)result.ai_addr;

		Logger::log("New connection accepted");
		std::string message = std::string(inet_ntoa(address->sin_addr)) + ":" + std::to_string(ntohs(address->sin_port)) 
			+ ":" + std::to_string(clientFd);
		Logger::log(message);

		//Parser wiadomosc o przyjeciu nowego gracza
		this->clientList.push_back(client);
	}

	void manageClientEvent(int descrIndex)
	{
		auto sock = this->descrList[descrIndex].fd;
		int index = -1;
		Client * managed = NULL;
		if (clientList[descrIndex].clientSocket == sock)
		{
			managed = &clientList[descrIndex];
			index = descrIndex;
		}
		else
		{
			for (int i=0;i<clientList.size();i++)
				if (clientList[i].clientSocket = sock)
				{
					managed = &clientList[i];
					index = i;
					break;
				}
		}

		if (managed == NULL)
		{
			Logger::log("Client not found. Socket index:", int(sock));
			return;
		}

		auto revent = descrList[descrIndex].revents;
		if (revent & POLLIN)
		{
			readFromClient(index);
		}
		if (revent & POLLHUP)
			deleteClient(index);
	}
	void readFromClient(int index)
	{
		
		//Parser wiadomosc o dostaniu informacji 
	}

	void deleteClient(int index)
	{
		int playerIndex = clientList[index].playerId;
		clientList.erase(clientList.begin() + index);
		//Parser wiadomosc o usunieciu playera
	}
};

