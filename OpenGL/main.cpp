//=======================================================================
//
// Base project for Computer Graphic, 
// third OpenGL program (illumination, texturing)
// WiSe 2022/23
//
// Name:
// Matrikelnummer:
//
//=======================================================================
#include "stb_image.h"
#include "Dependencies/glew/glew.h"
#include "Dependencies/glut/glut.h"
#include "utilities.h"
#include <string>


// define constant numbers for the box mesh
const int NumTriangles = 12;				// number faces, 2 triangles each
const int NumVertices = 3 * NumTriangles;	// number vertices

// define constant numbers for a sphere mesh
const int gSphereLong = 50;		// resolution longitude
const int gSphereLat = 25;		// resolution latitude
const int sNumberFinalVertices = (gSphereLong - 1)  * (gSphereLat - 1) * 6;
const int sNumberIndices = (gSphereLong - 1)  * (gSphereLat - 1) * 4;

// define types for the vertex position and color
typedef glm::vec4 point4;
typedef glm::vec4 color4;

// function declaration
void initDrawing();
void timer(int v);
void display();
void keyboard(unsigned char key, int x, int y);
void setLightMaterial();
void box();
void triangle(const point4& a, const point4& b, const point4& c, const color4 col);

// variables used in differnt functions
GLuint shader1, shader2, shader3, shader4;
GLuint vPosition;			// entry point for vertex positions into the vertex shader
GLuint vNormal;				// entry point for vertex normals into the vertex shader
GLuint vColor;				// entry point for vertex color into the vertex shader
GLuint vao1 = 0;
int g_Mode = 0;
int g_Index = 0;
float rotX = 0.0f;
float rotY = 0.0f;
float rotAnim = 0.0f;
float deltaTime, currentTime;
bool g_Animate = false;
int g_Shader = 0;
int g_Material = 0;

// arrays for vertex positions, colors and normals
point4 points[NumVertices];
color4 colors[NumVertices];
glm::vec3   normals[NumVertices];

Utilities::OBJ obj;			// data structure for geometry (sphere)
Utilities::PNG diffuse;		// data structore for a texture image

// Model-view and projection matrices uniform location
GLuint  hModel, hCamera, hProjection, hLightPos;
GLuint  hModel2, hCamera2, hProjection2;
GLuint  hModel3, hCamera3, hProjection3;
GLuint  hModel4, hCamera4, hProjection4;
GLuint diffuseTexture;

//=======================================================================
//
// Main function
//
//=======================================================================
int main(int argc, char* argv[])
{
	// initialize and open a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Third Shaderbased OpenGL Application - local illumination and texturing");

	glewInit();

	// do some initializations 
	initDrawing();

	// register display function 
	glutDisplayFunc(display);
	// register keyboard function 
	glutKeyboardFunc(keyboard);
	// register timer function (for animation)
	glutTimerFunc(100, timer, 0);

	// start the main loop
	glutMainLoop();

	return 0;
}



