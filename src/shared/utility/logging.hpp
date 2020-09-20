#pragma once

#include <string>

#define DEBUG_LOG(log_msg) Utility::_DEBUG_LOG(log_msg)
#define ERROR_LOG(log_msg) Utility::_ERROR_LOG(__FUNCTION__, __LINE__, log_msg)

namespace Utility
{
    void _DEBUG_LOG(std::string const & str);
    void _ERROR_LOG(char const * func_name, int const func_line, std::string const & str);
}