// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Shader library
#include <common/shader.hpp>

GLfloat randd() {
	return (GLfloat)rand() / ((float)RAND_MAX + 1);
}

class SnowFlake
{
public:
	std::vector<glm::vec3> flake_buffer_data;
	glm::vec3 center;
	double runtime;
	glm::vec3 color;

	SnowFlake(glm::vec3 inputCenter)
	{
		flake_buffer_data = std::vector<glm::vec3>();
		center = inputCenter;
		runtime = 0.0;
		color = glm::vec3(randd(), randd(), randd());
	}
};

#define BUFFER_OFFSET( offset ) ((GLvoid*) (offset))

GLFWwindow* window;
int windowWidth, windowHeight;

GLuint programID;
GLuint snowFlakeProgramID;
GLuint VAID;
GLuint VBID;

std::vector<glm::vec3> g_vertex_buffer_data;
std::vector<SnowFlake> sf_buffer_data;

glm::mat4 Projection;
glm::mat4 View;
float degree = 0.0f;
double oldtime = 0.0;
double elapsedtime = 0.0;
bool dir;

void koch_line(SnowFlake *snowFlake, glm::vec3 a, glm::vec3 b, int iter)
{
	glm::vec3 new_a = glm::vec3(a.x + (b.x - a.x) / 3, a.y + (b.y - a.y) / 3, 0);
	glm::vec3 new_b = glm::vec3(a.x +  2 * (b.x - a.x) / 3, a.y +  2 * (b.y - a.y) / 3, 0);

	glm::vec3 new_c = new_b - new_a;
	
	glm::mat3 rotate = glm::mat3(0.5f, -sqrt(0.75f), 0, sqrt(0.75f), 0.5f, 0, 0, 0, 1);
	new_c = rotate * new_c;
	new_c = new_a + new_c;

	snowFlake->flake_buffer_data.push_back(new_a);
	snowFlake->flake_buffer_data.push_back(new_b);
	snowFlake->flake_buffer_data.push_back(new_c);
	
	if (iter > 0)
	{
		koch_line(snowFlake, a, new_a, iter - 1);
		koch_line(snowFlake, new_a, new_c, iter - 1);
		koch_line(snowFlake, new_c, new_b, iter - 1);
		koch_line(snowFlake, new_b, b, iter - 1);
	}
}

void make_snowflake(glm::vec3 origin, float radius, int iter)
{
	SnowFlake snowFlake = SnowFlake(origin);
	glm::vec3 a = glm::vec3(origin.x - radius * sqrt(0.75f), origin.y - radius * 0.5f, 0);
	glm::vec3 b = glm::vec3(origin.x, origin.y + radius, 0);
	glm::vec3 c = glm::vec3(origin.x + radius * sqrt(0.75f), origin.y - radius * 0.5f, 0);

	snowFlake.flake_buffer_data.push_back(a);
	snowFlake.flake_buffer_data.push_back(b);
	snowFlake.flake_buffer_data.push_back(c);

	koch_line(&snowFlake, a, c, iter);
	koch_line(&snowFlake, b, a, iter);
	koch_line(&snowFlake, c, b, iter);

	sf_buffer_data.push_back(snowFlake);
}

// TODO: Initialize model
void init_model(void)
{
	g_vertex_buffer_data = std::vector<glm::vec3>();

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if(i % 2 == 0)
				make_snowflake(glm::vec3(-1.4f + j * 0.5f, 1.8f - i * 0.4f, 0), 0.2f, 3);
			else
				make_snowflake(glm::vec3(-1.2f + j * 0.5f, 1.8f - i * 0.4f, 0), 0.2f, 3);
		}
	}
	// Generates Vertex Array Objects in the GPU¡¯s memory and passes back their identifiers
	// Create a vertex array object that represents vertex attributes stored in a vertex buffer object.
	glGenVertexArrays(1, &VAID);
	glBindVertexArray(VAID);
	// Create and initialize a buffer object, Generates our buffers in the GPU¡¯s memory
	glGenBuffers(1, &VBID);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
}

void draw_snowFlake(double deltatime)
{
	if (!dir)
		elapsedtime += deltatime;
	else
		elapsedtime -= deltatime;

	if (elapsedtime > 8.0)
	{
		degree = 0;
		elapsedtime = 8.0;
		dir = true;

	}
	else if (elapsedtime < 0.0)
	{
		degree = 0;
		elapsedtime = 0.0;
		dir = false;
	}

	for (int i = 0; i < sf_buffer_data.size(); i++)
	{
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 origin_move = glm::translate(-sf_buffer_data[i].center);
		glm::mat4 translate_snow = glm::translate(glm::vec3(0, -elapsedtime * 0.15f, 0));
		glm::mat4 R;
		if(!dir)
			R = glm::rotate(degree, glm::vec3(0, 0, 1));
		else
			R = glm::rotate(-degree, glm::vec3(0, 0, 1));
		glm::mat4 reset_pos = glm::translate(sf_buffer_data[i].center);
		glm::mat4 MVP = Projection * View * Model * translate_snow * reset_pos * R * origin_move;

		glBindVertexArray(VAID);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*sf_buffer_data[i].flake_buffer_data.size(),
			&sf_buffer_data[i].flake_buffer_data[0], GL_STATIC_DRAW);
		GLuint MatrixID = glGetUniformLocation(snowFlakeProgramID, "MVP");
		GLuint ColorVectorID = glGetUniformLocation(snowFlakeProgramID, "colorIn");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		//printf("%f,%f,%f\n", color.x,color.y,color.z);
		glUniform3f(ColorVectorID, sf_buffer_data[i].color.x, sf_buffer_data[i].color.y, sf_buffer_data[i].color.z);
		glDrawArrays(GL_TRIANGLES, 0, sf_buffer_data[i].flake_buffer_data.size());
		glDisableVertexAttribArray(0);
	}

	degree += 2;
}

// TODO: Draw model
void draw_model()
{
	glUseProgram(programID);
	glUseProgram(snowFlakeProgramID);
	double deltatime = glfwGetTime() - oldtime;
	oldtime = glfwGetTime();
	draw_snowFlake(deltatime);
}

int main(int argc, char* argv[])
{
	// Step 1: Initialization
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(1024, 768, "Lab 1", NULL, NULL);
	if (window == NULL)
	{
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		return -1;
	}

	dir = false;

	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(glm::vec3(0, 0, 2),
				 				 glm::vec3(0, 0, 0),
								 glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glClearColor(0.39f,0.58f,0.93f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	programID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");
	snowFlakeProgramID = LoadShaders("SnowFlakeVertexShader.glsl", "SnowFlakeFragmentShader.glsl");
	GLuint MatrixID = glGetUniformLocation(snowFlakeProgramID, "MVP");

	init_model();

	// Step 2: Main event loop
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_model();

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	// Step 3: Termination
	g_vertex_buffer_data.clear();

	glDeleteBuffers(1, &VBID);
	glDeleteProgram(programID);
	glDeleteProgram(snowFlakeProgramID);
	glDeleteVertexArrays(1, &VAID);

	glfwTerminate();

	return 0;
}
