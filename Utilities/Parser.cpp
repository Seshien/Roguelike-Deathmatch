#include "Parser.h"
using namespace Parser;

void Messenger::addEvent(int sender, int receiver, int type, int subtype, std::string data)
{
	Event ev = Event(sender, receiver,(Type) type,(SubType) subtype, data);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEvent(Event ev) {
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addInnerInitPlayer(int sender, int receiver, int ID) {
	Event ev = Event(sender, receiver, Type::INNER, SubType::INITPLAYER, std::string(1,(char) ID));
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addInnerDiscPlayer(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::INNER, SubType::DISCPLAYER, std::string());
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventInitPlayer(int sender, int receiver, int ID) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INITPLAYER, std::string(1, (char)ID));
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventNewPlayer(int sender, int receiver, int ID) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::NEWPLAYER, std::string(1, (char)ID));
	eventList.push_back(ev);
	Logger::log(ev);
}


void Messenger::addEventNewPlayer(int sender, int receiver, std::string name) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::NEWPLAYER, name);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventDiscPlayer(int sender, int receiver, std::string name) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::DISCPLAYER, name);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventTimeoutAnswer(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::TIMEOUTANSWER, std::string());
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventTimeoutReached(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::TIMEOUT, std::string());
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventMovement(int sender, int receiver,std::string playerName, int x, int y)
{
	auto subdata = std::string(1, (char)x) + std::string(1, (char)y) + playerName;
	Event ev = Event(sender, receiver, Type::GAME, SubType::MOVE, subdata);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventSpawn(int sender, int receiver, int objectType, int x, int y)
{
	auto subdata = std::string(1, (char)x) + std::string(1, (char)y) + std::string(1, (char)objectType);
	Event ev = Event(sender, receiver, Type::GAME, SubType::SPAWN, subdata);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventDespawn(int sender, int receiver, int objectType, int x, int y)
{
	auto subdata = std::string(1, (char)x) + std::string(1, (char)y) + std::string(1, (char)objectType);
	Event ev = Event(sender, receiver, Type::GAME, SubType::DESPAWN, subdata);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventAttack(int sender, int receiver, std::string playerName)
{
	Event ev = Event(sender, receiver, Type::GAME, SubType::ATTACK, playerName);
	eventList.push_back(ev);
	Logger::log(ev);
}

//narazie nie przekazuje dmg
void Messenger::addEventHit(int sender, int receiver, std::string playerName, int damage)
{
	Event ev = Event(sender, receiver, Type::GAME, SubType::HIT, playerName);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventDamaged(int sender, int receiver, int newHealth)
{
	Event ev = Event(sender, receiver, Type::GAME, SubType::DAMAGE, std::to_string(newHealth));
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventPickUp(int sender, int receiver, int itemType)
{
	Event ev = Event(sender, receiver, Type::GAME, SubType::PICKUP, std::to_string(itemType));
	eventList.push_back(ev);
	Logger::log(ev);
}



void Messenger::addEventLobby(int sender, int receiver, int numOfVotes) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INFODUMP_LOBBY, std::to_string(numOfVotes));
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventMapID(int sender, int receiver, int mapID) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::MAP, std::to_string(mapID));
	eventList.push_back(ev);
	Logger::log(ev);
}

// Wysyla aktualny czas trwania gry w unsigned int
void Messenger::addEventMidGame(int sender, int receiver, std::chrono::duration<double> time) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INFODUMP_GAME_MID, std::to_string(time.count()));
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventGameEnd(int sender, int receiver, std::string results) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::INFODUMP_GAME_END, results);
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventVote(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::LOBBY, SubType::VOTE, std::string());
	eventList.push_back(ev);
	Logger::log(ev);
}
void Messenger::addEventVote(int sender, int receiver, int numOfVotes) {
	Event ev = Event(sender, receiver, Type::LOBBY, SubType::VOTE, std::to_string(numOfVotes));
	eventList.push_back(ev);
	Logger::log(ev);
}
void Messenger::addEventNoContact(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::ERRORNET, SubType::NOCONTACT, std::string());
	eventList.push_back(ev);
	Logger::log(ev);
}
void Messenger::addEventLostConnection(int sender, int receiver)
{
	Event ev = Event(sender, receiver, Type::ERRORNET, SubType::DISCPLAYER, std::string());
	eventList.push_back(ev);
	Logger::log(ev);
}

void Messenger::addEventNoAccept(int sender, int receiver)
{
	Event ev = Event(sender, receiver, Type::ERRORNET, SubType::NOACCEPT, std::string());
	eventList.push_back(ev);
	Logger::log(ev);
}

