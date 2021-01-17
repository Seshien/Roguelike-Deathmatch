#include "Client.h"

void Client::startClient()
{
	this->startLogger();
	this->loadConfig();
	this->startWindow();

	mainLoop();

}
void Client::startWindow()
{
	window.create(sf::VideoMode(1000, 1000), "SFML works!");
	// Initialize the view to a rectangle located at (100, 100) and with a size of 400x200
	gameView.reset(sf::FloatRect(100, 100, 400, 200));
	lobbyView.reset(sf::FloatRect(0, 0, 200, 800));
	interfaceView.reset(sf::FloatRect(0, 0, 400, 200));

	// Set its target viewport to be half of the window
	gameView.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 0.8f));
	lobbyView.setViewport(sf::FloatRect(0.8f, 0.f, 0.2f, 0.8f));
	interfaceView.setViewport(sf::FloatRect(0.0f, 0.8f, 1.0f, 0.2f));







	this->turnTimer.restart();
}

void Client::connectClient()
{
	if (this->cState == ConnectionState::CONNECTED)
	{
		Logger::log("You are connected already");
		return;
	}
	output = Parser::Messenger();
	input = Parser::Messenger();
	playerList.clear();
	this->ID = -1;
	//probujemy rozpoczac polaczenie
	if (network.startClient(this->IpAddress, this->port) == 0)
	{
		this->cState = ConnectionState::CONNECTED;
		Logger::log("Connecting to server succeed");
		output.addEventNewPlayer(ID, 0, playerName);
	}
	else
	{
		this->cState = ConnectionState::FAILED;
		Logger::log("Connecting to server failed");
	}

}
void Client::mainLoop()
{
	while (window.isOpen())
	{
		//odbieranie wiadomosci
		auto time = this->turnTimer.getElapsedTime().asSeconds();
		double wait = 0;
		if (time <= Constants::turnTimer)
			wait = Constants::turnTimer - time;

		if (cState == ConnectionState::CONNECTED)
			this->input = this->network.inputNetwork(wait);

		this->turnTimer.restart();

		if (input.eventList.size())
			Logger::log("------------ Handling Phase ------------");
		//przetwarzanie ich
		handleNetEvents(this->input);
		handleIntEvents();
		if (output.eventList.size())
			Logger::log("------------ Output Phase ------------");
		//wysylanie ich
		this->network.outputNetwork(this->output);
		if (output.eventList.size())
			Logger::log("------------ Input Phase ------------");
		this->output = Parser::Messenger();
	}
}
void Client::handleIntEvents()
{
	//handling interface events
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::L)
			{
				std::cout << "the L key was pressed" << std::endl;
				std::cout << "control:" << event.key.control << std::endl;
				std::cout << "alt:" << event.key.alt << std::endl;
				std::cout << "shift:" << event.key.shift << std::endl;
				std::cout << "system:" << event.key.system << std::endl;
				this->connectClient();
			}
		}
		else if (event.type == sf::Event::Closed)
			window.close();
	}

	window.clear();
	rectangle.setSize(sf::Vector2f(1000, 1000));

	window.setView(gameView);
	rectangle.setFillColor(sf::Color::Green);
	window.draw(rectangle);

	window.setView(interfaceView);
	rectangle.setFillColor(sf::Color::Blue);
	window.draw(rectangle);

	window.setView(lobbyView);

	sf::Text text;
	sf::Font font;
	font.loadFromFile("data/arial2.ttf");
	text.setFont(font);
	std::string str;
	for (auto player : playerList) str.append(player + "\n");
	text.setString(str);

	window.draw(text);


	window.display();
}
void Client::handleNetEvents(Parser::Messenger mess)
{
	//handling network input events
	for (auto& ev : mess.eventList) 
	{
		switch (ev.type)
		{
		case Parser::Type::SERVER:
			handleServer(ev);
			break;
		case Parser::Type::LOBBY:
			handleLobby(ev);
			break;
		case Parser::Type::GAME:
			//handleGame(ev);
			break;
		case Parser::Type::ERRORNET:
			if (ev.subtype == Parser::SubType::RESET)
			{
				this->cState = ConnectionState::FAILED;
			}
			//handleGame(ev);
			break;
		default:
			Logger::log("Error, event type not found.");
		}
	}
}


void Client::handleServer(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::SubType::INITPLAYER:
		handleInitPlayer(ev);
		break;
	case Parser::SubType::NEWPLAYER:
		handleNewPlayer(ev);
		break;
	case Parser::SubType::DISCPLAYER:
		handleDisconnectPlayer(ev);
		break;
	case Parser::SubType::TIMEOUT:
		handleTimeout(ev);
		break;
	case Parser::SubType::TIMEOUTANSWER:
		//tej wiadomosci nie powinien otrzymac klient nigdy
		Logger::log("Error: wrong type of event (TIMEOUTANSWER)");
		break;
	case Parser::SubType::INFODUMP_LOBBY:
		//wlaczamy widok lobby, z widokiem mapy
		Logger::log("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		output.addEventVote(this->ID, Constants::SERVER_ID);
		break;
	case Parser::SubType::INFODUMP_GAME_MID:
		Logger::log("Czas trwania gry:" + ev.subdata);
		break;
	case Parser::SubType::INFODUMP_GAME_END:
		Logger::log("Statystyki:" + ev.subdata);
		break;
	case Parser::SubType::RESET:

		break;
	default:
		Logger::log("Error, event type not found.");
	}
}

void Client::handleLobby(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::SubType::VOTE:
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


void Client::handleDisconnectPlayer(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	for (int i=0;i<playerList.size();i++)
		if (playerList[i] == playerName)
		{
			playerList.erase(playerList.begin() + i);
			return;
		}
}

void handleLostConnection(Parser::Event ev)
{

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