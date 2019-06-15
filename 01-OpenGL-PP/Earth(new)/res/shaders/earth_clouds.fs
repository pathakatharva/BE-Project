#version 440 core

in vec2 textureCoord_out;
in float NdotL;

out vec4 fragColor;

uniform int Lkey;

uniform sampler2D earthCloudSampler;


void main(void)
{

vec4 cloudsTexture=texture(earthCloudSampler,textureCoord_out);
cloudsTexture.g=cloudsTexture.r;
cloudsTexture.b=cloudsTexture.g;
cloudsTexture.a=cloudsTexture.b * (NdotL*1.4+0.07);

if(Lkey==1)
fragColor=cloudsTexture  ;
else
fragColor=vec4(1.0,1.0,1.0,0.0);

}