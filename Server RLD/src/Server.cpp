#include "Server.h"

void Server::startLogger()
{
	Logger::startLogger("Server", Config::debug);
}

void Server::StartServer()
{
	startLogger();
	//Config::loadConfig();
	this->loadConfig();
	startMap();

	if (network.startServer(this->port))
	{
		Logger::log("Server network start failed. Closing server.");
		return;
	}
	startLobby();
	mainLoop();
	//startGame();
	//loopGame();
	return;
}

// void GenerateMap();

void Server::startMap()
{
	this->game.startMap("data/map/map" + std::to_string(Config::mapID) + ".txt");
}

void Server::startLobby()
{
	//poczatek prawdziwej dzialalnosci serwera, nie wiem co tu dac zbytnio
	numOfVotes = 0;
	this->gameState = GameState::LOBBY;
	for (auto player : this->activePlayerList)
	{
		player->voted = false;
		output.addEventLobby(Config::SERVER_ID, player->playerID, numOfVotes);
	}
}

void Server::mainLoop()
{
	while (true)
	{
		Logger::debug("Start cyklu serwera.");
		auto temp = std::chrono::duration_cast<
			std::chrono::duration<double>>(std::chrono::system_clock::now() - this->turntimer);
		double wait = 0;
		if (temp.count() <= Config::serverTurnTimer)
			wait = Config::serverTurnTimer - temp.count();

		//odbieranie wiadomosci
		this->input = this->network.inputNetwork(wait);

		this->turntimer = std::chrono::system_clock::now();
		if (input.eventList.size())	
			Logger::debug("------------ Handling Phase ------------");
		//przetwarzanie ich
		handleEvents(this->input);
		//
		if (this->gameState == GameState::GAME_MID)
		{
			if (this->gameTickTimer < Config::gameTickRate)
			{
				gameTickTimer++;
			}
			else
			{
				gameTickTimer = 0;
				handleGameOutput(this->game.loopGame(this->gameInput));
				this->gameInput = Parser::Messenger();
			}
		}
		else if (this->gameState == GameState::GAME_END)
			if (this->gameTickTimer < 100)
				gameTickTimer++;
			else
			{
				this->startLobby();
			}
		if (output.eventList.size()) 
			Logger::debug("------------ Output Phase ------------");
		//
		if (this->stateChange != StateChange::NONE) handleStateChange();
		//wysylanie ich
		this->network.outputNetwork(this->output);
		if (output.eventList.size()) 
			Logger::debug("------------ Input Phase ------------");
		this->input = Parser::Messenger();
		this->output = Parser::Messenger();
	}

}

void Server::startGame()
{
	gameStartTime = std::chrono::system_clock::now();
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
				if (this->gameState==GameState::LOBBY)
					handleLobby(ev);
				break;
			case Parser::Type::GAME:
				if (this->gameState == GameState::GAME_MID)
					handleGame(ev);
				break;
			case Parser::Type::ERRORNET:
				handleError(ev);
				break;
			default:
				Logger::error("Error: Event type not found.");
				Logger::error(ev);
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
		Logger::error("Error: Event subtype not found.");
		Logger::error(ev);
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
		Logger::error("Error: Event subtype not found.");
		Logger::error(ev);
	}
}

