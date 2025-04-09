#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <vector>


class client
{
	private :
		std::string			_nick;
		std::string			_user;
		std::vector<int>	channels;
		int					_fd;
		// int				ports;
	public :
		client();
		~client();
		client(const client &other);
		client& operator=(const client &other);
};

// std::ostream& operator<<(std::ostream& os, const client& other);

#endif
