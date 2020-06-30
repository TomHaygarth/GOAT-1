#include "logging.hpp"

#include <iostream>

constexpr bool log_debug = true;

void Utility::DEBUG_LOG(std::string const & str)
{
    if (log_debug == true)
    {
        std::cout << "DEBUG: " << str << std::endl;
    }
}