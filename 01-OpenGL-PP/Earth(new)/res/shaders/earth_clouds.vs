#version 440 core

in vec3 vPosition;
in vec2 vTex0Coord;
in vec3 vNormal;


uniform mat4 uProjX;
uniform mat4 uViewX;
uniform mat4 uModelX;

uniform vec4 uLightPos;

out vec2 textureCoord_out;

out float NdotL;

void main(void)
{	

	vec4 surfaceNormal=normalize(uViewX * uModelX *vec4(vNormal,0.0));

	vec4 worldPosition=uModelX * vec4(vPosition,1.0);

	vec3 toLightVector=normalize(uLightPos.xyz-worldPosition.xyz);

	NdotL=clamp(dot(vec4(toLightVector,0.0),surfaceNormal),0.0,1.0);

	gl_Position = uProjX * uViewX * worldPosition; 
	textureCoord_out =vTex0Coord;

}