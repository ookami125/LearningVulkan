#include "VulkanException.h"

VulkanException::VulkanException(char const* const message, int line, char const* file) throw()
	: std::runtime_error(message), message(message), line(line), file(file)
{}

char const * VulkanException::what() const throw()
{
	return message;
}
