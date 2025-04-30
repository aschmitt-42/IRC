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
		std::vector<Client>	_client;	
	public :
		Channel();
		~Channel();
};

// std::ostream& operator<<(std::ostream& os, const Client& other);

#endif
