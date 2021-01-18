#pragma once
#include "..\..\Utilities\Constants.h"

enum class SpawnableObjectType {
	ITEM_SWORD, ITEM_SHIELD, ITEM_BOOTS, ITEM_POTION, PLAYER
};

// Pozycje poszczeg�lnych SpawnableObject musz� by� zapami�tywane, gdy� nie mo�na ich wywnioskowa� z po�o�enia w Map

class SpawnableObject
{
private:
	int timetoRespawn;
	bool isAlive;
	SpawnableObjectType type;
	int x;
	int y;

public:
	SpawnableObject(SpawnableObjectType type, int x, int y);


	void setAlive(bool state) { isAlive = state; };

	void spawn();

	void decreaseRespawnTime();

	void startRespawnTimer();

	void setSpawnCoords(int x, int y);
};



