#pragma once
namespace Constants
{
	const static bool debug = 0;
	const static int bufferLength = 200;
	const static int msgLengthLimit = 50;
	//to mozna dac w configu potem
	const static int sightValue = 5;
	const static int timeoutValue = 10000;
	const static int spikeDmg = 2;
	const static int defaultHealth = 10;
	const static int defaultDmg = 3;
	const static int bodyBonus = 2;
	const static int attackRange = 3;
	const static int attackCooldown = 3;
	//zmiana tego zepsuje program
	const static int SERVER_ID = 0;
	const static float serverTurnTimer = 0.05f;
	const static float clientTurnTimer = 0.01f;
	const static int gameTickRate = 5;
	// Opcje grafiki klienta
	const static int SCREEN_WIDTH = 960;
	const static int SCREEN_HEIGHT = 720;
	const static int SPRITE_WIDTH = 32;
	const static int SPRITE_HEIGHT = 32;

	const static int GAME_SCREEN_WIDTH = 768;
	const static int GAME_SCREEN_HEIGHT = 576;

	const static int ITEM_RESPAWN_TIME = 30;
	const static int PLAYER_RESPAWN_TIME = 5;
};

