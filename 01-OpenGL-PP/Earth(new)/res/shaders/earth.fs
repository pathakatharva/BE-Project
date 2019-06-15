#version 440 core

in vec2 textureCoord_out;
in vec4 surfaceNormal;
in vec3 toLightVector;
in vec3 toLightVectorForSurface;

in vec4 tangents;

out vec4 fragColor;

uniform int Lkey;

uniform sampler2D tSampler;
uniform sampler2D t2Sampler;
uniform sampler2D normSampler;

void main(void)
{
vec4 myNormals=normalize(2.0*texture(normSampler,textureCoord_out)-1.0);
float NdotL=clamp(dot(vec4(toLightVector,0.0),myNormals),0,1);
float brightness=max(NdotL,0.1);

float surfaceNdotL=clamp(dot(vec4(toLightVectorForSurface,0.0),surfaceNormal),0.0,1.0);

vec4 finalLight=vec4(1.0,1.0,1.0,1.0)*brightness;

vec4 fTexture=texture(tSampler,textureCoord_out);
vec4 ambientTexture=texture(t2Sampler,textureCoord_out);

vec4 mixTexture=mix(ambientTexture,fTexture*finalLight*2.5,surfaceNdotL+0.1);

if(Lkey==1)
fragColor=mixTexture;
else
fragColor=fTexture ;
}