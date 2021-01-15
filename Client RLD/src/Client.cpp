#include "Client.h"

void Client::startClient()
{
	this->startLogger();
	this->loadConfig();
	//
	this->refreshClient();

	mainLoop();

}
void Client::refreshClient()
{
	int result = 1;
	output = Parser::Messenger();
	input = Parser::Messenger();
	playerList.clear();
	this->ID = -1;
	//probujemy rozpoczac polaczenie
	while (true)
	{
		result = network.startClient(this->IpAddress, this->port);
		if (result == 0)
		{
			break;
		}
		Logger::log("Connecting to server failed. Press something to try again");
		std::getchar();
	}

	//przygotujemy prosbe o przyjecie i wysylamy nazwe;
	output.addEventNewPlayer(ID, 0, playerName);
}
void Client::mainLoop()
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

void Client::handleEvents(Parser::Messenger mess)
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
			//handleGame(ev);
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


void Client::handleServer(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::INITPLAYER:
		handleInitPlayer(ev);
		break;
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
		//tej wiadomosci nie powinien otrzymac klient nigdy
		Logger::log("Error: wrong type of event (TIMEOUTANSWER)");
		break;
	case Parser::INFODUMP_LOBBY:
		//wlaczamy widok lobby, z widokiem mapy
		Logger::log("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		output.addEventVote(this->ID, Constants::SERVER_ID);
		break;
	case Parser::INFODUMP_GAME_MID:
		Logger::log("Czas trwania gry:" + ev.subdata);
		break;
	case Parser::INFODUMP_GAME_END:
		Logger::log("Statystyki:" + ev.subdata);
		break;
	case Parser::RESET:
		refreshClient();
		break;
	default:
		Logger::log("Error, event type not found.");
	}
}

void Client::handleLobby(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::VOTE:
		Logger::log("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		break;

	default:
		Logger::log("Error, event type not found.");
	}
}

void Client::handleInitPlayer(Parser::Event ev)
{
	int newID = ev.subdata[0];

	if (newID == -1)
	{
		Logger::log("There was a existing player with that nick.");
		this->playerName += " Copy";
		output.addEventNewPlayer(ID, 0, playerName);
		return;
	}
	this->ID = newID;
	output.addInnerNewPlayer(0, -1, ID);
	Logger::log("New ID " + std::to_string(this->ID));

	//you are a player too now
	playerList.push_back(this->playerName);
}

void Client::handleNewPlayer(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	this->playerList.push_back(playerName);
}


void Client::handleDisconnect(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	for (int i=0;i<playerList.size();i++)
		if (playerList[i] == playerName)
		{
			playerList.erase(playerList.begin() + i);
			return;
		}
}

void Client::handleTimeout(Parser::Event ev)
{
	output.addEventTimeoutAnswer(ID, 0);
}

void Client::startLogger()
{
	Logger::startLogger();
}

void Client::loadConfig()
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

void Client::processConfigLine(std::string line)
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

void Client::setConfigValue(std::string token, std::string value)
{
	if (token == "time") this->time = std::stoi(value);
	else if (token == "special") return;
	else if (token == "port") this->port = value;
	else Logger::log("Unknown line in config file");
}