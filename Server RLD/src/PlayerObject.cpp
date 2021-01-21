#include "PlayerObject.h"

PlayerObject::PlayerObject(int id, std::string playerName, SpawnableObjectType type, std::shared_ptr<Tile> tile)
{
	this->x = tile->getX();
	this->lastMove = (int)'S';
	this->y = tile->getY();
	this->playerID = id;
	this->playerName = playerName;
	this->type = type;
	this->isExist = false;
	this->occupiedTile = tile;
	this->occupiedTile->isSpawnable = false;
	this->occupiedTile->isMovable = false;
	this->health = Constants::defaultHealth;
	this->maxHealth = Constants::defaultHealth;
	this->dmg = Constants::defaultDmg;
	this->attackRange = Constants::attackRange;
	this->readyToRespawn = false;
}

void PlayerObject::move(std::shared_ptr<Tile> newTile)
{
	this->occupiedTile->isPlayer = false;
	this->occupiedTile->playerID = -1;
	this->occupiedTile->isMovable = true;


	this->occupiedTile = newTile;
	this->x = newTile->getX();
	this->y = newTile->getY();


	this->occupiedTile->isPlayer = true;
	this->occupiedTile->playerID = this->playerID;
	this->occupiedTile->isMovable = false;
}

void PlayerObject::spawn()
{
	this->isExist = true;
	this->occupiedTile->isPlayer = true;
	this->occupiedTile->playerID = this->playerID;
	this->occupiedTile->isMovable = false;
}

void PlayerObject::despawn()
{
	this->isExist = false;
	this->occupiedTile->isPlayer = false;
	this->occupiedTile->playerID = this->playerID;
	this->occupiedTile->isMovable = true;
	// Operacje na obiekcie map
}
