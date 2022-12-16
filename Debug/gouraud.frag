#version 420 core 

//
// gouraud shader (fragment) -- based on an example from lighthouse3d.com
// see also https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
//

// input from the vertex shader
// per-fragment interpolated values from the vertex shader (color for Gouraud shading)
in  vec4 fC;	// color value per vertex

// output from fragment shader => a color for each fragment
out vec4 fColor;

void main() 
{ 
    fColor = fC;
}