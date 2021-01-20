#pragma once
#include <vector>
#include "SpawnableObject.h"
#include "Tile.h"
#include "Item.h"


class PlayerObject : public SpawnableObject
{
	int playerID;
	int health;
	int maxHealth;
	int dmg;
	int attackRange;
	std::vector<ItemType> items;
	std::string playerName;

public:
	PlayerObject(int id, std::string playerName, SpawnableObjectType type, std::shared_ptr<Tile> tile);

	int getHealth() { return this->health; };
	void setHealth(int h) { this->health = h; }
	int getMaxHealth() { return this->maxHealth; };
	void setMaxHealth(int h) { this->maxHealth = h; }
	int getplayerID() { return this->playerID; };
	int getRange() { return this->attackRange; };
	int getDamage() { return this->dmg; };

	std::string getName() { return this->playerName; };

	int lastMove;
	bool readyToRespawn;

	void move(std::shared_ptr<Tile> newTile);

	void spawn();

	void despawn();
};

