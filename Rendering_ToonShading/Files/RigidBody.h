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
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtc/constants.hpp>

#include "Mesh.h"


#include "AntTweakBar.h"

class RigidBody
{
public:

	glm::vec3 position;
	glm::vec3 orientation;

	glm::vec3 velocity;
	glm::vec3 ang_velocity;


	glm::mat4 translation_mat;
	glm::mat3 rotation_mat;

	glm::mat4 model_mat;



	Mesh mesh;

	RigidBody();
	RigidBody(Mesh _mesh);

	void load_mesh();
	void draw(GLuint spID);

	void addTBar(TwBar *bar);

	void update(float dt);

	void updateTranslation(float dt);
	void updateRotation(float dt);


	glm::vec3 getOrientationFromRotMat(glm::mat3 mat);

	
	
	glm::mat3 makeAngVelMat(glm::vec3 av_vec);


	void transformVertices();


	void reset();
};

