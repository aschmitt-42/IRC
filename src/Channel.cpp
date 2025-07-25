#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

Channel::Channel(std::string channel_name, std::string topic_name, Client *client, Server *server)
{
	_name = channel_name;
	_topic = topic_name;
	_topic_restriction = false;
	_password = "";
	_invite_only = true;
	_nb_max_user = 0;
	_operator.push_back(client);
	_server = server;
}
Channel::~Channel(){}


int	Channel::Add_User(Client *client)
{
	for (size_t i = 0; i < _client.size(); i++)
	{
		if (_client[i]->get_nick().compare(client->get_nick()) == 0) // Client already in the channel
			return 1; 
	}
	
	_client.push_back(client);
	return 0;
}


void	Channel::DELETE_User(Client *client)
{
	for (size_t	i = 0; i < _client.size(); ++i)
	{
		if (_client[i]->get_nick() == client->get_nick())
		{
			_client.erase(_client.begin() + i);
		}
	}
	for (size_t	i = 0; i < _operator.size(); ++i)
	{
		if (_operator[i]->get_nick() == client->get_nick())
		{
			_operator.erase(_operator.begin() + i);
		}
	}
}


void	Channel::SEND_Msg(std::string msg, Client *client)
{
	for (size_t	i = 0; i < _client.size(); ++i)
	{
		if (_client[i]->get_nick() != client->get_nick())
			_client[i]->Send_message(msg);
	}
}

void	Channel::Send_Msg_To_All_Client(std::string msg)
{
	for (size_t i = 0; i < _client.size(); i++)
	{
		_client[i]->Send_message(msg);
	}
	
}

std::string	Channel::ClientList()
{
	std::string client_list;

	for (size_t i = 0; i < _client.size(); i++)
	{
		if (Is_Operator(_client[i]))
			client_list += "@";
		client_list += _client[i]->get_nick();
		if (i != _client.size() - 1)
			client_list += " ";
	}
	return client_list;
}

int Channel::Try_Join(Client *client, std::string key)
{
	if (_nb_max_user != 0 && _client.size() >= _nb_max_user)
	{
		ERR(client, 471, _name, "Cannot join channel (+l)");
		return 1;
	}
	if (_invite_only)
	{
		for (std::vector<Client*>::iterator it = _invite.begin(); it != _invite.end(); ++it) 
		{
			if ((*it)->get_nick() == client->get_nick()) //client is invited
				return 0;
		}

		ERR(client, 473, _name, "Cannot join channel (+i)");
		return 1;
	}
	if (_password != "" && key != _password)
	{
		ERR(client, 475, _name, "Cannot join channel (+k)");
		return 1;
	}
	return 0;
}

int	Channel::Client_in_Channel(std::string client_name)
{
	for (size_t i = 0; i < _client.size(); i++)
	{
		if (_client[i]->get_nick() == client_name)
			return 1;
	}
	return 0;
}

int	Channel::Try_Invite(Client *client, Client *new_client)
{
	if (_invite_only && !this->Is_Operator(client))
		return 1;
	
	_invite.push_back(new_client);

	return 0;
}

int	Channel::Is_Operator(Client *client)
{
	for (size_t i = 0; i < _operator.size(); i++)
	{
		if (_operator[i]->get_nick() == client->get_nick())
			return 1;
	}
	return 0;
}


/////////////////    MOD     /////////////////


std::string Channel::GET_Mode_List()
{
	std::string mode_list = "+";

	if (_invite_only)
		mode_list += "i";
	if (_topic_restriction)
		mode_list += "t";
	if (_password != "")
		mode_list += "k";
	if (_nb_max_user > 0)
		mode_list += "l";

	if (_password != "")
		mode_list += " " + _password;
	if (_nb_max_user > 0)
		mode_list += " " + intToString(_nb_max_user);

	return mode_list;
}

void Channel::INVITE_Only(bool add, Client *client)
{
	std::string msg;
	if (add)
	{
		if (!_invite_only)
			msg = ":" + client->get_Prefix() + " MODE " + _name + " +i";
		_invite_only = true;
	}
	else
	{ 
		if (_invite_only)
			msg = ":" + client->get_Prefix() + " MODE " + _name + " -i";
		_invite_only = false;
	}
	Send_Msg_To_All_Client(msg);
}

void Channel::TOPIC_Restriction(Client *client, bool add)
{
	std::string msg = ":" + client->get_Prefix() + " MODE " + _name + " " + (add ? "+" : "-") + "t";
	Send_Msg_To_All_Client(msg);

	if (add)
		_topic_restriction = true;
	else 
		_topic_restriction = false;
}

void Channel::CHANGE_Pass(Client *client, bool add, std::vector<std::string> argument)
{
	std::string msg = ":" + client->get_Prefix() + " MODE " + _name + " " + (add ? "+" : "-") + "k";
	
	if (add)
	{
		_password = argument[0];
		msg += " " + _password;
	}
	else	
		_password = "";

	this->Send_Msg_To_All_Client(msg);
}

void Channel::CHANGE_Operator(Client *client, Server *serv, bool add, std::vector<std::string> argument)
{
	if (!Is_Operator(client))
		return ERR(client, 482, client->get_nick() + " " + _name, "You're not channel operator");
	Client *target_client = serv->FINDING_Client_str(argument[0]);
    if (!target_client)
        return ERR(client, 401, argument[0], "No such nick/channel");
    for (size_t i = 0; i < _operator.size(); ++i)
	{
		if (target_client == _operator[i])
			_operator.erase(_operator.begin() + i);
	}
	if (add == true)
		_operator.push_back(target_client);
	
	std::string msg = ":" + client->get_Prefix() + " MODE " + _name + " " + (add ? "+" : "-") + "o " + target_client->get_nick();
	this->Send_Msg_To_All_Client(msg);
}

void Channel::USER_Limit(Client *client, bool add, std::vector<std::string> argument)
{

	std::string msg = ":" + client->get_Prefix() + " MODE " + _name + " " + (add ? "+" : "-") + "l";
	
	if (add == false)
	 	_nb_max_user = 0;
	else
	{
		int nb_limit;
		std::istringstream iss(argument[0]);
		iss >> nb_limit;
		if (!iss.fail() && iss.eof())
		{
			if (nb_limit < 1)
			{
				//err nombre trop petit
				return ERR(client, 497, "MODE", "Bad mode value");
			}
			_nb_max_user = nb_limit;
		}
		else
		{
			//err pas un nombre
			return ERR(client, 472, "l", "unknown mode char to me");
		}
		msg += " " + intToString(_nb_max_user);
	}
	this->Send_Msg_To_All_Client(msg);
}
