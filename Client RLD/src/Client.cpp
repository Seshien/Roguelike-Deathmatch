#include "Client.h"

void Client::startClient()
{
	this->voted = false;

	this->startLogger();
	this->loadConfig();

	this->font.loadFromFile("data/arial2.ttf");

	this->loadTileTextures();
	this->loadPlayerTextures();
	this->loadUITextures();

	this->startWindow();

	this->gameStage = GameStage::NOTJOINED;

	mainLoop();

}
void Client::startWindow()
{
	window.create(sf::VideoMode(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT), "Roguelike Deathmatch");
	window.setFramerateLimit(60);
	// Initialize the view to a rectangle located at (100, 100) and with a size of 400x200
	gameView.reset(sf::FloatRect(100, 100, 768, 576));
	lobbyView.reset(sf::FloatRect(0, 0, 192, 576));
	interfaceView.reset(sf::FloatRect(0, 0, 400, 144));

	// Set its target viewport to be half of the window
	gameView.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 0.8f));
	lobbyView.setViewport(sf::FloatRect(0.8f, 0.f, 0.2f, 0.8f));
	interfaceView.setViewport(sf::FloatRect(0.0f, 0.8f, 1.0f, 0.2f));


	// Create UI elements
	getIn = UIButton(20, 25, 160, 64, "Login", buttonTexture);
	vote = UIButton(20, 460, 160, 64, "Vote", buttonTexture);
	getIn.changeVisibility(true);
	hpBar = UIBar(20, 25, 320, 64, barTexture, 100.0f);




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
		output.addEventNewPlayer(ID, Constants::SERVER_ID, playerName);
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
		graphicsUpdate();
		if (output.eventList.size())
			Logger::log("------------ Output Phase ------------");
		//wysylanie ich
		this->network.outputNetwork(this->output);
		if (output.eventList.size())
			Logger::log("------------ Input Phase ------------");
		this->input = Parser::Messenger();
		this->output = Parser::Messenger();
	}
}

void Client::graphicsUpdate() {
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

	text.setFont(this->font);
	std::string str;
	if (this->gameStage == Client::GameStage::LOBBY) {
		str.append("Votes: ");
		str.append(std::to_string(this->numVotes));
		str.append("/");
		str.append(std::to_string(this->playerList.size()));
		str.append("\n");
	}
	for (auto player : playerList) str.append(player + "\n");
	text.setString(str);

	// Rysujemy button
	if(this->gameStage == Client::GameStage::NOTJOINED)
		getIn.draw(window);
	if(this->gameStage == Client::GameStage::LOBBY)
		vote.draw(window);
	if (this->gameStage == Client::GameStage::ALIVE || this->gameStage == Client::GameStage::DEAD)
		hpBar.draw(window);

	window.draw(text);


	window.display();
}

void Client::handleIntEvents()
{
	//handling interface events
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Left)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(Constants::SERVER_ID, this->ID, "LEFT");
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Right)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(Constants::SERVER_ID, this->ID, "RIGHT");
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Up)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(Constants::SERVER_ID, this->ID, "UP");
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Down)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(Constants::SERVER_ID, this->ID, "DOWN");
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Space)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(Constants::SERVER_ID, this->ID, "SPACE");
				else
					Logger::log("You are not alive");
			}
		}
		else if (event.type == sf::Event::Closed)
			window.close();
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (getIn.isClickInBounds(event.mouseButton.x - Constants::SCREEN_WIDTH * 0.8f, event.mouseButton.y)) {
					this->connectClient();
					getIn.changeVisibility(false);
					vote.changeVisibility(true);
					Logger::log("Get in button clicked!");
				}
				else if (vote.isClickInBounds(event.mouseButton.x - Constants::SCREEN_WIDTH * 0.8f, event.mouseButton.y)) {
					output.addEventVote(this->ID, Constants::SERVER_ID);
					this->voted = !this->voted;
					Logger::log("Vote button clicked!");
					if (this->voted)
						vote.setText("Cancel");
					else
						vote.setText("Vote");
				}
				//std::cout << "the right button was pressed" << std::endl;
				//std::cout << "mouse x: " << event.mouseButton.x << std::endl;
				//std::cout << "mouse y: " << event.mouseButton.y << std::endl;
			}
		}
	}

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
			handleGame(ev);
			break;
		case Parser::Type::ERRORNET:
			if (ev.subtype == Parser::SubType::NOCONTACT)
			{
				this->cState = ConnectionState::FAILED;

			}
			else if (ev.subtype == Parser::SubType::DISCPLAYER)
			{
				this->cState = ConnectionState::FAILED;
				this->handleLostConnection(ev);
			}
			//handleGame(ev);
			break;
		default:
			Logger::log("Error, event type not found.");
			Logger::log(ev);
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
	case Parser::SubType::MAP:
		this->mapID = ev.subdata[0];
		break;
	case Parser::SubType::INFODUMP_LOBBY:
		//wlaczamy widok lobby, z widokiem mapy
		Logger::log("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		this->gameStage = GameStage::LOBBY;
		break;
	case Parser::SubType::INFODUMP_GAME_MID:
		Logger::log("Czas trwania gry:" + ev.subdata);
		this->gameStage = GameStage::DEAD;
		break;
	case Parser::SubType::INFODUMP_GAME_END:
		Logger::log("Statystyki:" + ev.subdata);
		break;
	default:
		Logger::log("Error, event subtype not found.");
		Logger::log(ev);
	}
}

