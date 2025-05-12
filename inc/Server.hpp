#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"
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

# include <vector>

# include <sstream>

class Server
{
	private :
		int						_server_socket;
		std::string				_port;
		std::string				_password;
		std::vector<pollfd>		_poll_fds;		// Tableau des descripteur
		std::vector<Client*>	_clients;	// Tableau des clients
		std::vector<Channel*>	_channels;

		void read_data_from_socket(Client *client);
		void accept_new_connection();

	public :

		Server(std::string port, std::string password);
		~Server();

		void	start();
		void	disconect_client(Client *client);
		
		Client* FINDING_Client_fd(int client_fd);
		Client* FINDING_Client_str(std::string username);

		//GETTER
		std::vector<Channel*>	get_channel(){return _channels;}
		std::vector<Client*>	GET_Client(){return _clients;}
		std::vector<pollfd>		get_pollfd(){return _poll_fds;}

		//command
		
		void PASS(Client *client, std::vector<std::string> argument);
		void NICK(Client *client, std::vector<std::string>argument);
		void USER(Client *client, std::vector<std::string>argument);

		void QUIT(Client *client, std::vector<std::string>argument);
		void JOIN(Client *user, std::vector<std::string> argument);
		void KICK(Client *client, std::vector<std::string> argument);
		void INVITE(Client *client, std::vector<std::string> argument);
		void TOPIC(Client *client, std::vector<std::string> argument);
		void MODE(Client *client, std::vector<std::string> argument);
		Channel	*CHANNEL_Exist(std::string channel_name);
};

// std::ostream& operator<<(std::ostream& os, const server& other);
int create_server_socket(int port);
void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size);
void IRC_Parser(std::string msg, Server *serv, Client *client);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FONCTION DERREUR
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void	err_461(std::string cmd, Client *client){ client->Send_message(cmd + " :Not enough parameters");}
void	err_462(Client *client){ client->Send_message(":Unauthorized command (already registered)");}
void	err_464(Client *client){ client->Send_message(":Password incorrect");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}
// void	err_461(Client *client){ client->Send_message("");}

#endif
