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
		//serwer, lobby, gra
		int type;
		//gracz sie polaczyl, gracz sie rozlaczyl, gracz wykonal ruch, gracz wykonal akcje, zmiana stanu gracza, gracz zaglosowal, timeout, 
		int subtype; // typ eventu, moze dac jako enumerator 
		char subdata[50];
		Event(int _sender, int _receiver, int _type, int _subtype, char _subdata[])
		{
			sender = _sender;
			receiver = _receiver;
			type = _type;
			subtype = _subtype;
			strncpy_s(subdata, 46, _subdata, 45);
		}
	};
	Parser()
	{

	}
	//ogolnie to jest raczej zle
	// moze uzyc json'a do tego?
	//ta funkcja nie powinna byc raczej uzywana
	void addEvent(int sender, int receiver, int type, int subtype, char data[])
	{
		Event ev = Event(sender,receiver,type,subtype,data);
		eventList.push_back(ev);
	}

	void addEvent(Event ev) {
		eventList.push_back(ev);
	}
	
	Event decodeBytes(char* data)
	{
		Event ev(data[0] - 48, data[1] - 48, data[2] - 48, data[3] - 48,data + 4);
		//ev.sender = data[0] - 48;
		//ev.receiver = data[1] - 48;
		//ev.type = data[2] - 48;
		//strcpy_s(ev.subdata, 3, data + 3);// Constants::msgLength - 3, data + 3);
		//strncpy_s(ev.subdata, 100, data + 3, 47);
		return ev;
	}

	char* encodeBytes(Event ev, char (&data)[50])
	{
		//char data[50];
		data[0] = ev.sender + 48;
		data[1] = ev.receiver + 48;
		data[2] = ev.type + 48;
		data[3] = ev.subtype + 48;
		strncpy_s(data + 4, 46, ev.subdata, 45);
		return data;
	}
	std::vector<Event> eventList;

};

