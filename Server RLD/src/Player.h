#pragma once

class Player
{
public:
	Player(int ID, std::string name)
	{
		playerID = ID;
		this->name = name;
	}
	enum State { ACTIVE, ALIVE, DEAD, INACTIVE };
	int playerID;
	std::string name;
	State state;

};