//=======================================================================
//
// Init function, will be called once at the beginnning
//
//=======================================================================
void initDrawing()
{
	GLuint vboPos, vboNormal, vboTexCoord;

	// =======================================================
	// load or create models
	// call 3D-box function
	// box();

	// =======================================================
	// load or create models	
	// call a 3D-sphere function => radius, resolution longitude, resulution latitdude, obj-stucture (stores data)
	Utilities::createSphere(0.5, gSphereLong, gSphereLat, obj);

	glEnable(GL_DEPTH_TEST);

	// =======================================================
	// load and compile shader program
	// vertex and fragment shsader

	std::string vertexShader = Utilities::loadFile("gouraud.vert");
	std::string fragmentShader = Utilities::loadFile("gouraud.frag");
	shader1 = Utilities::compileShader(vertexShader, fragmentShader);
	glUseProgram(shader1);

	std::string vertexShader2 = Utilities::loadFile("texture.vert");
	std::string fragmentShader2 = Utilities::loadFile("texture.frag");
	shader2 = Utilities::compileShader(vertexShader2, fragmentShader2);

	std::string vertexShader3 = Utilities::loadFile("phong.vert");
	std::string fragmentShader3 = Utilities::loadFile("phong.frag");
	shader3 = Utilities::compileShader(vertexShader3, fragmentShader3);


	std::string vertexShader4 = Utilities::loadFile("cartoon.vert");
	std::string fragmentShader4 = Utilities::loadFile("cartoon.frag");
	shader4 = Utilities::compileShader(vertexShader4, fragmentShader4);

	// =======================================================
	// Vertex array objects:
	// now create and activate vertex array objects to handle the link between the buffers
	// and the input attributes to the vertex shader
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	//=======================================================
	// handle textures
	//=======================================================
	//=======================================================
	// import image file as texture
	//=======================================================
	Utilities::loadPNG("world.png", diffuse);
	//unsigned char* data = stbi_load("world.png", &width, &height, &nrChannels, 0);

	// transfer texture data to the GPU
	glGenTextures(1, &diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, diffuse.width, diffuse.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &diffuse.colors[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	// =======================================================
	// Buffer objects:
	// Create and initialize buffer objects to copy vertex data (e.g. position, color)
	// to the GPU and link vertex data to the corresponding buffer object

	// generate and manage buffer object for vertex coordinates
	// transfer position data to the GPU
	glGenBuffers(1,&vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*obj.vertices.size(), &obj.vertices[0], GL_STATIC_DRAW);
	// link to shader where to find the vertex coordinates
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// generate and manage buffer object for vertex normals
	// transfer normal data to the GPU
	glGenBuffers(1,&vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*obj.normals.size(), &obj.normals[0], GL_STATIC_DRAW);
	// link to shader where to find the vertex color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// generate and manage buffer object for vertex texture coordinates
	// transfer texture coordinate data to the GPU
	glGenBuffers(1, &vboTexCoord);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*obj.textureCoordinates.size(), &obj.textureCoordinates[0], GL_STATIC_DRAW);
	// link to shader where to find the vertex color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Retrieve  uniform variable locations from shader program
	// for the modelview and projection matrix for illumination shader
	hModel = glGetUniformLocation(shader1, "Model");
	hCamera = glGetUniformLocation(shader1, "Camera");
	hProjection = glGetUniformLocation(shader1, "Projection");

	hModel2 = glGetUniformLocation(shader2, "Model");
	hCamera2 = glGetUniformLocation(shader2, "Camera");
	hProjection2 = glGetUniformLocation(shader2, "Projection");

	hModel3 = glGetUniformLocation(shader3, "Model");
	hCamera3 = glGetUniformLocation(shader3, "Camera");
	hProjection3 = glGetUniformLocation(shader3, "Projection");

	hModel4 = glGetUniformLocation(shader4, "Model");
	hCamera4 = glGetUniformLocation(shader4, "Camera");
	hProjection4 = glGetUniformLocation(shader4, "Projection");


}

//=======================================================================
//
// Handles the timer by incrementing the angle of rotation and requesting the
// window to display again, provided the program is in the spinning state.
// Since the timer function is only called once, it sets the same function to
// be called again.
//
//=======================================================================
void timer(int v) {
	if (g_Animate) {
		rotAnim += 1.0;
		if (rotAnim > 360.0) {
			rotAnim -= 360.0;
		}
		glutPostRedisplay();
	}
	glutTimerFunc(1000 / 60, timer, v);
}

//=======================================================================
//
// display function, will be called whenever the scene needs to be drawn
//
//=======================================================================
void display()
{

	// define background color
	const GLfloat color[] = { 0.9, 0.9, 0.9, 1 };

	// define light and material parameters for local illumniation
	// in display function if settings should be changed interactively
	setLightMaterial();

	// define camera parameter
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 projectionMatrix = glm::mat4(1.0);
	
	// rotate if animation is enabled
	if (g_Animate == true) {
		rotY = glm::radians(rotAnim);
		model = glm::rotate(model, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// show Europe in texture mode
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// define camera settings (position, orientation, projection mode)
	glm::vec3 eye(0, 0, 5), center(0, 0, 0), up(0, 1, 0);
	glm::mat4 camera = glm::lookAt(eye, center, up);


	// orthographic projection
	projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 100.0f);

	// some initalizations
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, color);

	// switch between illumination and texture shader
	if (g_Shader == 0) {
		glUseProgram(shader1);
		// model transformations if needed
		// set shader parameter if needed (e.g. modelview, projection matrix, light, material)
		glUniformMatrix4fv(hModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(hCamera, 1, GL_FALSE, glm::value_ptr(camera));
		glUniformMatrix4fv(hProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}

	// switch between illumination and texture shader
	if (g_Shader == 1) {
		//glBindTexture(GL_TEXTURE_2D, diffuseTexture);

		glUseProgram(shader2);
		// model transformations if needed
		// set shader parameter if needed (e.g. modelview, projection matrix)
		glUniformMatrix4fv(hModel2, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(hCamera2, 1, GL_FALSE, glm::value_ptr(camera));
		glUniformMatrix4fv(hProjection2, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}

	// switch between illumination and texture shader
	if (g_Shader == 2) {
		glUseProgram(shader3);
		// model transformations if needed
		// set shader parameter if needed (e.g. modelview, projection matrix, light, material)
		glUniformMatrix4fv(hModel3, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(hCamera3, 1, GL_FALSE, glm::value_ptr(camera));
		glUniformMatrix4fv(hProjection3, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}

	// switch between illumination and texture shader
	if (g_Shader == 3) {
		glUseProgram(shader4);
		// model transformations if needed
		// set shader parameter if needed (e.g. modelview, projection matrix, light, material)
		glUniformMatrix4fv(hModel4, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(hCamera4, 1, GL_FALSE, glm::value_ptr(camera));
		glUniformMatrix4fv(hProjection4, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}


	// activate object rendering - object to render will be controlled by the corresponding
	// VertexArrayObject; shader to use will be controlled by the shader program variable
	glDrawArrays(GL_TRIANGLES, 0, obj.vertices.size());


	glutSwapBuffers();
}

//=======================================================================
//
// keyboard function, will be called whenever a key is pressed 
//
//=======================================================================
void keyboard(unsigned char key, int x, int y)
{

	// select the pressed key
	switch (key)
	{
		// handle interaction
		
		case 'a':
		// activate / deactivate animation
			g_Animate = !g_Animate;
			break;
		
		case 's': // switch between diffent shaders
			g_Shader = (g_Shader + 1) % 4;
			break;

		case 'm': // switch between diffent materials
			g_Material = (g_Material + 1) % 3;
			break;

		case 'q':
			exit(EXIT_SUCCESS);
			break;
	}

	// enforce redrawing of the scene
	glutPostRedisplay();

}

//=======================================================================
//
// set light and material parameter for shader to calculate local illumination model
//
//=======================================================================
void setLightMaterial()
{

	// Initialize shader lighting parameters => use a white point light source
	point4 light_position(1.0, 1.0, 2.0, 1.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	// material variables
	color4 material_ambient; 
	color4 material_diffuse; 
	color4 material_specular; 
	float  material_shininess = 1.0f;

	// define the material properties for the sphere

	if (g_Material == 0) {
		// here: material ruby
		material_ambient = color4(0.17, 0.01, 0.01, 0.5);
		material_diffuse = color4(0.61, 0.04, 0.04, 0.5);
		material_specular = color4(0.73, 0.63, 0.63, 0.5);
		material_shininess = 76.8;
	}

	if (g_Material == 1) {
		// here: material ruby
		material_ambient = color4(1, 1, 1, 0.5);
		material_diffuse = color4(1, 1, 1, 0.5);
		material_specular = color4(1, 1, 1, 0.5);
		material_shininess = 1.0;
	}

	if (g_Material == 2) {
		// here: material ruby
		material_ambient = color4(1, 1, 1, 0.5);
		material_diffuse = color4(1, 1, 1, 0.5);
		material_specular = color4(1, 1, 1, 0.5);
		material_shininess = 100.0;
	}


	// calculate light reflection values based on the light and material properties
	// for ambient, diffuse and specular component
	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	// set reflection values in the shader for usage of the final color calculation
	// based on a local illumination model (like Phong or Blinn-Phong)
	glUniform4fv(glGetUniformLocation(shader1, "AmbientProduct"),
		1, glm::value_ptr(ambient_product));
	glUniform4fv(glGetUniformLocation(shader1, "DiffuseProduct"),
		1, glm::value_ptr(diffuse_product));
	glUniform4fv(glGetUniformLocation(shader1, "SpecularProduct"),
		1, glm::value_ptr(specular_product));
	glUniform1f(glGetUniformLocation(shader1, "Shininess"),
		material_shininess);

	// set light position in the vertex shader to calculate the light vector
	hLightPos = glGetUniformLocation(shader1, "LightPosition");
	glUniform4fv(hLightPos, 1, glm::value_ptr(light_position));

}


//=======================================================================
//
// generates a 3D-Box with 8 vertices and 6 faces/12 triangles 
//
//=======================================================================
void box()
{
	// define 8 box vertices
	// and the center of the box at the origin
	point4 v[8] = {
		glm::vec4(0.5f, -0.5f, 0.5f, 1.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
		glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),
		glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f),
		glm::vec4(0.5f, -0.5f, -0.5f, 1.0f),
		glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f),
		glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),
		glm::vec4(0.5f, 0.5f, -0.5f, 1.0f)
	};

	// define a color for each surface of the box
	color4 front = color4(1.0f, 1.0f, 1.0f, 1.0f);
	color4 back = color4(0.0f, 1.0f, 0.0f, 1.0f);
	color4 left = color4(1.0f, 0.0f, 0.0f, 1.0f);
	color4 right = color4(0.0f, 1.0f, 1.0f, 1.0f);
	color4 top = color4(0.0f, 0.0f, 1.0f, 1.0f);
	color4 bottom = color4(1.0f, 1.0f, 0.0f, 1.0f);

	// define the 12 triangles, counterclockwise
	// front => white
	triangle(v[0], v[1], v[2], front);
	triangle(v[0], v[2], v[3], front);

	// back => green
	triangle(v[4], v[5], v[6], back);
	triangle(v[4], v[6], v[7], back);

	// left => red
	triangle(v[5], v[3], v[2], left);
	triangle(v[5], v[2], v[6], left);

	// right => cyan
	triangle(v[0], v[4], v[7], right);
	triangle(v[0], v[7], v[1], right);

	// top => blue
	triangle(v[1], v[6], v[2], top);
	triangle(v[1], v[7], v[6], top);

	// bottom => yellow
	triangle(v[5], v[0], v[3], bottom);
	triangle(v[5], v[4], v[0], bottom);

}

//=======================================================================
//
// generates a single triangle with normals per vertex
// used to generate a triangle mesh
//
//=======================================================================
void triangle(const point4& a, const point4& b, const point4& c, const color4 col)
{


	//vec3  normal = normalize( cross(b - a, c - b) );
	// calculate normal via cross product based on two vectors between the vertices
	glm::vec3  normal = glm::cross(glm::vec3(b) - glm::vec3(a), glm::vec3(c) - glm::vec3(b));

	normal = glm::normalize(normal);

	glm::vec3  normalA(a.x, a.y, a.z);
	normalA = normalize(normalA);
	glm::vec3  normalB(b.x, b.y, b.z);
	normalB = normalize(normalB);
	glm::vec3  normalC(c.x, c.y, c.z);
	normalC = normalize(normalC);

	normals[g_Index] = normalA;  points[g_Index] = a; colors[g_Index] = col; g_Index++;
	normals[g_Index] = normalB;  points[g_Index] = b; colors[g_Index] = col; g_Index++;
	normals[g_Index] = normalC;  points[g_Index] = c; colors[g_Index] = col; g_Index++;

}
