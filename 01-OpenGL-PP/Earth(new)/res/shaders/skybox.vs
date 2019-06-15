#version 440 core

in vec3 vPosition;
out vec3 CubeTextCoord;

uniform mat4 uProjX;
uniform mat4 uViewX;


void main(void)
{	
		
	gl_Position = uProjX * uViewX  * vec4(vPosition,0.0); 
	vec3 flipPosition= vec3(-vPosition.x,-vPosition.y,vPosition.z);
	CubeTextCoord= flipPosition;

}