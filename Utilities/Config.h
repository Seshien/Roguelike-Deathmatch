#pragma once

#include <string>
#include <fstream>
#include "Logger.h"


namespace Config
{

	const static int bufferLength = 4096;
	const static int msgLengthLimit = 50;

	class ConfigHandler
	{
	public:

		std::string confName = "./data/config.txt";

		//to mozna dac w configu potem
		bool debug = false;
		int PlayersNeededToStartGame = 1;
		int sightValue = 5;
		int timeoutValue = 100;
		int spikeDmg = 2;
		int lavaDmg = 30;
		int defaultHealth = 10;
		int defaultDmg = 3;
		int bodyBonus = 2;
		int attackRange = 3;
		int attackCooldown = 10;

		int shieldBonus = 10;
		int potionBonus = 10;
		int swordBonus = 2;
		int bootsBonus = 2;

		int mapID = 0;

		std::string port = "7777";
		std::string playerName = "Player";
		std::string IPAddress = "127.0.0.1";

		int ITEM_RESPAWN_TIME = 90;
		int PLAYER_RESPAWN_TIME = 5;

		//zmiana tego zepsuje program
		int SERVER_ID = 0;
		float serverTurnTimer = 0.05f;
		float clientTurnTimer = 0.01f;
		int gameTickRate = 5;

		// Opcje grafiki klienta
		int SCREEN_WIDTH = 960;
		int SCREEN_HEIGHT = 720;
		int SPRITE_WIDTH = 32;
		int SPRITE_HEIGHT = 32;

		int GAME_SCREEN_WIDTH = 768;
		int GAME_SCREEN_HEIGHT = 576;

		void loadConfig();

	private:

		void processConfigLine(std::string line);

		void setConfigValue(std::string token, std::string value);

	};

	std::shared_ptr<ConfigHandler> getConfigHandler();

};

