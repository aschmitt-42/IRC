#ifndef SERVER_HPP
# define SERVER_HPP

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

class Server
{
	private :
		int					_server_socket;
		std::string			_port;
		std::string			_password;
		std::vector<pollfd>	_poll_fds;		// Tableau des descripteur

	public :
		Server(std::string port, std::string password);
		~Server();
		Server(const Server &other);
		Server& operator=(const Server &other);
		void	start();
};

// std::ostream& operator<<(std::ostream& os, const server& other);

#endif
