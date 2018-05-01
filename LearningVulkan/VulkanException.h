#pragma once
#include <stdexcept>
	
class VulkanException : public std::runtime_error
{
	char const * message;
	char const * file;
	int line;
public:
	VulkanException(char const* const message, int line, char const* file) throw();
	virtual char const* what() const throw();
};