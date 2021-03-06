#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <string>

#include "Event.h"
#include "Logger.h"
#include "Contact.h"
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

	Parser::Messenger inputNetwork(double time = 0.1);
	//moze przechowywac eventy do zrobienia w outpucie zamiast bufforach klientow? chociaz nie wiem jak by to mialo dzialac
	//wtedy bysmy zamiast zastepowac outputa, appendować zawartość nowego do starego
	void outputNetwork(Parser::Messenger _output);
private:
	std::vector <Contact> clientList;
	SOCKET mainSocket;
	std::string ipAddress;
	std::string port;
	std::vector<pollfd> descrList;

	//decrease or increase, depending on connected clients, obsoleted
	//int clientAmount;

	Parser::Messenger input;
	Parser::Messenger output;
	int networkID;
	bool isServer;
	bool isClient;
	// player id, only increases so two players never have the same id
	// przepraszam za ten ponglish
	// teoretycznie mozemy sprawdzac id lub cus, i jezeli jest takie samo to nadawac to samo player id 
	int playerID;

	void handleOutputEvent(Parser::Event ev);
	void handleInnerEvent(Parser::Event ev);

	int createServerSocket();
	int createClientSocket();

	void manageEvents(int events);

	void manageConnectionEvent();

	void manageListenEvent();
	void acceptClient();


	void manageClientEvent(int descrIndex);

	void readFromClient(int index);
	void readFromClient(Contact* client);

	void sendToClient(Contact* client);

	void deleteClient(int index);
	void deleteClient(Contact* client);

	void processMessage(Contact* client);
	int checkMessage(Contact* client, Parser::Event * ev);

	void increaseTimeout();
	Contact * FindClient(int index, SOCKET sock);
};
