#include "client.hpp"

client::client()
{
	// std::cout << "Default constructor called" << std::endl;
}

client::~client()
{
	// std::cout << "Destructor called" << std::endl;
}

client::client(const client& other)
{
	(void)other;
	// std::cout << "Recopy constructor called" << std::endl;
}

client& client::operator=(const client &other)
{
	(void)other;
	return *this;
	// std::cout << "Assignment operator called" << std::endl;
}
