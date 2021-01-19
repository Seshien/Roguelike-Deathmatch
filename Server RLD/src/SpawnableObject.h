#pragma once
#include <iostream>
#include "..\..\Utilities\Constants.h"
#include "Tile.h"

enum class SpawnableObjectType {
	ITEM_SWORD, ITEM_SHIELD, ITEM_BOOTS, ITEM_POTION, PLAYER, BODY
};
enum class tickResult {NOTHING, SPAWNED, DESPAWNED};
// Pozycje poszczeg�lnych SpawnableObject musz� by� zapami�tywane, gdy� nie mo�na ich wywnioskowa� z po�o�enia w Map

class SpawnableObject
{
private:
	int x;
	int y;
protected:
	int timetoRespawn;
	int timetoDespawn;
	bool isExist;
	int itemID;
	SpawnableObjectType type;
	std::shared_ptr<Tile> occupiedTile;

public:
	SpawnableObject() {};
	SpawnableObject(int itemID, SpawnableObjectType type, std::shared_ptr<Tile> tile);

	void setExist(bool state) { isExist = state; };
	int getX() { return this->x; };
	int getY() { return this->y; };
	int getItemID() { return this->itemID; };
	SpawnableObjectType getType() { return this->type; };
	std::shared_ptr<Tile> getTile() { return occupiedTile; };

	void spawn();

	void despawn();

	tickResult tick();


	void startSpawnTimer();

};



