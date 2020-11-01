#include "file_helper.hpp"

#include "utility/logging.hpp"

#include <fstream>

std::vector<char> FileHelpers::read_file(std::string const & filename)
{
    std::vector<char> result;

    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (file.is_open() == false)
    {
        ERROR_LOG("Failed to open file: " + filename);
    }
    else
    {
        std::streampos file_size = file.tellg();
        if (file_size > 0)
        {
            file.seekg(0);
            result.reserve(static_cast<size_t>(file_size));
            file.read(result.data(), static_cast<size_t>(file_size));
        }
        else
        {
            ERROR_LOG("Failed to get the size of file: " + filename);
        }
    }

    file.close();

    return result;
}