#ifndef SERVER_HPP
# define SERVER_HPP

# include "irc.hpp"
# include "Client.hpp"
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
		int						_server_socket;
		std::string				_port;
		std::string				_password;
		std::vector<pollfd>		_poll_fds;		// Tableau des descripteur
		std::vector<Client*>	_clients;	// Tableau des clients


		void read_data_from_socket(int clien_fd);
		void accept_new_connection();

	public :
		Server(std::string port, std::string password);
		~Server();
		void	start();
		void	disconect_client(int client_fd) ;
};

// std::ostream& operator<<(std::ostream& os, const server& other);
int create_server_socket(int port);
void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size);

#endif
