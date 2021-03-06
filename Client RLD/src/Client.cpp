#include "Client.h"

// Ustawia wszystkie dane potrzebne dla klienta przy rozpoczeciu gry. Jezeli plik konfiguracyjny istnieje i jest uzupelniony to czesc wartosci
// zostanie zmienione na te z pliku konfiguracyjnego.
void Client::startClient()
{


	Logger::startLogger("client");
	conf = Config::getConfigHandler();
	Logger::setLogLevel(conf->debug);


	this->voted = false;
	this->currentTextureSet = 0;
	this->spawnedFirstTime = false;
	this->health = conf->defaultHealth;
	this->maxHealth = conf->defaultHealth;


	this->playerName = conf->playerName;
	

	this->font.loadFromFile("data/arial2.ttf");

	this->loadTileTextures();
	this->loadPlayerTextures();
	this->loadUITextures();

	this->startWindow();

	this->gameStage = GameStage::NOTJOINED;

	mainLoop();

}

// Tworzy okno SFML i trzy widoki pozwalajace wyswietlac grafike niezaleznie od siebie.
// Inicjowane sa takze wartosci wszystkich elementow UI
void Client::startWindow()
{
	window.create(sf::VideoMode(conf->SCREEN_WIDTH, conf->SCREEN_HEIGHT), "Roguelike Deathmatch");
	window.setFramerateLimit(60);
	// Initialize the view to a rectangle located at (100, 100) and with a size of 400x200
	gameView.reset(sf::FloatRect(0, 0, conf->GAME_SCREEN_WIDTH, conf->GAME_SCREEN_HEIGHT));
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

// Wywolywane przy wcisnieciu przez uzytkownika przycisku Login
void Client::connectClient()
{
	if (this->cState == ConnectionState::CONNECTED)
	{
		Logger::info("You are connected already");
		return;
	}
	output = Parser::Messenger();
	input = Parser::Messenger();
	playerList.clear();
	this->ID = -1;
	//probujemy rozpoczac polaczenie
	if (network.startClient(conf->IPAddress, conf->port) == 0)
	{
		this->cState = ConnectionState::CONNECTED;
		Logger::info("Connecting to server succeed");
		output.addEventNewPlayer(ID, conf->SERVER_ID, playerName);
		this->getIn.changeVisibility(false);
	}
	else
	{
		this->cState = ConnectionState::FAILED;
		Logger::error("Connecting to server failed");
	}
}

// Glowna petla
void Client::mainLoop()
{
	while (window.isOpen())
	{
		//odbieranie wiadomosci
		auto time = this->turnTimer.getElapsedTime().asSeconds();
		double wait = 0;
		if (time <= conf->clientTurnTimer)
			wait = conf->clientTurnTimer - time;

		if (cState == ConnectionState::CONNECTED)
			this->input = this->network.inputNetwork(wait);

		this->turnTimer.restart();

		if (input.eventList.size())
			Logger::debug("------------ Handling Phase ------------");
		//przetwarzanie ich
		handleNetEvents(this->input);
		handleIntEvents();
		graphicsUpdate();
		if (output.eventList.size())
			Logger::debug("------------ Output Phase ------------");
		//wysylanie ich
		this->network.outputNetwork(this->output);
		if (output.eventList.size())
			Logger::debug("------------ Input Phase ------------");
		this->input = Parser::Messenger();
		this->output = Parser::Messenger();
	}
}

// Centruje mape tak zeby gracz byl mniejwiecej w srodku
void Client::centerMap() {
	int x = this->xOurPos * conf->SPRITE_WIDTH;
	int y = this->yOurPos * conf->SPRITE_HEIGHT;
	if (x <= conf->GAME_SCREEN_WIDTH / 2) x = conf->GAME_SCREEN_WIDTH / 2;
	if (y <= conf->GAME_SCREEN_WIDTH / 2) y = conf->GAME_SCREEN_HEIGHT / 2;
	if (x > this->map.MAP_WIDTH* conf->SPRITE_WIDTH - conf->GAME_SCREEN_WIDTH / 2) 
		x = this->map.MAP_WIDTH * conf->SPRITE_WIDTH - conf->GAME_SCREEN_WIDTH / 2;
	if (y > this->map.MAP_HEIGHT* conf->SPRITE_HEIGHT - conf->GAME_SCREEN_HEIGHT / 2)
		y = this->map.MAP_HEIGHT * conf->SPRITE_HEIGHT - conf->GAME_SCREEN_HEIGHT / 2;
	gameView.setCenter((float)x, (float)y);
}

// Wyswietlanie grafik
void Client::graphicsUpdate() {
	window.clear();
	rectangle.setSize(sf::Vector2f(1000, 1000));

	window.setView(gameView);
	rectangle.setFillColor(sf::Color::Green);
	window.draw(rectangle);

	// Rysuje wszystko w grze (co jest jeszcze potrzebne to interpolacja pomiedzy ruchem gracza TO DO)
	// Tylko w przypadku, gdy gracz juz jest bezposrednio w grze, niezaleznie czy zywy, czy martwy
	if (this->gameStage == Client::GameStage::ALIVE || this->gameStage == Client::GameStage::DEAD) {
		this->centerMap();
		this->map.drawMap(window);
		// Rysuje polprzezroczysta mgle wojny (widzimy nadal tile mapy, a gracze i przedmioty i tak sa despawnowane w dalszej odleglosci)
		sf::Sprite fog;
		fog.setTexture(this->fogTexture);
		for (int i = 0; i < this->map.MAP_WIDTH; i++) {
			for (int j = 0; j < this->map.MAP_HEIGHT; j++) {
				if (i <= this->xOurPos - conf->sightValue || i >= this->xOurPos + conf->sightValue || j <= this->yOurPos - conf->sightValue || j >= this->yOurPos + conf->sightValue) {
					fog.setPosition((float)i * conf->SPRITE_WIDTH, (float)j * conf->SPRITE_HEIGHT);
					window.draw(fog);
				}
			}
		}
		// Rysujemy wszystkich graczy, ktorzy sa w naszym polu widzenia (ci, ktorzy znalezli sie za nim maja ustawione isAlive na false
		// gdyz gracz nie wie czy aktualnie zyja czy tez nie)
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			// Interpolacja aktualnie odbywa sie w jednym kroku zeby uniknac problemow
			this->playerInfos[i]->interpolate(1.0f, 1.0f);
			this->playerInfos[i]->draw(window);
		}
		// Rysuje wszystkie przedmioty znajdujace sie na mapie w polu widzenia gracza
		for (size_t i = 0; i < this->items.size(); i++) {
			this->items[i]->draw(window);
		}
		// Rysuje wszystkie animacje dostania obrazen (w przypadku, kiedy ich czas zycia sie skonczy to sa usuwane z wektora)
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
	// Faza pokazywania zwyciezcy i ikonki korony
	if (gameStage == Client::GameStage::END) {
		sf::Sprite crown;
		crown.setTexture(*(this->tileObjectsTextures[22]));
		crown.setPosition(conf->SCREEN_WIDTH / 2 - 32, conf->SCREEN_HEIGHT / 2);
		window.draw(crown);
		sf::Text wText;
		std::string tmpWinner = "Winner: ";
		tmpWinner.append(this->winner);
		wText.setFont(this->font);
		wText.setPosition(conf->SCREEN_WIDTH / 2, conf->SCREEN_HEIGHT / 2);
		wText.setString(tmpWinner);
		window.draw(wText);
	}

	window.setView(interfaceView);
	rectangle.setFillColor(sf::Color::Blue);
	window.draw(rectangle);

	// Pokazujemy stan zycia naszej postaci oraz przedmioty w jej ekwipunku
	if (this->gameStage == Client::GameStage::ALIVE || this->gameStage == Client::GameStage::DEAD) {
		hpBar.changeFillPercent((double)this->health / (double)this->maxHealth * 100.0f);
		hpBar.changeVisibility(true);
		hpBar.draw(window);
		for (size_t i = 0; i < this->items.size(); i++) {
			this->items[i]->drawInPocket(window, 330 + (i * (conf->SPRITE_WIDTH + 5)), 65);
		}
	}

	window.setView(lobbyView);

	sf::Text text;

	text.setFont(this->font);
	// Tworzymy stringa z nazwami graczy i oddanymi glosami (dostepne tylko w lobby, w przypadku bycia w grze lista graczy jest rysowana
	// w inny sposob)
	std::string str;
	if (this->gameStage == Client::GameStage::LOBBY) {
		str.append("Votes: ");
		str.append(std::to_string(this->numVotes));
		str.append("/");
		str.append(std::to_string(this->playerList.size()));
		str.append("\n");
		for (auto player : playerList) str.append(player + "\n");
	}
	// Tworzymy stringa z nazwami graczy i iloscia ich zabojstw (dostepne tylko podczas gry)
	if (this->gameStage == Client::GameStage::ALIVE || this->gameStage == Client::GameStage::DEAD) {
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			str.append(this->playerInfos[i]->getPlayerName());
			str.append(" [");
			str.append(std::to_string(this->playerInfos[i]->getKillCount()));
			str.append("]\n");
		}
	}
	text.setString(str);

	// Rysujemy buttony
	if (this->gameStage == Client::GameStage::NOTJOINED)
		getIn.draw(window);
	if (this->gameStage == Client::GameStage::LOBBY)
		vote.draw(window);
	if (this->gameStage == Client::GameStage::DEAD || this->gameStage == Client::GameStage::ALIVE)
		respawn.draw(window);

	window.draw(text);


	window.display();
}

