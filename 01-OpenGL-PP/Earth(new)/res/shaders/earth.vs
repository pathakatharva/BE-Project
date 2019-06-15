#version 440 core

in vec3 vPosition;
in vec2 vTex0Coord;
in vec3 vNormal;
in vec3 vTangent;

uniform mat4 uProjX;
uniform mat4 uViewX;
uniform mat4 uModelX;

uniform vec4 uLightPos;

out vec2 textureCoord_out;
out vec4 surfaceNormal;
out vec3 toLightVector;
out vec3 toLightVectorForSurface;

out vec4 tangents;

void main(void)
{	
	
	 surfaceNormal=normalize(uViewX * uModelX *vec4(vNormal,0.0));

	 vec3 norm=normalize(uViewX * uModelX *vec4(vNormal,0.0)).xyz;
	 vec3 tan=normalize(uViewX * uModelX *vec4(vTangent,0.0)).xyz;
	 	tan = normalize(tan - dot(tan, norm) * norm);//reorthogonalize....Gram-Schmidt process 	 
	vec3 bitan = normalize(cross(norm,tan));

		 if (dot(cross(norm, tan), bitan) < 0.0)  //negative tangent correction
              tan = tan * -1.0;

	mat3 toTangentSpace = mat3(
	tan.x,bitan.x,norm.x,
	tan.y,bitan.y,norm.y,
	tan.z,bitan.z,norm.z
	);

	tangents=vec4(bitan,0.0);

	//vec4 lightPosition=vec4(-15.0,5.0,1.0,1.0);
	vec4 worldPosition=uModelX * vec4(vPosition,1.0);

	 toLightVector= normalize(uLightPos.xyz - worldPosition.xyz);
	 toLightVectorForSurface=normalize(toLightVector);
	 toLightVector=normalize(toTangentSpace*toLightVector);
	gl_Position = uProjX * uViewX * worldPosition; 
	textureCoord_out =vTex0Coord;

}