#pragma once

#include <span>
#include <Windows.h>

namespace wex {

std::span<const char> get_resource_data( HINSTANCE hInstance, const wchar_t * name );
std::span<const char> get_resource_data( HINSTANCE hInstance, unsigned int nID );

} // namespace resource
