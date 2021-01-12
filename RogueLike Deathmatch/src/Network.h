#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <string>
#include <chrono>

#include "Logger.h"
#include "Client.h"
#include "Parser.h"


class Network
{
public:
	Network();
	~Network()
	{
		WSACleanup();
	}
	int startServer(std::string port);
	int startClient(std::string ipAdress, std::string port);

	Parser::Messenger inputNetwork();
	//moze przechowywac eventy do zrobienia w outpucie zamiast bufforach klientow? chociaz nie wiem jak by to mialo dzialac
	//wtedy bysmy zamiast zastepowac outputa, appendowaæ zawartoœæ nowego do starego
	void outputNetwork(Parser::Messenger _output);
private:
	//Client clientList[16];
	std::vector <Client> clientList;
	SOCKET mainSocket;
	std::string ipAddress;
	std::string port;
	std::vector<pollfd> descrList;
	//pollfd descrList[17];

	//decrease or increase, depending on connected clients, obsoleted
	//int clientAmount;

	Parser::Messenger input;
	Parser::Messenger output;
	int networkID;
	bool isServer;
	bool isClient;
	// player id, only increases so two players never have the same id
	// przepraszam za ten polnglish
	// teoretycznie mozemy sprawdzac id lub cus, i jezeli jest takie samo to nadawac to samo player id 
	int playerID;


	std::chrono::system_clock::time_point timer = std::chrono::system_clock::now();

	void handleEvent(Parser::Event ev);

	int createServerSocket();
	int createClientSocket();

	void manageEvents(int events);

	void manageConnectionEvent();

	void manageListenEvent();
	void acceptClient();


	void manageClientEvent(int descrIndex);

	void readFromClient(int index);
	void readFromClient(Client* client);

	void sendToClient(Client* client);

	void deleteClient(int index);
	void increaseTimeout();
};

