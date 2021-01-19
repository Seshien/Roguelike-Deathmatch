#pragma once
#include "SpawnableObject.h"
#include "PlayerObject.h"
#include "Map.h"
#include "..\..\Utilities\Event.h"
#include "..\..\Utilities\Logger.h"
#include "..\..\Utilities\Parser.h"
#include "..\..\Utilities\Constants.h"

class Game
{
public:
	Game()
	{

	}

	void startGame()
	{
		spawnObjects();
		spawnPlayers();
	}

	void startMap(std::string mapPath)
	{
		this->map.init(mapPath);

	}

	void spawnObjects()
	{

	}

	void spawnPlayers()
	{

	}

	Parser::Messenger loopGame(Parser::Messenger input)
	{
		output = Parser::Messenger();
		if (this->tickTimer > Constants::gameTickRate)
		{
			tickTimer++;
			return output;
		}

		tickTimer = 0;
		for (auto object = tickObjList.begin(); object != tickObjList.end();)
		{
			auto result = object->get()->tick();
			if (result == tickResult::SPAWNED)
			{
				object->get()->spawn();
				this->spawnEvent(*object);
				object = tickObjList.erase(object);
				
			}
			//to jest body
			else if (result == tickResult::DESPAWNED)
			{
				object->get()->despawn();
				this->despawnEvent(*object);
				object = tickObjList.erase(object);
			}
			else ++object;
		}
		for (auto object = tickPlayList.begin(); object != tickPlayList.end();)
		{
			auto result = object->get()->tick();
			if (result == tickResult::SPAWNED)
			{
				object->get()->spawn();
				this->spawnEvent(*object);
				object = tickPlayList.erase(object);

			}
			else ++object;
		}
		for (auto ev : input.eventList)
		{

		}

	}
	//wasd
	void handleMovement(Parser::Event ev)
	{
		int playerIndex = this->findPlayerIndex(ev.sender);
		if (playerIndex == -1)
		{
			Logger::log("Error: Move Player failure, player not found");
			return;
		}
		handleMovement(this->gamePlayerList[playerIndex], ev.subdata[0]);
	}

