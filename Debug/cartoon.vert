#version 420 core

//
// cartoon shader (vertex) -- based on an example from lighthouse3d.com
//

// input from application (CPU) => vertex positions and normals
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;

// output values that will be interpretated per-fragment from the fragment shader
out  vec3 fN;
out  vec3 fL;

// variables that will be defined from the application program
// light position, ModelView and Projection matrix
uniform vec4 LightPosition;
uniform mat4 ModelView;
uniform mat4 Projection;

void main (void)
{
	// fN: normal vector for the fragment shader
    fN = vNormal;
	// fL: light vector for the fragment shader
    fL = LightPosition.xyz;
	
	// if light is not a direction light the light vector will be calculated 
	// from the vertex position to the light position
	if( LightPosition.w != 0.0 ) {
		fL = LightPosition.xyz - vPosition.xyz;
    }

	// transform each vertex position with the modelView and projection matrix
	// result will be clipping coordinates
    gl_Position = Projection*ModelView*vPosition;
	
}
