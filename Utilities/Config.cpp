#include "Config.h"

void Config::loadConfig()
{
	std::fstream file;
	std::string line;
	file.open(confName);
	if (file.is_open())
	{
		Logger::log("Config file opened:");
		while (std::getline(file, line))
		{
			Logger::log(line);
			processConfigLine(line);
		}
	}

	else
	{
		Logger::log("Config file not found");
		Logger::log("Creating default config file TODO");
		//something something

	}
	file.close();
}

void Config::processConfigLine(std::string line)
{
	std::string delimiter = ":";
	int pos = line.find(delimiter);
	if (pos == -1)
	{
		Logger::error("Error during parsing of config file");
		return;
	}
	std::string token = line.substr(0, pos);
	std::string value = line.substr(pos + 1, line.length() - 1);
	Logger::debug("Token: " + token + " Value: " + value);
	setConfigValue(token, value);
	return;
}

void Config::setConfigValue(std::string token, std::string value)
{
	if (token == "special") return;
	else if (token == "port") port = value;
	else if (token == "playerName") playerName = value; 
	else if (token == "Server Address") IPAddress= value;
	else if (token == "Item Respawn Time") ITEM_RESPAWN_TIME = std::stoi(value);
	else if (token == "Player Respawn Time") PLAYER_RESPAWN_TIME = std::stoi(value);
	else Logger::debug("Unknown line in config file");
}
