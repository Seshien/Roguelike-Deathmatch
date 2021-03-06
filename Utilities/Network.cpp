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
		Logger::error("WSAStartup failed", iResult);
	}
}

int Network::startServer(std::string port)
{
	this->ipAddress = "127.0.0.1";
	this->port = port;
	//serwer ma player id 0
	this->networkID = 0;
	//ogolnie playerID ktore nadaje Network s� tymczasowe, b�d� si� zmienia� po nadaniu przez gr�
	this->playerID = -1;
	this->isServer = true;
	this->clientList.clear();
	this->descrList.clear();
	this->input = Parser::Messenger();
	this->output = Parser::Messenger();


	if (createServerSocket())
		return 1;
	Logger::info("Starting server network succeed");
	//updateDescrList();
	return 0;
}

int Network::startClient(std::string ipAdress, std::string port)
{
	this->isClient = true;
	this->ipAddress = ipAdress;
	this->networkID = -1;
	this->port = port;
	this->clientList.clear();
	this->descrList.clear();
	this->input = Parser::Messenger();
	this->output = Parser::Messenger();

	if (createClientSocket())
		return 1;
	Logger::info("Starting client network succeed");
	return 0;
}

Parser::Messenger Network::inputNetwork(double time)
{
	//timeout to bedzie kiedy cos ostatnio dzialo sie z klientem
	// jezeli timeout bedzie duzy to wyslac wiadomosc sprawdzajaca czy klient dziala 
	this->input = Parser::Messenger();
	int result = -1;

	//jezeli czas trwania cyklu jest dluzszy od ustalonego, WSAPoll jest natychmiastowy
	if (descrList.size() > 0)
	{
		result = WSAPoll(&this->descrList[0], this->descrList.size(), (int)(time * 1000));
	}
	else
	{
		//to znaczy ze klient nie jest podlaczony do serwera, albo cos zlego sie stalo z serwerem. Chyba najlepszym wyjsciem bylby reset aplikacji?
		Logger::error("Your application is not connected to any contact. Do you want to restart connection? (Your only choice is yes)");
		std::getchar();
		input.addEventNoContact(this->networkID, this->networkID);
	}

	if (isServer)
		this->increaseTimeout();

	if (result == -1) {
		Logger::error("Poll failed\n");
		Logger::logNetworkError();
	}
	else if (result > 0) 
	{
		this->manageEvents(result);
	}


	//Zwroc parser albo liste eventow
	return input;
}

//moze przechowywac eventy do zrobienia w outpucie zamiast bufforach klientow? chociaz nie wiem jak by to mialo dzialac
//wtedy bysmy zamiast zastepowac outputa, appendowa� zawarto�� nowego do starego

void Network::outputNetwork(Parser::Messenger _output)
{
	//updateDescrList();
	this->output = _output;
	for (auto & ev : this->output.eventList) 
	{
		//jezeli receiver eventu to nasze wlasne id, to jest to event wewnetrzny
		if (ev.type == Parser::Type::INNER)
			handleInnerEvent(ev);
		else
			handleOutputEvent(ev);
	}
	for (auto & client : this->clientList)
		if (client.bufferOutput.size() > 0)
			sendToClient(&client);

}

void Network::handleOutputEvent(Parser::Event ev)
{
	//char *data = output.encodeBytes(ev);
	Logger::debug("Output Event handling.");
	int receiver = ev.receiver;
	for (auto &client : clientList)
	{
		if (client.playerId == receiver)
		{
			Logger::debug("Message was added to buffer.");
			std::string data = Parser::encodeBytes(ev);
			client.bufferOutput.append(data);
			return;
		}
	}
	Logger::error("Output Event was not served. Probably cause: Player disconnected");
}

