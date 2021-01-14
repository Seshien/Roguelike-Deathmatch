#include "Server.h"

void Server::startLogger()
{
	Logger::startLogger();
}

// void GenerateMap();

void Server::startMap()
{
	this->map.init(mapPath);
}

void Server::loopLobby()
{
	while (true)
	{
		//odbieranie wiadomosci
		this->input = this->network.inputNetwork();
		if (input.eventList.size())	
			Logger::log("------------ Handling Phase ------------");
		//przetwarzanie ich
		handleEvents(this->input);
		if (output.eventList.size()) 
			Logger::log("------------ Output Phase ------------");
		//wysylanie ich
		this->network.outputNetwork(this->output);
		if (output.eventList.size()) 
			Logger::log("------------ Input Phase ------------");
		this->output = Parser::Messenger();
	}

}

void Server::handleEvents(Parser::Messenger mess)
{
	for (auto& ev : mess.eventList) {
		switch (ev.type)
		{
			case Parser::SERVER:
				handleServer(ev);
				break;
			case Parser::LOBBY:
				handleLobby(ev);
				break;
			case Parser::GAME:
				handleGame(ev);
				break;
			default:
				Logger::log("Error, event type not found.");
		}

		// Wykonaj logike
		//sender, receiver, type, subdata
		//Parser::Event newEv = { 0, ev.sender, ev.type, ev.subtype, ev.subdata };
		// Wpisz wynik w output parser
		//this->output.addEvent(newEv);
	}
}

void Server::handleServer(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::NEWPLAYER:
		handleNewPlayer(ev);
		break;
	case Parser::DISCPLAYER:
		handleDisconnect(ev);
		break;
	case Parser::TIMEOUT:
		handleTimeout(ev);
		break;
	case Parser::TIMEOUTANSWER:
		handleTimeoutAnswer(ev);
		break;
	default:
		Logger::log("Error, event type not found.");
	}
}

void Server::handleLobby(Parser::Event ev)
{

}
void Server::handleGame(Parser::Event ev)
{

}
void Server::handleNewPlayer(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	for (Player &player : this->playerList)
	{
		if (player.name == playerName)
		{
			if (player.state == Player::INACTIVE)
			{
				//przyjmujemy tego gracza i nadajemy mu stare ID
				player.state = Player::ACTIVE;
				output.addInnerNewPlayer(ev.sender, 0, player.playerID);
				return;
			}
			else
			{
				//odrzucamy gracza, bo juz taki istnieje
				output.addInnerNewPlayer(ev.sender, 0, -1);
				return;
			}
		}

	}
	//nie ma gracza o takiej nazwie
	//czyli tworzymy nowego gracza
	int playerID = this->playerList.size() + 1;
	playerList.push_back(Player(playerID, playerName));
	//tworzymy Event wewnetrzny ktory mowi network o tym ze trzeba zmienic id na playerID, network potem przekazuje to dalej
	output.addInnerNewPlayer(ev.sender, 0, playerID);

	//teraz powinnismy podac info o tej grze Playerowi
	this->InfoDump(playerID);

	//Podanie kazdemu graczowi informacje ze nowy gracz dolaczyc
	for (Player &player : this->playerList)
	{
		if (player.playerID == playerID) continue;
		output.addEventNewPlayer(0, player.playerID, playerName);
	}

	
}


void Server::handleDisconnect(Parser::Event ev)
{
	handleDisconnect(ev.sender);
}

void Server::handleDisconnect(int playerID)
{
	int pIndex = playerID - 1;
	std::string playerName = playerList[pIndex].name;
	playerList[pIndex].state = Player::INACTIVE;

	//rzeczy zwiazane z wewnetrzna logika gry,
	// TODO

	//powiadomienie networka o usunieciu tego gracza
	output.addInnerDiscPlayer(playerID, 0);

	//powiadomienie innych graczy o zniknieciu tego gracza
	for (Player &player : this->playerList)
	{
		if (player.playerID == playerID) continue;
		output.addEventDiscPlayer(0, player.playerID, playerName);
	}
}

void Server::handleTimeout(Parser::Event ev)
{
	Player * player = &playerList[ev.sender];
	//sprawdzamy czy jest zagrozony
	if (player->timeout)
	{
		handleDisconnect(ev.sender);
	}
	else
	{
		output.addEventTimeoutReached(0, ev.sender);
	}

}

void Server::handleTimeoutAnswer(Parser::Event ev)
{
	Player * player = &playerList[ev.sender - 1];
	//wylaczamy timeout bo odpowiedzial
	player->timeout = false;
}

// TO DO
std::string Server::getResults() {
	return std::string();
}

std::chrono::duration<double> Server::getCurrentGameTime() {
	return std::chrono::system_clock::now() - this->gameStartTime;
}

void Server::InfoDump(int playerId) {
	std::string subdata;
	// Wysylamy graczowi nazwy wszystkich pozostalych graczy w przypadku dolaczenia do gry
	for (auto player : this->playerList) {
		if (player.playerID != playerId) {
			output.addEventNewPlayer(SERVER_ID, playerId, player.name);
		}
	}
	// Wysylamy ID mapy w kazdym wypadku niezaleznie od stanu gry
	output.addEventMapID(SERVER_ID, playerId, this->map.getMapID());

	// Wiadomosci zalezne od stanu gry
	switch (gameState) {
	case LOBBY:
		output.addEventLobby(SERVER_ID, playerId, numOfVotes);
		break;
	case GAME:
		// Wysylamy fakt, ze jest w grze oraz czas trwania danej gry. (za pomoca subType?)
		output.addEventGame(SERVER_ID, playerId, getCurrentGameTime());
		break;
	case GAME_END:
		// wysylamy wyniki wszystkich graczy (nazwy juz dostanie)
		output.addEventGameEnd(SERVER_ID, playerId, getResults());
		break;
	}
	//output.addEvent(SERVER_ID, playerId, Parser::SERVER, Parser::INFODUMP, subdata);
}


void Server::loadConfig()
{
	std::fstream file;
	std::string line;
	file.open(this->confName);
	if (file.is_open())
	{
		Logger::log("Config file opened:");
		while (std::getline(file, line))
		{
			Logger::log(line);
			this->processConfigLine(line);
		}
	}

	else
	{
		Logger::log("Config file not found");
		Logger::log("Creating default config file TODO");
		//something something

	}
	file.close();
}

void Server::processConfigLine(std::string line)
{
	std::string delimiter = ":";
	int pos = line.find(delimiter);
	if (pos == -1)
	{
		Logger::log("Error during parsing of config file");
		return;
	}
	std::string token = line.substr(0, pos);
	std::string value = line.substr(pos + 1, line.length() - 1);
	Logger::log("Token: " + token + " Value: " + value);
	this->setConfigValue(token, value);
	return;
}

void Server::setConfigValue(std::string token, std::string value)
{
	if (token == "time") this->time = std::stoi(value);
	else if (token == "special") return;
	else if (token == "port") this->port = value;
	else Logger::log("Unknown line in config file");
}
