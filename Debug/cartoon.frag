#version 420 core

//
// cartoon shader (fragment) -- based on an example from lighthouse3d.com
//

// input from the vertex shader
// per-fragment interpolated values from the vertex shader (normal, light vector)
in  vec3 fN;
in  vec3 fL;

// output from fragment shader => a color for each fragment
out vec4 fColor;

void main (void)
{
  float intensity;
  vec4 color;
  
  // Normalize the input lighting vectors (normal and light vector)
  vec3 N = normalize(fN);
  vec3 L = normalize(fL);
 
  // calculate an intensity value based on the angle between normal and light vector
  // cosine(angle(lightvector, normal) => dot product
  intensity = max(dot(L, N), 0.0);

  // define several intervals depending on the intensity for color definition
  if (intensity > 0.95)
    color = vec4 (1.0, 0.5, 0.5, 1.0);
  else if (intensity > 0.7)
    color = vec4 (0.6, 0.3, 0.3, 1.0);
  else if( intensity > 0.4)
    color = vec4 (0.4, 0.2, 0.2, 1.0);
  else if( intensity > 0.05)
    color = vec4 (0.3, 0.15, 0.15, 1.0);	
  else
	color = vec4 (0.2, 0.1, 0.1, 1.0);

  // calculate final color for each fragment
  fColor = color;
}
