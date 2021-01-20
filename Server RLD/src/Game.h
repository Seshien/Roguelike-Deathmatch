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
#include "..\..\Utilities\Constants.h"

class Game
{
public:
	Game()
	{

	}

	void startGame(std::vector<int> playerID, std::vector<std::string> playerName)
	{
		spawnObjects();
		addPlayers(playerID, playerName);
	}

	void startMap(std::string mapPath)
	{
		this->map.init(mapPath);

	}

	void spawnObjects()
	{

	}

	void addPlayers(std::vector<int> playerID, std::vector<std::string> playerName)
	{
		for (int x = 0; x < playerID.size();x++)
		{
			if (addPlayer(playerID[x], playerName[x]))
			{
				Logger::log("Spawn was unsuccesfull");
			}
		}

	}

	int addPlayer(int playerID, std::string playerName)
	{
		int i = 0, j = 0;
		for (i = 0; i < map.MAP_HEIGHT; i++)
		{
			for (j = 0; j < map.MAP_WIDTH; j++)
			{
				auto tile = this->map.tileArray[i][j];
				if (tile->isSpawnable && !tile->isItem && !tile->isPlayer)
				{
					auto player = std::make_shared<PlayerObject>(playerID, playerName, SpawnableObjectType::PLAYER, tile);
					gamePlayerList.push_back(player);
					this->addPlayerSpawnToTick(player);
					return 0;
				}

			}

		}
		return 1;
	}

	Parser::Messenger loopGame(Parser::Messenger input)
	{
		Logger::log("Loop game.");
		output = Parser::Messenger();
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
			Logger::log("Tick!");
			auto result = object->get()->tick();
			if (result == tickResult::SPAWNED)
			{
				object->get()->readyToRespawn = true;
				this->respawnAskEvent(*object);
				object = tickPlayList.erase(object);

			}
			else ++object;
		}
		for (auto ev : input.eventList)
		{
			switch (ev.subtype)
			{
			case Parser::SubType::MOVE:
				handleMovement(ev);
				break;
			case Parser::SubType::ATTACK:
				handleAttack(ev);
				break;
			case Parser::SubType::ACTION:
				//do something
				break;
			case Parser::SubType::NEWPLAYER:
				addPlayer(ev.receiver, ev.subdata);
				break;
			case Parser::SubType::RESPAWN:
				handleRespawn(ev);
				break;
			default:
				Logger::log("Error: Unknown Game Subtype");
			}
		}
		return output;
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

