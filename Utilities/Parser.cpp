#include "Parser.h"
using namespace Parser;
Type Parser::convertToType(int type)
{
	switch (type)
	{
		//mozna to tak zrobic
		case static_cast<int>(Type::SERVER) :
			return static_cast<Type>(type);
			break;
			case static_cast<int>(Type::LOBBY) :
				return static_cast<Type>(type);
				break;
				case static_cast<int>(Type::GAME) :
					return static_cast<Type>(type);
					break;
				default:
					return Type::SLASH;
	}
	//albo tak
	if (type >= 47 && type <= 50)
		return static_cast<Type>(type);
}

SubType Parser::convertToSubType(int type)
{
	/*
	switch (type)
	{
	//mozna to tak zrobic
	case static_cast<int>(SubType::SERVER):
	return static_cast<SubType>(type);
	break;
	case static_cast<int>(SubType::LOBBY):
	return static_cast<SubType>(type);
	break;
	case static_cast<int>(SubType::GAME):
	return static_cast<SubType>(type);
	break;
	default:
	return SubType::SLASH;
	}
	*/
	//albo tak
	if (type >= 47 && type <= 55)
		return static_cast<SubType>(type);
	return SubType::ERRSUB;
}

Event Parser::decodeBytes(std::string data)
{
	//sender, receiver, type, subtype, data
	Event ev(data[0] - '0', data[1] - '0', data[2], data[3], std::string(data, 4));
	return ev;
}

std::string Parser::encodeBytes(Event ev)
{
	//char data[50];
	std::string data = std::string(ev.size);
	data.push_back(ev.sender + '0');
	data.push_back(ev.receiver + '0');
	data.push_back(ev.type);
	data.push_back(ev.subtype);
	data.append(ev.subdata);
	return data;
}

void Messenger::addEvent(int sender, int receiver, int type, int subtype, std::string data)
{
	Event ev = Event(sender, receiver, type, subtype, data);
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEvent(Event ev) {
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addInnerNewPlayer(int sender, int receiver, int ID) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INITPLAYER, std::string(1,(char) ID));
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addInnerDiscPlayer(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::DISCPLAYER, std::string());
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventNewPlayer(int sender, int receiver, std::string name) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::NEWPLAYER, name);
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventDiscPlayer(int sender, int receiver, std::string name) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::DISCPLAYER, name);
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventTimeoutAnswer(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::TIMEOUTANSWER, std::string());
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventTimeoutReached(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::TIMEOUT, std::string());
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventLobby(int sender, int receiver, int numOfVotes) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INFODUMP_LOBBY, std::to_string(numOfVotes));
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventMapID(int sender, int receiver, int mapID) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::MAP, std::to_string(mapID));
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

// Wysyla aktualny czas trwania gry w unsigned int
void Messenger::addEventGame(int sender, int receiver, std::chrono::duration<double> time) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::GAME, std::to_string(time.count()));
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventGameEnd(int sender, int receiver, std::string results) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::GAME_END, results);
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}