void Network::handleInnerEvent(Parser::Event ev)
{
	Logger::debug("Inner Event handling.");
	if (ev.subtype == Parser::SubType::INITPLAYER)
	{
		//tu powinno byc zapisane nowe id
		int newID = ev.subdata[0];

		if (isServer)
			for (auto & client : clientList)
			{
				if (client.playerId == ev.sender)
				{
					client.playerId = newID;
					Logger::debug("New ID was given to client. n:o " + std::to_string(newID) + ":" + std::to_string(ev.sender));
					//kazdy poczatkowy client ma playerid = -1, mimo ze serwer nadaje jakies swoje id 
					//ev.receiver = newID;
					//ev.sender = 0;
					//ev.type = Parser::Type::SERVER;
					//std::string data = Parser::encodeBytes(ev);
					//client.bufferOutput.append(data);
					return;
				}
			}
		else if (isClient && this->networkID == -1)
		{
			this->networkID = newID;
			Logger::debug("New ID was assigned to network");
		}
		else
		{
			Logger::error("Error: Client already have ID");
		}
	}
	else if (ev.subtype == Parser::SubType::DISCPLAYER)
	{
		for (auto & client : clientList)
		{
			if (client.playerId == ev.sender)
			{
				Logger::debug("Deleting Client");
				this->deleteClient(&client);
				return;
			}
		}
	}
	else
	{
		Logger::error("Error: Event with no existing receiver");
	}
}

