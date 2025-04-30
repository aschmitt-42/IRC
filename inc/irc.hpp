#ifndef IRC_HPP
# define IRC_HPP

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
# include <sstream>
# include "Client.hpp"
# include "Server.hpp"
# include "Channel.hpp"


std::string intToString(int number);
void IRC_Parser(std::string msg, Server *serv, Client *client);


#endif