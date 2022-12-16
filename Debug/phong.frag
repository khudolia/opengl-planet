#version 420 core 

//
// phong shader => vertex shader - need to be implemented
// code based on an example from lighthouse3d.com
// see also https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
//
// name:
// student id:
// short description:
//

// input from the vertex shader
// per-fragment interpolated values from the vertex shader (normal, light, eye vector)
in  vec3 fN;
in  vec3 fL;
in  vec3 fE;

// output from fragment shader => a color for each fragment
out vec4 fColor;

// variables that will be defined from the application program
// variables for reflection properties depending on the light and material settings
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;

// variable for the shininess exponent of the local illumination model - specular component
uniform float Shininess;

void main() 
{ 

    // Normalize the input lighting vectors (normal, eye and light vector)
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);
	
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	// Calculate color value based on Phong illumination model using ambient, diffuse and specular
	// ... must be implemented
	

    fColor = ambient + diffuse + specular;
    fColor.a = 1.0;
}