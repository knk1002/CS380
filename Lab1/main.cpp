// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

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

#define BUFFER_OFFSET( offset ) ((GLvoid*) (offset))

GLFWwindow* window;
int windowWidth, windowHeight;

GLuint programID;
GLuint snowFlakeProgramID;
GLuint VAID;
GLuint VBID;

std::vector<glm::vec3> g_vertex_buffer_data;

glm::mat4 Projection;
glm::mat4 View;
float degree = 0.0f;

// TODO: Implement koch snowflake
void koch_line(glm::vec3 a, glm::vec3 b, int iter)
{
	glm::vec3 new_a = glm::vec3(a.x + (b.x - a.x) / 3, a.y + (b.y - a.y) / 3, 0);
	glm::vec3 new_b = glm::vec3(a.x +  2 * (b.x - a.x) / 3, a.y +  2 * (b.y - a.y) / 3, 0);

	glm::vec3 new_c = new_b - new_a;
	
	glm::mat3 rotate = glm::mat3(0.5f, -sqrt(0.75f), 0, sqrt(0.75f), 0.5f, 0, 0, 0, 1);
	new_c = rotate * new_c;
	new_c = new_a + new_c;

	/*printf("%lf,%lf,%lf\n", new_a.x,new_a.y,new_a.z);
	printf("%lf,%lf,%lf\n", new_b.x, new_b.y, new_b.z);
	printf("%lf,%lf,%lf\n", new_c.x, new_c.y, new_c.z);*/

	g_vertex_buffer_data.push_back(new_a);
	g_vertex_buffer_data.push_back(new_b);
	g_vertex_buffer_data.push_back(new_c);
	
	if (iter > 0)
	{
		koch_line(a, new_a, iter - 1);
		koch_line(new_a, new_c, iter - 1);
		koch_line(new_c, new_b, iter - 1);
		koch_line(new_b, b, iter - 1);
	}
}

void make_snowflake(glm::vec3 origin, float radius, int iter)
{
	glm::vec3 a = glm::vec3(origin.x - radius * sqrt(0.75f), origin.y - radius * 0.5f, 0);
	glm::vec3 b = glm::vec3(origin.x, origin.y + radius, 0);
	glm::vec3 c = glm::vec3(origin.x + radius * sqrt(0.75f), origin.y - radius * 0.5f, 0);

	g_vertex_buffer_data.push_back(a);
	g_vertex_buffer_data.push_back(b);
	g_vertex_buffer_data.push_back(c);

	koch_line(a, c, iter);
	koch_line(b, a, iter);
	koch_line(c, b, iter);
}

// TODO: Initialize model
void init_model(void)
{
	g_vertex_buffer_data = std::vector<glm::vec3>();
	
	make_snowflake(glm::vec3(-0.5f,-0.5f,0), 0.2f, 3);
	make_snowflake(glm::vec3(0, 0, 0), 0.2f, 3);
	// Generates Vertex Array Objects in the GPU¡¯s memory and passes back their identifiers
	// Create a vertex array object that represents vertex attributes stored in a vertex buffer object.
		glGenVertexArrays(1, &VAID);
	glBindVertexArray(VAID);
	// Create and initialize a buffer object, Generates our buffers in the GPU¡¯s memory
	glGenBuffers(1, &VBID);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*g_vertex_buffer_data.size(),
		&g_vertex_buffer_data[0], GL_STATIC_DRAW);
}

// TODO: Draw model
void draw_model()
{
	glUseProgram(programID);
	glBindVertexArray(VAID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 R = glm::rotate(degree, glm::vec3(0, 0, 1));
	glm::mat4 MVP = Projection * View * Model * R;
	degree += 2;
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, g_vertex_buffer_data.size());
	glDisableVertexAttribArray(0);
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

	// TODO: GLFW create window and context
	window = glfwCreateWindow(1024, 768, "Lab 1", NULL, NULL);
	if (window == NULL)
	{
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// END

	// TODO: Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		return -1;
	}

	// END

	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(glm::vec3(0, 0, 2),
				 				 glm::vec3(0, 0, 0),
								 glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	// TODO: Initialize OpenGL and GLSL
	glClearColor(0.39f,0.58f,0.93f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	programID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");
	snowFlakeProgramID = LoadShaders("SnowFlakeVertexShader.glsl", "SnowFlakeFragmentShader.glsl");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// END
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
	glDeleteVertexArrays(1, &VAID);

	glfwTerminate();

	return 0;
}
