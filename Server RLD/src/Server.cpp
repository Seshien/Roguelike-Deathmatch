#include "Server.h"

void Server::startLogger()
{
	Logger::startLogger("Server");
}

// void GenerateMap();

void Server::startMap()
{
	this->game.startMap("data/map/map" + std::to_string(this->mapID) + ".txt");
}

void Server::mainLoop()
{
	while (true)
	{

		auto temp = std::chrono::duration_cast<
			std::chrono::duration<double>>(std::chrono::system_clock::now() - this->turntimer);
		double wait = 0;
		if (temp.count() <= Constants::turnTimer)
			wait = Constants::turnTimer - temp.count();

		//odbieranie wiadomosci
		this->input = this->network.inputNetwork(wait);

		this->turntimer = std::chrono::system_clock::now();
		if (input.eventList.size())	
			Logger::log("------------ Handling Phase ------------");
		//przetwarzanie ich
		handleEvents(this->input);
		//
		if (this->gameState == GameState::GAME_MID)
		{
			handleGameOutput(this->game.loopGame(this->gameInput));
		}

		if (output.eventList.size()) 
			Logger::log("------------ Output Phase ------------");
		//
		if (this->stateChange != StateChange::NONE) handleStateChange();
		//wysylanie ich
		this->network.outputNetwork(this->output);
		if (output.eventList.size()) 
			Logger::log("------------ Input Phase ------------");
		this->input = Parser::Messenger();
		this->output = Parser::Messenger();
	}

}

void Server::handleEvents(Parser::Messenger mess)
{
	for (auto& ev : mess.eventList) {
		switch (ev.type)
		{
			case Parser::Type::SERVER:
				handleServer(ev);
				break;
			case Parser::Type::LOBBY:
				handleLobby(ev);
				break;
			case Parser::Type::GAME:
				handleGame(ev);
				break;
			case Parser::Type::ERRORNET:
				handleError(ev);
				break;
			default:
				Logger::log("Error: Event type not found.");
				Logger::log(ev);
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
	case Parser::SubType::NEWPLAYER:
		handleNewPlayer(ev);
		break;
	case Parser::SubType::DISCPLAYER:
		handleDisconnect(ev);
		break;
	case Parser::SubType::TIMEOUT:
		handleTimeout(ev);
		break;
	case Parser::SubType::TIMEOUTANSWER:
		handleTimeoutAnswer(ev);
		break;
	default:
		Logger::log("Error: Event subtype not found.");
		Logger::log(ev);
	}
}

void Server::handleLobby(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::SubType::VOTE:
		handleVote(ev);
		break;
	default:
		Logger::log("Error: Event subtype not found.");
		Logger::log(ev);
	}
}

void Server::handleGame(Parser::Event ev)
{
	for (auto it = begin(this->gameInput.eventList); it != end(this->gameInput.eventList);) {
		if (it->sender == ev.sender && ev.sender != Constants::SERVER_ID)
		{
			it = gameInput.eventList.erase(it);
			break;
		}

		else
			++it;
	}
	this->gameInput.addEvent(ev);
}

void Server::handleError(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::SubType::NOACCEPT:
		Logger::log("Error: Problem with accepting contacts");
		break;
	default:
		Logger::log("Error: Event subtype not found.");
		Logger::log(ev);
	}
}

void Server::handleStateChange()
{
	if (this->stateChange == StateChange::VOTE_END)
	{
		this->gameState = GameState::GAME_MID;
		std::vector<int> playerIDs;
		std::vector<std::string> playerNames;
		for (auto player : this->activePlayerList)
		{
			playerIDs.push_back(player->playerID);
			playerNames.push_back(player->name);
		}
		for (auto player : this->activePlayerList)
		{
			output.addEventMidGame(Constants::SERVER_ID, player->playerID, this->getCurrentGameTime());
		}
		this->game.startGame(playerIDs, playerNames);

	}
}

void Server::handleGameOutput(Parser::Messenger gOutput)
{
	for (auto ev : gOutput.eventList)
	{
		for (auto player : this->activePlayerList)
		{
			if (ev.receiver == player->playerID)
			{
				this->output.addEvent(ev);
				break;
			}
		}

	}
}

void Server::handleNewPlayer(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	int playerID;
	bool old = false;
	for (Player &player_ : this->playerList)
	{
		if (player_.name == playerName)
		{
			if (player_.state == Player::INACTIVE)
			{
				//przyjmujemy tego gracza i nadajemy mu stare ID
				player_.state = Player::ACTIVE;
				playerID = player_.playerID;
				old = true;
				break;
			}
			else
			{
				//odrzucamy gracza, bo juz taki istnieje
				output.addEventInitPlayer(Constants::SERVER_ID, ev.sender, -1);
				return;
			}
		}
	}
	//nie ma gracza o takiej nazwie
	//czyli tworzymy nowego gracza
	if (!old)
	{
		playerID = this->playerList.size() + 1;
		playerList.push_back(Player(playerID, playerName));
	}
	this->refreshActivePlayerList();
	//tworzymy Event wewnetrzny ktory mowi network o tym ze trzeba zmienic id na playerID, network potem przekazuje to dalej
	output.addInnerInitPlayer(ev.sender, Constants::SERVER_ID, playerID);

	output.addEventInitPlayer(Constants::SERVER_ID, playerID, playerID);

	//teraz powinnismy podac info o tej grze Playerowi
	this->InfoDump(playerID);

	//Podanie kazdemu graczowi informacje ze nowy gracz dolaczyc
	for (auto player_ : this->activePlayerList)
	{
		if (player_->playerID == playerID) continue;
		output.addEventNewPlayer(Constants::SERVER_ID, player_->playerID, playerName);
	}

}


