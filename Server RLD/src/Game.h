#pragma once
#include <string>
#include <vector>
#include "SpawnableObject.h"
#include "PlayerObject.h"
#include "ItemObject.h"
#include "Map.h"
#include "..\..\Utilities\Event.h"
#include "..\..\Utilities\Logger.h"
#include "..\..\Utilities\Parser.h"
#include "..\..\Utilities\Config.h"

class Game
{
public:
	Game()
	{

	}

	void startGame(std::vector<int> playerID, std::vector<std::string> playerName);

	void startMap(std::string mapPath);

	void spawnObjects();

	void addPlayers(std::vector<int> playerID, std::vector<std::string> playerName);

	int addPlayer(int playerID, std::string playerName);


	Parser::Messenger loopGame(Parser::Messenger input);

	void handleKey(Parser::Event ev);
	//wasd
	void handleMovement(int playerID, Map::MOVEDIR movement);

	void handleMovement(std::shared_ptr<PlayerObject> player);

	void handleMovement(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile);

	void handleAttack(int playerID);

	void handleAttack(std::shared_ptr<PlayerObject> player);

	void checkVisionTiles(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile);

	void getVision(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile);

	void damagePlayer(std::shared_ptr<PlayerObject> hitPlayer, int dmgValue);

	void damagePlayer(std::shared_ptr<PlayerObject> player, std::shared_ptr<PlayerObject> hitPlayer, int dmgValue);

	void endGame(std::shared_ptr<PlayerObject> player);

	void maxHealthEvent(std::shared_ptr<PlayerObject> player);

	void killCountEvent(std::shared_ptr<PlayerObject> receiver, std::shared_ptr<PlayerObject> player);


	void killCountEvent(std::shared_ptr<PlayerObject> player);
	void allKillCountsEvent(std::shared_ptr<PlayerObject> player);
	void deathCountEvent(std::shared_ptr<PlayerObject> player);
	void allDeathCountsEvent(std::shared_ptr<PlayerObject> player);
	void addPlayerSpawnToTick(std::shared_ptr<PlayerObject> player);

	void killPlayer(std::shared_ptr<PlayerObject> player);

	void pickupItem(std::shared_ptr<PlayerObject> player, int itemID);

	void moveEvent(std::shared_ptr<PlayerObject> obj, std::shared_ptr<Tile> oldTile);

	void moveOutEvent(std::shared_ptr<Tile> oldTile, std::shared_ptr<PlayerObject> obj);

	void spawnPlayerEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<PlayerObject> object);

	void spawnPlayerEvent(std::shared_ptr<PlayerObject> object);

	void spawnEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<SpawnableObject> object);

	void spawnEvent(std::shared_ptr<ItemObject> object);

	void respawnEvent(std::shared_ptr<PlayerObject> object);

	void respawnAskEvent(std::shared_ptr<PlayerObject> object);

	void despawnEvent(std::shared_ptr<SpawnableObject> object);

	void despawnPlayerEvent(std::shared_ptr<PlayerObject> object);

	void attackEvent(std::shared_ptr<PlayerObject> obj);

	void hitEvent(std::shared_ptr<PlayerObject> obj, int dmg);

	void damageEvent(std::shared_ptr<PlayerObject> player, int newHealth);

	void pickupEvent(std::shared_ptr<PlayerObject> player, std::shared_ptr<SpawnableObject> object);

	void pickupEvent(std::shared_ptr<PlayerObject> player, int itemType);

	void handleRespawn(Parser::Event ev);

	void handleRespawn(std::shared_ptr<PlayerObject> object);

	void getFullVision(std::shared_ptr<PlayerObject> player);

	void deletePlayer(int playerID);

	void deleteItem(int itemID);

	int getPlayerIndex(int playerID);

	int getItemIndex(int itemID);

	std::shared_ptr<PlayerObject> getPlayer(int playerID);

private:

	Map map;
	Parser::Messenger output;
	int itemID=0;
	std::vector<std::shared_ptr<ItemObject>> gameObjectList;
	std::vector<std::shared_ptr<ItemObject>> tickObjList;
	std::vector<std::shared_ptr<PlayerObject>> gamePlayerList;
	std::vector<std::shared_ptr<PlayerObject>> tickPlayList;
};

