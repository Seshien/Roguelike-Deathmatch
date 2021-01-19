#include "PlayerObject.h"

PlayerObject::PlayerObject(int id, SpawnableObjectType type, std::shared_ptr<Tile> tile)
{
	this->playerID = id;
	this->type = type;
	this->isExist = false;
	this->occupiedTile = tile;
	tile->isPlayer = true;
	this->health = Constants::defaultHealth;
	this->maxHealth = Constants::defaultHealth;
	this->dmg = Constants::defaultDmg;
	this->attackRange = Constants::attackRange;
}

void PlayerObject::move(std::shared_ptr<Tile> newTile)
{
	this->occupiedTile->isPlayer = false;
	this->occupiedTile->playerID = -1;
	this->occupiedTile = newTile;
	this->occupiedTile->isPlayer = true;
	this->occupiedTile->playerID = this->playerID;
}