// Przetwarzanie akcji gracza (przyciski z klawiatury oraz przyciski interfejsu klikniete myszka)
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
					output.addEventKeyInput(this->ID, conf->SERVER_ID, 'A');
				else
					Logger::info("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Right)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, conf->SERVER_ID, 'D');
				else
					Logger::info("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Up)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, conf->SERVER_ID, 'W');
				else
					Logger::info("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Down)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, conf->SERVER_ID, 'S');
				else
					Logger::info("You are not alive");
			}
			if (event.key.code == sf::Keyboard::Space)
			{
				if (this->gameStage == GameStage::ALIVE)
					output.addEventKeyInput(this->ID, conf->SERVER_ID, ' ');
				else
					Logger::info("You are not alive");
			}
		}
		else if (event.type == sf::Event::Closed)
			window.close();
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (getIn.isClickInBounds((int)(event.mouseButton.x - conf->SCREEN_WIDTH * 0.8f), event.mouseButton.y)) {
					this->connectClient();
					vote.changeVisibility(true);
					Logger::debug("Get in button clicked!");
				}
				else if (vote.isClickInBounds((int)(event.mouseButton.x - conf->SCREEN_WIDTH * 0.8f), event.mouseButton.y)) {
					output.addEventVote(this->ID, conf->SERVER_ID);
					this->voted = !this->voted;
					Logger::debug("Vote button clicked!");
					if (this->voted)
						vote.setText("Cancel");
					else
						vote.setText("Vote");
				}
				else if (respawn.isClickInBounds((int)(event.mouseButton.x - conf->SCREEN_WIDTH * 0.8f), event.mouseButton.y)) {
					Logger::debug("Respawn button clicked!");
					respawn.changeVisibility(false);
					this->output.addEventWillToRespawn(this->ID, conf->SERVER_ID);
				}
				//std::cout << "the right button was pressed" << std::endl;
				//std::cout << "mouse x: " << event.mouseButton.x << std::endl;
				//std::cout << "mouse y: " << event.mouseButton.y << std::endl;
			}
		}
	}

}

