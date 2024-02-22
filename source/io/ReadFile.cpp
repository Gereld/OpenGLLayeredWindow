#include "ReadFile.h"

#include <fstream>

namespace io {

void read_binary_file( const std::filesystem::path & file, std::vector<unsigned char> & buffer ) {
	auto size = std::filesystem::file_size( file );
	buffer.resize( size );
	std::ifstream is( file.native().c_str() );
	is.read( reinterpret_cast<char *>( buffer.data() ), size );
}

std::vector<unsigned char> read_binary_file( const std::filesystem::path & file ) {
	std::vector<unsigned char> buffer;
	read_binary_file( file, buffer );
	return buffer;
}

void read_text_file( const std::filesystem::path & file, std::string & buffer ) {
	auto size = std::filesystem::file_size( file );
	buffer.resize( size + 1 );
	std::ifstream is( file.native().c_str() );
	is.read( reinterpret_cast<char *>( buffer.data() ), size );
	buffer[buffer.size() - 1] = 0;
}

std::string read_text_file( const std::filesystem::path & file ) {
	std::string buffer;
	read_text_file( file, buffer );
	return buffer;
}

} // namespace io
