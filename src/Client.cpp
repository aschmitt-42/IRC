#include "Client.hpp"


Client::Client(pollfd client_poll, int client_fd)
{
	_client_fd = client_fd;
	
	_client_poll = client_poll;

	_nickname = "";
	_msg = "";
	_username = "";
	_hostname = "";
	
	_registred_password = 0;
	_registred_user = 0;
}

Client::~Client()
{
	// std::cout << "Destructor called" << std::endl;
}

void Client::Send_message(std::string msg)
{
	msg += "\r\n";
	// std::cout << "--------SEND_MSG--------" << std::endl;
	// std::cout <<  msg << std::endl;
	// std::cout << "------------------------" << std::endl;

	if (send(_client_fd, msg.c_str(), msg.size(), 0) == -1)
	{
		std::cerr << "Error sending message to client" << std::endl;
	}
}

int	Client::REGISTRED()
{
	if (!_nickname.empty() && _registred_user == 1 && _registred_password == 1)
		return 1;
	return 0;
}

void	Client::SET_Username(std::vector<std::string> argument)
{
	_username = argument[0];
	_mode = std::atoi(argument[1].c_str());
	_realname = argument[3];
}

void	Client::Join_Channel(Channel *channel)
{
	_channels.push_back(channel);
}

std::string	Client::get_Prefix() const
{
	std::string s = _nickname + "!" + _username + "@" + _hostname;
	return s;
}

