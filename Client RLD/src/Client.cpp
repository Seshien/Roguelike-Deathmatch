#include "Client.h"

void Client::startClient()
{
	this->voted = false;
	this->currentTextureSet = 0;
	this->spawnedFirstTime = false;
	this->health = Config::defaultHealth;
	this->maxHealth = Config::defaultHealth;

	this->IPAddress = Config::IPAddress;
	this->port = Config::port;

	this->startLogger();

	this->playerName = Config::playerName;
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
	window.create(sf::VideoMode(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT), "Roguelike Deathmatch");
	window.setFramerateLimit(60);
	// Initialize the view to a rectangle located at (100, 100) and with a size of 400x200
	gameView.reset(sf::FloatRect(0, 0, Config::GAME_SCREEN_WIDTH, Config::GAME_SCREEN_HEIGHT));
	lobbyView.reset(sf::FloatRect(0, 0, 192, 576));
	interfaceView.reset(sf::FloatRect(0, 0, 960, 144));

	// Set its target viewport to be half of the window
	gameView.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 0.8f));
	lobbyView.setViewport(sf::FloatRect(0.8f, 0.f, 0.2f, 0.8f));
	interfaceView.setViewport(sf::FloatRect(0.0f, 0.8f, 1.0f, 0.2f));


	// Create UI elements
	getIn = UIButton(20, 25, 160, 64, "Login", buttonTexture);
	vote = UIButton(20, 460, 160, 64, "Vote", buttonTexture);
	respawn = UIButton(20, 160, 160, 64, "Respawn", buttonTexture);
	getIn.changeVisibility(true);
	hpBar = UIBar(5, 5, 320, 64, barTexture, 100.0f);




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
	if (network.startClient(this->IPAddress, this->port) == 0)
	{
		this->cState = ConnectionState::CONNECTED;
		Logger::log("Connecting to server succeed");
		output.addEventNewPlayer(ID, Config::SERVER_ID, playerName);
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
		if (time <= Config::clientTurnTimer)
			wait = Config::clientTurnTimer - time;

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

void Client::centerMap() {
	int x = this->xOurPos * Config::SPRITE_WIDTH;
	int y = this->yOurPos * Config::SPRITE_HEIGHT;
	if (x <= Config::GAME_SCREEN_WIDTH / 2) x = Config::GAME_SCREEN_WIDTH / 2;
	if (y <= Config::GAME_SCREEN_WIDTH / 2) y = Config::GAME_SCREEN_HEIGHT / 2;
	if (x > this->map.MAP_WIDTH* Config::SPRITE_WIDTH - Config::GAME_SCREEN_WIDTH / 2) 
		x = this->map.MAP_WIDTH * Config::SPRITE_WIDTH - Config::GAME_SCREEN_WIDTH / 2;
	if (y > this->map.MAP_HEIGHT* Config::SPRITE_HEIGHT - Config::GAME_SCREEN_HEIGHT / 2)
		y = this->map.MAP_HEIGHT * Config::SPRITE_HEIGHT - Config::GAME_SCREEN_HEIGHT / 2;
	gameView.setCenter(x, y);
}

void Client::graphicsUpdate() {
	window.clear();
	rectangle.setSize(sf::Vector2f(1000, 1000));

	window.setView(gameView);
	rectangle.setFillColor(sf::Color::Green);
	window.draw(rectangle);

	// Rysuje wszystko w grze (co jest jeszcze potrzebne to interpolacja pomiedzy ruchem gracza TO DO)
	if (this->gameStage == Client::GameStage::ALIVE || this->gameStage == Client::GameStage::DEAD) {
		this->centerMap();
		this->map.drawMap(window);
		// Draw semi-transparent fog-of-war (so we see map tiles and players and items are despawned/moved out anyway)
		sf::Sprite fog;
		fog.setTexture(this->fogTexture);
		for (int i = 0; i < this->map.MAP_WIDTH; i++) {
			for (int j = 0; j < this->map.MAP_HEIGHT; j++) {
				if (i <= this->xOurPos - Config::sightValue || i >= this->xOurPos + Config::sightValue || j <= this->yOurPos - Config::sightValue || j >= this->yOurPos + Config::sightValue) {
					fog.setPosition(i * Config::SPRITE_WIDTH, j * Config::SPRITE_HEIGHT);
					window.draw(fog);
				}
			}
		}
		for (int i = 0; i < this->playerInfos.size(); i++) {
			this->playerInfos[i]->interpolate(1.0f, 1.0f);
			this->playerInfos[i]->draw(window);
		}
		for (int i = 0; i < this->items.size(); i++) {
			this->items[i]->draw(window);
		}
		for (auto it = begin(this->damages); it != end(this->damages);) {
			it->draw(window);
			it->decreaseTimer();
			if (int(it->getTimer()) == 0) {
				it = this->damages.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	if (gameStage == Client::GameStage::END) {
		sf::Sprite crown;
		crown.setTexture(*(this->tileObjectsTextures[22]));
		crown.setPosition(Config::SCREEN_WIDTH / 2 - 32, Config::SCREEN_HEIGHT / 2);
		window.draw(crown);
		sf::Text wText;
		std::string tmpWinner = "Winner: ";
		tmpWinner.append(this->winner);
		wText.setFont(this->font);
		wText.setPosition(Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2);
		wText.setString(tmpWinner);
		window.draw(wText);
	}

	window.setView(interfaceView);
	rectangle.setFillColor(sf::Color::Blue);
	window.draw(rectangle);

	if (this->gameStage == Client::GameStage::ALIVE || this->gameStage == Client::GameStage::DEAD) {
		hpBar.changeFillPercent((double)this->health / (double)this->maxHealth * 100.0f);
		hpBar.changeVisibility(true);
		hpBar.draw(window);
		for (int i = 0; i < this->items.size(); i++) {
			this->items[i]->drawInPocket(window, 330 + (i * (Config::SPRITE_WIDTH + 5)), 65);
		}
	}

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
		for (auto player : playerList) str.append(player + "\n");
	}
	if (this->gameStage == Client::GameStage::ALIVE || this->gameStage == Client::GameStage::DEAD) {
		for (int i = 0; i < this->playerInfos.size(); i++) {
			str.append(this->playerInfos[i]->getPlayerName());
			str.append(" [");
			str.append(std::to_string(this->playerInfos[i]->getKillCount()));
			str.append("]\n");
		}
	}
	text.setString(str);

	// Rysujemy button
	if (this->gameStage == Client::GameStage::NOTJOINED)
		getIn.draw(window);
	if (this->gameStage == Client::GameStage::LOBBY)
		vote.draw(window);
	if (this->gameStage == Client::GameStage::DEAD || this->gameStage == Client::GameStage::ALIVE)
		respawn.draw(window);

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
					output.addEventKeyInput(this->ID, Config::SERVER_ID, 'A');
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Right)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, Config::SERVER_ID, 'D');
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Up)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, Config::SERVER_ID, 'W');
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Down)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, Config::SERVER_ID, 'S');
				else
					Logger::log("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Space)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, Config::SERVER_ID, ' ');
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
				if (getIn.isClickInBounds(event.mouseButton.x - Config::SCREEN_WIDTH * 0.8f, event.mouseButton.y)) {
					this->connectClient();
					getIn.changeVisibility(false);
					vote.changeVisibility(true);
					Logger::log("Get in button clicked!");
				}
				else if (vote.isClickInBounds(event.mouseButton.x - Config::SCREEN_WIDTH * 0.8f, event.mouseButton.y)) {
					output.addEventVote(this->ID, Config::SERVER_ID);
					this->voted = !this->voted;
					Logger::log("Vote button clicked!");
					if (this->voted)
						vote.setText("Cancel");
					else
						vote.setText("Vote");
				}
				else if (respawn.isClickInBounds(event.mouseButton.x - Config::SCREEN_WIDTH * 0.8f, event.mouseButton.y)) {
					Logger::log("Respawn button clicked!");
					respawn.changeVisibility(false);
					this->output.addEventWillToRespawn(this->ID, Config::SERVER_ID);
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
		this->map.init(std::string("tmp"), this->tileObjectsTextures);
		Logger::log("Map id received.");
		break;
	case Parser::SubType::INFODUMP_LOBBY:
		//wlaczamy widok lobby, z widokiem mapy
		Logger::log("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		this->numVotes = ev.subdata[0] - '0';
		this->gameStage = GameStage::LOBBY;
		this->gameReset();
		break;
	case Parser::SubType::INFODUMP_GAME_MID:
		Logger::log("Czas trwania gry:" + ev.subdata);
		this->gameStage = GameStage::DEAD;
		this->vote.changeVisibility(false);
		break;
	case Parser::SubType::INFODUMP_GAME_END:
		Logger::log("Statystyki:" + ev.subdata);
		this->gameStage = GameStage::END;
		this->winner = ev.subdata;
		this->gameView.setCenter(Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2);
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
	bool newPlayer = true;
	switch (ev.subtype)
	{
	case Parser::SubType::MOVE:
		// To jest madry sposob na wybieranie czesci char arraya do sstringa
		evString = std::string(ev.subdata);
		evString = evString.substr(2, ev.subdata.size() - 2);
		Logger::log("Ruch:" + evString + "x: " + std::to_string(ev.subdata[0]) + "y: " + std::to_string(ev.subdata[1]));
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerInfos[i]->getPlayerName() == evString) {
				this->playerInfos[i]->setNewPosition((int)ev.subdata[0], (int)ev.subdata[1]);
				this->playerInfos[i]->setIsAlive(true);
			if (this->playerInfos[i]->getPlayerName() == this->playerName) {
				if (this->playerName == evString) {
					this->xOurPos = this->playerInfos[i]->getX();
					this->yOurPos = this->playerInfos[i]->getY();
				}
					for (int j = 0; j < this->playerInfos.size(); j++) {
						Logger::log("Our player found.");
						if (abs((int)ev.subdata[0] - this->playerInfos[j]->getX()) >= Config::sightValue || abs((int)ev.subdata[1] - this->playerInfos[j]->getY()) >= Config::sightValue) {
							this->playerInfos[j]->setIsAlive(false);
							Logger::log("Player moved out from another player");
						}
						else {
							Logger::log("Player didnt move out from another player");
						}
					}
				}
			}
		}
		break;
	case Parser::SubType::ASKRESPAWN:
		this->respawn.changeVisibility(true);
		Logger::log("Server asked player to respawn");
		break;
	case Parser::SubType::RESPAWN:
		Logger::log("Our player respawn info received.");
		Logger::log(std::to_string(ev.subdata[0]) + std::to_string(ev.subdata[1]));
		if (this->spawnedFirstTime == false) {
			this->playerInfos.push_back(std::make_shared<OurPlayerInfo>(this->playerName, (int)ev.subdata[0], (int)ev.subdata[1], this->playerTextures[this->currentTextureSet], 0));
			this->currentTextureSet++;
			if (this->currentTextureSet == 4) {
				this->currentTextureSet = 0;
			}
			this->spawnedFirstTime = true;
		}
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerInfos[i]->getPlayerName() == this->playerName) {
				this->playerInfos[i]->setIsAlive(true);
				this->playerInfos[i]->setNewPosition(ev.subdata[0], ev.subdata[1]);
				this->playerInfos[i]->setPrevPosition(0, 0);
			}
		}
		this->xOurPos = (int)ev.subdata[0];
		this->yOurPos = (int)ev.subdata[1];
		this->gameStage = GameStage::ALIVE;
		break;
	case Parser::SubType::PSPAWN:
		newPlayer = true;
		evString = std::string(ev.subdata);
		Logger::log("PSpawn:" + evString.substr(2, evString.size() - 2) + "x: " + evString.substr(0, 1) + "y: " + evString.substr(1, 1));
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerInfos[i]->getPlayerName() == evString.substr(2, evString.size() - 2)) {
				Logger::log("Other player spawn existing info received");
				this->playerInfos[i]->setNewPosition((int)ev.subdata[0], (int)ev.subdata[1]);
				this->playerInfos[i]->setIsAlive(true);
				newPlayer = false;
			}
		}
		if (newPlayer == true) {
			Logger::log("Other player spawn new info received.");
			this->playerInfos.push_back(std::make_shared<PlayerInfo>(evString.substr(2, evString.size() - 2), (int)ev.subdata[0], (int)ev.subdata[1], this->playerTextures[this->currentTextureSet], 0));
			this->currentTextureSet++;
			if (this->currentTextureSet == 4) {
				this->currentTextureSet = 0;
			}
		}
		break;
	case Parser::SubType::DAMAGE:
		Logger::log("Player new hp: " + ev.subdata);
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerName == this->playerInfos[i]->getPlayerName()) {
				OurPlayerInfo *ourPlayer = (OurPlayerInfo*)(&*(this->playerInfos[i]));
				ourPlayer->health = std::stoi(ev.subdata);
				this->maxHealth = ourPlayer->maxHealth;
				this->health = ourPlayer->health;
			}
		}
		break;
	case Parser::SubType::PICKUP:
		Logger::log("Picked item: " + ev.subdata[0]);
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerName == this->playerInfos[i]->getPlayerName()) {
				OurPlayerInfo* ourPlayer = (OurPlayerInfo*)(&*(this->playerInfos[i]));
				// Na serwerze ITEM_SWORD, ITEM_SHIELD, ITEM_BOOTS, ITEM_POTION, PLAYER, BODY
				int clientItemID;
				switch (ev.subdata[0]) {
				case 0:
					clientItemID = (int)ItemType::SWORD;
					break;
				case 1:
					clientItemID = (int)ItemType::SHIELD;
					break;
				case 2:
					clientItemID = (int)ItemType::BOOTS;
					break;
				case 3:
					clientItemID = (int)ItemType::POTION;
					break;
				case 5:
					clientItemID = (int)ItemType::BONES;
					break;
				default:
					Logger::log("Wrong item type received!");
				}
				ourPlayer->pocket.push_back(std::make_shared<Item>((ItemType)clientItemID, -1, -1, *(tileObjectsTextures[clientItemID]), false, true));
			}
		}
		break;
	case Parser::SubType::MOVEOUT:
		Logger::log("Moveout: " + ev.subdata.substr(2, ev.subdata.size() - 2));
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (ev.subdata.substr(2, ev.subdata.size() - 2) == this->playerInfos[i]->getPlayerName()) {
				this->playerInfos[i]->setIsAlive(false);
				this->playerInfos[i]->setNewPosition((int)ev.subdata[0], (int)ev.subdata[1]);
			}
		}
		break;
	// DESPAWN PDESPAWN KILLCOUNT SPAWN
	case Parser::SubType::SPAWN:
		Logger::log("Spawn item: " + std::string("ObjID: ") + ev.subdata[2] + "X: " + ev.subdata[0] + "Y: " + ev.subdata[1]);
		int clientItemID;
		bool wrongItem;
		switch (ev.subdata[2]) {
		case 0:
			clientItemID = (int)ItemType::SWORD;
			wrongItem = false;
			break;
		case 1:
			clientItemID = (int)ItemType::SHIELD;
			wrongItem = false;
			break;
		case 2:
			clientItemID = (int)ItemType::BOOTS;
			wrongItem = false;
			break;
		case 3:
			clientItemID = (int)ItemType::POTION;
			wrongItem = false;
			break;
		case 5:
			clientItemID = (int)ItemType::BONES;
			wrongItem = false;
			break;
		default:
			Logger::log("Wrong item type received!");
			wrongItem = true;
		}
		if (!wrongItem) 
			this->items.push_back(std::make_shared<Item>((ItemType)clientItemID, (int)ev.subdata[0], (int)ev.subdata[1], *(tileObjectsTextures[clientItemID]), true, false));
		break;
	case Parser::SubType::DESPAWN:
		// Despawnuje item na danym X i Y wiec musze znalezc w vectorze itemsow miejsce w ktorym jest item o danym X i Y
	 	Logger::log("Despawn item: " + std::string(" X: ") + ev.subdata[0] + " Y: " + ev.subdata[1]);
		for (int i = 0; i < this->items.size(); i++) {
			if (this->items[i]->getX() == (int)ev.subdata[0] && this->items[i]->getY() == (int)ev.subdata[1]) {
				this->items.erase(this->items.begin() + i);
				Logger::log("Despawning item " + std::to_string(i));
				break;
			}
		}
		break;
	case Parser::SubType::PDESPAWN:
		Logger::log("Player despawn: " + ev.subdata.substr(2, ev.subdata.size() - 2));
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (ev.subdata.substr(2, ev.subdata.size() - 2) == this->playerInfos[i]->getPlayerName()) {
				this->playerInfos[i]->setIsAlive(false);
			}
		}
		break;
	case Parser::SubType::KILLCOUNT:
		Logger::log("Killcount" + ev.subdata.substr(1, ev.subdata.size()-1) + "num:" + std::to_string((int)ev.subdata[0]));
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (ev.subdata.substr(1, ev.subdata.size() - 1) == this->playerInfos[i]->getPlayerName()) {
				this->playerInfos[i]->setKillCount((int)ev.subdata[0]);
			}
		}
		break;
	case Parser::SubType::HIT:
		Logger::log("Hit: " + ev.subdata);
		for (int i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerInfos[i]->getPlayerName() == ev.subdata) {
				this->damages.push_back(Damage(Config::SPRITE_WIDTH* this->playerInfos[i]->getX(), Config::SPRITE_WIDTH* this->playerInfos[i]->getY(), damageTextures, 3));
			}
		}
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
	for (auto it = this->playerInfos.begin(); it < this->playerInfos.end(); ++it) {
		if ((*it)->getPlayerName() == playerName) {
			this->playerInfos.erase(it);
			break;
		}
	}
}

