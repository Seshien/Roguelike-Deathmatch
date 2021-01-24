#pragma once
#include <iostream>
#include <string>
#include <stdlib.h>
#include <SFML\Graphics.hpp>
#include "..\..\Utilities\Event.h"
#include "..\..\Utilities\Network.h"
#include "..\..\Utilities\Parser.h"
#include "..\..\Utilities\Logger.h"
#include "..\..\Utilities\Config.h"
#include "Map.h"
#include "Item.h"
#include "PlayerInfo.h"
#include "OurPlayerInfo.h"
#include "Damage.h"

#include "UIButton.h"
#include "UIBar.h"

class Client
{
public:

	Client()
	{
		//tymczasowe ID
		ID = -1;
		cState = ConnectionState::NOTCONNECTED;
	}

	void startClient();

private:

	enum class ConnectionState {NOTCONNECTED, CONNECTED, FAILED};
	enum class GameStage {NOTJOINED, LOBBY, DEAD, ALIVE, END};
		

	void connectClient();
	void startWindow();


	sf::RenderWindow window;
	sf::RectangleShape rectangle;
	Parser::Messenger output;
	Parser::Messenger input;
	
	sf::View gameView;
	sf::View lobbyView;
	sf::View interfaceView;
	sf::Font font;

	int numVotes;
	std::string playerName="DefaultName";

	int ID;
	int mapID;

	int health;
	int maxHealth;

	ConnectionState cState;
	GameStage gameStage;

	std::string port;
	std::string IPAddress;

	std::vector<std::string> playerList;

	Network network;

	std::string winner;

	//std::array<sf::Texture, 16> tileTextures;

	sf::Texture buttonTexture;
	sf::Texture barTexture;
	sf::Texture fogTexture;

	std::vector<std::shared_ptr<sf::Texture>> tileObjectsTextures;
	std::vector<std::vector<std::shared_ptr<sf::Texture>>> playerTextures;

	std::vector<std::shared_ptr<sf::Texture>> damageTextures;

	UIButton getIn;
	UIButton respawn;
	UIButton vote;
	UIBar hpBar;

	int xOurPos;
	int yOurPos;

	Map map;

	std::vector<std::shared_ptr<Item>> items;
	std::vector<std::shared_ptr<PlayerInfo>> playerInfos;
	std::vector<Damage> damages;


	// Information about which player textureSet has to be used now to create new player in PlayerInfo object
	int currentTextureSet;

	boolean voted;

	boolean spawnedFirstTime;

	sf::Clock turnTimer;

	void startLogger();

	void mainLoop();
	void handleNetEvents(Parser::Messenger mess);
	void handleIntEvents();
	void handleServer(Parser::Event ev);
	void handleLobby(Parser::Event ev);
	void handleGame(Parser::Event ev);

	void handleInitPlayer(Parser::Event ev);
	void handleNewPlayer(Parser::Event ev);
	void handleTimeout(Parser::Event ev);
	void handleDisconnectPlayer(Parser::Event ev);
	void handleLostConnection(Parser::Event ev);

	void loadTileTextures();
	void loadPlayerTextures();
	void loadUITextures();
	void graphicsUpdate();

	void centerMap();


	void loadConfig();
	void processConfigLine(std::string line);
	void setConfigValue(std::string token, std::string value);

	void gameReset();

};

