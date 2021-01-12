#include "Network.h"


Network::Network()
{
	WSADATA wsaData;
	int iResult;
	this->isServer = false;
	this->isClient = false;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		Logger::log("WSAStartup failed", iResult);
	}
}

int Network::startServer(std::string port)
{
	this->ipAddress = "127.0.0.1";
	this->port = port;
	//serwer ma player id 0, a wiec pierwszy mozliwy playerId to 1
	this->networkID = 0;
	this->playerID = 1;
	this->isServer = true;
	this->input = Parser::Messenger();
	this->output = Parser::Messenger();


	createServerSocket();
	//updateDescrList();
	return 0;
}

int Network::startClient(std::string ipAdress, std::string port)
{
	this->isClient = true;

	this->ipAddress = ipAddress;
	this->port = port;
	this->input = Parser::Messenger();
	this->output = Parser::Messenger();

	createClientSocket();
	return 0;
}

Parser::Messenger Network::inputNetwork()
{
	//timeout to bedzie kiedy cos ostatnio dzialo sie z klientem
	// jezeli timeout bedzie duzy to wyslac wiadomosc sprawdzajaca czy klient dziala 
	this->input = Parser::Messenger();
	int result = -1;
	this->increaseTimeout();
	//czas w sekundach, ile ma trwac jeden cykl
	double time = 0.1;
	//koniec cyklu
	std::chrono::duration<double> diff = std::chrono::system_clock::now() - timer;
	//czas w sekundach, ile trwal jeden cykl
	auto t = diff.count();
	//jezeli czas trwania cyklu jest dluzszy od ustalonego, WSAPoll jest natychmiastowy
	if (t > time) 
		result = WSAPoll(&this->descrList[0], this->descrList.size(), 0);
	//inaczej trwa tyle ile 
	else
		result = WSAPoll(&this->descrList[0], this->descrList.size(),(time - t) * 1000);
	//poczatek cyklu
	timer = std::chrono::system_clock::now();
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
			return;
		}
	}
}