		if (tile->isMovable)
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
					this->checkVisionTiles(player, movement, player->getTile());
				}
				break;
			case TileType::EMPTY:
				killPlayer(player);
				break;
			case TileType::SPIKES:
				damagePlayer(player, Constants::spikeDmg);
				break;
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
		handleAttack(this->gamePlayerList[playerIndex]);
	}
	void handleAttack(std::shared_ptr<PlayerObject> player)
	{
		int movement = player->lastMove;
		bool moved = false;
		this->attackEvent(player);
		for (int i = 0; i < player->getRange(); i++)
		{
			auto tile = player->getTile();
			auto moveTile = getMovementTile(movement, tile);
			if (moveTile->isPlayer)
			{
				auto hitPlayerIndex = findPlayerIndex(moveTile->playerID);
				if (hitPlayerIndex == -1)
				{
					Logger::log("Error: Attack Player failure, atacked player not found");
					return;
				}
				auto hitPlayer = this->gamePlayerList[hitPlayerIndex];
				damagePlayer(hitPlayer, player->getDamage());
			}
			else if (i < player->getRange() - 1 && moveTile->isMovable)
			{
				this->checkVisionTiles(player, movement, tile);
				player->move(moveTile);
				moved = true;
			}
			else
				break;
		}

		handleMovement(player, movement);
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
	//nieuzywane
	std::shared_ptr<Tile> getAttackTile(int movement, std::shared_ptr<Tile> tile, int range = 1)
	{
		int x = tile->x;
		int y = tile->y;
		switch (movement)
		{
		case 'W':
			for (int i = range; i > 1; i--) if (this->getTile(x, y + range)->isMovable) return this->getTile(x, y + range);
			return this->getTile(x, y + 1);
			break;
		case 'S':
			for (int i = range; i > 1; i--) if (this->getTile(x, y - range)->isMovable) return this->getTile(x, y - range);
			return this->getTile(x, y - 1);
			break;
		case 'A':
			for (int i = range; i > 1; i--) if (this->getTile(x - range, y)->isMovable) return this->getTile(x - range, y);
			return this->getTile(x - 1, y);
			break;
		case 'D':
			for (int i = range; i > 1; i--) if (this->getTile(x + range, y)->isMovable) return this->getTile(x + range, y);
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
			if (item->getExist())
				this->spawnEvent(player, item);
		}
		if (tile->isPlayer)
		{
			auto _player = this->gamePlayerList[this->findPlayerIndex(tile->playerID)];
			if (_player->getExist())
				this->spawnPlayerEvent(player, _player);
		}

	}

	void damagePlayer(std::shared_ptr<PlayerObject> player, int dmgValue)
	{
		int newHealth = player->getHealth() - dmgValue;
		if (newHealth > player->getMaxHealth()) player->setHealth(player->getMaxHealth());
		else
			player->setHealth(newHealth);

		this->hitEvent(player, dmgValue);
		this->damageEvent(player, dmgValue);

		if (newHealth <= 0)
			killPlayer(player);
	}

	void addPlayerSpawnToTick(std::shared_ptr<PlayerObject> player)
	{
		player->startSpawnTimer();

		tickPlayList.push_back(player);

	}

	void killPlayer(std::shared_ptr<PlayerObject> player)
	{
		// usuwamy gracza z mapy
		this->despawnEvent(player);
		player->despawn();
		player->startSpawnTimer();
		// tworzyc nowy obiekt z trupem
		std::shared_ptr<ItemObject> body = std::make_shared<ItemObject>(this->gameObjectList.size(),SpawnableObjectType::BODY, player->getTile());

		body->spawn();
		body->startSpawnTimer();
		this->gameObjectList.push_back(body);
		//dodajemy do tickow


		this->tickObjList.push_back(body);
		this->tickPlayList.push_back(player);
		//eventy
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

	void moveEvent(std::shared_ptr<PlayerObject> obj)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(obj, player))
				output.addEventMovement(Constants::SERVER_ID, player->getplayerID(), obj->getName(), obj->getX(), obj->getY());
		}
	}

	void spawnPlayerEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<PlayerObject> object)
	{
		output.addEventPlayerSpawn(Constants::SERVER_ID, player->getplayerID(), object->getName(), object->getX(), object->getY());
	}

	void spawnPlayerEvent(std::shared_ptr<PlayerObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventPlayerSpawn(Constants::SERVER_ID, player->getplayerID(), object->getName(), object->getX(), object->getY());
		}
	}

	void spawnEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<SpawnableObject> object)
	{
		output.addEventSpawn(Constants::SERVER_ID, player->getplayerID(), (int)object->getType(), object->getX(), object->getY());
	}

	void spawnEvent(std::shared_ptr<ItemObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventSpawn(Constants::SERVER_ID, player->getplayerID(), (int)object->getType(), object->getX(), object->getY());
		}
	}

	void respawnEvent(std::shared_ptr<PlayerObject> object)
	{
		output.addEventRespawn(Constants::SERVER_ID, object->getplayerID(), object->getX(), object->getY());
	}

	void respawnAskEvent(std::shared_ptr<PlayerObject> object)
	{
		output.addEventAskRespawn(Constants::SERVER_ID, object->getplayerID(), object->getX(), object->getY());
		Logger::log("Respawn ask event added");
	}

	void despawnEvent(std::shared_ptr<SpawnableObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventSpawn(Constants::SERVER_ID, player->getplayerID(), (int)object->getType(), object->getX(), object->getY());
		}
	}
	void attackEvent(std::shared_ptr<PlayerObject> obj)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(obj, player))
				output.addEventAttack(Constants::SERVER_ID, player->getplayerID(), obj->getName());
		}
	}
	void hitEvent(std::shared_ptr<PlayerObject> obj, int dmg)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(obj, player))
				output.addEventHit(Constants::SERVER_ID, player->getplayerID(), obj->getName(), dmg);
		}
	}
	void damageEvent(std::shared_ptr<PlayerObject> player, int newHealth)
	{
		output.addEventDamaged(Constants::SERVER_ID, player->getplayerID(), newHealth);
	}

	void pickupEvent(std::shared_ptr<PlayerObject> player, std::shared_ptr<SpawnableObject> object)
	{
		output.addEventPickUp(Constants::SERVER_ID, player->getplayerID(), (int) object->getType());
	}

	void handleRespawn(Parser::Event ev)
	{
		int playerIndex = this->findPlayerIndex(ev.sender);

		if (playerIndex == -1)
		{
			Logger::log("Error: Respawn event failed");
			return;
		}
		handleRespawn(this->gamePlayerList[playerIndex]);

	}

	void handleRespawn(std::shared_ptr<PlayerObject> object)
	{
		if (object->readyToRespawn)
		{
			object->spawn();
			this->spawnPlayerEvent(object);
			this->respawnEvent(object);
			this->getFullVision(object);
		}
	}

	void getFullVision(std::shared_ptr<PlayerObject> player)
	{
		int x1 = player->getX() - Constants::sightValue;
		int y1 = player->getY() - Constants::sightValue;
		int x2 = player->getX() + Constants::sightValue;
		int y2 = player->getY() + Constants::sightValue;
		for (int i = x1; i <= x2; i++)
			for (int j = y1; j <= y2; j++)
				this->getVision(player, this->getTile(i, j));

	}

	int checkRange(std::shared_ptr<SpawnableObject> objectF, std::shared_ptr<SpawnableObject> objectS, int range = Constants::sightValue)
	{
		int objX = objectF->getX();
		int objY = objectF->getY();
		int x1 = objectS->getX() - Constants::sightValue;
		int y1 = objectS->getY() - Constants::sightValue;
		int x2 = x1 + Constants::sightValue * 2;
		int y2 = y1 + Constants::sightValue * 2;
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
		//if (gamePlayerList[playerID - 1]->getplayerID() == playerID) return playerID - 1;
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

	Map map;
	Parser::Messenger output;
	std::vector<std::shared_ptr<ItemObject>> gameObjectList;
	std::vector<std::shared_ptr<ItemObject>> tickObjList;
	std::vector<std::shared_ptr<PlayerObject>> gamePlayerList;
	std::vector<std::shared_ptr<PlayerObject>> tickPlayList;
};

