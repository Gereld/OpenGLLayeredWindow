#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 position[];

out vec3 fragPosition;
out vec3 fragNormal;

void main() {
	vec3 n = normalize( cross( position[1] - position[0], position[2] - position[0] ) );

	for( int i = 0; i < 3; i++ ) {
		fragPosition = position[i];
		fragNormal = n;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}
