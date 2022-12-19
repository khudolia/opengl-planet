#version 420 core
out vec4 FragColor;

//
// name:
// student id:
// short description:
//

uniform sampler2D texture;
in vec2 TexCoord;

// output from fragment shader => a color for each fragment


void main(void)
{
	FragColor = texture(texture, TexCoord);
}
