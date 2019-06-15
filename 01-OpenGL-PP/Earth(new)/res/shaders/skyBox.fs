#version 440 core

out vec4 fragColor;
in vec3 CubeTextCoord;
in vec2 textureCoord_out;

uniform sampler2D tSampler;
uniform samplerCube cubeMapTexture;

void main(void)
{

vec4 fTexture = texture(cubeMapTexture ,CubeTextCoord);
fragColor=fTexture;
}