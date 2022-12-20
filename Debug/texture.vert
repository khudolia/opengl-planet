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

out vec2 TexCoord;

// output values that will be interpreted per-fragment from the fragment shader
out  vec4 fC;	// color value per vertex

// variables that will be defined from the application program
// variables for reflection properties depending on the light and material settings
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;

// variable for the shininess exponent of the local illumination model - specular component
uniform float Shininess;

// variables that will be defined from the application program
// light position, ModelView and Projection matrix
uniform vec4 LightPosition;
uniform mat4 Model;
uniform mat4 Camera;
uniform mat4 Projection;

void main(void)
{
// fN: normal vector for the vertex shader
  	// transform normals to camera coordinate system - for rigid body use upper 3x3 matrix of ModelView, otherwise transpose(inverse(mat3(ModelView)))
	mat4 ModelView = Camera * Model;
	vec3 fN = normalize(mat3(ModelView) * vNormal);
	
	// fE: calculate eye vector (vertex position in camera coordinates) for the fragment shader
    vec3 fE = -(ModelView*vPosition).xyz;
	// fL: light vector for the fragment shader
    vec3 fL = LightPosition.xyz;
    
	// if light is not a direction light the light vector will be calculated 
	// from the vertex position to the light position
    if( LightPosition.w != 0.0 ) {
		fL = LightPosition.xyz - vPosition.xyz;
    }
	
    // Normalize the input lighting vectors (normal, eye and light vector)
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);

	// Calculate halfway vector => Phong illumination model
	vec3 R = normalize(-reflect(L,N)); 
	
	// set ambient component of the local illumination model
    vec4 ambient = AmbientProduct;

	// calculate diffuse component of the local illumination model
    float Kd = max(dot(L, N), 0.0);
    vec4 diffuse = Kd*DiffuseProduct;
    
	// calculate specular component of the local illumination model
	// use reflection vector for Phong illumination model
    float Ks = pow(max(dot(R, E), 0.0), 0.3*Shininess);
	
    vec4 specular = Ks*SpecularProduct;

    // discard the specular highlight if the light's behind the vertex
    if( dot(L, N) < 0.0 ) {
		specular = vec4(0.0, 0.0, 0.0, 1.0);
    }

	// calculate final color for each fragment by adding the ambient, diffuse and specular components
	// of the local illumination model
    fC = ambient + diffuse + specular;
    fC.a = 1.0;	
	gl_Position = Projection * Camera * Model * vPosition;
	
	TexCoord = vTexCoord;
}
