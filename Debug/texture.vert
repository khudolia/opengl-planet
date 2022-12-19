#version 420 core

//
// name:
// student id:
// short description:
//

// input from application (CPU) => vertex positions and normals
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec2 TexCoord;
//out vec3 ourColor;

void main(void)
{
	gl_Position = vec4(vPosition, 1.0);
	
	TexCoord = vec2(vTexCoord.x, vTexCoord.y);
}
