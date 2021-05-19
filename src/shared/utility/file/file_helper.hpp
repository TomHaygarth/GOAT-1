#pragma once

#include <cinttypes>
#include <string>
#include <vector>

namespace FileHelpers
{
    std::vector<char> read_file(std::string const & filename);

    bool folder_exists(std::string const & path);

    std::string to_native_path(std::string const & path);
    std::string to_standard_path(std::string const & path);
}
