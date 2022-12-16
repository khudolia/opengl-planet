#version 420 core

//
// name:
// student id:
// short description:
//

// input from application (CPU) => vertex positions and normals
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;


void main(void)
{
	// need to be implemented

	gl_Position = vPosition;
}
