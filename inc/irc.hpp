#ifndef IRC_HPP
# define IRC_HPP

# include "Server.hpp"
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


int     server(void);
void    del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count);
void    add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size);

#endif