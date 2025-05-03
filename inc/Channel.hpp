#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <arpa/inet.h>
# include <netdb.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <sys/socket.h> 
# include <fcntl.h>
# include <errno.h>
# include <stdlib.h>
# include <sys/select.h>
# include <sys/types.h>
# include <poll.h>
# include <sys/poll.h>
# include <sys/socket.h>
# include <vector>
# include "Client.hpp"


class Channel
{
	private :
		std::vector<Client*>	_client;
		std::string			_name;
		std::string			_topic;	
	public :
		Channel(std::string channel_name, std::string topic);
		~Channel();

		void	ADD_User(Client *client);
		void	SEND_Msg(std::string msg, Client *client);
		void	DELETE_User(){}

		//GETTER
		std::string	GET_Topic(){return _topic;}
};

// std::ostream& operator<<(std::ostream& os, const Client& other);

#endif