#include "logging.hpp"

#include <iostream>

constexpr bool log_debug = true;

void Utility::_DEBUG_LOG(std::string const & str)
{
    if (log_debug == true)
    {
        std::cout << "DEBUG: " << str << std::endl;
    }
}

void Utility::_ERROR_LOG(char const * func_name, int const func_line, std::string const & str)
{
    std::cerr << "ERROR: [" << func_name << ", line : " << func_line << "] : " << str << std::endl;
}