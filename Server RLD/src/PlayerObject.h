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
	int killCount;
	int deathCount;
	int spawnX;
	int spawnY;
	std::vector<ItemType> items;
	std::string playerName;

public:
	PlayerObject(int id, std::string playerName, SpawnableObjectType type, std::shared_ptr<Tile> tile);

	int getHealth() { return this->health; };
	void setHealth(int h) { this->health = h; }
	int getMaxHealth() { return this->maxHealth; };
	void setMaxHealth(int h) { this->maxHealth = h; }
	int getPlayerID() { return this->playerID; };
	int getRange() { return this->attackRange; };
	int getDamage() { return this->dmg; };
	void setDamage(int value) { this->dmg = value; };
	int getKillCount() { return this->killCount; };
	void setkillCount(int newKillCount) { this->killCount = newKillCount; };

	int getDeathCount() { return this->deathCount; };
	void setDeathCount(int newDeathCount) { this->deathCount = newDeathCount; };

	int getspawnX() { return this->spawnX; };
	int getspawnY() { return this->spawnY; };

	tickResult tick();


	std::vector<ItemType> getItems() { return this->items; };
	void addItem(int value) { this->items.push_back((ItemType)value); };

	std::string getName() { return this->playerName; };

	int lastMove;
	bool readyToRespawn;
	int cooldownTimer;

	void move(std::shared_ptr<Tile> newTile);

	void spawn();

	void despawn();
};

