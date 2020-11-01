#pragma once

#include <cinttypes>
#include <string>
#include <vector>

namespace FileHelpers
{
    std::vector<char> read_file(std::string const & filename);
}