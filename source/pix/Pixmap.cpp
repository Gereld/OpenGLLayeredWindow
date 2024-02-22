#include "Pixmap.h"

namespace pix {

void multiply_alpha( View dst ) {
	transform( dst, multiply_pixel_alpha );
}

} // namespace pix
