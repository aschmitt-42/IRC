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
		std::vector<char>		_mode;
		std::vector<Client*>	_client;
		std::vector<Client*>	_invite;
		std::string				_name;
		std::string				_topic;
		std::string				_password;
		Client					*_client_owner;
		size_t					_nb_max_user;

	public :
		Channel(std::string channel_name, std::string topic, Client *client);
		~Channel();

		std::string	ClientList();
		void		SEND_Msg(std::string msg, Client *client);
		void		DELETE_User(Client *client);
		void		New_User_msg(std::string msg);
		int			Add_User(Client *client);
		int			Try_Join(Client *client, std::string key);
		int			Client_in_Channel(std::string client_name);
		int			Try_Invite(Client *client, Client *new_client);
		;

		//GETTER
		std::string	GET_Topic(){return _topic;}
		std::string	GET_Name(){return _name;}
		Client*		GET_Owner(){return _client_owner;}

		void		SET_Topic(std::string new_topic){_topic = new_topic;}
		void		SET_Owner(Client *client){_client_owner = client;}
};


#define LIMITE_USER			'l'
#define INVITE_ONLY			'i'
#define PASSWORD			'k'

#endif