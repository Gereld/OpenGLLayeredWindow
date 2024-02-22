#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace io {

void                       read_binary_file( const std::filesystem::path & file, std::vector<unsigned char> & buffer );
std::vector<unsigned char> read_binary_file( const std::filesystem::path & file );

void        read_text_file( const std::filesystem::path & file, std::string & buffer );
std::string read_text_file( const std::filesystem::path & file );

} // namespace io
