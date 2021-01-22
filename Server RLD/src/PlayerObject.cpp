#include "PlayerObject.h"

PlayerObject::PlayerObject(int id, std::string playerName, SpawnableObjectType type, std::shared_ptr<Tile> tile)
{
	this->lastMove = 1;
	this->x = tile->getX();
	this->y = tile->getY();
	this->spawnX = x;
	this->spawnY = y;
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
	this->deathCount = 0;
	this->killCount = 0;
}

void PlayerObject::move(std::shared_ptr<Tile> newTile)
{
	if (this->isExist)
	{
		this->occupiedTile->setPlayer(false);
		this->occupiedTile->setPlayerID(-1);
		this->occupiedTile->setMove(true);

		newTile->setPlayer(true);
		newTile->setPlayerID(this->playerID);
		newTile->setMove(false);
	}

	this->occupiedTile = newTile;
	this->x = newTile->getX();
	this->y = newTile->getY();



}

void PlayerObject::spawn()
{
	this->isExist = true;
	this->occupiedTile->setPlayer(true);
	this->occupiedTile->setPlayerID(this->playerID);
	this->occupiedTile->setMove(false);

	this->health = Constants::defaultHealth;
	this->maxHealth = Constants::defaultHealth;
	this->dmg = Constants::defaultDmg;
	this->attackRange = Constants::attackRange;
	this->items.clear();
	this->readyToRespawn = false;
	this->cooldownTimer = 0;

}

void PlayerObject::despawn()
{
	this->isExist = false;
	this->occupiedTile->setPlayer(false);
	this->occupiedTile->setPlayerID(this->playerID);
	this->occupiedTile->setMove(true);
	this->deathCount++;
	// Operacje na obiekcie map
}

tickResult PlayerObject::tick()
{
	if (isExist)
	{
		if (cooldownTimer)
			cooldownTimer--;
		else
		{
			return tickResult::CAN_ATTACK;
		}
	}
	else
	{
		if (timetoRespawn)
			timetoRespawn--;
		else
		{
			return tickResult::SPAWNED;
		}

	}
	return tickResult::NOTHING;
}