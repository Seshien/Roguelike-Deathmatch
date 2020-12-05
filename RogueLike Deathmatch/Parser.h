#pragma once
#include <vector>

class Parser
{
public:
	struct Event
	{
		int sender; // od kogo jest ta wiadomosc
		int receiver; // dla kogo jest ta wiadomosc, 0 to serwer, reszta to playerId poszczegolnych graczy
		//gracz sie polaczyl, gracz sie rozlaczyl, gracz wykonal ruch, gracz wykonal akcje, zmiana stanu gracza, gracz zaglosowal, timeout, 
		int type; // typ eventu, moze dac jako enumerator 
		char subdata[];
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
	
	Event decodeBytes(char data[])
	{
		return Event();
	}
	char* encodeBytes(Event ev)
	{
		char data[5] = "TODO";
		return data;
	}
	std::vector<Event> eventList;

};

