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
	enum class MOVEDIR { UP, DOWN, LEFT, RIGHT };
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

		if (int playerIndex = this->getPlayerIndex(playerID) == -1)
		{
			for (int i = 0; i < map.MAP_HEIGHT; i++)
			{
				for (int j = 0; j < map.MAP_WIDTH; j++)
				{
					auto tile = this->getTile(i, j);//map.tileArray[i][j];
					if (tile == nullptr)
						continue;
					if (tile->canSpawn())
					{
						auto player = std::make_shared<PlayerObject>(playerID, playerName, SpawnableObjectType::PLAYER, tile);
						gamePlayerList.push_back(player);
						this->addPlayerSpawnToTick(player);
						allKillCountsEvent(player);
						return 0;
					}
				}
			}
		}
		else
		{
			auto player = gamePlayerList[playerIndex];
			allKillCountsEvent(player);
			if (player->getExist())
			{
				this->respawnEvent(player);
				this->damageEvent(player, player->getHealth());
				for (auto Item :player->getItems())
					this->pickupEvent(player, (int)Item);
				this->getFullVision(player);
				return 0;
			}
			else
			{
				this->addPlayerSpawnToTick(player);
				return 0;
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
				this->deleteItem((*object)->getItemID());
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
				//handleMovement(ev);
				break;
			case Parser::SubType::ATTACK:
				//handleAttack(ev);
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
			case Parser::SubType::KEY:
				handleKey(ev);
				break;
			default:
				Logger::log("Error: Unknown Game Subtype");
			}
		}
		return output;
	}

	void handleKey(Parser::Event ev)
	{
		switch (ev.subdata[0])
		{
		case ('W'):
			handleMovement(ev.sender, MOVEDIR::UP);
			break;
		case ('S'):
			handleMovement(ev.sender, MOVEDIR::DOWN);
			break;
		case ('A'):
			handleMovement(ev.sender, MOVEDIR::LEFT);
			break;
		case ('D'):
			handleMovement(ev.sender, MOVEDIR::RIGHT);
			break;
		case ' ':
			handleAttack(ev.sender);
			break;
		default:
			Logger::error("Error: Not handled key");
		}
	}
	//wasd
	void handleMovement(int playerID, MOVEDIR movement)
	{
		auto player = this->getPlayer(playerID);
		if (player == nullptr)
		{
			Logger::log("Error: Move Player failure, player not found");
			return;
		}
		player->lastMove = (int) movement;
		handleMovement(player);
	}

	void handleMovement(std::shared_ptr<PlayerObject> player)
	{
		std::shared_ptr<Tile> oldTile = player->getTile();
		MOVEDIR movement = (MOVEDIR) player->lastMove;
		std::shared_ptr<Tile> tile = getMovementTile(movement, player->getTile());

		if (tile->canMove())
		{
			player->move(tile);
			this->moveEvent(player);
			this->moveOutEvent(oldTile, player);
			this->checkVisionTiles(player, player->getTile());
			switch (tile->getType())
			{
			case TileType::GROUND:
				break;
			case TileType::GROUND_SLIPPY:
				if (getMovementTile(movement, tile)->getType() == TileType::GROUND_SLIPPY)
				{
					player->move(tile);
					this->moveEvent(player);
					this->moveOutEvent(tile, player);
					this->checkVisionTiles(player, player->getTile());
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
			if (tile->haveItem())
			{
				pickupItem(player, tile->getItemID());
			}
		}
	}
	void handleMovement(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile)
	{
		std::shared_ptr<Tile> oldTile = player->getTile();
		MOVEDIR movement = (MOVEDIR) player->lastMove;
		if (tile->canMove())
		{
			player->move(tile);
			this->moveEvent(player);
			this->moveOutEvent(oldTile, player);
			this->checkVisionTiles(player, player->getTile());
			switch (tile->getType())
			{
			case TileType::GROUND:
				break;
			case TileType::GROUND_SLIPPY:
				if (getMovementTile(movement, tile)->getType() == TileType::GROUND_SLIPPY)
				{
					player->move(tile);
					this->moveEvent(player);
					this->moveOutEvent(tile, player);
					this->checkVisionTiles(player, player->getTile());
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
			if (tile->haveItem())
			{
				pickupItem(player, tile->getItemID());
			}
		}
	}
	void handleAttack(int playerID)
	{
		int playerIndex = getPlayerIndex(playerID);
		if (playerIndex == -1)
		{
			Logger::log("Error: Attack Player failure, player not found");
			return;
		}
		handleAttack(this->gamePlayerList[playerIndex]);
	}

	void handleAttack(std::shared_ptr<PlayerObject> player)
	{
		MOVEDIR movement = (MOVEDIR)player->lastMove;
		bool moved = false;
		this->attackEvent(player);
		auto startTile = player->getTile();
		auto finalTile = startTile;
		auto iterTile = player->getTile();
		for (int i = 0; i < player->getRange(); i++)
		{
			iterTile = getMovementTile(movement, iterTile);
			if (iterTile == nullptr) break;
			if (iterTile->havePlayer())
			{

				auto hitPlayer = this->getPlayer(iterTile->getPlayerID());
				if (hitPlayer == nullptr)
				{
					Logger::log("Error: Attack Player failure, atacked player not found");
					return;
				}
				damagePlayer(player, hitPlayer, player->getDamage());
				this->checkVisionTiles(player, iterTile);
			}
			else if (i < player->getRange() && iterTile->canMove())
			{
				finalTile = iterTile;
				this->checkVisionTiles(player, iterTile);
			}
			else
				break;
		}
		handleMovement(player, finalTile);
	}
	std::shared_ptr<Tile> getMovementTile(MOVEDIR movement, std::shared_ptr<Tile> tile, int range=1)
	{
		int x = tile->getX();
		int y = tile->getY();
		switch (movement)
		{
		case MOVEDIR::UP:
			return this->getTile(x, y - range);
			break;
		case MOVEDIR::DOWN:
			return this->getTile(x, y + range);
			break;
		case MOVEDIR::LEFT:
			return this->getTile(x - range, y);
			break;
		case MOVEDIR::RIGHT:
			return this->getTile(x + range, y);
			break;
		}
	}

	void checkVisionTiles(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile)
	{
		MOVEDIR movement = (MOVEDIR)player->lastMove;
		int x = tile->getX();
		int y = tile->getY();
		switch (movement)
		{
		case MOVEDIR::UP:
			for (int i = x - Constants::sightValue; i <= x + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(i, y - Constants::sightValue + 1));
			}
			break;
		case MOVEDIR::DOWN:
			for (int i = x - Constants::sightValue; i <= x + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(i, y + Constants::sightValue - 1));
			}
			break;
		case MOVEDIR::LEFT:
			for (int i = y - Constants::sightValue; i <= y + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(x - Constants::sightValue + 1, i));
			}
			break;
		case MOVEDIR::RIGHT:
			for (int i = y - Constants::sightValue; i <= y + Constants::sightValue; i++)
			{
				getVision(player, this->getTile(x + Constants::sightValue - 1, i));
			}
			break;
		}
	}
	void getVision(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile)
	{
		if (tile == nullptr) return;
		if (tile->haveItem())
		{
			auto item = this->gameObjectList[this->getItemIndex(tile->getItemID())];
			if (item->getExist())
				this->spawnEvent(player, item);
		}
		if (tile->havePlayer())
		{
			auto _player = this->gamePlayerList[this->getPlayerIndex(tile->getPlayerID())];
			if (_player->getExist())
				this->spawnPlayerEvent(player, _player);
		}

	}

	void damagePlayer(std::shared_ptr<PlayerObject> hitPlayer, int dmgValue)
	{
		int newHealth = hitPlayer->getHealth() - dmgValue;
		if (newHealth > hitPlayer->getMaxHealth()) hitPlayer->setHealth(hitPlayer->getMaxHealth());
		else
			hitPlayer->setHealth(newHealth);

		this->hitEvent(hitPlayer, dmgValue);
		this->damageEvent(hitPlayer, newHealth);

		if (newHealth <= 0)
		{
			killPlayer(hitPlayer);
		}
	}

	void damagePlayer(std::shared_ptr<PlayerObject> player, std::shared_ptr<PlayerObject> hitPlayer, int dmgValue)
	{
		int newHealth = hitPlayer->getHealth() - dmgValue;
		if (newHealth > hitPlayer->getMaxHealth()) hitPlayer->setHealth(hitPlayer->getMaxHealth());
		else
			hitPlayer->setHealth(newHealth);

		this->hitEvent(hitPlayer, dmgValue);
		this->damageEvent(hitPlayer, newHealth);

		if (newHealth <= 0)
		{
			killPlayer(hitPlayer);
			player->setkillCount(player->getkillCount() + 1);
			killCountEvent(player);
		}
	}

	void killCountEvent(std::shared_ptr<PlayerObject> receiver, std::shared_ptr<PlayerObject> player)
	{
		output.addEventKillCount(Constants::SERVER_ID, receiver->getPlayerID(), player->getName(), player->getkillCount());
	}

	void killCountEvent(std::shared_ptr<PlayerObject> player)
	{
		for (auto _player : this->gamePlayerList)
			output.addEventKillCount(Constants::SERVER_ID, _player->getPlayerID(), player->getName(), player->getkillCount());
	}
	void allKillCountsEvent(std::shared_ptr<PlayerObject> player)
	{
		for (auto _player : this->gamePlayerList)
			output.addEventKillCount(Constants::SERVER_ID, player->getPlayerID(), _player->getName(), _player->getkillCount());
	}
	void addPlayerSpawnToTick(std::shared_ptr<PlayerObject> player)
	{
		player->startSpawnTimer();

		tickPlayList.push_back(player);

	}

	void killPlayer(std::shared_ptr<PlayerObject> player)
	{
		// usuwamy gracza z mapy
		this->despawnPlayerEvent(player);
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
		auto item = this->gameObjectList[this->getItemIndex(itemID)];
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
				output.addEventMovement(Constants::SERVER_ID, player->getPlayerID(), obj->getName(), obj->getX(), obj->getY());
		}
	}
	void moveOutEvent(std::shared_ptr<Tile> oldTile, std::shared_ptr<PlayerObject> obj)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(oldTile, player->getTile()) && !checkRange(obj, player))
				output.addEventMovedOut(Constants::SERVER_ID, player->getPlayerID(), obj->getName(), obj->getX(), obj->getY());
		}
	}
	void spawnPlayerEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<PlayerObject> object)
	{
		if (player->getPlayerID() != object->getPlayerID()) 
			output.addEventPlayerSpawn(Constants::SERVER_ID, player->getPlayerID(), object->getName(), object->getX(), object->getY());
	}

	void spawnPlayerEvent(std::shared_ptr<PlayerObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (player->getPlayerID() != object->getPlayerID())
				if (checkRange(object, player))
					output.addEventPlayerSpawn(Constants::SERVER_ID, player->getPlayerID(), object->getName(), object->getX(), object->getY());
		}
	}

	void spawnEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<SpawnableObject> object)
	{
		output.addEventSpawn(Constants::SERVER_ID, player->getPlayerID(), (int)object->getType(), object->getX(), object->getY());
	}

	void spawnEvent(std::shared_ptr<ItemObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventSpawn(Constants::SERVER_ID, player->getPlayerID(), (int)object->getType(), object->getX(), object->getY());
		}
	}

	void respawnEvent(std::shared_ptr<PlayerObject> object)
	{
		output.addEventRespawn(Constants::SERVER_ID, object->getPlayerID(), object->getX(), object->getY());
	}

	void respawnAskEvent(std::shared_ptr<PlayerObject> object)
	{
		output.addEventAskRespawn(Constants::SERVER_ID, object->getPlayerID(), object->getX(), object->getY());
		Logger::log("Respawn ask event added");
	}

	void despawnEvent(std::shared_ptr<SpawnableObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventDespawn(Constants::SERVER_ID, player->getPlayerID(), (int)object->getType(), object->getX(), object->getY());
		}
	}

	void despawnPlayerEvent(std::shared_ptr<SpawnableObject> object)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(object, player))
				output.addEventDespawnPlayer(Constants::SERVER_ID, player->getPlayerID(), player->getName(), object->getX(), object->getY());
		}
	}

	void attackEvent(std::shared_ptr<PlayerObject> obj)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(obj, player))
				output.addEventAttack(Constants::SERVER_ID, player->getPlayerID(), obj->getName());
		}
	}
	void hitEvent(std::shared_ptr<PlayerObject> obj, int dmg)
	{
		for (auto player : gamePlayerList)
		{
			if (checkRange(obj, player))
				output.addEventHit(Constants::SERVER_ID, player->getPlayerID(), obj->getName(), dmg);
		}
	}
	void damageEvent(std::shared_ptr<PlayerObject> player, int newHealth)
	{
		output.addEventDamaged(Constants::SERVER_ID, player->getPlayerID(), newHealth);
	}

	void pickupEvent(std::shared_ptr<PlayerObject> player, std::shared_ptr<SpawnableObject> object)
	{
		output.addEventPickUp(Constants::SERVER_ID, player->getPlayerID(), (int) object->getType());
	}
	void pickupEvent(std::shared_ptr<PlayerObject> player, int itemType)
	{
		output.addEventPickUp(Constants::SERVER_ID, player->getPlayerID(), itemType);
	}
	void handleRespawn(Parser::Event ev)
	{
		int playerIndex = this->getPlayerIndex(ev.sender);

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
	int checkRange(std::shared_ptr<Tile> objectF, std::shared_ptr<Tile> objectS, int range = Constants::sightValue)
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
		int index = getPlayerIndex(playerID);
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
		int index = getItemIndex(itemID);
		if (index == -1)
		{
			Logger::log("Error: Delete item failure, item not found");
			return;
		}
		gameObjectList[index]->deleteItem();
		gameObjectList.erase(gameObjectList.begin() + index);

	}
	int getPlayerIndex(int playerID)
	{
		//if (gamePlayerList[playerID - 1]->getplayerID() == playerID) return playerID - 1;
		for (int i = 0; i < gamePlayerList.size(); i++)
			if (gamePlayerList[i]->getPlayerID() == playerID)
				return i;

		return -1;
	}

	int getItemIndex(int itemID)
	{
		if (this->gameObjectList[itemID]->getItemID() == itemID) return itemID;
		for (int i = 0; i < gameObjectList.size(); i++)
			if (gameObjectList[i]->getItemID() == itemID)
				return i;

		return -1;
	}

	std::shared_ptr<PlayerObject> getPlayer(int playerID)
	{
		for (auto player : this->gamePlayerList)
			if (player->getPlayerID() == playerID) return player;
		return std::shared_ptr<PlayerObject>(nullptr);
	}

	std::shared_ptr<Tile> getTile(int x, int y)
	{
		if (x >= 0 && x < this->map.MAP_WIDTH && y >= 0 && y < this->map.MAP_HEIGHT)
			return this->map.tileArray[x][y];
		else
			return std::shared_ptr<Tile>(nullptr);
	}

private:

	Map map;
	Parser::Messenger output;
	std::vector<std::shared_ptr<ItemObject>> gameObjectList;
	std::vector<std::shared_ptr<ItemObject>> tickObjList;
	std::vector<std::shared_ptr<PlayerObject>> gamePlayerList;
	std::vector<std::shared_ptr<PlayerObject>> tickPlayList;
};

