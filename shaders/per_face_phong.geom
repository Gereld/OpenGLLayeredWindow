#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 position[];

out vec4 inColor;

const vec3 lightPos = vec3(0.0, 50.0, 50.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 40.0;
const vec3 ambientColor = vec3(0.5, 0.0, 0.0);
const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float shininess = 16.0;

vec4 phong( vec3 pos, vec3 normal ) {
	vec3 L = normalize( lightPos - pos );

	// calculate Ambient Term:
	vec3 Iamb = ambientColor; 

	float lambertian = max( dot( normal, L ), 0.0 );

	// calculate Diffuse Term:  
	vec3 Idiff = diffuseColor * lambertian;

	// calculate Specular Term:
	float specular = 0.0;

	if( lambertian > 0.0 ) {
		vec3 E = normalize( -pos ); // we are in Eye Coordinates, so EyePos is (0,0,0)
		vec3 R = normalize( -reflect( L, normal ) );
		specular = pow( max( dot( R, E ), 0.0 ), shininess ); 
	} 
	vec3 Ispec = specColor * specular;

	return vec4( Iamb + Idiff + Ispec, 1.0 );
}

vec4 blinn( vec3 pos, vec3 normal ) {
	vec3 L = normalize( lightPos - pos );

	// calculate Ambient Term:
	vec3 Iamb = ambientColor; 

	float lambertian = max( dot( normal, L ), 0.0 );

	// calculate Diffuse Term:  
	vec3 Idiff = diffuseColor * lambertian;

	// calculate Specular Term:
	float specular = 0.0;

	if( lambertian > 0.0 ) {
		vec3 E = normalize( -pos ); // we are in Eye Coordinates, so EyePos is (0,0,0)
		vec3 H = normalize( E + L );
		specular = pow( max( dot( H, normal ), 0.0 ), shininess );
	}
	vec3 Ispec = specColor * specular;

	return vec4( Iamb + Idiff + Ispec, 1.0 );
}

void main() {
	vec3 n = normalize( cross( position[1] - position[0], position[2] - position[0] ) );

	inColor = blinn( position[0], n );

	for( int i = 0; i < 3; i++ ) {
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}
