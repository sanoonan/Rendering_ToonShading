#pragma once

#define _CRT_SECURE_NO_DEPRECATE
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>


// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <string>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "stb_image.h"

#include "AntTweakBar.h"

using namespace std;

class Mesh
{
public:
	const char* filename;
	GLuint vao;
	int point_count;

	bool has_tex;
	GLuint tex;
	bool load_image_to_texture(const char* file_name, unsigned int& _tex, bool gen_mips);
	
	Mesh();
	Mesh(const char* _filename);

	bool load_mesh(std::vector<glm::vec3> &v);
	bool load_mesh();
	void assign_vao(const aiScene* scene);
	void draw(GLuint spID, glm::mat4 model_mat);

	std::vector<glm::vec3> getVertices();

	struct Material
	{
		glm::vec3 ambient, diffuse, specular;
		float shininess, beckmann_m, refraction;
		glm::vec3 cool, warm;

		Material(){ambient = diffuse = specular = cool = warm = glm::vec3(0.0f); shininess = beckmann_m = refraction = 0.0f;}
	};

	Material material;

	void load_material(const aiScene* scene);
	
	void addTBar(TwBar *bar);


};

