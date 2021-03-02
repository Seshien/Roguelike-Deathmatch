#include "Config.h"

namespace Config
{
	std::shared_ptr<ConfigHandler> configHandler;

	std::shared_ptr<ConfigHandler> getConfigHandler()
	{
		if (configHandler)
		{
			return configHandler;
		}
		else
		{
			configHandler = std::make_shared<ConfigHandler>();
			configHandler->loadConfig();
			return configHandler;
		}
	}

	void ConfigHandler::loadConfig()
	{
		std::fstream file;
		std::string line;
		file.open(confName);
		if (file.is_open())
		{
			Logger::info("Config file opened:");
			while (std::getline(file, line))
			{
				Logger::info(line);
				processConfigLine(line);
			}
		}
		else
		{
			Logger::info("Config file not found");
			Logger::info("Creating default config file TODO");
			//something something

		}
		file.close();
	}

	void ConfigHandler::processConfigLine(std::string line)
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

	void ConfigHandler::setConfigValue(std::string token, std::string value)
	{
		if (token[0] == '#') return;
		else if (token == "special") return;
		else if (token == "port") port = value;
		else if (token == "playerName") playerName = value;
		else if (token == "Server Address") IPAddress = value;
		else if (token == "Item Respawn Time") ITEM_RESPAWN_TIME = std::stoi(value);
		else if (token == "Player Respawn Time") PLAYER_RESPAWN_TIME = std::stoi(value);
		else Logger::debug("Unknown line in config file");
	}

}
