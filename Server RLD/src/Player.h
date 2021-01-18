#pragma once
#include <string>

class Player
{
public:
	Player(int ID, std::string name)
	{
		playerID = ID;
		this->name = name;
	}
	enum State { ACTIVE, INACTIVE };
	bool timeout = false;
	bool voted = false;
	int playerID;
	std::string name;
	State state;

};