void Server::handleGame(Parser::Event ev)
{
	for (auto it = begin(this->gameInput.eventList); it != end(this->gameInput.eventList);) {
		if (it->sender == ev.sender && ev.sender != Config::SERVER_ID)
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
		Logger::error("Error: Problem with accepting contacts");
		break;
	default:
		Logger::error("Error: Event subtype not found.");
		Logger::error(ev);
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
			output.addEventMidGame(Config::SERVER_ID, player->playerID, this->getCurrentGameTime());
		}
		this->game.startGame(playerIDs, playerNames);
		this->stateChange = StateChange::NONE;
	}
	if (this->stateChange == StateChange::GAME_END)
	{
		this->gameState = GameState::GAME_END;
		gameTickTimer = 0;
		for (auto player : this->activePlayerList)
		{
			output.addEventGameEnd(Config::SERVER_ID, player->playerID, winner);
		}
		this->stateChange = StateChange::NONE;
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
		if (ev.receiver == Constants::SERVER_ID && ev.subtype == Parser::SubType::INFODUMP_GAME_END)
		{
			stateChange = StateChange::GAME_END;
			winner = ev.subdata;
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
				output.addEventInitPlayer(Config::SERVER_ID, ev.sender, -1);
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
	output.addInnerInitPlayer(ev.sender, Config::SERVER_ID, playerID);

	output.addEventInitPlayer(Config::SERVER_ID, playerID, playerID);

	//teraz powinnismy podac info o tej grze Playerowi
	this->InfoDump(playerID);

	//Podanie kazdemu graczowi informacje ze nowy gracz dolaczyc
	for (auto player_ : this->activePlayerList)
	{
		if (player_->playerID == playerID) continue;
		output.addEventNewPlayer(Config::SERVER_ID, player_->playerID, playerName);
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
		output.addInnerDiscPlayer(playerID, Config::SERVER_ID);
		return;
	}

	auto player = this->getPlayer(playerID);
	if (player == nullptr)
	{
		Logger::error("Error: Failure in deleting player");
		return;
	}
	player->state = Player::INACTIVE;
	this->refreshActivePlayerList();
	//rzeczy zwiazane z wewnetrzna logika gry,

	// TODO

	//powiadomienie networka o usunieciu tego gracza
	output.addInnerDiscPlayer(playerID, Config::SERVER_ID);

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
		Logger::error("Error: Failure in handling timeout, player not found");
		return;
	}
	//sprawdzamy czy jest zagrozony
	if (player->timeout)
	{
		handleDisconnect(player->playerID);
		player->timeout = false;
	}
	else
	{
		player->timeout = true;
		output.addEventTimeoutReached(Config::SERVER_ID, player->playerID);
	}

}

void Server::handleTimeoutAnswer(Parser::Event ev)
{
	auto player = this->getPlayer(ev.sender);
	if (player == nullptr)
	{
		Logger::error("Error: Failure in handling timeout answer, player not found");
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
		Logger::error("Error: Failure in handling vote, player not found");
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
	Logger::debug("Amount of vote changed. Votes:" + std::to_string(this->numOfVotes) + "/" + std::to_string(this->activePlayerCount));
	for (auto player : this->activePlayerList)
		output.addEventVote(Config::SERVER_ID, player->playerID, numOfVotes);
	if (this->activePlayerCount >= Config::PlayersNeededToStartGame && this->numOfVotes > this->activePlayerCount / 2)
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
			output.addEventNewPlayer(Config::SERVER_ID, playerId, player->name);
		}
	}
	auto newPlayer = this->getPlayer(playerId);
	if (newPlayer == nullptr)
	{
		Logger::error("Error: New player was not found");
	}
	// Wysylamy ID mapy w kazdym wypadku niezaleznie od stanu gry
	output.addEventMapID(Config::SERVER_ID, playerId, Config::mapID);

	// Wiadomosci zalezne od stanu gry
	switch (gameState) {
	case GameState::LOBBY:
		output.addEventLobby(Config::SERVER_ID, playerId, numOfVotes);
		break;
	case GameState::GAME_MID:
		// Wysylamy fakt, ze jest w grze oraz czas trwania danej gry. (za pomoca subType?)
		output.addEventMidGame(Config::SERVER_ID, playerId, getCurrentGameTime());
		gameInput.addEvent(Config::SERVER_ID, playerId, Parser::Type::GAME, Parser::SubType::NEWPLAYER, newPlayer->name);

		break;
	case GameState::GAME_END:
		// wysylamy wyniki wszystkich graczy (nazwy juz dostanie)
		output.addEventGameEnd(Config::SERVER_ID, playerId, getResults());
		break;
	}
	//output.addEvent(SERVER_ID, playerId, Parser::SERVER, Parser::INFODUMP, subdata);
}

Player * Server::getPlayer(int playerID)
{
	
	if (playerID > 0 && playerID < playerList.size() && playerID == playerList[playerID - 1].playerID) 
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
	Logger::error("Player not found");
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


void Server::loadConfig()
{
	std::ifstream file;
	std::string line;
	file.open("data/config.txt");
	if (file.is_open())
	{
		Logger::info("Config file opened:");
		while (std::getline(file, line))
		{
			Logger::debug(line);
			processConfigLine(line);
		}
	}

	else
	{
		Logger::debug("Config file not found");
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
		Logger::error("Error during parsing of config file");
		return;
	}
	std::string token = line.substr(0, pos);
	std::string value = line.substr(pos + 1, line.length() - 1);
	Logger::debug("Token: " + token + " Value: " + value);
	setConfigValue(token, value);
	return;
}

void Server::setConfigValue(std::string token, std::string value)
{
	if (token == "port") this->port = value;
	else Logger::debug("(Unknown / Not Handled) line in config file");
}