void Server::handleDisconnect(Parser::Event ev)
{
	handleDisconnect(ev.sender);
}

void Server::handleDisconnect(int playerID)
{
	//to znaczy ze to nie jest zalogowany gracz, tylko ktos kto probowal wejsc i mu sie nie udalo
	if (playerID < 0)
	{
		output.addInnerDiscPlayer(playerID, Constants::SERVER_ID);
		return;
	}

	auto player = this->getPlayer(playerID);
	if (player == nullptr)
	{
		Logger::log("Error: Failure in deleting player");
		return;
	}
	player->state = Player::INACTIVE;
	this->refreshActivePlayerList();
	//rzeczy zwiazane z wewnetrzna logika gry,

	// TODO

	//powiadomienie networka o usunieciu tego gracza
	output.addInnerDiscPlayer(playerID, Constants::SERVER_ID);

	//powiadomienie innych graczy o zniknieciu tego gracza
	for (auto player_ : this->activePlayerList)
	{
		output.addEventDiscPlayer(0, player_->playerID, player->name);
	}
}

void Server::handleTimeout(Parser::Event ev)
{
	auto player = this->getPlayer(ev.sender);
	if (player == nullptr)
	{
		Logger::log("Error: Failure in handling timeout, player not found");
		return;
	}
	//sprawdzamy czy jest zagrozony
	if (player->timeout)
	{
		handleDisconnect(player->playerID);
	}
	else
	{
		output.addEventTimeoutReached(Constants::SERVER_ID, player->playerID);
	}

}

void Server::handleTimeoutAnswer(Parser::Event ev)
{
	auto player = this->getPlayer(ev.sender);
	if (player == nullptr)
	{
		Logger::log("Error: Failure in handling timeout answer, player not found");
		return;
	}
	//wylaczamy timeout bo odpowiedzial
	player->timeout = false;
}

void Server::handleVote(Parser::Event ev)
{
	auto player = this->getPlayer(ev.sender);
	if (player == nullptr)
	{
		Logger::log("Error: Failure in handling vote, player not found");
		return;
	}
	if (player->voted)
	{
		player->voted = false;
		this->numOfVotes--;
	}
	else
	{
		player->voted = true;
		this->numOfVotes++;
	}
	Logger::log("Amount of vote changed. Votes:" + std::to_string(this->numOfVotes) + "/" + std::to_string(this->activePlayerCount));
	for (auto player : this->activePlayerList)
		output.addEventVote(Constants::SERVER_ID, player->playerID, numOfVotes);
	if (this->activePlayerCount >= 2 && this->numOfVotes > this->activePlayerCount / 2)
	{
		this->stateChange = StateChange::VOTE_END;
	}
}
// TO DO
std::string Server::getResults() {
	return std::string();
}

std::chrono::duration<double> Server::getCurrentGameTime() {
	return std::chrono::system_clock::now() - this->gameStartTime;
}

void Server::InfoDump(int playerId) 
{
	std::string subdata;
	// Wysylamy graczowi nazwy wszystkich pozostalych graczy w przypadku dolaczenia do gry
	for (auto player : this->activePlayerList) {
		if (player->playerID != playerId) {
			output.addEventNewPlayer(Constants::SERVER_ID, playerId, player->name);
		}
	}
	auto newPlayer = this->getPlayer(playerId);
	if (newPlayer == nullptr)
	{
		Logger::log("Error: New player was not found");
	}
	// Wysylamy ID mapy w kazdym wypadku niezaleznie od stanu gry
	output.addEventMapID(Constants::SERVER_ID, playerId, this->mapID);

	// Wiadomosci zalezne od stanu gry
	switch (gameState) {
	case GameState::LOBBY:
		output.addEventLobby(Constants::SERVER_ID, playerId, numOfVotes);
		break;
	case GameState::GAME_MID:
		// Wysylamy fakt, ze jest w grze oraz czas trwania danej gry. (za pomoca subType?)
		output.addEventMidGame(Constants::SERVER_ID, playerId, getCurrentGameTime());
		gameInput.addEvent(Constants::SERVER_ID, playerId, Parser::Type::GAME, Parser::SubType::NEWPLAYER, newPlayer->name);
		break;
	case GameState::GAME_END:
		// wysylamy wyniki wszystkich graczy (nazwy juz dostanie)
		output.addEventGameEnd(Constants::SERVER_ID, playerId, getResults());
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

Player * Server::getPlayer(int playerID)
{
	if (playerID == playerList[playerID - 1].playerID) 
		return &playerList[playerID - 1];
	else
	{
		//czy tak konstrukcja dziala?
		for (auto& player : playerList)
		{
			if (playerID == player.playerID)
				return &player;
		}
	}
	Logger::log("Player not found");
	return nullptr;
}

std::vector<Player *> Server::refreshActivePlayerList()
{
	std::vector<Player *> res;
	for (auto& player : this->playerList)
		if (player.state == Player::ACTIVE) res.push_back(&player);
	this->activePlayerList = res;
	this->activePlayerCount = res.size();
	return res;
}

int Server::getPlayerCount()
{
	return this->playerList.size();
}

int Server::getPlayerCount(Player::State state)
{
	int count = 0;
	for (auto& player : this->playerList)
		if (player.state == state)
			count++;
	return count;
}