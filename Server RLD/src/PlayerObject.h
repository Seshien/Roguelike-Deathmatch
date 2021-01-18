#pragma once
#include "SpawnableObject.h"

class PlayerObject : public SpawnableObject
{
	int playerId;
	int x;
	int y;

public:
	PlayerObject(int id, SpawnableObjectType type, int x, int y);
};