	void handleMovement(std::shared_ptr<PlayerObject> player, int movement)
	{
		std::shared_ptr<Tile> tile = getMovementTile(movement, player->getTile());

		if (tile->isRuchAble)
		{
			player->move(tile);
			this->moveEvent(player);
			this->checkVisionTiles(player, movement, player->getTile());
			switch (tile->type)
			{
			case TileType::GROUND:
				break;
			case TileType::GROUND_SLIPPY:
				if (getMovementTile(movement, tile)->type == TileType::GROUND_SLIPPY)
				{
					player->move(tile);
					this->moveEvent(player);
					this->checkVisionTiles(player, ev.subdata[0], player->getTile());
				}
				break;
			case TileType::EMPTY:
				killPlayer(player);
				break;
			case TileType::SPIKES:
				damagePlayer(player, Constants::spikeDmg);
			case TileType::LAVA:
				killPlayer(player);
				break;
			}
			if (tile->isItem)
			{
				pickupItem(player, tile->itemID);
			}
			player->lastMove = movement;
		}

	}
	void handleAttack(Parser::Event ev)
	{
		int playerIndex = findPlayerIndex(ev.sender);
		if (playerIndex == -1)
		{
			Logger::log("Error: Attack Player failure, player not found");
			return;
		}
		auto player = this->gamePlayerList[playerIndex];
		int movement = player->lastMove;
		this->checkVisionTiles(player, movement, player->getTile());
		for (int i = 1; i < player->getRange(); i++)
		{
			this->checkVisionTiles(player, movement, this->getMovementTile();

		}
	}

	std::shared_ptr<Tile> getMovementTile(int movement, std::shared_ptr<Tile> tile, int range=1)
	{
		int x = tile->x;
		int y = tile->y;
		switch (movement)
		{
		case 'w':
			return this->getTile(x, y + range);
			break;
		case 's':
			return this->getTile(x, y - range);
			break;
		case 'a':
			return this->getTile(x - range, y);
			break;
		case 'd':
			return this->getTile(x + range, y);
			break;
		}
	}
	std::shared_ptr<Tile> getAttackTile(int movement, std::shared_ptr<Tile> tile, int range = 1)
	{
		int x = tile->x;
		int y = tile->y;
		switch (movement)
		{
		case 'W':
			for (int i = range; i > 1; i--) if (this->getTile(x, y + range)->isRuchAble) return this->getTile(x, y + range);
			return this->getTile(x, y + 1);
			break;
		case 'S':
			for (int i = range; i > 1; i--) if (this->getTile(x, y - range)->isRuchAble) return this->getTile(x, y - range);
			return this->getTile(x, y - 1);
			break;
		case 'A':
			for (int i = range; i > 1; i--) if (this->getTile(x - range, y)->isRuchAble) return this->getTile(x - range, y);
			return this->getTile(x - 1, y);
			break;
		case 'D':
			for (int i = range; i > 1; i--) if (this->getTile(x + range, y)->isRuchAble) return this->getTile(x + range, y);
			return this->getTile(x + 1, y);
			break;
		}
	}
	void checkVisionTiles(std::shared_ptr<PlayerObject> player, int movement, std::shared_ptr<Tile> tile)
	{
		int x = tile->x;
		int y = tile->y;
		switch (movement)
		{
		case 'W':
			for (int i = x - Constants::sightValue; i <= x + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(i, y + Constants::sightValue));
			}
			break;
		case 'S':
			for (int i = x - Constants::sightValue; i <= x + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(i, y - Constants::sightValue));
			}
			break;
		case 'A':
			for (int i = y - Constants::sightValue; i <= y + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(x - Constants::sightValue, i));
			}
			break;
		case 'D':
			for (int i = y - Constants::sightValue; i <= y + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(x + Constants::sightValue, i));
			}
			break;
		}
	}
	void getVision(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile)
	{
		if (tile->isItem)
		{
			auto item = this->gameObjectList[this->findItemIndex(tile->itemID)];
			this->spawnEvent(player, item);
		}
		if (tile->isPlayer)
		{
			auto player = this->gamePlayerList[this->findPlayerIndex(tile->playerID)];
			this->spawnEvent(player, player);
		}

	}

	void damagePlayer(std::shared_ptr<PlayerObject> player, int dmgValue)
	{
		int newHealth = player->getHealth() - dmgValue;
		if (newHealth > player->getMaxHealth()) player->setHealth(player->getMaxHealth());
		else
			player->setHealth(newHealth);

		this->hitEvent(player, dmgValue);

		if (newHealth <= 0)
			killPlayer(player);
	}

	void addPlayer(int playerID)
	{
		// znalezc jakies miejsce gdzie moze sie zespawnic
		auto player = std::make_shared<PlayerObject>(playerID, SpawnableObjectType::PLAYER, this->getTile(playerID, playerID));
		gamePlayerList.push_back(player);
		tickPlayList.push_back(player);

	}

	void killPlayer(std::shared_ptr<PlayerObject> player)
	{
		// usuwamy gracza z mapy
		player->despawn();
		player->startSpawnTimer();
		// tworzyc nowy obiekt z trupem
		std::shared_ptr<SpawnableObject> body = std::make_shared<SpawnableObject>(SpawnableObjectType::BODY, player->getTile());
		body->spawn();
		body->startSpawnTimer();
		//dodajemy do tickow
		this->gameObjectList.push_back(body);
		this->tickObjList.push_back(body);
		this->tickPlayList.push_back(player);
		//eventy
		this->despawnEvent(player);
		this->spawnEvent(body);
		// 
	}

	void pickupItem(std::shared_ptr<PlayerObject> player, int itemID)
	{
		auto item = this->gameObjectList[this->findItemIndex(itemID)];
		switch (item->getType())
		{
		case SpawnableObjectType::BODY:
			this->damagePlayer(player, Constants::bodyHeal * -1);
			item->despawn();
			break;
		default:
			this->pickupEvent(player, item);
			item->despawn();
			item->startSpawnTimer();
			this->tickObjList.push_back(item);
			break;
		}
		this->despawnEvent(item);
	}

	void createObject(std::shared_ptr<SpawnableObject> object)
	{
		
	}

	void createPlayer(std::shared_ptr<PlayerObject> object)
	{

	}

	void moveEvent(std::shared_ptr<PlayerObject> player)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(player, player))
				output.addEventMovement();
		}
	}
	void spawnEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<SpawnableObject> object)
	{
		output.addEventSpawn();
	}
	void spawnEvent(std::shared_ptr<SpawnableObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventSpawn();
		}
	}
	void despawnEvent(std::shared_ptr<SpawnableObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventDespawn();
		}
	}
	void attackEvent(std::shared_ptr<PlayerObject> player)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(player, player))
				output.addEventAttack();
		}
	}
	void hitEvent(std::shared_ptr<PlayerObject> player, int dmg)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(player, player))
				output.addEventHit();
		}
	}
	void pickupEvent(std::shared_ptr<PlayerObject> player, std::shared_ptr<SpawnableObject> object)
	{
		output.addEventPickUp();
	}

	int checkRange(std::shared_ptr<SpawnableObject> objectF, std::shared_ptr<SpawnableObject> objectS, int range = Constants::sightValue)
	{
		int objX = objectF->getX();
		int objY = objectF->getY();
		int x1 = objectS->getX() - Constants::sightValue;
		int y1 = objectS->getY() - Constants::sightValue;
		int x2 = x1 + Constants::sightValue * 2;
		int y2 = y2 + Constants::sightValue * 2;
		return objX > x1 && objX < x2 && objY > y1 && objY < y2;

	}

	void deletePlayer(int playerID)
	{
		int index = findPlayerIndex(playerID);
		if (index == -1)
		{
			Logger::log("Error: Delete Player failure, player not found");
			return;
		}

		killPlayer(gamePlayerList[index]);

		gamePlayerList.erase(gamePlayerList.begin() + index);

	}

	void deleteItem(int itemID)
	{
		int index = findItemIndex(itemID);
		if (index == -1)
		{
			Logger::log("Error: Delete item failure, item not found");
			return;
		}
		gameObjectList.erase(gameObjectList.begin() + index);

	}
	int findPlayerIndex(int playerID)
	{
		if (gamePlayerList[playerID - 1]->getplayerID() == playerID) return playerID - 1;
		for (int i = 0; i < gamePlayerList.size(); i++)
			if (gamePlayerList[i]->getplayerID() == playerID)
				return i;

		return -1;
	}

	int findItemIndex(int itemID)
	{
		if (this->gameObjectList[itemID]->getItemID() == itemID) return itemID;
		for (int i = 0; i < gameObjectList.size(); i++)
			if (gameObjectList[i]->getItemID() == itemID)
				return i;

		return -1;
	}

	std::shared_ptr<Tile> getTile(int x, int y)
	{
		if (x >= 0 && x <= 99 && y >= 0 && y <= 99)
			return this->map.tileArray[y][x];
		else
			return this->map.tileArray[0][0];
	}

private:

	int tickTimer;
	Map map;
	Parser::Messenger output;
	std::vector<std::shared_ptr<SpawnableObject>> gameObjectList;
	std::vector<std::shared_ptr<SpawnableObject>> tickObjList;
	std::vector<std::shared_ptr<PlayerObject>> gamePlayerList;
	std::vector<std::shared_ptr<PlayerObject>> tickPlayList;
};

