#version 440 core

in vec3 vPosition;

uniform mat4 uProjX;
uniform mat4 uViewX;


void main(void)
{	
	
	vec4 eyeCoord =uViewX * vec4(vPosition,1.0);

	gl_PointSize=((length(eyeCoord.xyz)/500.0f)*2.5f)-2.49f;
	if(gl_PointSize<0)
	gl_PointSize*=-1.0;
		
	gl_Position = uProjX * eyeCoord; 

}