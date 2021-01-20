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
	tile->isSpawnable = false;
	this->health = Constants::defaultHealth;
	this->maxHealth = Constants::defaultHealth;
	this->dmg = Constants::defaultDmg;
	this->attackRange = Constants::attackRange;
	this->readyToRespawn = false;
}

void PlayerObject::move(std::shared_ptr<Tile> newTile)
{
	this->x = newTile->getX();
	this->y = newTile->getY();
	this->occupiedTile->isPlayer = false;
	this->occupiedTile->playerID = -1;
	this->occupiedTile = newTile;
	this->occupiedTile->isPlayer = true;
	this->occupiedTile->playerID = this->playerID;
}

void PlayerObject::spawn()
{
	this->isExist = true;
	this->occupiedTile->isPlayer = true;
	this->occupiedTile->playerID = this->playerID;
}

void PlayerObject::despawn()
{
	this->isExist = false;
	this->occupiedTile->isPlayer = false;
	this->occupiedTile->playerID = this->playerID;
	// Operacje na obiekcie map
}
