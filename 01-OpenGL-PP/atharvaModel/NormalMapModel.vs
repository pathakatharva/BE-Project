#version 450 core

in vec4 vPosition;
in vec2 vTex0Coord;
in vec3 vNormal;
in vec3 vTangent;


uniform vec4 uLightPos;

uniform mat4 uMVPx;
uniform mat4 uModelViewX;
uniform mat4 uProjX;
uniform mat4 uModelX;

out vec3 toCameraVector;
out vec2 textureCoord_out;
out vec3 toLightVector;
out vec3 surfaceNormal;

void main(void)
{	
	
	 surfaceNormal = (uModelViewX * vec4(vNormal,0.0)).xyz;

	vec3 norm = normalize(surfaceNormal);
	vec3 tan = normalize(uModelViewX * vec4(vTangent,0.0)).xyz;
	vec3 bitan = normalize(cross(norm,tan));

	mat3 toTangentSpace = mat3(
	tan.x,bitan.x,norm.x,
	tan.y,bitan.y,norm.y,
	tan.z,bitan.z,norm.z
	);
	
	//light
	vec4 eyeCoord = uModelViewX * vPosition;


	 toLightVector = toTangentSpace * vec3(uLightPos - eyeCoord );//eyecoords ...not worldPos
	 toCameraVector =toTangentSpace * (-eyeCoord.xyz);

	gl_Position = uMVPx * vPosition;
	textureCoord_out = vTex0Coord;
}
