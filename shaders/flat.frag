#version 330 core

in vec4 inColor;

out vec4 outColor;

void main() {
	if( gl_FrontFacing ) {
		outColor = inColor;
	}
	else {
		outColor = vec4( 0, 0, 0, 1 );
	}
}