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
# include <algorithm>


class Server
{
	private :
		int						_server_socket;
		std::string				_port; 
		std::string				_password;
		std::vector<pollfd>		_poll_fds;		// Tableau des descripteur
		std::vector<Client*>	_clients;		// Tableau des clients
		std::vector<Channel*>	_channels;		// Tableau des channels existant

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
		void PING(Client *client, std::vector<std::string>argument);
		void PONG(Client *client, std::vector<std::string>argument);
		void QUIT(Client *client, std::string msg);
		void JOIN(Client *user, std::vector<std::string> argument);
		void KICK(Client *client, std::vector<std::string> argument);
		void INVITE(Client *client, std::vector<std::string> argument);
		void TOPIC(Client *client, std::vector<std::string> argument);
		void MODE(Client *client, std::vector<std::string> argument);
		void PRIVMSG(Client *client, std::vector<std::string> argument, std::string prv_msg);

		void MessageRegister(Client *client);

		int 	NICK_Already_Exist(std::string nickname);
		Channel	*CHANNEL_Exist(std::string channel_name);
};

int create_server_socket(int port);
void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size);
void IRC_Parser(std::string msg, Server *serv, Client *client);
std::string intToString(int number);
bool isValidNick(const std::string& nick);


void	ERR(Client *client, int err_id, std::string command, std::string msg);
//ERR(client, 1, "", "");

#endif
