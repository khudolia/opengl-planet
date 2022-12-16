//=======================================================================
//
// Utilitie class - load shader programs and textures
//					compile shader programs
// Greg Rauhöft, WS 15/16
// modified by Uwe Kloos, SS 16
//
//=======================================================================

#include "utilities.h"

Utilities::Utilities(){}

std::string Utilities::loadFile(const std::string& filename){
    std::ifstream file(filename.c_str());
    std::string contents((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    file.close();
    return contents;
}

void Utilities::loadBMP(const std::string &filename, BMP &bmp){
	std::ifstream file(filename.c_str());

	file.seekg(18);
	char widthString[2];
	file.read(widthString, 2);
	bmp.width = widthString[0] | widthString[1] << 8;

	file.seekg(22);
	char heightString[2];
	file.read(heightString, 2);
	bmp.height = heightString[0] | heightString[1] << 8;

	size_t fileSize = bmp.width*bmp.height*3;
	bmp.colors.resize(fileSize);
	file.seekg(54);
	file.read((char*) &bmp.colors[0], fileSize); 
	file.close();
}

void Utilities::loadPNG(const std::string& filename, PNG &png){
	lodepng::decode(png.colors, png.width, png.height, filename);
}


void Utilities::loadObj(const std::string &filename, OBJ &obj){
    std::ifstream file(filename.c_str());

    std::string line;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
	std::vector<glm::vec2> coordinates;
    std::vector<int> vertexIndices, normalIndices, coordinateIndices;
    while(std::getline(file, line)){
        if(line.substr(0,2) == "v "){
            glm::vec4 vertex;
            std::istringstream stream(line.substr(2));
            stream >> vertex.x;
            stream >> vertex.y;
            stream >> vertex.z;
            vertex.w = 1.0f;
            vertices.push_back(vertex);
        }
        else if(line.substr(0,3) == "vt "){
            glm::vec2 coordinate;
            std::istringstream stream(line.substr(3));
            stream >> coordinate.x;
            stream >> coordinate.y;
			coordinate.y *= -1;
            coordinates.push_back(coordinate);
        }
        else if(line.substr(0,3) == "vn "){
            glm::vec3 normal;
            std::istringstream stream(line.substr(3));
            stream >> normal.x;
            stream >> normal.y;
            stream >> normal.z;
            normals.push_back(normal);
        }
        else if(line.substr(0,2) == "f "){
            std::string data = line.substr(2);
            size_t numberOfSlashes = std::count(data.begin(),data.end(),'/');
            std::vector<std::string> parts;
            Utilities::tokenize(data," ",parts);
            std::vector<std::string> tokens;
            for(size_t i = 0; i < parts.size(); i++){
                Utilities::tokenize(parts[i],"/",tokens);
            }
            for(size_t i = 0; i<3; ++i){
                if(tokens.size() == 3){
                    vertexIndices.push_back(atoi(tokens[i].c_str()));
                }
                else if(tokens.size() == 6){
                    if(numberOfSlashes == 3){
                        vertexIndices.push_back(atoi(tokens[i*2].c_str()));
                        coordinateIndices.push_back(atoi(tokens[i*2+1].c_str()));
                    }
                    else if(numberOfSlashes == 6){
                        vertexIndices.push_back(atoi(tokens[i*2].c_str()));
                        normalIndices.push_back(atoi(tokens[i*2+1].c_str()));
                    }
                }
                else if(tokens.size() == 9){
                    vertexIndices.push_back(atoi(tokens[i*3].c_str()));
                    coordinateIndices.push_back(atoi(tokens[i*3+1].c_str()));
                    normalIndices.push_back(atoi(tokens[i*3+2].c_str()));
                }
            }
        }
    }
	file.close();
    for(std::vector<int>::iterator it = vertexIndices.begin(); it != vertexIndices.end(); ++it){
        obj.vertices.push_back(vertices[*it-1]);
    }
    for(std::vector<int>::iterator it = coordinateIndices.begin(); it != coordinateIndices.end(); ++it){
        obj.textureCoordinates.push_back(coordinates[*it-1]);
    }
    for(std::vector<int>::iterator it = normalIndices.begin(); it != normalIndices.end(); ++it){
        obj.normals.push_back(normals[*it-1]);
    }

	std::vector<glm::vec3> tangents;
	for(size_t i = 0; i<obj.vertices.size(); i+=3){
		glm::vec3 edge1(obj.vertices[i+1] - obj.vertices[i+0]);
		glm::vec3 edge2(obj.vertices[i+2] - obj.vertices[i+0]);

		glm::vec2 uv1(obj.textureCoordinates[i+1] - obj.textureCoordinates[i+0]);
		glm::vec2 uv2(obj.textureCoordinates[i+2] - obj.textureCoordinates[i+0]);

		float r = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);
		glm::vec3 tangent = glm::normalize((edge1 * uv2.y - edge2 * uv1.y) * r);

		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);
	}
	//std::cout << "Tangent calculation " << tangents.size() << std::endl;
	//for(size_t i = 0; i < tangents.size(); i++){
	//	glm::vec3 currentTangent = tangents[i];
	//	for(size_t j = 0; j < tangents.size(); j++){
	//		if(i!=j &&	obj.normals[i] == obj.normals[j] && 
	//					obj.vertices[i] == obj.vertices[j] && 
	//					obj.textureCoordinates[i] == obj.textureCoordinates[j]){
	//			currentTangent += tangents[j];	
	//		}
	//	}
	//	obj.tangents.push_back(glm::normalize(currentTangent));
	//}
	//std::cout << "End Tangent Calculation" << std::endl;
}