void Client::handleLobby(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::SubType::VOTE:
		Logger::log("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		this->numVotes = ev.subdata[0] - '0';
		break;

	default:
		Logger::log("Error, event subtype not found.");
		Logger::log(ev);
	}
}

void Client::handleGame(Parser::Event ev)
{
	std::string evString;
	switch (ev.subtype)
	{
	case Parser::SubType::MOVE:
		// To jest madry sposob na wybieranie czesci char arraya do sstringa
		evString = std::string(ev.subdata);
		evString = evString.substr(3, ev.subdata.size() - 2);
		Logger::log("Ruch:" + evString + "x: " + std::string(1, ev.subdata[0]) + "y: " + std::string(1, ev.subdata[1]));
		break;

	default:
		Logger::log("Error, event subtype not found.");
		Logger::log(ev);
	}
}

void Client::handleInitPlayer(Parser::Event ev)
{
	int newID = ev.subdata[0];

	if (newID == -1)
	{
		Logger::log("There was a existing player with that nick.");
		//this->playerName += " Copy";
		//output.addEventNewPlayer(ID, 0, playerName);
		return;
	}
	this->ID = newID;
	output.addInnerInitPlayer(0, -1, ID);
	Logger::log("New ID " + std::to_string(this->ID));

	//you are a player too now
	playerList.push_back(this->playerName);

}

void Client::handleNewPlayer(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	Logger::log("New player: " + playerName);
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

void Client::handleLostConnection(Parser::Event ev)
{
	this->output.addInnerDiscPlayer(Constants::SERVER_ID, this->ID);
	this->cState = ConnectionState::FAILED;
	this->gameStage = GameStage::NOTJOINED;
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
	}
	file.close();
}

void Client::processConfigLine(std::string line)
{
	std::string delimiter = "=";
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
	if (token == "player_name") this->playerName = value;
	else if (token == "IP") this->IpAddress = value;
	else if (token == "port") this->port = value;
	else Logger::log("Unknown line in config file");
}

void Client::loadUITextures() {
	if (!buttonTexture.loadFromFile("data/button.png"))
	{
		Logger::log("Error. File button.png not found.");
	}
	if (!barTexture.loadFromFile("data/hpbar.png"))
	{
		Logger::log("Error. File hpbar.png not found.");
	}
}

void Client::loadPlayerTextures() {
	for (int i = 0; i < 4; i++) {
		std::vector<std::shared_ptr<sf::Texture>> texture;
		texture.push_back(std::make_shared<sf::Texture>());
		texture.push_back(std::make_shared<sf::Texture>());
		texture.push_back(std::make_shared<sf::Texture>());
		texture.push_back(std::make_shared<sf::Texture>());
		texture[0]->loadFromFile("data/knight" + std::to_string(i) + ".png", sf::IntRect(0, 0, 32, 32));
		texture[1]->loadFromFile("data/knight" + std::to_string(i) + ".png", sf::IntRect(32, 0, 32, 32));
		texture[2]->loadFromFile("data/knight" + std::to_string(i) + ".png", sf::IntRect(0, 32, 32, 32));
		texture[3]->loadFromFile("data/knight" + std::to_string(i) + ".png", sf::IntRect(32, 32, 32, 32));
		playerTextures.push_back(texture);
	}
}

void Client::loadTileTextures() {
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 8; i++) {
			if (j == 2 && i == 7) break;
			tileObjectsTextures.push_back(std::make_shared<sf::Texture>());
			tileObjectsTextures[i + j * 8]->loadFromFile("data/tilesObjects.png", sf::IntRect(i * 32, j * 32, 32, 32));
		}
	}
}