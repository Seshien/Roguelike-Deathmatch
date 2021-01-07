#include "Network.h"


Network::Network()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		Logger::log("WSAStartup failed", iResult);
	}
}

int Network::startNetwork(std::string port)
{
	this->port = port;
	//serwer ma player id 0, a wiec pierwszy mozliwy playerId to 1
	this->playerID = 1;
	this->input = Parser::Messenger();
	this->output = Parser::Messenger();


	createServerSocket();
	//updateDescrList();
	return 0;
}

Parser::Messenger Network::inputNetwork()
{
	//timeout to bedzie kiedy cos ostatnio dzialo sie z klientem
	// jezeli timeout bedzie duzy to wyslac wiadomosc sprawdzajaca czy klient dziala 
	this->input = Parser::Messenger();
	this->increaseTimeout();
	int result = WSAPoll(&this->descrList[0], this->descrList.size(), 0);
	if (result == -1) {
		Logger::log("Poll failed\n");
		Logger::logNetworkError();
	}
	else if (result > 0) this->manageEvents(result);

	//Zwroc parser albo liste eventow
	return input;
}

//moze przechowywac eventy do zrobienia w outpucie zamiast bufforach klientow? chociaz nie wiem jak by to mialo dzialac
//wtedy bysmy zamiast zastepowac outputa, appendowaæ zawartoœæ nowego do starego

void Network::outputNetwork(Parser::Messenger _output)
{
	//updateDescrList();
	this->output = _output;
	for (auto & ev : this->output.eventList) {
		handleEvent(ev);
	}
	for (auto & client : this->clientList)
		if (client.bufferOutput.size() > 0)
			sendToClient(&client);

}

void Network::handleEvent(Parser::Event ev)
{
	//char *data = output.encodeBytes(ev);
	int receiver = ev.receiver;
	for (auto &client : clientList) {
		if (client.playerId == receiver)
		{
			std::string data = Parser::encodeBytes(ev);
			client.bufferOutput.append(data);
		}
	}
}

int Network::createServerSocket()
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
	pollfd fd = pollfd();
	fd.fd = listenSocket;
	fd.events = POLLIN;
	descrList.push_back(fd);

	//Everything worked


	return 0;
}
//Raczej to nie bedzie potrzebne po updacie
void Network::updateDescrList()
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

void Network::manageEvents(int events)
{
	//ktos dolacza
	if (this->descrList[0].revents)
	{
		manageServerEvent();
		events--;
	}
	//klient cos robi
	for (int i = 1; i < this->descrList.size() && events > 0; i++) {
		if (this->descrList[i].revents) {
			manageClientEvent(i);
			events--;
		}
	}

}

void Network::manageServerEvent()
{
	auto revent = this->descrList[0].revents;
	if (revent & POLLIN)
	{
		acceptClient();
	}

}

void Network::acceptClient()
{
	sockaddr_in infoStorage;
	socklen_t addressSize = sizeof(infoStorage);
	auto client = Client();


	auto clientFd = accept(this->listenSocket, (struct sockaddr*)& infoStorage, &addressSize);
	if (clientFd == -1)
	{
		Logger::log("Accepting new client failed");
		Logger::logNetworkError();
		return;
	}

	unsigned long mode = 1;

	if (ioctlsocket(clientFd, FIONBIO, &mode) != 0)
	{
		Logger::log("ioctl failed\n");
		Logger::logNetworkError();
		return;
	}
	pollfd fd = pollfd();
	fd.fd = clientFd;
	fd.events = POLLIN;
	descrList.push_back(fd);


	client.initClient(playerID++, clientFd, infoStorage);


	Logger::log("New connection accepted");
	//std::string message = std::string(inet_ntoa(address->sin_addr)) + ":" + std::to_string(ntohs(address->sin_port)) 
	//	+ ":" + std::to_string(clientFd);
	//Logger::log(message);
	//Parser wiadomosc o przyjeciu nowego gracza
	input.addEventNewPlayer(playerID, 0);
	this->clientList.push_back(client);
}

void Network::manageClientEvent(int descrIndex)
{
	auto sock = this->descrList[descrIndex].fd;
	auto revent = descrList[descrIndex].revents;
	int index = -1;
	Client * managed = nullptr;
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
		for (int i = 0; i<clientList.size(); i++)
			if (clientList[i].clientSocket = sock)
			{
				managed = &clientList[i];
				index = i;
				break;
			}
	}

	if (managed == nullptr)
	{
		Logger::log("Client not found. Socket index:", int(sock));
		return;
	}
	auto message = "Client: " + std::to_string(index) + " player_id:" + std::to_string(managed->playerId)
		+ " socket: " + std::to_string(sock) + " event";
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

void Network::readFromClient(int index)
{
	Client* currentClient = &clientList[index];
	readFromClient(currentClient);
}

