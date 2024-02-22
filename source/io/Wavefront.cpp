#include "Wavefront.h"

#include <charconv>

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

namespace io {

void split_lines( std::string_view sv, std::vector<std::string_view> & output ) {
	static const std::string_view delims = "\n\r";

	size_t first = 0;
	while( first < sv.size() ) {
		auto second = sv.find_first_of( delims, first );

		if( first != second ) {
			output.emplace_back( sv.substr( first, second - first ) );
		}

		if( second == std::string_view::npos ) {
			break;
		}

		// handle CRLF
		if( (sv[second] == '\r') && ((second + 1) < sv.size()) && (sv[second + 1] == '\n') ) {
			++second;
		}

		first = second + 1;
	}
}

std::vector<std::string_view> split_lines( std::string_view sv ) {
    std::vector<std::string_view> output;
	split_lines( sv, output );
    return output;
}

void split( std::string_view sv, char delim, std::vector<std::string_view> & output ) {
	size_t first = 0;
	while( first < sv.size() ) {
		const auto second = sv.find( delim, first );

		if( first != second ) {
			output.emplace_back( sv.substr( first, second - first ) );
		}

		if( second == std::string_view::npos ) {
			break;
		}

		first = second + 1;
	}
}

std::vector<std::string_view> split( std::string_view sv, char delim = ' ' ) {
    std::vector<std::string_view> output;
	split( sv, delim, output );
    return output;
}

void basic_wavefront_obj_importer( std::string_view sv, std::vector<glm::vec3> & vertices, std::vector<std::array<int, 3>> & facets ) {
	std::vector<std::string_view> lines = split_lines( sv );
	std::vector<std::string_view> tokens;

	for( auto line : lines ) {
		if( line.empty() ) {
			continue;		
		}

		if( line[0] == '#' ) {
			// comment
			continue;
		}

		tokens.clear();
		split( line, ' ', tokens );
		auto type = tokens[0];

		if( type == "v" ) {
			// geometric vertices (v)
			// format: v x y z [w]

			std::array<float, 3> coord = { 0.f, 0.f, 0.f };
			size_t count = std::min( coord.size(), tokens.size() - 1 );
			for( int i = 0; i < count; ++i ) {
				auto token = tokens[i + 1];
				std::from_chars( token.data(), token.data() + token.size(), coord[i] );
			}

			vertices.push_back( glm::vec3( coord[0], coord[1], coord[2] ) );
		}
		else if( type == "f" ) {
			// face (f)
			// format: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
			// indices start at 1

			std::array<int, 3> facet = { 0, 0, 0 };
			std::vector<std::string_view> idxs;

			size_t count = std::min( 3ull, tokens.size() - 1 );
			for( int i = 0; i < count; ++i ) {
				auto token = tokens[i + 1];
	
				idxs.clear();
				split( token, '/', idxs );
				if( !idxs.empty() ) {
					// only use the first attribute, skip the rest
					auto idx_sv = idxs[0];
					int idx = 0;
					std::from_chars( idx_sv.data(), idx_sv.data() + idx_sv.size(), idx );
					facet[i] = idx - 1;
				}
			}

			facets.push_back( facet );
		}
	}
}

} // namesspace io