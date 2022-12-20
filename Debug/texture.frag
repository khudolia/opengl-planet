#version 420 core

//
// name:
// student id:
// short description:
//

uniform sampler2D bindedTexture;
in  vec4 fC;	// color value per vertex
in vec2 TexCoord;

out vec4 FragColor;

void main(void)
{
	FragColor = texture(bindedTexture, TexCoord) * fC;
}
