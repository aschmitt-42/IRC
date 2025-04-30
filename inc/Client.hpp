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


class Client
{
	private :
		std::string	_nickname;
		int			_operator;
		
	public :
		pollfd		_client_poll;
		std::string	_username;
		Client(pollfd client_poll);
		~Client();

		//GETTER
		pollfd	GET_Pollfd(){return _client_poll;}
};

// std::ostream& operator<<(std::ostream& os, const Client& other);

#endif