void Client::handleLostConnection(Parser::Event ev)
{
	this->output.addInnerDiscPlayer(Config::SERVER_ID, this->ID);
	this->cState = ConnectionState::FAILED;
	this->gameStage = GameStage::NOTJOINED;
}

void Client::handleTimeout(Parser::Event ev)
{
	output.addEventTimeoutAnswer(ID, 0);
}

void Client::startLogger()
{
	Logger::startLogger("Client", Config::debug);
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
	if (!fogTexture.loadFromFile("data/fog.png"))
	{
		Logger::log("Error. File fog.pnh not found.");
	}
	for (int i = 0; i < 3; i++) {
		this->damageTextures.push_back(std::make_shared<sf::Texture>());
		this->damageTextures[i]->loadFromFile("data/damage.png", sf::IntRect(i * Config::SPRITE_WIDTH, 0, 32, 32));
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

void Client::loadConfig()
{
	std::fstream file;
	std::string line;
	file.open("data/config.txt");
	if (file.is_open())
	{
		Logger::log("Config file opened:");
		while (std::getline(file, line))
		{
			Logger::log(line);
			processConfigLine(line);
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
		Logger::error("Error during parsing of config file");
		return;
	}
	std::string token = line.substr(0, pos);
	std::string value = line.substr(pos + 1, line.length() - 1);
	Logger::debug("Token: " + token + " Value: " + value);
	setConfigValue(token, value);
	return;
}

void Client::setConfigValue(std::string token, std::string value)
{
	if (token == "special") return;
	else if (token == "port") this->port = value;
	else if (token == "playerName") this->playerName = value;
	else if (token == "Server Address") this->IPAddress = value;
	else Logger::debug("Unknown line in config file");
}

void Client::gameReset() {
	this->voted = false;
	this->vote.changeVisibility(true);
	this->vote.setText("Vote");
	this->currentTextureSet = 0;
	this->spawnedFirstTime = false;
	this->health = Config::defaultHealth;
	this->maxHealth = Config::defaultHealth;
	this->playerInfos.clear();
	this->items.clear();
	this->damages.clear();
}