void Network::readFromClient(Client* currentClient)
{
	//nie znamy jeszcze dlugosci wiadomosci
	if (currentClient->msgExpectedLenght = -1)
	{
		int result = recv(currentClient->clientSocket, &currentClient->bufferInput[0] + currentClient->bufferInputCounter, Constants::msgLengthLimit - currentClient->bufferInputCounter, 0);

		Logger::log("Przyjeto " + std::to_string(result) + " Razem " + std::to_string(result + currentClient->bufferInputCounter)
			+ " Pozostalo " + std::to_string(Constants::msgLengthLimit - (currentClient->bufferInputCounter + result)));
		Logger::log("Tresc " + std::string(currentClient->bufferInput));
		if (result == -1 || result == 0) 
		{
			Logger::log("Recv error on client: %d\n", currentClient->playerId);
		}
		else 
		{
			currentClient->bufferInputCounter += result;
		}
		int pos = currentClient->bufferInput.find('|');
		if (pos != -1)
		{
			//bierzemy bez znaku |
			currentClient->msgExpectedLenght = std::stoi(currentClient->bufferInput.substr(0, pos));
			//usuwamy takze znak |
			currentClient->bufferInput.erase(0, pos + 1);
			currentClient->bufferInputCounter -= pos + 1;
		}
	}
	//znamy dlugosc wiadomosci
	else
	{
		int result = recv(currentClient->clientSocket, &currentClient->bufferInput[0] + currentClient->bufferInputCounter, currentClient->msgExpectedLenght - currentClient->bufferInputCounter, 0);
		//jezeli je za krotka zapisac w bufferze klienta i czekac za reszt¹
		Logger::log("Przyjeto " + std::to_string(result) + " Razem " + std::to_string(result + currentClient->bufferInputCounter)
			+ " Pozostalo " + std::to_string(Constants::msgLengthLimit - (currentClient->bufferInputCounter + result)));
		Logger::log("Tresc " + std::string(currentClient->bufferInput));
		if (result == -1 || result == 0) {
			Logger::log("Recv error on client: %d\n", currentClient->playerId);
		}
		else {
			currentClient->bufferInputCounter += result;
		}
		if (currentClient->bufferInputCounter == currentClient->msgExpectedLenght) {
			Parser::Event ev = Parser::decodeBytes(currentClient->bufferInput);
			//zrobic funkcje ktora lepiej sprawdza to czy wiadomosc dziala TODO
			if (ev.receiver != 0)
			{
				Logger::log("Wrong message destination. ID:Expected " + std::to_string(ev.receiver) + ":0");
				ev.receiver = 0;
			}
			if (ev.sender != currentClient->playerId)
			{
				Logger::log("Wrong message sender on client. ID:Expected " + std::to_string(ev.sender) + ":" + std::to_string(currentClient->playerId));
				ev.sender = currentClient->playerId;
			}
			//Parser wiadomosc o dostaniu informacji 
			input.addEvent(ev);
			currentClient->bufferInput.erase(0, currentClient->msgExpectedLenght);
			currentClient->bufferInputCounter = 0;
			currentClient->msgExpectedLenght = -1;
		}
	}

}

void Network::sendToClient(Client * client) {
	int result = send(client->clientSocket, &client->bufferOutput[0], client->bufferOutput.size(), 0);
	if (result == SOCKET_ERROR) 
	{
		Logger::log("Send error on player id: %d\n", client->playerId);
	}
	else 
	{
		Logger::log("Wyslano " + std::to_string(result) + " Razem " + std::to_string(result + client->bufferInputCounter)
			+ " Pozostalo " + std::to_string(Constants::msgLengthLimit - (client->bufferInputCounter + result)));
		// Przesuwanie bufora wzgledem ilosci wyslanych juz znakow (tak, aby zawsze wysylac go wzgledem poczatku)
		client->bufferOutput.erase(0, result);
		//to ze wiadomosc dotarla uznajemy przynajmniej na razie ze jest dobrze
		client->timeoutTimer = 0;
	}
}

void Network::deleteClient(int index)
{
	int playerIndex = clientList[index].playerId;
	clientList.erase(clientList.begin() + index);
	descrList.erase(descrList.begin() + index);
	//Parser wiadomosc o usunieciu playera
	input.addEventDiscPlayer(playerIndex, 0);
}

void Network::increaseTimeout()
{
	for (int i = 0; i < this->clientList.size(); i++)
	{
		clientList[i].timeoutTimer++;
		if (clientList[i].timeoutTimer > Constants::timeoutValue * 2) this->deleteClient(i);
		else if (clientList[i].timeoutTimer > Constants::timeoutValue) this->input.addEventTimeoutReached(clientList[i].playerId, 0);
	}

}
