#include "Parser.h"
using namespace Parser;

void Messenger::addEvent(int sender, int receiver, int type, int subtype, std::string data)
{
	Event ev = Event(sender, receiver,(Type) type,(SubType) subtype, data);
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
void Messenger::addEventMidGame(int sender, int receiver, std::chrono::duration<double> time) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INFODUMP_GAME_MID, std::to_string(time.count()));
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventGameEnd(int sender, int receiver, std::string results) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INFODUMP_GAME_END, results);
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}

void Messenger::addEventVote(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::LOBBY, SubType::VOTE, std::string());
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}
void Messenger::addEventVote(int sender, int receiver, int numOfVotes) {
	Event ev = Event(sender, receiver, Type::LOBBY, SubType::VOTE, std::to_string(numOfVotes));
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}
void Messenger::addEventResetClient(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::RESET, std::string());
	eventList.push_back(ev);
	Logger::log(ev.sender, ev.receiver, ev.type, ev.subtype, ev.subdata);
}
