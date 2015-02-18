#include "Mesh.h"

using namespace std;

Mesh :: Mesh()
{
	filename = "";
	vao = 0;
	point_count = 0;
}

Mesh :: Mesh(const char* _filename)
{
	filename = _filename;
	vao = 0;
	point_count = 0;
}

bool Mesh :: load_mesh (std::vector<glm::vec3> &v)
{
	
	v = getVertices();
	
  return load_mesh();
}



bool Mesh :: load_mesh ()
{
	

	
	const aiScene* scene = aiImportFile (filename, aiProcess_Triangulate); // TRIANGLES!

  
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fprintf (stderr, "ERROR: reading mesh %s\n", filename);
		return false;
	}
	printf ("  %i animations\n", scene->mNumAnimations);
	printf ("  %i cameras\n", scene->mNumCameras);
	printf ("  %i lights\n", scene->mNumLights);
	printf ("  %i materials\n", scene->mNumMaterials);
	printf ("  %i meshes\n", scene->mNumMeshes);
	printf ("  %i textures\n", scene->mNumTextures);
  
  
	load_material(scene);
	assign_vao(scene);

 
	aiReleaseImport (scene);
	printf("mesh loaded\n");
  
	return true;
}

bool Mesh :: load_image_to_texture(const char* file_name, unsigned int& _tex, bool gen_mips)
{
	int n;
	int force_channels = 4;
	int _ix, _iy;

	unsigned char* _img = stbi_load(file_name, &_ix, &_iy, &n, force_channels);

	if(!_img)
	{
		fprintf(stderr, "ERROR: could not load image %s. Check file type and path\n", file_name);
		return false;
	}

	// FLIP UP-SIDE DIDDLY-DOWN
	// make upside-down copy for GL
	unsigned char* imagePtr = &_img[0];
	int half_height_in_pixels = _iy/2;
	int height_in_pixels = _iy;

	//assuming RGBA for 4 components per pixel
	int num_colour_components = 4;

	//assuming each colour component is an unsigned char
	int width_in_chars = _ix * num_colour_components;

	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;

	for(int h=0; h<half_height_in_pixels; h++)
	{
		top = imagePtr + h * width_in_chars;
		bottom = imagePtr + (height_in_pixels - h - 1) * width_in_chars;

		for(int w=0; w<width_in_chars; w++)
		{
			//swap the chars around
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			++top;
			++bottom;
		}
	}

	//copy into an OpenGL texture
	

//	if(!sss)
//	{
		glActiveTexture(GL_TEXTURE0);
//		sss = true;
//	}
//	else
//		glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_2D, _tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _ix, _iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, _img);



	if(gen_mips)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	}
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Mesh :: load_material(const aiScene* scene)
{
	//get materials
	if (scene->mNumMaterials > 0)
	{
		const aiMaterial* a_mat = scene->mMaterials[0];


		int num_textures = a_mat->GetTextureCount(aiTextureType_DIFFUSE);

		aiString tex_filename;
		if(num_textures > 0)
		{
			has_tex = true;
			a_mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex_filename);
			string dir = filename;
			dir = dir.substr(0, dir.find_last_of('/'));
			dir += "/";
			dir += tex_filename.C_Str();
			const char* s_tex = dir.c_str();
			glGenTextures(1, &tex);
			load_image_to_texture(s_tex, tex, true);
			glBindTexture(GL_TEXTURE_2D, tex);
		}
		else
			has_tex = false;
		
		aiColor4D ambient;
		aiGetMaterialColor(a_mat, AI_MATKEY_COLOR_AMBIENT, &ambient);

		
		aiColor4D diffuse;
		aiGetMaterialColor(a_mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);

		aiColor4D specular;
		aiGetMaterialColor(a_mat, AI_MATKEY_COLOR_SPECULAR, &specular);

		float shininess = 0.0f;
		aiGetMaterialFloat(a_mat, AI_MATKEY_SHININESS, &shininess);


		if(!has_tex)
			material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
		else
			material.diffuse = glm::vec3(1.0f);


		material.specular = glm::vec3(specular.r, specular.g, specular.b);
		material.shininess = shininess;

		material.beckmann_m = 0.3f;
		material.refraction = 0.9f;

		material.ambient = (material.diffuse) * (0.1f);

		material.cool = glm::vec3(0.0, 0.0, 1.0);
		material.warm = glm::vec3(1.0, 1.0, 0.4);
	}
}


