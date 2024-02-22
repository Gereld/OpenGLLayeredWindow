#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 position;

void main() {
	gl_Position = projMatrix * viewMatrix * vec4( inPosition, 1.0 );
	position = inPosition;
}