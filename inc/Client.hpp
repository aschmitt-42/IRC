#ifndef CLIENT_HPP
# define CLIENT_HPP

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

class Channel;

class Client
{
	private :
		std::string	_nickname;
		int			_operator;
		Channel		*_channel;
		int			_client_fd;
	public :
		pollfd		_client_poll;
		std::string	_username;
		Client(pollfd client_poll, int client_fd);
		~Client();

		void JOIN_Channel(Channel *channel){_channel = channel;}

		//GETTER
		pollfd		GET_Pollfd(){return _client_poll;}
		std::string	GET_Nickname(){return _nickname;}
		std::string	GET_Username(){return _username;}
		Channel		*GET_Channel(){return _channel;}
		int			GET_Client_Fd(){return _client_fd;}
};

// std::ostream& operator<<(std::ostream& os, const Client& other);

#endif
