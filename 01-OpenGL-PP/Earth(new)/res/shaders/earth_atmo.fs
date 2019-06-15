#version 440 core


in float NdotL;
in float NdotC;

out vec4 fragColor;

uniform int Lkey;


void main(void)
{
if(Lkey==1)
fragColor=vec4(0.0, 0.67, 1.0,((1.0-NdotC)*0.87-0.30)*(NdotL*1.4+0.02));//*0.77=blend value
else
fragColor=vec4(1.0,1.0,1.0,0.0);

}