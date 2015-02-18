#include "MultiToonShaders.h"

using namespace std;

MultiToonShaders :: MultiToonShaders()
{
	num_shaders = 0;
	current_shader_num = 0;

}

MultiToonShaders :: MultiToonShaders(std::vector<Shaders> shader_array)
{
	num_shaders = shader_array.size();
	shaders = shader_array;
}



void MultiToonShaders :: addShader(Shaders shader)
{
	num_shaders++;
	shaders.push_back(shader);
	
	
	current_shader = shaders[current_shader_num];
}
	

void MultiToonShaders :: addShader(string name, const char* vertex, const char* fragment)
{
	Shaders shader(name, vertex, fragment);

	addShader(shader);
}



void MultiToonShaders :: compileShaders()
{
	for(int i=0; i<num_shaders; i++)
		shaders[i].CompileShaders();

	current_shader = shaders[current_shader_num];
}

	
void MultiToonShaders :: nextShader()
{
	current_shader_num++;

	if(current_shader_num >= num_shaders)
		current_shader_num = 0;

	current_shader = shaders[current_shader_num];

}




