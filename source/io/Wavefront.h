#pragma once

#include "glm/glm.hpp"

#include <array>
#include <string_view>
#include <vector>

namespace io {

void basic_wavefront_obj_importer( std::string_view sv, std::vector<glm::vec3> & vertices, std::vector<std::array<int, 3>> & facets );

} // namespace io