int Network::createServerSocket()
{
	this->mainSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, this->port.c_str(), &hints, &result);
	if (iResult != 0) {
		Logger::log("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	mainSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (mainSocket == INVALID_SOCKET) {
		Logger::log("Socket failed", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}


	// Setup the TCP listening socket
	iResult = bind(mainSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		Logger::log("Bind failed", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(mainSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(mainSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		Logger::log("Listen failed", WSAGetLastError());
		closesocket(mainSocket);
		WSACleanup();
		return 1;
	}
	pollfd fd = pollfd();
	fd.fd = mainSocket;
	fd.events = POLLIN;
	descrList.push_back(fd);

	//Everything worked


	return 0;
}

int Network::createClientSocket()
{
	// Create a SOCKET for connecting to server
	int result;
	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainSocket == INVALID_SOCKET) {
		Logger::log("Socket function failed");
		Logger::logNetworkError();
		return 1;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	//clientService.sin_addr.s_addr = inet_addr(this->ipAddress.c_str());
	InetPton(AF_INET, (PCWSTR)this->ipAddress.c_str(), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(std::stoi(this->port));

	//----------------------
	// Connect to server.
	result = connect(mainSocket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (result == SOCKET_ERROR) {
		Logger::log("Connect function failed");
		Logger::logNetworkError();
		return 1;
	}

	auto client = Client();

	pollfd fd = pollfd();
	fd.fd = mainSocket;
	fd.events = POLLIN;
	descrList.push_back(fd);


	client.initClient(playerID++, mainSocket, clientService);

	input.addEventNewPlayer(playerID, 0);
	this->clientList.push_back(client);
	Logger::log("Connected to server.\n");

}
void Network::manageEvents(int events)
{
	//ktos dolacza
	while (events > 0)
	{
		if (this->isServer)
		{
			if (this->descrList[0].revents)
			{
				manageListenEvent();
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
		else if (this->isClient && this->descrList[0].revents)
		{
			manageConnectionEvent();
			events--;
		}
	}

}

void Network::manageListenEvent()
{
	auto revent = this->descrList[0].revents;
	if (revent & POLLIN)
	{
		acceptClient();
	}

}

void Network::manageConnectionEvent()
{
	auto sock = this->descrList[0].fd;
	auto revent = descrList[0].revents;
	if (revent & POLLIN)
	{
		readFromClient(0);
	}
	if (revent & POLLHUP)
		deleteClient(0);

}

void Network::acceptClient()
{
	sockaddr_in infoStorage;
	socklen_t addressSize = sizeof(infoStorage);
	auto client = Client();


	auto clientFd = accept(this->mainSocket, (struct sockaddr*)& infoStorage, &addressSize);
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

void Network::readFromClient(Client* client)
{
	char bufferTemp[Constants::bufferLength];
	//nie znamy jeszcze dlugosci wiadomosci
	int limit = max(Constants::msgLengthLimit, client->msgExpectedLenght);

	//odbieranie wiadomosci
	int result = recv(client->clientSocket, &bufferTemp[0], limit - client->bufferInput.size(), 0);


	if (result == -1 || result == 0)
	{
		Logger::log("Recv error on client: %d\n", client->playerId);
	}
	else
	{
		client->bufferInput.append(std::string(&bufferTemp[0], result));
		client->bufferInputCounter += result;
	}


	Logger::log("Tresc " + std::string(client->bufferInput));

	//przetwarzanie wiadomosci
	while (true)
	{
		if (client->msgExpectedLenght = -1)
		{
			Logger::log("Brak rozmiaru wiadomosci\n");

			int pos = client->bufferInput.find('|');

			if (pos != -1)
			{
				//bierzemy bez znaku |
				client->msgExpectedLenght = std::stoi(client->bufferInput.substr(0, pos));

				Logger::log("Rozmiar wiadomosci uzyskany: " + client->bufferInput.substr(0, pos));

				//usuwamy takze znak |
				client->bufferInput.erase(0, pos + 1);

			}
			//nie mamy rozmiaru wiadomosci nawet, nic wiecej tu nie zrobimy
			else
				return;
		}
		if (client->bufferInput.size() >= client->msgExpectedLenght)
		{
			Logger::log("Buffer wiekszy lub rowny spodziewanej wiadomosci");
			Parser::Event ev = Parser::decodeBytes(std::string(client->bufferInput, 0, client->msgExpectedLenght));
			Logger::log("Wiadomosc stworzona");
			//zrobic funkcje ktora lepiej sprawdza to czy wiadomosc dziala TODO
			if (ev.receiver != this->networkID)
			{
				Logger::log("Wrong message destination. ID:Expected " + std::to_string(ev.receiver) + ":0");
				ev.receiver = this->networkID;
			}
			if (ev.sender != client->playerId)
			{
				Logger::log("Wrong message sender on client. ID:Expected " + std::to_string(ev.sender) + ":" + std::to_string(client->playerId));
				ev.sender = client->playerId;
			}
			//Parser wiadomosc o dostaniu informacji 
			input.addEvent(ev);
			Logger::log("Wiadomosc dodana");

			client->bufferInput.erase(0, client->msgExpectedLenght);
			client->msgExpectedLenght = -1;

			Logger::log("Buffer wyczyszczony");
		}
		//nie mamy wystarczajaco duzo wiadomosci by cos z tym zrobic, nic wiecej nie zrobimy
		else
			return;
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
		//Logger::log("Wyslano " + std::to_string(result) + " Razem " + std::to_string(result + client->bufferInputCounter)
		//	+ " Pozostalo " + std::to_string(Constants::msgLengthLimit - (client->bufferInputCounter + result)));
		// Przesuwanie bufora wzgledem ilosci wyslanych juz znakow (tak, aby zawsze wysylac go wzgledem poczatku)
		client->bufferOutput.erase(0, result);
		//to ze wiadomosc dotarla uznajemy przynajmniej na razie ze jest dobrze
		client->timeoutTimer = 0;
	}
}

void Network::deleteClient(int index)
{

	int playerIndex = clientList[index].playerId;
	closesocket(clientList[index].clientSocket);
	clientList.erase(clientList.begin() + index);
	descrList.erase(descrList.begin() + index + 1);
	//Parser wiadomosc o usunieciu playera
	Logger::log("Klient " + std::to_string(playerIndex) + " zostal usuniety");
	input.addEventDiscPlayer(playerIndex, 0);
}

void Network::increaseTimeout()
{
	for (int i = 0; i < this->clientList.size(); i++)
	{
		clientList[i].timeoutTimer++;
		if (clientList[i].timeoutTimer > Constants::timeoutValue * 2) 
			this->deleteClient(i);
		else if (clientList[i].timeoutTimer > Constants::timeoutValue) 
			this->input.addEventTimeoutReached(clientList[i].playerId, 0);
	}

}
