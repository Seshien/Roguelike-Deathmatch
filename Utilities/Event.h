#pragma once
#include <string>

namespace Parser
{

	enum class Type { TYPEERROR, SERVER, LOBBY, GAME, INNER, ERRORNET };
	//sprobujmy tak zrobic zeby ERRSUB i ERRORNET byly na poczatku i koncu
	enum class SubType {
		SUBERROR, NEWPLAYER, INITPLAYER, DISCPLAYER, VOTE, TIMEOUT, TIMEOUTANSWER,
		MOVE, MOVEOUT, ACTION, ATTACK, HIT, DAMAGE, SPAWN, PSPAWN, ASKRESPAWN, RESPAWN, DESPAWN, MAXHEALTH, PDESPAWN, PICKUP, KILLCOUNT, DEATHCOUNT,
		CHANGESTATE, START, INFODUMP_LOBBY, MAP, INFODUMP_GAME_END, INFODUMP_GAME_MID, NOCONTACT, NOACCEPT, KEY, ERRORNET
	};
	const static std::string typeNames[] = { "Type Error", "Server", "Lobby", "Game", "Inner", "Error" };

	const static std::string subTypeNames[] = { "SUBERROR", "NEWPLAYER", "INITPLAYER", "DISCPLAYER", "VOTE", "TIMEOUT", "TIMEOUTANSWER",
		"MOVE", "MOVEOUT", "ACTION", "ATTACK", "HIT", "DAMAGE", "SPAWN","PSPAWN", "ASKRESPAWN", "DESPAWN", "MAXHEALTH", "PDESPAWN", "PICKUP", "KILLCOUNT", "DEATHCOUNT",
		"CHANGESTATE", "START", "INFODUMP_LOBBY", "MAP", "INFODUMP_GAME_END", "INFODUMP_GAME_MID", "NOCONTACT", "KEY", "ERRORNET" };

	Type convertToType(int type);
	SubType convertToSubType(int type);

	std::string convertToString(Type type);
	std::string convertToString(SubType type);
	struct Event
	{
		std::string size;
		int sender; // od kogo jest ta wiadomosc
		int receiver; // dla kogo jest ta wiadomosc, 0 to serwer, reszta to playerId poszczegolnych graczy
		//serwer, lobby, gra
		Type type;
		//gracz sie polaczyl, gracz sie rozlaczyl, gracz wykonal ruch, gracz wykonal akcje, zmiana stanu gracza, gracz zaglosowal, timeout, 
		SubType subtype; // typ eventu, moze dac jako enumerator 
		std::string subdata;

		Event(int _sender, int _receiver, Type _type, SubType _subtype, std::string _subdata);
		Event(int _sender, int _receiver, int _type, int _subtype, std::string _subdata);
	};

	Event decodeBytes(std::string data);

	std::string encodeBytes(Event ev);


}