#include "Game.h"

void Game::startGame(std::vector<int> playerID, std::vector<std::string> playerName)
{
	this->gameObjectList.clear();
	this->gamePlayerList.clear();
	this->tickObjList.clear();
	this->tickPlayList.clear();
	this->map.refresh();
	spawnObjects();
	addPlayers(playerID, playerName);
}

void Game::startMap(std::string mapPath)
{
	this->map.init(mapPath);

}

void Game::spawnObjects()
{
	for (int i = 0; i < map.MAP_HEIGHT; i++)
	{
		for (int j = 0; j < map.MAP_WIDTH; j++)
		{
			auto tile = this->map.getTile(i, j);//map.tileArray[i][j];
			if (tile == nullptr)
				continue;
			if (tile->isItemSpawner())
			{
				tile->setItemSpawner(false);
				// bardzo prosty sposob na wybor rodzaju itemu
				int temp = (i + j) % 4;
				auto item = std::make_shared<ItemObject>(itemID++, (SpawnableObjectType)temp, tile);
				gameObjectList.push_back(item);
				item->startSpawnTimer();
				this->tickObjList.push_back(item);
			}
		}
	}
}

void Game::addPlayers(std::vector<int> playerID, std::vector<std::string> playerName)
{
	for (int x = 0; x < playerID.size(); x++)
	{
		if (addPlayer(playerID[x], playerName[x]))
		{
			Logger::error("Spawn was unsuccesfull");
		}
	}

}