//loads assimp mesh data into the vao
void Mesh :: assign_vao(const aiScene* scene)
{
	

	 const aiMesh* mesh = scene->mMeshes[0]; 


	 printf("   %i vertices in mesh[%i]\n", mesh->mNumVertices, 0);

	  point_count = mesh->mNumVertices;


	  glGenVertexArrays(1, &vao);
	  glBindVertexArray(vao);

		GLfloat* points = NULL;
		GLfloat* normals = NULL;
		GLfloat* texcoords = NULL;


		GLint* bone_ids_lbs = NULL;
		GLfloat* bone_weights = NULL;

		//get vertex positions
		  if (mesh->HasPositions ())
		  {

			points = (GLfloat*)malloc (point_count * 3 * sizeof (GLfloat));

			for (int i=0; i< point_count; i++)
			{
				const aiVector3D* vp = &(mesh->mVertices[i]);
				points[i*3] = (GLfloat)vp->x;
				points[i*3+1] = (GLfloat)vp->y;
				points[i*3+2] = (GLfloat)vp->z;
			}


		  }

		  //get normals
		  if (mesh->HasNormals ()) {
			normals = (GLfloat*)malloc(point_count * 3 * sizeof (GLfloat));

			for (int i=0; i<point_count; i++)
			{
				const aiVector3D* vn = &(mesh->mNormals[i]);
				normals[i*3] = (GLfloat)vn->x;
				normals[i*3+1] = (GLfloat)vn->y;
				normals[i*3+2] = (GLfloat)vn->z;
			}
		  }

		  //get texture coordinates
		  if (mesh->HasTextureCoords (0)) {
			texcoords = (GLfloat*)malloc(point_count * 2 * sizeof (GLfloat));

			for (int i=0; i<point_count; i++)
			{
				const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
				texcoords[i*2] = (GLfloat)vt->x;
				texcoords[i*2+1] = (GLfloat)vt->y;
		
			}
		  }




	
		  // copy mesh data in VBOs


		  //load in vertex positions
		  if(mesh->HasPositions())
		  {
			  GLuint vbo;
			  glGenBuffers (1, &vbo);
			  glBindBuffer(GL_ARRAY_BUFFER, vbo);
			  glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), points, GL_STATIC_DRAW);

			  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			  glEnableVertexAttribArray(0);
			  free (points);
		  }

		  //load in normals
		  if(mesh->HasNormals())
		  {
			  GLuint vbo;
			  glGenBuffers (1, &vbo);
			  glBindBuffer(GL_ARRAY_BUFFER, vbo);
			  glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);

			  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			  glEnableVertexAttribArray(1);
			  free (normals);
		  }

		  //load in texture coordinates
		  if(mesh->HasTextureCoords(0))
		  {
			  GLuint vbo;
			  glGenBuffers (1, &vbo);
			  glBindBuffer(GL_ARRAY_BUFFER, vbo);
			  glBufferData(GL_ARRAY_BUFFER, 2 * point_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);

			  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			  glEnableVertexAttribArray(2);
			  free (texcoords);
		  }


}

void Mesh :: draw(GLuint spID, glm::mat4 model_mat)
{
	glBindTexture(GL_TEXTURE_2D, tex);


	int ka_location = glGetUniformLocation (spID, "ka");
	int kd_location = glGetUniformLocation (spID, "kd");
	int ks_location = glGetUniformLocation (spID, "ks");
	int shine_location = glGetUniformLocation (spID, "spec_ex");
	int beckmann_shine_location = glGetUniformLocation (spID, "beckmann_m");
	int refraction_location = glGetUniformLocation (spID, "refraction");
	int kcool_location = glGetUniformLocation (spID, "kcool");
	int kwarm_location = glGetUniformLocation (spID, "kwarm");
	int texbool_location = glGetUniformLocation (spID, "has_texture");

	glUniform3fv(ka_location, 1, glm::value_ptr(material.ambient));
	glUniform3fv(kd_location, 1, glm::value_ptr(material.diffuse));
	glUniform3fv(ks_location, 1, glm::value_ptr(material.specular));
	glUniform1fv(shine_location, 1, &material.shininess);
	glUniform1fv(beckmann_shine_location, 1, &material.beckmann_m);
	glUniform1fv(refraction_location, 1, &material.refraction);
	glUniform3fv(kcool_location, 1, glm::value_ptr(material.cool));
	glUniform3fv(kwarm_location, 1, glm::value_ptr(material.warm));

	int int_bool;
	int_bool = has_tex;

	glUniform1i(texbool_location, int_bool);

	int matrix_location = glGetUniformLocation (spID, "model");
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, point_count);
}

std::vector<glm::vec3> Mesh :: getVertices()
{
	const aiScene* scene = aiImportFile (filename, aiProcess_Triangulate);

	const aiMesh* mesh = scene->mMeshes[0]; 

	int vertex_count = mesh->mNumVertices;
	std::vector<glm::vec3> v (vertex_count);

	//get vertex positions
	if (mesh->HasPositions ())
	{

		for (int i=0; i< vertex_count; i++)
		{
			const aiVector3D* vp = &(mesh->mVertices[i]);
			v[i].x = (GLfloat)vp->x;
			v[i].y = (GLfloat)vp->y;
			v[i].z = (GLfloat)vp->z;
		}
	}
	return v;
}


void Mesh :: addTBar(TwBar *bar)
{
	TwAddVarRW(bar, vao+"    Ambient", TW_TYPE_COLOR3F, &material.ambient, "");
	TwAddVarRW(bar, vao+"    Diffuse", TW_TYPE_COLOR3F, &material.diffuse, "");
	TwAddVarRW(bar, vao+"    Speculular", TW_TYPE_COLOR3F, &material.specular, "");
	TwAddVarRW(bar, vao+"    Shininess", TW_TYPE_FLOAT, &material.shininess, "");
	TwAddVarRW(bar, vao+"    Roughness", TW_TYPE_FLOAT, &material.beckmann_m, " step = 0.05 ");
	TwAddVarRW(bar, vao+"    Refraction", TW_TYPE_FLOAT, &material.refraction, " step = 0.05 ");
	TwAddVarRW(bar, vao+"    Cool", TW_TYPE_COLOR3F, &material.cool, "");
	TwAddVarRW(bar, vao+"    Warm", TW_TYPE_COLOR3F, &material.warm, "");
}