int Network::createServerSocket()
{

	Logger::debug("Creating server socket.");

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
		Logger::error("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	mainSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (mainSocket == INVALID_SOCKET) {
		Logger::error("Socket failed");
		Logger::logNetworkError();
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	unsigned long mode = 1;

	if (ioctlsocket(mainSocket, FIONBIO, &mode) != 0)
	{
		Logger::error("ioctl failed");
		Logger::logNetworkError();
		return 1;
	}
	// Setup the TCP listening socket
	iResult = bind(mainSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		Logger::error("Bind failed");
		Logger::logNetworkError();
		freeaddrinfo(result);
		closesocket(mainSocket);
		WSACleanup();
		return 1;
	}


	freeaddrinfo(result);

	Logger::debug("Bind succecced.");

	iResult = listen(mainSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		Logger::error("Listen failed");
		Logger::logNetworkError();
		closesocket(mainSocket);
		WSACleanup();
		return 1;
	}

	Logger::debug("Listen succecced.");


	pollfd fd = pollfd();
	fd.fd = mainSocket;
	fd.events = POLLIN;
	descrList.push_back(fd);

	//Everything worked


	return 0;
}

int Network::createClientSocket()
{
	Logger::debug("Creating client socket.");
	// Create a SOCKET for connecting to server


	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	iResult = getaddrinfo(this->ipAddress.c_str(), this->port.c_str(), &hints, &result);
	if (iResult != 0) {
		Logger::error("Getaddrinfo failed\n");
		Logger::logNetworkError();
		WSACleanup();
		return 1;
	}

	int Iresult;
	mainSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (mainSocket == INVALID_SOCKET) {
		Logger::error("Socket function failed");
		Logger::logNetworkError();
		return 1;
	}

	Logger::debug("Connecting to server.");

	Iresult = connect(mainSocket, result->ai_addr, (int)result->ai_addrlen);
	if (Iresult == SOCKET_ERROR) {
		Logger::error("Connect function failed");
		Logger::logNetworkError();
		return 1;
	}
	Logger::info("Connected to server.");

	freeaddrinfo(result);

	Logger::debug("Creating contact.");

	auto client = Contact();



	pollfd fd = pollfd();
	fd.fd = mainSocket;
	fd.events = POLLIN;
	descrList.push_back(fd);

	//czy to w ogole zadziala?
	sockaddr clientData = *result->ai_addr;

	//w tym wypadku klientem jest serwer, serwer ma zawsze playerID = 0
	client.initContact(0, mainSocket, clientData);

	Logger::debug(client.info());
	// addEventNewPlayer bedzie sie robil w kliencie, nie tutaj
	//input.addEventNewPlayer(playerID, -1);
	this->clientList.push_back(client);

	Logger::debug("Contact created.");

	return 0;

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
			for (size_t i = 1; i < this->descrList.size() && events > 0; i++) {
				if (this->descrList[i].revents) {
					manageClientEvent(i - 1);
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
	if (revent & ~POLLIN)
	{
		Logger::debug("Contact disconnected: creating disconnect Event");
		input.addEventLostConnection(clientList[0].playerId, this->networkID);
	}


}

void Network::acceptClient()
{
	sockaddr infoStorage;
	socklen_t addressSize = sizeof(infoStorage);
	auto client = Contact();


	auto clientFd = accept(this->mainSocket, &infoStorage, &addressSize);
	if (clientFd == INVALID_SOCKET)
	{
		Logger::error("Accepting new client failed");
		Logger::logNetworkError();
		input.addEventNoAccept(networkID, networkID);
		return;
	}

	unsigned long mode = 1;

	if (ioctlsocket(clientFd, FIONBIO, &mode) != 0)
	{
		Logger::error("ioctl failed");
		Logger::logNetworkError();
		return;
	}
	pollfd fd = pollfd();
	fd.fd = clientFd;
	fd.events = POLLIN;
	descrList.push_back(fd);


	client.initContact(playerID, clientFd, infoStorage);


	Logger::info("New connection accepted");
	//std::string message = std::string(inet_ntoa(address->sin_addr)) + ":" + std::to_string(ntohs(address->sin_port)) 
	//	+ ":" + std::to_string(clientFd);
	//Logger::log(message);
	//Parser wiadomosc o przyjeciu nowego gracza z tymczasowym ID
	Logger::debug(client.info());

	playerID--;

	this->clientList.push_back(client);
}

void Network::manageClientEvent(int cIndex)
{
	auto sock = this->descrList[cIndex + 1].fd;
	auto revent = descrList[cIndex + 1].revents;

	int index = -1;
	Contact * managed = nullptr;
	//ogolnie descrList  0 to jest serwer
	int clientIndex = cIndex;
	//tutaj szukam klienta dla ktorego jest to wydarzenie, sprawdzajac po socketach
	//moze byc nie potrzebne, ale wolalem to zrobic dla sanity
	if (clientList[clientIndex].clientSocket == sock)
	{
		managed = &clientList[clientIndex];
		index = clientIndex;
	}
	else
	{
		for (size_t i = 0; i<clientList.size(); i++)
			if (clientList[i].clientSocket == sock)
			{
				managed = &clientList[i];
				index = i;
				break;
			}
	}

	if (managed == nullptr)
	{
		Logger::error("Client not found. Socket index:", int(sock));
		return;
	}
	//auto message = "Client: " + std::to_string(index) + " player_id:" + std::to_string(managed->playerId)
	//	+ " socket: " + std::to_string(sock) + " event";
	//Logger::log(message);

	if (revent & POLLIN)
	{
		readFromClient(index);
	}
	if (revent & ~POLLIN)
	{
		Logger::debug("Contact disconnected: creating disconnect Event");
		input.addEventDiscPlayer(clientList[index].playerId, 0);
	}


	// resetujemy timeout, bo cos sie stalo z klientem
	managed->timeoutTimer = 0;
}

void Network::readFromClient(int index)
{
	Contact* currentClient = &clientList[index];
	readFromClient(currentClient);
}

void Network::readFromClient(Contact* client)
{
	auto cof = Config::getConfigHandler();
	char bufferTemp[Config::bufferLength];
	//nie znamy jeszcze dlugosci wiadomosci
	int limit = max(Config::msgLengthLimit, client->msgExpectedLenght);

	//odbieranie wiadomosci
	Logger::debug("Receiving message");
	int result = recv(client->clientSocket, &bufferTemp[0], Config::bufferLength - 1, 0);


	if (result == -1 || result == 0)
	{
		Logger::debug("Recv error on client: %d\n", client->playerId);
	}
	else
	{
		client->bufferInput.append(std::string(&bufferTemp[0], result));
		client->bufferInputCounter += result;
	}

	Logger::debug("Content of buffer:  " + std::string(client->bufferInput));

	//przetwarzanie wiadomosci
	processMessage(client);

}

void Network::sendToClient(Contact * client) 
{
	Logger::debug("Sending message");
	int result = send(client->clientSocket, &client->bufferOutput[0], client->bufferOutput.size(), 0);
	if (result == SOCKET_ERROR) 
	{
		Logger::error("Send error on player id: %d\n", client->playerId);

	}
	else 
	{
		//Logger::log("Wyslano " + std::to_string(result) + " Razem " + std::to_string(result + client->bufferInputCounter)
		//	+ " Pozostalo " + std::to_string(Constants::msgLengthLimit - (client->bufferInputCounter + result)));
		// Przesuwanie bufora wzgledem ilosci wyslanych juz znakow (tak, aby zawsze wysylac go wzgledem poczatku)
		Logger::debug("Message was send successfully");
		client->bufferOutput.erase(0, result);
		//to ze wiadomosc dotarla uznajemy przynajmniej na razie ze jest dobrze
		client->timeoutTimer = 0;
	}
}

void Network::deleteClient(int index)
{
	int playerIndex = clientList[index].playerId;
	closesocket(clientList[index].clientSocket);
	Logger::info("Client " + std::to_string(index) + " with playerID" + std::to_string(clientList[index].playerId) + " was deleted.");
	clientList.erase(clientList.begin() + index);
	if (isServer)
		descrList.erase(descrList.begin() + index + 1);
	else
		descrList.erase(descrList.begin() + index);

}

void Network::deleteClient(Contact * client)
{
	for (size_t i = 0; i < clientList.size(); i++)
		if (client->clientSocket == clientList[i].clientSocket)
		{
			this->deleteClient(i);
			return;
		}
			
	Logger::error("Error: Client meant to be deleted was not found");
}

void Network::processMessage(Contact * client)
{
	Logger::debug("Processsing message");
	while (true)
	{
		if (client->msgExpectedLenght == -1)
		{
			Logger::debug("Trying to get message lenght");

			int pos = client->bufferInput.find('|');

			if (pos != -1)
			{
				//bierzemy bez znaku |
				client->msgExpectedLenght = std::stoi(client->bufferInput.substr(0, pos));

				Logger::debug("Message lenght: " + client->bufferInput.substr(0, pos));

				//usuwamy takze znak |
				client->bufferInput.erase(0, pos + 1);

			}
			//nie mamy rozmiaru wiadomosci nawet, nic wiecej tu nie zrobimy
			else
				return;
		}
		if ((int)client->bufferInput.size() >= client->msgExpectedLenght)
		{
			Logger::debug("Buffer is bigger or same size as message lenght.");
			Parser::Event ev = Parser::decodeBytes(std::string(client->bufferInput, 0, client->msgExpectedLenght));
			Logger::debug("Message recreated");
			Logger::debug("Checking message integrity");
			//zrobic funkcje ktora lepiej sprawdza to czy wiadomosc dziala TODO
			if (checkMessage(client, &ev))
			{
				input.addEvent(ev);
				Logger::debug("Message added to input");
			}
			else
			{
				Logger::error("Message had wrong data and was not accepted");
			}
			//Parser wiadomosc o dostaniu informacji 

			client->bufferInput.erase(0, client->msgExpectedLenght);
			client->msgExpectedLenght = -1;

			Logger::debug("Buffer erased.");
		}
		//nie mamy wystarczajaco duzo wiadomosci by cos z tym zrobic, nic wiecej nie zrobimy
		else
			return;
	}
}

int Network::checkMessage(Contact * client, Parser::Event * ev)
{
	bool good = true;
	if (this->networkID == -1)
	{
		Logger::debug("This network have a temporary ID");
		//ev->receiver = this->networkID;
	}
	else if (ev->receiver != this->networkID)
	{
		Logger::error("Wrong message destination. ID:Expected " + std::to_string(ev->receiver) + ":" + std::to_string(this->networkID));
		return false;
		//ev.receiver = this->networkID;
	}

	if (ev->sender < 0 && ev->subtype == Parser::SubType::NEWPLAYER)
	{
		Logger::debug("Sender have a temporary ID");
		ev->sender = client->playerId;
	}
	else if(ev->sender < 0){
		Logger::error("Sender has sent wrong message.");
		return false;
	}
	else if (ev->sender != client->playerId)
	{
		Logger::error("Wrong message sender. ID:Expected " + std::to_string(ev->sender) + ":" + std::to_string(client->playerId));
		return false;
	}
	return good;
}

void Network::increaseTimeout()
{
	for (size_t i = 0; i < this->clientList.size(); i++)
	{
		clientList[i].timeoutTimer++;
		if (clientList[i].timeoutTimer > Config::getConfigHandler()->timeoutValue)
		{
			Logger::debug("Network Event: Player " + std::to_string(clientList[i].playerId) + " reached timeout time");
			this->input.addEventTimeoutReached(clientList[i].playerId, 0);
			clientList[i].timeoutTimer = 0;
		}

	}

}

Contact * Network::FindClient(int index, SOCKET sock = -1)
{
	//ogolnie descrList  0 to jest serwer
	int clientIndex = index;
	//tutaj szukam klienta dla ktorego jest to wydarzenie, sprawdzajac po socketach
	//moze byc nie potrzebne, ale wolalem to zrobic dla sanity
	if (sock == -1)
		return &clientList[clientIndex];
	else
		if (clientList[clientIndex].clientSocket == sock)
		{
			return &clientList[clientIndex];
		}
		else
		{
			for (size_t i = 0; i < clientList.size(); i++)
				if (clientList[i].clientSocket == sock)
				{
					return &clientList[i];
					break;
				}
		}
	return nullptr;
}
