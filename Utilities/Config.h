#pragma once

#include <string>
#include <fstream>
#include "Logger.h"


namespace Config
{

	//real static

	const static int bufferLength = 200;
	const static int msgLengthLimit = 50;
	const static std::string confName = "./data/config.txt";


	const static bool debug = 0;

	//to mozna dac w configu potem
	static int sightValue = 5;
	static int timeoutValue = 10000;
	static int spikeDmg = 2;
	static int lavaDmg = 30;
	static int defaultHealth = 10;
	static int defaultDmg = 3;
	static int bodyBonus = 2;
	static int attackRange = 3;
	static int attackCooldown = 10;

	static int shieldBonus = 10;
	static int potionBonus = 10;
	static int swordBonus = 2;
	static int bootsBonus = 2;


	static int mapID = 0;



	static std::string port= "7777";
	static std::string playerName = "Player";
	static std::string IPAddress = "127.0.0.1";

	static int ITEM_RESPAWN_TIME = 90;
	static int PLAYER_RESPAWN_TIME = 5;

	//zmiana tego zepsuje program
	const static int SERVER_ID = 0;
	static float serverTurnTimer = 0.05f;
	static float clientTurnTimer = 0.01f;
	static int gameTickRate = 5;

	// Opcje grafiki klienta
	const static int SCREEN_WIDTH = 960;
	const static int SCREEN_HEIGHT = 720;
	const static int SPRITE_WIDTH = 32;
	const static int SPRITE_HEIGHT = 32;

	const static int GAME_SCREEN_WIDTH = 768;
	const static int GAME_SCREEN_HEIGHT = 576;


	void loadConfig();

	void processConfigLine(std::string line);

	void setConfigValue(std::string token, std::string value);
};