int Game::addPlayer(int playerID, std::string playerName)
{

	if (int playerIndex = this->getPlayerIndex(playerID) == -1)
	{
		for (int i = 0; i < map.MAP_HEIGHT; i++)
		{
			for (int j = 0; j < map.MAP_WIDTH; j++)
			{
				auto tile = this->map.getTile(i, j);//map.tileArray[i][j];
				if (tile == nullptr)
					continue;
				if (tile->isPlayerSpawner())
				{
					tile->setPlayerSpawner(false);
					auto player = std::make_shared<PlayerObject>(playerID, playerName, SpawnableObjectType::PLAYER, tile);
					gamePlayerList.push_back(player);

					this->addPlayerSpawnToTick(player);
					allKillCountsEvent(player);
					this->allDeathCountsEvent(player);
					return 0;
				}
			}
		}
	}
	else
	{
		auto player = gamePlayerList[playerIndex];
		allKillCountsEvent(player);
		this->allDeathCountsEvent(player);
		if (player->getExist())
		{
			this->respawnEvent(player);
			this->damageEvent(player, player->getHealth());
			this->maxHealthEvent(player);
			for (auto Item : player->getItems())
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


Parser::Messenger Game::loopGame(Parser::Messenger input)
{
	Logger::debug("Loop game.");
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
		Logger::debug("Tick!");
		auto result = object->get()->tick();
		if (result == tickResult::SPAWNED)
		{
			object->get()->readyToRespawn = true;
			this->respawnAskEvent(*object);
			object = tickPlayList.erase(object);

		}
		else if (result == tickResult::CAN_ATTACK && object->get()->getTimetoRespawn() <= 0)
		{
			object = tickPlayList.erase(object);

		}
		else ++object;
	}
	for (auto ev : input.eventList)
	{
		switch (ev.subtype)
		{
		case Parser::SubType::KEY:
			handleKey(ev);
			break;
		case Parser::SubType::NEWPLAYER:
			addPlayer(ev.receiver, ev.subdata);
			break;
		case Parser::SubType::RESPAWN:
			handleRespawn(ev);
			break;
		case Parser::SubType::MOVE:
			//handleMovement(ev);
			break;
		case Parser::SubType::ATTACK:
			//handleAttack(ev);
			break;
		case Parser::SubType::ACTION:
			//do something
			break;
		default:
			Logger::error("Error: Unknown Game Subtype");
		}
	}
	return output;
}

void Game::handleKey(Parser::Event ev)
{
	auto player = this->getPlayer(ev.sender);
	if (player->getExist() == false)
		return;
	switch (ev.subdata[0])
	{
	case ('W'):
		handleMovement(ev.sender, Map::MOVEDIR::UP);
		break;
	case ('S'):
		handleMovement(ev.sender, Map::MOVEDIR::DOWN);
		break;
	case ('A'):
		handleMovement(ev.sender, Map::MOVEDIR::LEFT);
		break;
	case ('D'):
		handleMovement(ev.sender, Map::MOVEDIR::RIGHT);
		break;
	case ' ':
		handleAttack(ev.sender);
		break;
	default:
		Logger::error("Error: Not handled key");
	}
}
//wasd
void Game::handleMovement(int playerID, Map::MOVEDIR movement)
{
	auto player = this->getPlayer(playerID);
	if (player == nullptr)
	{
		Logger::error("Error: Move Player failure, player not found");
		return;
	}
	player->lastMove = (int)movement;
	handleMovement(player);
}

void Game::handleMovement(std::shared_ptr<PlayerObject> player)
{
	handleMovement(player, this->map.getMovementTile((Map::MOVEDIR) player->lastMove, player->getTile()));
}

void Game::handleMovement(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile)
{
	std::shared_ptr<Tile> oldTile = player->getTile();
	Map::MOVEDIR movement = (Map::MOVEDIR) player->lastMove;
	if (tile->canMove())
	{
		player->move(tile);
		this->moveEvent(player, oldTile);
		this->moveOutEvent(oldTile, player);
		this->checkVisionTiles(player, player->getTile());
		if (tile->haveItem())
			pickupItem(player, tile->getItemID());
		switch (tile->getType())
		{
		case TileType::GROUND:
			break;
		case TileType::GROUND_SLIPPY:
			while (1)
			{
				oldTile = tile;
				tile = this->map.getMovementTile(movement, tile);
				if (tile->getType() == TileType::GROUND_SLIPPY)
				{
					player->move(tile);
					this->moveEvent(player, oldTile);
					this->moveOutEvent(oldTile, player);
					this->checkVisionTiles(player, player->getTile());
					if (tile->haveItem())
						pickupItem(player, tile->getItemID());
				}
				else
					break;
			}

			break;
		case TileType::EMPTY:
			killPlayer(player);
			break;
		case TileType::SPIKES:
			damagePlayer(player, Config::spikeDmg);
			break;
		case TileType::LAVA:
			damagePlayer(player, Config::lavaDmg);
			break;
		}
	}
}

void Game::handleAttack(int playerID)
{
	int playerIndex = getPlayerIndex(playerID);
	if (playerIndex == -1)
	{
		Logger::error("Error: Attack Player failure, player not found");
		return;
	}
	handleAttack(this->gamePlayerList[playerIndex]);
}

void Game::handleAttack(std::shared_ptr<PlayerObject> player)
{
	if (player->cooldownTimer > 0) return;
	Map::MOVEDIR movement = (Map::MOVEDIR) player->lastMove;
	bool moved = false;
	this->attackEvent(player);
	auto startTile = player->getTile();
	auto finalTile = startTile;
	auto iterTile = player->getTile();
	for (int i = 0; i < player->getRange(); i++)
	{
		iterTile = this->map.getMovementTile(movement, iterTile);
		if (iterTile == nullptr) break;
		if (iterTile->havePlayer())
		{

			auto hitPlayer = this->getPlayer(iterTile->getPlayerID());
			if (hitPlayer == nullptr)
			{
				Logger::error("Error: Attack Player failure, atacked player not found");
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
	player->cooldownTimer = Config::attackCooldown;
	tickPlayList.push_back(player);
	handleMovement(player, finalTile);
}

void Game::checkVisionTiles(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile)
{
	Map::MOVEDIR movement = (Map::MOVEDIR)player->lastMove;
	int x = tile->getX();
	int y = tile->getY();
	switch (movement)
	{
	case Map::MOVEDIR::UP:
		for (int i = x - Config::sightValue; i < x + Config::sightValue; i++)
		{
			getVision(player, this->map.getTile(i, y - Config::sightValue + 1));
		}
		break;
	case Map::MOVEDIR::DOWN:
		for (int i = x - Config::sightValue + 1; i <= x + Config::sightValue; i++)
		{
			getVision(player, this->map.getTile(i, y + Config::sightValue - 1));
		}
		break;
	case Map::MOVEDIR::LEFT:
		for (int i = y - Config::sightValue; i < y + Config::sightValue; i++)
		{
			getVision(player, this->map.getTile(x - Config::sightValue + 1, i));
		}
		break;
	case Map::MOVEDIR::RIGHT:
		for (int i = y - Config::sightValue + 1; i <= y + Config::sightValue; i++)
		{
			getVision(player, this->map.getTile(x + Config::sightValue - 1, i));
		}
		break;
	}
}
void Game::getVision(std::shared_ptr<PlayerObject> player, std::shared_ptr<Tile> tile)
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

void Game::damagePlayer(std::shared_ptr<PlayerObject> hitPlayer, int dmgValue)
{
	int newHealth = hitPlayer->getHealth() - dmgValue;
	if (newHealth > hitPlayer->getMaxHealth()) newHealth = hitPlayer->getMaxHealth();
	hitPlayer->setHealth(newHealth);

	this->hitEvent(hitPlayer, dmgValue);
	this->damageEvent(hitPlayer, newHealth);

	if (newHealth <= 0)
	{
		killPlayer(hitPlayer);
	}
}

void Game::damagePlayer(std::shared_ptr<PlayerObject> player, std::shared_ptr<PlayerObject> hitPlayer, int dmgValue)
{
	int newHealth = hitPlayer->getHealth() - dmgValue;
	if (newHealth > hitPlayer->getMaxHealth()) newHealth = hitPlayer->getMaxHealth();
	hitPlayer->setHealth(newHealth);

	this->hitEvent(hitPlayer, dmgValue);
	this->damageEvent(hitPlayer, newHealth);

	if (newHealth <= 0)
	{
		killPlayer(hitPlayer);
		int newKillCount = player->getKillCount() + 1;
		player->setkillCount(newKillCount);
		killCountEvent(player);
		if (newKillCount >= 5)
			endGame(player);
	}
}

void Game::endGame(std::shared_ptr<PlayerObject> player)
{
	output.addEventGameEnd(Config::SERVER_ID, Config::SERVER_ID, player->getName());
}

void Game::maxHealthEvent(std::shared_ptr<PlayerObject> player)
{
	output.addEventMaxHealth(Config::SERVER_ID, player->getPlayerID(), player->getMaxHealth());
}

void Game::killCountEvent(std::shared_ptr<PlayerObject> receiver, std::shared_ptr<PlayerObject> player)
{
	output.addEventKillCount(Config::SERVER_ID, receiver->getPlayerID(), player->getName(), player->getKillCount());
}


void Game::killCountEvent(std::shared_ptr<PlayerObject> player)
{
	for (auto _player : this->gamePlayerList)
		output.addEventKillCount(Config::SERVER_ID, _player->getPlayerID(), player->getName(), player->getKillCount());
}
void Game::allKillCountsEvent(std::shared_ptr<PlayerObject> player)
{
	for (auto _player : this->gamePlayerList)
		output.addEventKillCount(Config::SERVER_ID, player->getPlayerID(), _player->getName(), _player->getKillCount());
}
void Game::deathCountEvent(std::shared_ptr<PlayerObject> player)
{
	for (auto _player : this->gamePlayerList)
		output.addEventDeathCount(Config::SERVER_ID, _player->getPlayerID(), player->getName(), player->getDeathCount());
}
void Game::allDeathCountsEvent(std::shared_ptr<PlayerObject> player)
{
	for (auto _player : this->gamePlayerList)
		output.addEventDeathCount(Config::SERVER_ID, player->getPlayerID(), _player->getName(), _player->getDeathCount());
}
void Game::addPlayerSpawnToTick(std::shared_ptr<PlayerObject> player)
{
	player->startSpawnTimer();

	tickPlayList.push_back(player);

}

void Game::killPlayer(std::shared_ptr<PlayerObject> player)
{
	// usuwamy gracza z mapy
	player->despawn();
	player->startSpawnTimer();
	this->despawnPlayerEvent(player);
	this->deathCountEvent(player);

	// tworzyc nowy obiekt z trupem
	std::shared_ptr<ItemObject> body = std::make_shared<ItemObject>(itemID++, SpawnableObjectType::BODY, player->getTile());
	body->spawn();
	body->startSpawnTimer();
	this->gameObjectList.push_back(body);

	//poruszamy gracza do respawna
	auto tile = this->map.getTile(player->getspawnX(), player->getspawnY());
	if (tile != nullptr)
		player->move(tile);

	//dodajemy do tickow
	this->tickObjList.push_back(body);
	this->tickPlayList.push_back(player);
	//eventy
	this->spawnEvent(body);
	// 
}

void Game::pickupItem(std::shared_ptr<PlayerObject> player, int itemID)
{
	auto item = this->gameObjectList[this->getItemIndex(itemID)];
	switch (item->getType())
	{
	case SpawnableObjectType::BODY:
		this->damagePlayer(player, Config::bodyBonus * -1);
		item->despawn();
		break;
	case SpawnableObjectType::ITEM_SWORD:
		this->pickupEvent(player, item);
		player->setDamage(Config::defaultDmg + Config::swordBonus);
		player->addItem((int)item->getType());
		item->despawn();
		item->startSpawnTimer();
		this->tickObjList.push_back(item);
		break;
	case SpawnableObjectType::ITEM_POTION:
		this->damagePlayer(player, -1 * Config::potionBonus);
		item->despawn();
		item->startSpawnTimer();
		this->tickObjList.push_back(item);
		break;
	case SpawnableObjectType::ITEM_SHIELD:
		this->pickupEvent(player, item);
		player->setMaxHealth(Config::defaultHealth + Config::shieldBonus);
		this->maxHealthEvent(player);
		this->damagePlayer(player, -1 * Config::shieldBonus);
		item->despawn();
		item->startSpawnTimer();
		this->tickObjList.push_back(item);
		break;
	case SpawnableObjectType::ITEM_BOOTS:
		this->pickupEvent(player, item);
		player->setRange(Config::attackRange + Config::bootsBonus);
		item->despawn();
		item->startSpawnTimer();
		this->tickObjList.push_back(item);
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

void Game::moveEvent(std::shared_ptr<PlayerObject> obj, std::shared_ptr<Tile> oldTile)
{
	for (auto player : gamePlayerList)
	{
		if (this->map.checkRange(obj->getTile(), player->getTile()))
			output.addEventMovement(Config::SERVER_ID, player->getPlayerID(), obj->getName(), obj->getX(), obj->getY(), oldTile->getX(), oldTile->getY());
	}
}
void Game::moveOutEvent(std::shared_ptr<Tile> oldTile, std::shared_ptr<PlayerObject> obj)
{
	for (auto player : gamePlayerList)
	{
		if (this->map.checkRange(oldTile, player->getTile()) && !this->map.checkRange(obj->getTile(), player->getTile()))
			output.addEventMovedOut(Config::SERVER_ID, player->getPlayerID(), obj->getName(), obj->getX(), obj->getY(), oldTile->getX(), oldTile->getY());
	}
}
void Game::spawnPlayerEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<PlayerObject> object)
{
	if (player->getPlayerID() != object->getPlayerID())
	{
		output.addEventPlayerSpawn(Config::SERVER_ID, player->getPlayerID(), object->getName(), object->getX(), object->getY());
		output.addEventKillCount(Config::SERVER_ID, player->getPlayerID(), object->getName(), object->getKillCount());
	}

}

void Game::spawnPlayerEvent(std::shared_ptr<PlayerObject> object)
{
	for (auto player : gamePlayerList)
	{
		if (player->getPlayerID() != object->getPlayerID())
			if (this->map.checkRange(object->getTile(), player->getTile()))
				output.addEventPlayerSpawn(Config::SERVER_ID, player->getPlayerID(), object->getName(), object->getX(), object->getY());
	}
}

void Game::spawnEvent(std::shared_ptr<PlayerObject>player, std::shared_ptr<SpawnableObject> object)
{
	output.addEventSpawn(Config::SERVER_ID, player->getPlayerID(), (int)object->getType(), object->getX(), object->getY());
}

void Game::spawnEvent(std::shared_ptr<ItemObject> object)
{
	for (auto player : gamePlayerList)
	{
		if (this->map.checkRange(object->getTile(), player->getTile()))
			output.addEventSpawn(Config::SERVER_ID, player->getPlayerID(), (int)object->getType(), object->getX(), object->getY());
	}
}

void Game::respawnEvent(std::shared_ptr<PlayerObject> object)
{
	output.addEventRespawn(Config::SERVER_ID, object->getPlayerID(), object->getX(), object->getY());

}

void Game::respawnAskEvent(std::shared_ptr<PlayerObject> object)
{
	output.addEventAskRespawn(Config::SERVER_ID, object->getPlayerID(), object->getX(), object->getY());
	Logger::error("Respawn ask event added");
}

void Game::despawnEvent(std::shared_ptr<SpawnableObject> object)
{
	for (auto player : gamePlayerList)
	{
		if (this->map.checkRange(object->getTile(), player->getTile()))
			output.addEventDespawn(Config::SERVER_ID, player->getPlayerID(), (int)object->getType(), object->getX(), object->getY());
	}
}

void Game::despawnPlayerEvent(std::shared_ptr<PlayerObject> object)
{
	for (auto player : gamePlayerList)
	{
		if (this->map.checkRange(object->getTile(), player->getTile()))
			output.addEventDespawnPlayer(Config::SERVER_ID, player->getPlayerID(), object->getName(), object->getX(), object->getY());
	}
}

void Game::attackEvent(std::shared_ptr<PlayerObject> obj)
{
	for (auto player : gamePlayerList)
	{
		if (this->map.checkRange(obj->getTile(), player->getTile()))
			output.addEventAttack(Config::SERVER_ID, player->getPlayerID(), obj->getName());
	}
}
void Game::hitEvent(std::shared_ptr<PlayerObject> obj, int dmg)
{
	for (auto player : gamePlayerList)
	{
		if (this->map.checkRange(obj->getTile(), player->getTile()))
			output.addEventHit(Config::SERVER_ID, player->getPlayerID(), obj->getName(), dmg);
	}
}
void Game::damageEvent(std::shared_ptr<PlayerObject> player, int newHealth)
{
	output.addEventDamaged(Config::SERVER_ID, player->getPlayerID(), newHealth);
}

void Game::pickupEvent(std::shared_ptr<PlayerObject> player, std::shared_ptr<SpawnableObject> object)
{
	output.addEventPickUp(Config::SERVER_ID, player->getPlayerID(), (int)object->getType());
}
void Game::pickupEvent(std::shared_ptr<PlayerObject> player, int itemType)
{
	output.addEventPickUp(Config::SERVER_ID, player->getPlayerID(), itemType);
}
void Game::handleRespawn(Parser::Event ev)
{
	int playerIndex = this->getPlayerIndex(ev.sender);

	if (playerIndex == -1)
	{
		Logger::error("Error: Respawn event failed");
		return;
	}
	handleRespawn(this->gamePlayerList[playerIndex]);

}

void Game::handleRespawn(std::shared_ptr<PlayerObject> object)
{
	if (object->readyToRespawn)
	{
		object->spawn();
		this->spawnPlayerEvent(object);
		this->respawnEvent(object);
		this->damageEvent(object, object->getHealth());
		this->maxHealthEvent(object);
		this->getFullVision(object);
	}
}

void Game::getFullVision(std::shared_ptr<PlayerObject> player)
{
	int x1 = player->getX() - Config::sightValue;
	int y1 = player->getY() - Config::sightValue;
	int x2 = player->getX() + Config::sightValue;
	int y2 = player->getY() + Config::sightValue;
	for (int i = x1; i <= x2; i++)
		for (int j = y1; j <= y2; j++)
			this->getVision(player, this->map.getTile(i, j));

}
void Game::deletePlayer(int playerID)
{
	int index = getPlayerIndex(playerID);
	if (index == -1)
	{
		Logger::error("Error: Delete Player failure, player not found");
		return;
	}

	killPlayer(gamePlayerList[index]);

	gamePlayerList.erase(gamePlayerList.begin() + index);

}

void Game::deleteItem(int itemID)
{
	int index = getItemIndex(itemID);
	if (index == -1)
	{
		Logger::error("Error: Delete item failure, item not found");
		return;
	}
	gameObjectList[index]->deleteItem();
	gameObjectList.erase(gameObjectList.begin() + index);

}
int Game::getPlayerIndex(int playerID)
{
	//if (gamePlayerList[playerID - 1]->getplayerID() == playerID) return playerID - 1;
	for (int i = 0; i < gamePlayerList.size(); i++)
		if (gamePlayerList[i]->getPlayerID() == playerID)
			return i;

	return -1;
}

int Game::getItemIndex(int itemID)
{
	//if (this->gameObjectList[itemID]->getItemID() == itemID) return itemID;
	for (int i = 0; i < gameObjectList.size(); i++)
		if (gameObjectList[i]->getItemID() == itemID)
			return i;

	return -1;
}

std::shared_ptr<PlayerObject> Game::getPlayer(int playerID)
{
	for (auto player : this->gamePlayerList)
		if (player->getPlayerID() == playerID) return player;
	return std::shared_ptr<PlayerObject>(nullptr);
}