// Przetwarzanie eventow, ktore klient dostal od serwera
void Client::handleNetEvents(Parser::Messenger mess)
{
	//handling network input events
	for (auto& ev : mess.eventList) 
	{
		// W zaleznosci od typu jest odpalana odpowiednia metoda
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
			Logger::error("Error, event type not found.");
			Logger::error(ev);
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
		Logger::error("Error: wrong type of event (TIMEOUTANSWER)");
		break;
	case Parser::SubType::MAP:
		this->mapID = ev.subdata[0];
		this->map.init(std::string("tmp"), this->tileObjectsTextures);
		Logger::debug("Map id received.");
		break;
		// INFODUMP to specjalne typy eventow dostarczane graczowi gdy dolaczy do gry w danej jej fazie, a nie gdy znajdowal sie przy ich starcie
	case Parser::SubType::INFODUMP_LOBBY:
		//wlaczamy widok lobby, z widokiem mapy
		Logger::debug("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		this->numVotes = ev.subdata[0] - '0';
		this->gameStage = GameStage::LOBBY;
		this->gameReset();
		break;
	case Parser::SubType::INFODUMP_GAME_MID:
		Logger::debug("Czas trwania gry:" + ev.subdata);
		this->gameStage = GameStage::DEAD;
		this->vote.changeVisibility(false);
		break;
	case Parser::SubType::INFODUMP_GAME_END:
		Logger::debug("Statystyki:" + ev.subdata);
		this->gameStage = GameStage::END;
		this->winner = ev.subdata;
		this->gameView.setCenter(conf->SCREEN_WIDTH / 2, conf->SCREEN_HEIGHT / 2);
		break;
	default:
		Logger::error("Error, event subtype not found.");
		Logger::error(ev);
	}
}

// KLient dostaje event VOTE w momencie, gdy jakikolwiek gracz odda glos lub go anuluje
void Client::handleLobby(Parser::Event ev)
{
	switch (ev.subtype)
	{
	case Parser::SubType::VOTE:
		Logger::debug("Ilosc glosow: " + std::string(1, ev.subdata[0]) + ":" + std::to_string(playerList.size()));
		this->numVotes = ev.subdata[0] - '0';
		break;

	default:
		Logger::error("Error, event subtype not found.");
		Logger::error(ev);
	}
}

// Tutaj juz przetwarzamy eventy, ktore dzieja sie podczas gry
void Client::handleGame(Parser::Event ev)
{
	std::string evString;
	bool newPlayer = true;
	bool newItem = true;
	switch (ev.subtype)
	{
		// Kiedy jakikolwiek gracz sie poruszy w naszym polu widzenia (dostaniemy tego eventa tez gdy sami wykonamy dozwolony ruch)
	case Parser::SubType::MOVE:
		// To jest madry sposob na wybieranie czesci char arraya do sstringa
		evString = std::string(ev.subdata);
		evString = evString.substr(4, ev.subdata.size() - 4);
		Logger::debug("Ruch:" + evString + "x: " + std::to_string(ev.subdata[0]) + "y: " + std::to_string(ev.subdata[1]));
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			//
			if (this->playerInfos[i]->getPlayerName() == evString) {
				this->playerInfos[i]->setNewPosition((int)ev.subdata[0], (int)ev.subdata[1]);
				this->playerInfos[i]->setIsAlive(true);
				this->playerInfos[i]->setPrevPosition((int)ev.subdata[2], (int)ev.subdata[3]);
				newPlayer = false;
				}
			if (this->playerInfos[i]->getPlayerName() == this->playerName) {
				if (this->playerName == evString) {
					this->xOurPos = this->playerInfos[i]->getX();
					this->yOurPos = this->playerInfos[i]->getY();
					for (auto it = items.begin(); it != items.end();) {
						Logger::debug("Checking items despawn.");
						if ((*it)->isOnMap()) {
							if (abs((int)ev.subdata[0] - (*it)->getX()) >= conf->sightValue || abs((int)ev.subdata[1] - (*it)->getY()) >= conf->sightValue) {
								it = items.erase(it);
								Logger::debug("Player moved out from item");
							}
							else {
								Logger::debug("Player didn't move out from item");
								++it;
							}
						}
						else
							++it;
					}
					for (size_t j = 0; j < this->playerInfos.size(); j++) {
						Logger::debug("Our player found.");
						if (abs((int)ev.subdata[0] - this->playerInfos[j]->getX()) >= conf->sightValue || abs((int)ev.subdata[1] - this->playerInfos[j]->getY()) >= conf->sightValue) {
							this->playerInfos[j]->setIsAlive(false);
							Logger::debug("Player moved out from another player");
						}
						else {
							Logger::debug("Player didnt move out from another player");
						}
					}
				}
			}
		}
		if (newPlayer) {
			Logger::debug("Other player spawn new info received.");
			this->playerInfos.push_back(std::make_shared<PlayerInfo>(evString, (int)ev.subdata[0], (int)ev.subdata[1], this->playerTextures[this->currentTextureSet], 0));
			this->playerInfos[this->playerInfos.size() - 1]->setPrevPosition((int)ev.subdata[2], (int)ev.subdata[3]);
			this->currentTextureSet++;
			if (this->currentTextureSet == 4) {
				this->currentTextureSet = 0;
			}
		}
		break;
		// Serwer odpytuje gracza, czy ten chce sie zrespawnowac
	case Parser::SubType::ASKRESPAWN:
		this->respawn.changeVisibility(true);
		Logger::debug("Server asked player to respawn");
		break;
		// Event przekazujacy informacje o tym gdzie sie zrespawnowalismy (odnosi sie tylko i wylacznie do naszej postaci)
		// Nadajemy mu dany kolor za pomoca TextureSeta jezeli spawnuje sie poraz pierwszy 
		// Resetujemy jego ekwipunek (this->items)
	case Parser::SubType::RESPAWN:
		Logger::debug("Our player respawn info received.");
		Logger::debug(std::to_string(ev.subdata[0]) + std::to_string(ev.subdata[1]));
		if (this->spawnedFirstTime == false) {
			this->playerInfos.push_back(std::make_shared<OurPlayerInfo>(this->playerName, (int)ev.subdata[0], (int)ev.subdata[1], this->playerTextures[this->currentTextureSet], 0));
			this->currentTextureSet++;
			if (this->currentTextureSet == 4) {
				this->currentTextureSet = 0;
			}
			this->spawnedFirstTime = true;
		}
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerInfos[i]->getPlayerName() == this->playerName) {
				this->playerInfos[i]->setIsAlive(true);
				this->playerInfos[i]->setNewPosition(ev.subdata[0], ev.subdata[1]);
				this->playerInfos[i]->setPrevPosition(0, 0);
			}
		}
		this->items.clear();
		this->xOurPos = (int)ev.subdata[0];
		this->yOurPos = (int)ev.subdata[1];
		this->gameStage = GameStage::ALIVE;
		break;
		// Event, ktory dostajemy, jezeli jakis gracz wejdzie w nasze pole widzenia. Jezeli widzimy go po raz pierwszy to takze
		// zostanie mu nadany nowy kolor
	case Parser::SubType::PSPAWN:
		newPlayer = true;
		evString = std::string(ev.subdata);
		Logger::debug("PSpawn:" + evString.substr(3, evString.size() - 3) + "x: " + evString.substr(0, 1) + "y: " + evString.substr(1, 1));
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerInfos[i]->getPlayerName() == evString.substr(3, evString.size() - 3)) {
				Logger::debug("Other player spawn existing info received");
				this->playerInfos[i]->setNewPosition((int)ev.subdata[0], (int)ev.subdata[1]);
				this->playerInfos[i]->setIsAlive(true);
				newPlayer = false;
				switch (ev.subdata[2]) {
				case 'W':
					this->playerInfos[i]->setPrevPosition(this->playerInfos[i]->getX(), this->playerInfos[i]->getY() + 1);
					break;
				case 'A':
					this->playerInfos[i]->setPrevPosition(this->playerInfos[i]->getX()+1, this->playerInfos[i]->getY());
					break;
				case 'S':
					this->playerInfos[i]->setPrevPosition(this->playerInfos[i]->getX(), this->playerInfos[i]->getY() - 1);
					break;
				case 'D':
					this->playerInfos[i]->setPrevPosition(this->playerInfos[i]->getX()-1, this->playerInfos[i]->getY());
					break;

				}
				
			}
		}
		if (newPlayer == true) {
			Logger::debug("Other player spawn new info received.");
			this->playerInfos.push_back(std::make_shared<PlayerInfo>(evString.substr(3, evString.size() - 3), (int)ev.subdata[0], (int)ev.subdata[1], this->playerTextures[this->currentTextureSet], 0));
			this->currentTextureSet++;
			if (this->currentTextureSet == 4) {
				this->currentTextureSet = 0;
			}
			switch (ev.subdata[2]) {
			case 'W':
				this->playerInfos[this->playerInfos.size() - 1]->setPrevPosition(this->playerInfos[this->playerInfos.size() - 1]->getX(), this->playerInfos[this->playerInfos.size() - 1]->getY() + 1);
				break;
			case 'A':
				this->playerInfos[this->playerInfos.size() - 1]->setPrevPosition(this->playerInfos[this->playerInfos.size() - 1]->getX() + 1, this->playerInfos[this->playerInfos.size() - 1]->getY());
				break;
			case 'S':
				this->playerInfos[this->playerInfos.size() - 1]->setPrevPosition(this->playerInfos[this->playerInfos.size() - 1]->getX(), this->playerInfos[this->playerInfos.size() - 1]->getY() - 1);
				break;
			case 'D':
				this->playerInfos[this->playerInfos.size() - 1]->setPrevPosition(this->playerInfos[this->playerInfos.size() - 1]->getX() - 1, this->playerInfos[this->playerInfos.size() - 1]->getY() + 1);
				break;
			}
		}
		break;
		// Informacja o tym jak zmienilo sie nasze HP
	case Parser::SubType::DAMAGE:
		Logger::debug("Player new hp: " + ev.subdata);
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerName == this->playerInfos[i]->getPlayerName()) {
				OurPlayerInfo *ourPlayer = (OurPlayerInfo*)(&*(this->playerInfos[i]));
				ourPlayer->health = std::stoi(ev.subdata);
				this->maxHealth = ourPlayer->maxHealth;
				this->health = ourPlayer->health;
			}
		}
		break;
		// Informacja o tym jak zmieniolo sie nasze MaxHp (w przypadku zebrania tarczy)
	case Parser::SubType::MAXHEALTH:
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerName == this->playerInfos[i]->getPlayerName()) {
				OurPlayerInfo* ourPlayer = (OurPlayerInfo*)(&*(this->playerInfos[i]));
				ourPlayer->maxHealth = std::stoi(ev.subdata);
				this->maxHealth = ourPlayer->maxHealth;
				this->health = ourPlayer->health;
			}
		}
		break;
		// Informacja o tym, ze podnieslismy dany przedmiot
	case Parser::SubType::PICKUP:
		Logger::debug("Picked item: " + ev.subdata[0]);
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerName == this->playerInfos[i]->getPlayerName()) {
				OurPlayerInfo* ourPlayer = (OurPlayerInfo*)(&*(this->playerInfos[i]));
				// Na serwerze ITEM_SWORD, ITEM_SHIELD, ITEM_BOOTS, ITEM_POTION, PLAYER, BODY
				int clientItemID;
				switch (std::stoi(ev.subdata)) {
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
					Logger::error("Wrong item type received!");
					clientItemID = (int)ItemType::BONES;
				}
				for (size_t i = 0; i < this->items.size(); i++) {
					if ((int)this->items[i]->getType() == clientItemID && this->items[i]->isOnMap() == false) {
						newItem = false;
					}
				}
				if (newItem) {
					this->items.push_back(std::make_shared<Item>((ItemType)clientItemID, -1, -1, *(tileObjectsTextures[clientItemID]), false, true));

				}
				//ourPlayer->pocket.push_back(std::make_shared<Item>((ItemType)clientItemID, -1, -1, *(tileObjectsTextures[clientItemID]), false, true));
			}
		}
		break;
		// Informacja o tym, ze dany gracz wyszedl z danego pola widzenia. Jezeli to my sparwimy, ze inny gracz znajdzie sie poza naszym
		// polem widzenia, to wtedy nie potrzebujemy eventa, ktory nas o tym powiadiomi.
	case Parser::SubType::MOVEOUT:
		Logger::debug("Moveout: " + ev.subdata.substr(4, ev.subdata.size() - 4));
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (ev.subdata.substr(4, ev.subdata.size() - 4) == this->playerInfos[i]->getPlayerName()) {
				this->playerInfos[i]->setIsAlive(false);
				this->playerInfos[i]->setNewPosition((int)ev.subdata[0], (int)ev.subdata[1]);
				this->playerInfos[i]->setPrevPosition((int)ev.subdata[2], (int)ev.subdata[3]);
			}
		}
		break;
	// DESPAWN PDESPAWN KILLCOUNT SPAWN
		// Informacja o spawnie danego przedmiotu na mapie
	case Parser::SubType::SPAWN:
		Logger::debug("Spawn item: " + std::string("ObjID: ") + ev.subdata[2] + "X: " + ev.subdata[0] + "Y: " + ev.subdata[1]);
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
			Logger::error("Wrong item type received!");
			wrongItem = true;
		}
		if (!wrongItem) 
			this->items.push_back(std::make_shared<Item>((ItemType)clientItemID, (int)ev.subdata[0], (int)ev.subdata[1], *(tileObjectsTextures[clientItemID]), true, false));
		break;
		// Informacja o zniknieciu z mapy danego itema
	case Parser::SubType::DESPAWN:
		// Despawnuje item na danym X i Y wiec musze znalezc w vectorze itemsow miejsce w ktorym jest item o danym X i Y
	 	Logger::debug("Despawn item: " + std::string(" X: ") + ev.subdata[0] + " Y: " + ev.subdata[1]);
		for (size_t i = 0; i < this->items.size(); i++) {
			if (this->items[i]->getX() == (int)ev.subdata[0] && this->items[i]->getY() == (int)ev.subdata[1]) {
				this->items.erase(this->items.begin() + i);
				Logger::debug("Despawning item " + std::to_string(i));
				break;
			}
		}
		break;
		// Informacja o zniknieciu danego gracza
	case Parser::SubType::PDESPAWN:
		Logger::debug("Player despawn: " + ev.subdata.substr(2, ev.subdata.size() - 2));
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (ev.subdata.substr(2, ev.subdata.size() - 2) == this->playerInfos[i]->getPlayerName()) {
				this->playerInfos[i]->setIsAlive(false);
			}
		}
		break;
		// Informacja o liczbie zabojstw danego gracza
	case Parser::SubType::KILLCOUNT:
		Logger::debug("Killcount" + ev.subdata.substr(1, ev.subdata.size()-1) + "num:" + std::to_string((int)ev.subdata[0]));
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (ev.subdata.substr(1, ev.subdata.size() - 1) == this->playerInfos[i]->getPlayerName()) {
				this->playerInfos[i]->setKillCount((int)ev.subdata[0]);
			}
		}
		break;
		// Informacja o tym, ze jakis gracz zostal uderzony (tylko powoduje stworzeniu obiektu animacji)
	case Parser::SubType::HIT:
		Logger::debug("Hit: " + ev.subdata);
		for (size_t i = 0; i < this->playerInfos.size(); i++) {
			if (this->playerInfos[i]->getPlayerName() == ev.subdata) {
				this->damages.push_back(Damage(conf->SPRITE_WIDTH* this->playerInfos[i]->getX(), conf->SPRITE_WIDTH* this->playerInfos[i]->getY(), damageTextures, 3));
			}
		}
		break;
	default:
		Logger::error("Error, event subtype not found.");
		Logger::error(ev);
	}
}

