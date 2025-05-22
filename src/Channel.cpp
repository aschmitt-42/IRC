#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

Channel::Channel(std::string channel_name, std::string topic_name, Client *client)
{
	_name = channel_name;
	_topic = topic_name;
	_password = "";
	_nb_max_user = -1; 
	_client_owner = client;
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

void	Channel::New_User_msg(std::string msg)
{
	for (size_t i = 0; i < _client.size(); i++)
	{
		_client[i]->Send_message(msg);
	}
	
}

void	Channel::DELETE_User(Client *client)
{
	for (size_t	i = 0; i < _client.size(); ++i)
	{
		if (_client[i] == client)
		{
			_client.erase(_client.begin() + i);
			// client->DELETE_Channel();
		}
	}
}

void	Channel::SEND_Msg(std::string msg, Client *client)
{
	// msg = "[" + client->get_username() + "]" + " : " + msg;

	for (size_t	i = 0; i < _client.size(); ++i)
	{
		if (_client[i]->get_nick() != client->get_nick())
			_client[i]->Send_message(msg);
	}
}

std::string	Channel::ClientList()
{
	std::string client_list;

	for (size_t i = 0; i < _client.size(); i++)
	{
		if (_client_owner->get_nick() == _client[i]->get_nick())
		client_list += _client[i]->get_nick();
		if (i != _client.size() - 1)
			client_list += " ";
	}
	return client_list;
}

int Channel::Try_Join(Client *client, std::string key)
{
	for (std::vector<char>::iterator it = _mode.begin(); it != _mode.end(); ++it)
	{
		if (*it == 'l' && _client.size() >= _nb_max_user)
		{
			ERR(client, 471, _name, "Cannot join channel (+l)");
			return 1;
		}
		if (*it == 'i' && 1 == 1) // && pas invite alors err
		{
			ERR(client, 473, _name, "Cannot join channel (+i)");
			return 1;
		}
		if (*it == 'k' && key != _password)
		{
			ERR(client, 475, _name, "Cannot join channel (+k)");
			return 1;
		}
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
	int i = 0;
	for (std::vector<char>::iterator it = _mode.begin(); it != _mode.end(); ++it)
	{
		if (*it == 'i'){
			i = 1;
			break;
		}
	}

	if (i && client->get_nick() != _client_owner->get_nick()) // mode 'invite_only' et client pas operateur alors erreur pour inviter quelqu'un
		return 1;
	
	// PAS DE PROBLEME POUR INVITER
	_invite.push_back(new_client);

	return 0;
}