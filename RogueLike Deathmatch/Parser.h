#pragma once
#include <vector>
#include <cstring>
#include <string.h>
#include "Constants.h"

class Parser
{
public:
	struct Event
	{
		int sender; // od kogo jest ta wiadomosc
		int receiver; // dla kogo jest ta wiadomosc, 0 to serwer, reszta to playerId poszczegolnych graczy
		//gracz sie polaczyl, gracz sie rozlaczyl, gracz wykonal ruch, gracz wykonal akcje, zmiana stanu gracza, gracz zaglosowal, timeout, 
		int type; // typ eventu, moze dac jako enumerator 
		char subdata[100];
	};
	Parser()
	{

	}
	//ogolnie to jest raczej zle
	// moze uzyc json'a do tego?
	void addEvent(int sender, int receiver, int type, char data[])
	{
		Event ev = { sender,receiver,type,*data };
		eventList.push_back(ev);
	}

	void addEvent(Event ev) {
		eventList.push_back(ev);
	}
	
	Event decodeBytes(char* data)
	{
		Event ev;
		ev.sender = data[0];
		ev.receiver = data[1];
		ev.type = data[2];
		//strcpy_s(ev.subdata, 3, data + 3);// Constants::msgLength - 3, data + 3);
		strncpy_s(ev.subdata, 100, data + 3, 47);
		return ev;
	}

	char* encodeBytes(Event ev)
	{
		char data[5] = "TODO";
		return data;
	}
	std::vector<Event> eventList;

};

