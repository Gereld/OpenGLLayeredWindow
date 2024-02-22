#include "Resource.h"

namespace wex {

std::span<const char> get_resource_data( HINSTANCE hInstance, const wchar_t * name ) {
	HRSRC hResInfo = ::FindResourceW( hInstance, name, RT_RCDATA );

	if( !hResInfo ) {
		return {};
	}

	HGLOBAL hGlobal = ::LoadResource( hInstance, hResInfo );
	if( !hGlobal ) {
		return {};
	}

	const char * p    = reinterpret_cast<const char *>( ::LockResource( hGlobal ) );
	unsigned int size = ::SizeofResource( hInstance, hResInfo );

	return std::span{ p, p + size };
}

std::span<const char> get_resource_data( HINSTANCE hInstance, unsigned int nID ) {
	return get_resource_data( hInstance, MAKEINTRESOURCEW( nID ) );
}

} // namespace wex
