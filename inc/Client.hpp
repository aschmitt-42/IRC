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
		int			_operator;
		int			_client_fd;

		pollfd		_client_poll;
		Channel		*_channel;

		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_msg;
		int			_mode;
		
	public :
		int 		_registred_password;
		int			_registred_user;

		Client(pollfd client_poll, int client_fd);
		~Client();

		void	JOIN_Channel(Channel *channel){_channel = channel;}
		void	DELETE_Channel(){_channel = NULL;}
		void	OPERATOR(){_operator = 1;}

		void	Send_message(std::string msg);
		int		REGISTRED();
	
		//GETTER
		std::string		get_nick(){return _nickname;}
		std::string		get_username(){return _username;}
		std::string&	get_message(){return _msg;}
		pollfd			get_pollfd(){return _client_poll;}
		Channel			*get_channel(){return _channel;}
		int				get_clientfd(){return _client_fd;}
		int				is_operator(){return _operator;}

		//SETTER
		void			SET_Nick(std::string nick){_nickname = nick;}
		void			SET_Username(std::vector<std::string> argument);

};

// std::ostream& operator<<(std::ostream& os, const Client& other);

#endif
