#version 420 core 

//
// phong shader => vertex shader - is implemented
// code based on an example from lighthouse3d.com
//
// name:
// student id:
// short description:
//

// input from application (CPU) => vertex positions and normals
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;

// output values that will be interpretated per-fragment from the fragment shader
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;

// variables that will be defined from the application program
// light position, ModelView and Projection matrix
uniform vec4 LightPosition;
uniform mat4 Model;
uniform mat4 Camera;
uniform mat4 Projection;

void main()
{
	// fN: normal vector for the fragment shader  
	// transform normals to camera coordinate system - for rigid body use upper 3x3 matrix of ModelView, otherwise transpose(inverse(mat3(ModelView)))
	mat4 ModelView = Camera * Model;
	fN = normalize(mat3(ModelView) * vNormal);
	
	// fE: calculate eye vector (vertex position in camera coordinates) for the fragment shader
    fE = -(ModelView*vPosition).xyz;
	
	// fL: light vector for the fragment shader
    fL = LightPosition.xyz;
    
	// if light is not a direction light the light vector will be calculated 
	// from the vertex position to the light position
    if( LightPosition.w != 0.0 ) {
		fL = LightPosition.xyz - vPosition.xyz;
    }

	// transform each vertex position with the modelView and projection matrix
	// result will be clipping coordinates
    gl_Position = Projection * Camera * Model * vPosition;
}
