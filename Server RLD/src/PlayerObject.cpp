#include "PlayerObject.h"

PlayerObject::PlayerObject(int id, std::string playerName, SpawnableObjectType type, std::shared_ptr<Tile> tile)
{
	this->x = tile->getX();
	this->y = tile->getY();
	this->playerID = id;
	this->playerName = playerName;
	this->type = type;
	this->isExist = false;
	this->occupiedTile = tile;
	this->occupiedTile->setSpawn(false);
	this->occupiedTile->setMove(false);
	this->health = Constants::defaultHealth;
	this->maxHealth = Constants::defaultHealth;
	this->dmg = Constants::defaultDmg;
	this->attackRange = Constants::attackRange;
	this->readyToRespawn = false;
}

void PlayerObject::move(std::shared_ptr<Tile> newTile)
{
	this->occupiedTile->setPlayer(false);
	this->occupiedTile->setPlayerID(-1);
	this->occupiedTile->setMove(true);


	this->occupiedTile = newTile;
	this->x = newTile->getX();
	this->y = newTile->getY();


	this->occupiedTile->setPlayer(true);
	this->occupiedTile->setPlayerID(this->playerID);
	this->occupiedTile->setMove(false);
}

void PlayerObject::spawn()
{
	this->isExist = true;
	this->occupiedTile->setPlayer(true);
	this->occupiedTile->setPlayerID(this->playerID);
	this->occupiedTile->setMove(false);
}

void PlayerObject::despawn()
{
	this->isExist = false;
	this->occupiedTile->setPlayer(false);
	this->occupiedTile->setPlayerID(this->playerID);
	this->occupiedTile->setMove(true);
	// Operacje na obiekcie map
}
