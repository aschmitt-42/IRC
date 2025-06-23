#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <stdlib.h>
# include <sys/poll.h>
# include <sys/socket.h>
# include <vector>
# include "Channel.hpp"
// # include <fstream>
// # include <string>
// # include <arpa/inet.h>
// # include <netdb.h>
// # include <stdio.h>
// # include <string.h>
// # include <unistd.h>
// # include <sys/socket.h> 
// # include <fcntl.h>
// # include <sys/select.h>
// # include <sys/types.h>
// # include <poll.h>

class Client
{
	private :
		int						_client_fd;

		pollfd					_client_poll;
		std::vector<Channel*>	_channels;

		int						_mode;
		std::string				_nickname;
		std::string				_username;
		std::string				_realname;
		std::string				_msg;
		std::string				_hostname;
		
	public :
		int 		_registred_password;
		int			_registred_user;

		Client(pollfd client_poll, int client_fd);
		~Client();

		int		REGISTRED();
		void	Send_message(std::string msg);
		void	Join_Channel(Channel *channel);

		void	SEND_Quit_Msg(std::string msg);
		void	SET_Username(std::vector<std::string> argument, std::string msg);
		void 	Leave_Channel(Channel *channel);
		
		//GETTER
		int				get_clientfd()	{return _client_fd;}
		pollfd			get_pollfd()	{return _client_poll;}
		std::string		get_nick()		{return _nickname;} 
		std::string&	get_message()	{return _msg;}
		std::string		get_hostname()	{return _hostname;}
		std::string		get_username()	{return _username;}
		std::string		get_realname()	{return _realname;}
		std::string 	get_Prefix() const;


		//SETTER
		void			SET_Nick(std::string nick){_nickname = nick;}


};

int     IS_Client_In_Vector(std::vector<Client*> client_vector, Client *client);

#endif
