#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>

#include "Logger.h"
#include "Client.h"
#include "Parser.h"


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
		//serwer ma player id 0, a wiec pierwszy mozliwy playerId to 1
		playerID = 1;

		WSADATA wsaData;
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			Logger::log("WSAStartup failed", iResult);
			return 1;
		}
		createServerSocket();
		updateDescrList();

	}
	Parser inputNetwork()
	{
		//timeout to bedzie kiedy cos ostatnio dzialo sie z klientem
		// jezeli timeout bedzie duzy to wyslac wiadomosc sprawdzajaca czy klient dziala 
		this->input = Parser();
		increaseTimeout();
		int result = WSAPoll(this->descrList, this->clientAmount + 1, 0);
		if (result == -1) {
			Logger::log("Poll failed\n");
			Logger::logNetworkError();
		}
		else if (result > 0) this->manageEvents(result);

		//Zwroc parser albo liste eventow
		return input;
	}
	void outputNetwork(Parser _output)
	{
		updateDescrList();
		this->output = _output;
		for (auto ev : this->output.eventList) {
			handleEvent(ev);
		}
		for (auto client : this->clientList) {
			sendToClient(&client);
		}
		
		//Zwroc parser albo liste eventow
	}
private:
	//Client clientList[16];
	std::vector <Client> clientList;
	SOCKET listenSocket;
	std::string port;
	pollfd descrList[17];

	//decrease or increase, depending on connected clients
	int clientAmount;

	Parser input;
	Parser output;
	// player id, only increases so two players never have the same id
	// przepraszam za ten polnglish
	// teoretycznie mozemy sprawdzac id lub cus, i jezeli jest takie samo to nadawac to samo player id 
	int playerID;
	void handleEvent(Parser::Event ev) 
	{
		//char *data = output.encodeBytes(ev);
		int receiver = ev.receiver;
		for (auto &client : clientList) {
			if (client.playerId == receiver - 47) {
				strcpy_s(client.bufferOutput + client.bufferOutputCounter, Constants::msgLength, output.encodeBytes(ev));
				client.msgNumberToSend += 1;
			}
		}
	}

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
		this->descrList[0].fd = listenSocket;
		this->descrList[0].events = POLLIN;
		int i = 1;
		for (auto & client : this->clientList)
		{
			this->descrList[i].fd = client.clientSocket;
			this->descrList[i].events = POLLIN;
			i++;
		}
	}

	void manageEvents(int events)
	{
		//ktos dolacza
		if (this->descrList[0].revents)
		{
			manageServerEvent();
			events--;
		}
		//klient cos robi
		for (int i = 1; i <= this->clientAmount && events > 0; i++) {
			if (this->descrList[i].revents) {
				manageClientEvent(i);
				events--;
			}
		}

	}
	void manageServerEvent()
	{
		auto revent = this->descrList[0].revents;
		if (revent & POLLIN) 
		{
			acceptClient();
		}

	}
	void acceptClient()
	{
		sockaddr_in infoStorage;
		addrinfo result;
		socklen_t addressSize = sizeof(infoStorage);
		auto client = Client();


		auto clientFd = accept(this->listenSocket, (struct sockaddr*)& infoStorage, &addressSize);
		if (clientFd == -1)
		{
			Logger::log("Accepting new client failed");
			Logger::logNetworkError();
			return;
		}
		result.ai_addr = (sockaddr*)&infoStorage;
		result.ai_addrlen = addressSize;

		unsigned long mode = 1;

		if (ioctlsocket(clientFd, FIONBIO, &mode) != 0)
		{
			Logger::log("ioctl failed\n");
			Logger::logNetworkError();
			return;
		}
		this->clientAmount++;
		descrList[this->clientAmount].fd = clientFd;
		descrList[this->clientAmount].events = POLLIN;


		client.initClient(playerID++, clientFd, result);

		auto address = (sockaddr_in*)result.ai_addr;

		Logger::log("New connection accepted");
		//std::string message = std::string(inet_ntoa(address->sin_addr)) + ":" + std::to_string(ntohs(address->sin_port)) 
		//	+ ":" + std::to_string(clientFd);
		//Logger::log(message);

		//Parser wiadomosc o przyjeciu nowego gracza
		this->clientList.push_back(client);
	}


	void manageClientEvent(int descrIndex)
	{
		auto sock = this->descrList[descrIndex].fd;
		auto revent = descrList[descrIndex].revents;
		int index = -1;
		Client * managed = NULL;
		//ogolnie descrList  0 to jest serwer
		int clientIndex = descrIndex - 1;
		//tutaj szukam klienta dla ktorego jest to wydarzenie, sprawdzajac po socketach
		//moze byc nie potrzebne, ale wolalem to zrobic dla sanity
		if (clientList[clientIndex].clientSocket == sock)
		{
			managed = &clientList[clientIndex];
			index = clientIndex;
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
		auto message = "Client: " + std::to_string(index) + " player_id:" + std::to_string(managed->playerId) 
			+ " socket: "+ std::to_string(sock) +" event";
		Logger::log(message);

		if (revent & POLLIN)
		{
			readFromClient(index);
		}
		if (revent & POLLHUP)
			deleteClient(index);

		// resetujemy timeout, bo cos sie stalo z klientem
		managed->timeoutTimer = 0;
	}
	void readFromClient(int index)
	{
		Client* currentClient = &clientList[index];
		int result = recv(currentClient->clientSocket, currentClient->bufferInput + currentClient->bufferInputCounter, Constants::msgLength - currentClient->bufferInputCounter, 0);
		//jezeli je za krotka zapisac w bufferze klienta i czekac za reszt¹
		Logger::log("Przyjeto " + std::to_string(result) + " Razem " + std::to_string(result + currentClient->bufferInputCounter)
			+ " Pozostalo " + std::to_string(Constants::msgLength - (currentClient->bufferInputCounter + result)));
		Logger::log("Tresc " + std::string(currentClient->bufferInput));
		if (result == -1 || result == 0) {
			Logger::log("Recv error on client: %d\n", index);
		}
		else {
			currentClient->bufferInputCounter += result;
		}
		//Parser wiadomosc o dostaniu informacji 
		if (currentClient->bufferInputCounter == Constants::msgLength) {
			input.addEvent(input.decodeBytes(currentClient->bufferInput));
			currentClient->bufferInputCounter = 0;
		}
	}

	void sendToClient(Client* client) {
		if (client->msgNumberToSend == 0) {
			return;
		}
		int result = send(client->clientSocket, client->bufferOutput, Constants::msgLength - client->bufferOutputCounter, 0);
		if (result == SOCKET_ERROR) {
			Logger::log("Send error on player id: %d\n", client->playerId);
		}
		else {
			// Przesuwanie bufora wzgledem ilosci wyslanych juz znakow (tak, aby zawsze wysylac go wzgledem poczatku)
			strcpy_s(client->bufferOutput, Constants::bufferLength - client->bufferOutputCounter, client->bufferOutput + client->bufferOutputCounter);
			client->bufferOutputCounter += result;
			if (client->bufferOutputCounter == Constants::msgLength) {
				client->bufferOutputCounter = 0;
				client->msgNumberToSend -= 1;
			}
		}
	}

	void deleteClient(int index)
	{
		int playerIndex = clientList[index].playerId;
		clientList.erase(clientList.begin() + index);
		//Parser wiadomosc o usunieciu playera
	}
	void increaseTimeout()
	{
		for (auto & client : this->clientList)
			client.timeoutTimer++;
	}
};

