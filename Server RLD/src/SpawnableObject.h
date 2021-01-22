#pragma once
#include <iostream>
#include "..\..\Utilities\Config.h"
#include "Tile.h"

enum class SpawnableObjectType {
	ITEM_SWORD, ITEM_SHIELD, ITEM_BOOTS, ITEM_POTION, PLAYER, BODY
};
enum class tickResult {NOTHING, SPAWNED, DESPAWNED, CAN_ATTACK};
// Pozycje poszczególnych SpawnableObject musz¹ byæ zapamiêtywane, gdy¿ nie mo¿na ich wywnioskowaæ z po³o¿enia w Map

class SpawnableObject
{

protected:
	int x;
	int y;
	int timetoRespawn;
	int timetoDespawn;
	bool isExist;
	SpawnableObjectType type;
	std::shared_ptr<Tile> occupiedTile;

public:
	SpawnableObject() {};
	SpawnableObject(int itemID, SpawnableObjectType type, std::shared_ptr<Tile> tile);

	void setExist(bool state) { isExist = state; };
	bool getExist() { return isExist; };
	int getX() { return this->x; };
	int getY() { return this->y; };

	int getTimetoRespawn() { return this->timetoRespawn; };
	int getTimetoDespawn() { return this->timetoDespawn; };

	SpawnableObjectType getType() { return this->type; }
	std::shared_ptr<Tile> getTile() { return occupiedTile; }


	tickResult tick();


	void startSpawnTimer();

};



