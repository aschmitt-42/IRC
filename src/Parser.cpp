#include "Server.hpp"
#include "Client.hpp"

std::string CMD_Finder(std::string msg)
{
	std::string cmd;
	size_t		pos;
	
	pos = msg.find(' ');
	if (pos == std::string::npos)
		pos = msg.find('\n');
	if (pos != std::string::npos)
	{
		cmd = msg.substr(0, pos);
		std::string possible_cmd[12] = {"PASS", "NICK", "USER", "QUIT", "PONG", "PING", "PRIVMSG", "KICK", "JOIN", "TOPIC", "INVITE", "MODE"};
		for (size_t i = 0; i != 9; i++)
		{
			if (cmd == possible_cmd[i])
				return cmd;
		}
		cmd.clear();
	}
	return cmd;
}

std::vector<std::string> ARG_Finder(std::string msg)
{
    std::vector<std::string> words;
    std::string word;

    std::stringstream ss(msg);
    ss >> word;
	while (ss >> word) 
	{
        words.push_back(word);
		// std::cout << word << std::endl;
    }
	return words;
}

// void PRINT_Msg(std::string msg, Server *serv, Client *client)
// {
// 	if (client->get_channel())
// 		client->get_channel()->SEND_Msg(msg, client);
// 	(void)serv;
// }

void IRC_Parser(std::string msg, Server *serv, Client *client)
{
	std::string cmd = CMD_Finder(msg);
	if (cmd.empty() && client->_registred_user)
	{
		// PRINT_Msg(msg, serv, client);
		return;
	}

	std::vector<std::string> argument = ARG_Finder(msg);

	if (cmd == "PASS")
		return serv->PASS(client, argument);
	// else if (!client->_registred_password)
	// 	return ERR(client, 1, "", "You Need to enter the password first");
	else if (cmd == "NICK")
		return serv->NICK(client, argument);
	else if (cmd == "USER")
		return serv->USER(client, argument);
	else if (cmd == "QUIT")
		return serv->QUIT(client, argument);
	else if (cmd == "PING")
		return serv->PING(client, argument);
	

	// if (!client->_registred_user)
	// {
	// 	ERR(client, 1, "", "You need to register with USER and NICK command");
	// 	return;
	// }
	if (cmd == "JOIN")
		serv->JOIN(client, argument);
	else if (cmd == "PRIVMSG")
		return serv->PRIVMSG(client, argument);
	// else if (cmd == "KICK")
	// 	serv->KICK(client, argument);
	// else if (cmd == "INVITE")
	// 	serv->INVITE(client, argument);
	// else if (cmd == "TOPIC")
	// 	serv->TOPIC(client, argument);
	// else if (cmd == "MODE")
	// 	serv->MODE(client, argument);
}