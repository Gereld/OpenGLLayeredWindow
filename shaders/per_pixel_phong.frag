#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 outColor;

const vec3 lightPos = vec3(0.0, 0.0, 50.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 40.0;
const vec3 ambientColor = vec3(0.5, 0.0, 0.0);
const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float shininess = 16.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space
const int mode = 1;

vec4 blinn0( vec3 vertPos, vec3 normal ) {
	vec3 lightDir = lightPos - vertPos;
	float distance = length( lightDir );
	distance = distance * distance;
	lightDir = normalize( lightDir );

	float lambertian = max( dot( lightDir, normal ), 0.0 );
	float specular = 0.0;

	if( lambertian > 0.0 ) {
		vec3 viewDir = normalize( -vertPos );

		// this is blinn phong
		vec3 halfDir = normalize( lightDir + viewDir );
		float specAngle = max( dot( halfDir, normal ), 0.0 );
		specular = pow( specAngle, shininess );
       
		// this is phong (for comparison)
		if( mode == 2 ) {
			vec3 reflectDir = reflect( -lightDir, normal );
			specAngle = max( dot( reflectDir, viewDir ), 0.0 );
			// note that the exponent is different here
			specular = pow( specAngle, shininess/4.0 );
		}
	}

	vec3 colorLinear = ambientColor +
						diffuseColor * lambertian * lightColor * lightPower / distance +
						specColor * specular * lightColor * lightPower / distance;
	// apply gamma correction (assume ambientColor, diffuseColor and specColor
	// have been linearized, i.e. have no gamma correction in them)
	vec3 colorGammaCorrected = pow( colorLinear, vec3(1.0 / screenGamma) );

	// use the gamma corrected color in the fragment
	return vec4( colorGammaCorrected, 1.0 );
}

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
	//outputColor = texture2D( gSampler, texCoord );
	if( gl_FrontFacing ) {
		outColor = phong( fragPosition, fragNormal );
		//outColor = vec4( normalize( fragPosition ), 1.0 );
	}
	else {
		outColor = vec4( 0, 0, 0, 1 );
	}
}