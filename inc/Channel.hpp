#ifndef CHANNEL_HPP
# define CHANNEL_HPP

// # include <iostream>
// # include <fstream>
// # include <string>
// # include <arpa/inet.h>
// # include <netdb.h>
// # include <stdio.h>
// # include <string.h>
// # include <unistd.h>
// # include <sys/socket.h> 
// # include <fcntl.h>
// # include <stdlib.h>
// # include <sys/select.h>
// # include <sys/types.h>
// # include <poll.h>
// # include <sys/poll.h>
// # include <sys/socket.h>
// # include <vector>

class Client;
class Server;

#define	I_MOD 1
#define	T_MOD 2
#define	K_MOD 3
#define	O_MOD 4
#define	L_MOD 5

struct ModChange 
{
    char mode;
    bool add; // true = +, false = -
    std::vector<std::string> argument; 
};

class Channel
{
	private :
		std::vector<Client*>	_client;
		std::vector<Client*>	_invite;
		std::vector<Client*>	_operator;
		std::string				_name;
		std::string				_topic;
		std::string				_password;
		size_t					_nb_max_user;
		bool					_invite_only;
		bool					_topic_restriction;
		Server					*_server;

	public :
		Channel(std::string channel_name, std::string topic, Client *client, Server *server);
		~Channel();

		std::string	ClientList();

		int			Add_User(Client *client);
		int			Is_Operator(Client *client);
		int			Try_Join(Client *client, std::string key);
		int			Client_in_Channel(std::string client_name);
		int			Try_Invite(Client *client, Client *new_client);

		void		DELETE_User(Client *client);
		void		Send_Msg_To_All_Client(std::string msg);
		void		SEND_Msg(std::string msg, Client *client);

		//MOD
		void INVITE_Only(bool add, Client *client);
		void TOPIC_Restriction(Client *client, bool add);
		void USER_Limit(Client *client, bool add, std::vector<std::string> argument);
		void CHANGE_Pass(Client *client, bool add, std::vector<std::string> argument);
		void CHANGE_Operator(Client *client, Server *serv, bool add, std::vector<std::string> argument);

		//GETTER
		int 					GET_Nb_User()			{return _client.size();}
		std::string				GET_Name()				{return _name;}
		std::string				GET_Topic()				{return _topic;}
		std::string 			GET_Mode_List();
		std::vector<Client*>	GET_Clients_Vector()	{return _client;}
		
		bool	Is_Topic_Restriction()					{return _topic_restriction;}
		bool	Is_Invite_Only()						{return _invite_only;}
		void	SET_Topic(std::string new_topic)		{_topic = new_topic;}
};

#endif