void Client::handleInitPlayer(Parser::Event ev)
{
	int newID = ev.subdata[0];

	if (newID == -1)
	{
		Logger::error("There was a existing player with that nick.");
		//this->playerName += " Copy";
		//output.addEventNewPlayer(ID, 0, playerName);
		return;
	}
	this->ID = newID;
	output.addInnerInitPlayer(0, -1, ID);
	Logger::debug("New ID " + std::to_string(this->ID));

	//you are a player too now
	playerList.push_back(this->playerName);

}

void Client::handleNewPlayer(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	Logger::debug("New player: " + playerName);
	this->playerList.push_back(playerName);
}


void Client::handleDisconnectPlayer(Parser::Event ev)
{
	std::string playerName = ev.subdata;
	for (size_t i=0;i<playerList.size();i++)
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
	this->output.addInnerDiscPlayer(conf->SERVER_ID, this->ID);
	this->cState = ConnectionState::FAILED;
	this->gameStage = GameStage::NOTJOINED;
	this->reconnect();
}

void Client::handleTimeout(Parser::Event ev)
{
	output.addEventTimeoutAnswer(ID, 0);
}

void Client::loadUITextures() {
	if (!buttonTexture.loadFromFile("data/button.png"))
	{
		Logger::error("Error. File button.png not found.");
	}
	if (!barTexture.loadFromFile("data/hpbar.png"))
	{
		Logger::error("Error. File hpbar.png not found.");
	}
	if (!fogTexture.loadFromFile("data/fog.png"))
	{
		Logger::error("Error. File fog.pnh not found.");
	}
	for (int i = 0; i < 3; i++) {
		this->damageTextures.push_back(std::make_shared<sf::Texture>());
		this->damageTextures[i]->loadFromFile("data/damage.png", sf::IntRect(i * conf->SPRITE_WIDTH, 0, 32, 32));
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
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 8; i++) {
			if (j == 3 && i == 1) break;
			tileObjectsTextures.push_back(std::make_shared<sf::Texture>());
			tileObjectsTextures[i + j * 8]->loadFromFile("data/tilesObjects.png", sf::IntRect(i * 32, j * 32, 32, 32));
		}
	}
}

void Client::reconnect() {
	this->currentTextureSet = 0;
	this->spawnedFirstTime = false;
	this->health = conf->defaultHealth;
	this->maxHealth = conf->defaultHealth;
	this->playerInfos.clear();
	this->items.clear();
	this->damages.clear();
	getIn.changeVisibility(true);
}

// Wywolywane w momencie wyjscia z fazy GameEnd. Resetujemy stan naszej mapy, UI.
void Client::gameReset() {
	this->voted = false;
	this->vote.changeVisibility(true);
	this->vote.setText("Vote");
	this->currentTextureSet = 0;
	this->spawnedFirstTime = false;
	this->health = conf->defaultHealth;
	this->maxHealth = conf->defaultHealth;
	this->playerInfos.clear();
	this->items.clear();
	this->damages.clear();
}