//////////////////////////////////////////////////////////////
//
// 05.04.2017, uk
// function to create a sphere of size radius
// and a resolution of nLong x nLat vertices
//
//////////////////////////////////////////////////////////////
void Utilities::createSphere(float radius, int nLong, int nLat, OBJ &obj) {

	float const R = 1.0f / (float)(nLat - 1);
	float const S = 1.0f / (float)(nLong - 1);

	int const iNumberVertices = nLat * nLong;
	int const iNumberIndices = (nLat - 1) * (nLong - 1) * 4;


	int r, s;
	int counter = 0;

	float x, y, z;
	float u, v;

	// use constant values for pi
	float const halfPi = glm::half_pi<glm::float_t>();
	float const Pi = glm::pi<glm::float_t>();

	glm::vec4 front = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 top = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// create local arrays for vertex data
	glm::vec4 *lVertices = new glm::vec4[iNumberVertices];
	glm::vec3 *lNormals = new glm::vec3[iNumberVertices];
	glm::vec2 *lTexCoord = new glm::vec2[iNumberVertices];

	// calculate vertices for a sphere based on longitude/latitude calculation
	// parametric sphere description
	for (r = 0; r < nLat; r++)
		for (s = 0; s < nLong; s++) {

			// calculate x,y,z-coordinate based on longitude/latitude angles
			y = sin(-halfPi + Pi * r * R);
			x = cos(2 * Pi * s * S) * sin(Pi * r * R);
			z = sin(2 * Pi * s * S) * sin(Pi * r * R);

			// geometry data
			lVertices[counter].x = x * radius;
			lVertices[counter].y = y * radius;
			lVertices[counter].z = z * radius;
			lVertices[counter].w = 1.0;
	
			// normal data
			lNormals[counter].x = x;
			lNormals[counter].y = y;
			lNormals[counter].z = z;

			// texture coordinates
			u = s * S;
			v = r * R;
			lTexCoord[counter].s = u;
			lTexCoord[counter].t = v;

			counter++;

		}

	// create triangles based on the formerly calculated vertices
	// and a quad model
	int i1, i2, i3, i4;
	for (r = 0; r < nLat - 1; r++)
		for (s = 0; s < nLong - 1; s++) {

			// quad indices
			i1 = r * nLong + s;
			i2 = r * nLong + (s + 1);
			i3 = (r + 1) * nLong + (s + 1);
			i4 = (r + 1) * nLong + s;

			// one quad => 2 triangles
			// 1. triangle
			obj.vertices.push_back(lVertices[i1]);
			obj.normals.push_back(lNormals[i1]);
			obj.textureCoordinates.push_back(lTexCoord[i1]);

			obj.vertices.push_back(lVertices[i2]);
			obj.normals.push_back(lNormals[i2]);
			obj.textureCoordinates.push_back(lTexCoord[i2]);

			obj.vertices.push_back(lVertices[i3]);
			obj.normals.push_back(lNormals[i3]);
			obj.textureCoordinates.push_back(lTexCoord[i3]);

			// 2. triangle
			obj.vertices.push_back(lVertices[i1]);
			obj.normals.push_back(lNormals[i1]);
			obj.textureCoordinates.push_back(lTexCoord[i1]);

			obj.vertices.push_back(lVertices[i3]);
			obj.normals.push_back(lNormals[i3]);
			obj.textureCoordinates.push_back(lTexCoord[i3]);

			obj.vertices.push_back(lVertices[i4]);
			obj.normals.push_back(lNormals[i4]);
			obj.textureCoordinates.push_back(lTexCoord[i4]);

		}

	delete lVertices;
	delete lNormals;
	delete lTexCoord;


}

void Utilities::tokenize(const std::string &str, const std::string &delimiter, std::vector<std::string> &result){
    size_t last = 0, next = 0;
    while((next = str.find(delimiter, last))!=std::string::npos){
        std::string token = str.substr(last,next-last);
        if(token!=""){
            result.push_back(token);
        }
        last = next+1;
    }
    std::string token = str.substr(last);
    if(token!=""){
        result.push_back(token);
    }
}

void Utilities::compileStatus(GLuint shader){
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE){
		std::cout << "Shader failed to compile" << std::endl;
	}
}

GLuint Utilities::compileShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	const char *vertexShaderStr = vertexShaderSource.c_str();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderStr, NULL);
	glCompileShader(vertexShader);
	compileStatus(vertexShader);
	const char *fragmentShaderStr = fragmentShaderSource.c_str();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderStr, NULL);
	glCompileShader(fragmentShader);
	compileStatus(fragmentShader);
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}
