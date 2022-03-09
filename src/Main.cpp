#include "GLShader.hpp"
#include "Main.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <sstream>
#include <chrono>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(512, 512, "OpenGL Test", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create a window!\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	glDebugMessageCallback(openGLDebugCallback, 0);

	glViewport(0, 0, 512, 512);

	GLuint program = LoadShader("src/shader/vertexShaderSource.glsl", "src/shader/fragmentShaderSource.glsl");
	glUseProgram(program);

	GLfloat vertices[] =
	{
		//-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
		// 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
		// 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right

		// 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
		//-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left
		//-0.5f,  0.5f, 1.0f, 0.0f, 0.0f  // Top-left

		-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.0f, 0.0f, 0.0f, // Lower left corner
		 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.0f, 0.0f, 0.0f, // Lower right corner
		 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 1.0f, 1.0f, // Upper corner
		-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.0f, 0.0f, 1.0f, // Inner left
		 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.0f, 1.0f, 0.0f, // Inner right
		 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     1.0f, 0.0f, 0.0f, // Inner down
	};

	GLuint indices[] =
	{
		0, 3, 5, // Lower left triangle
		3, 2, 4, // Lower right triangle
		5, 4, 1, // Upper triangle
		3, 4, 5, // Mid triangle
	};

	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float timeElapsed = 0;
	std::vector<int> fps;
	
	auto time = std::chrono::high_resolution_clock::now();
	int fpsCount = 0;

	while (!glfwWindowShouldClose(window))
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - time).count();
		time = newTime;

		timeElapsed += frameTime;

		if (timeElapsed >= 1.0f)
		{
			std::cout << fpsCount << std::endl;
			fps.push_back(fpsCount);
			fpsCount = 0;
			timeElapsed -= 1;
		}

		fpsCount++;

		glClearColor(timeElapsed, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	getFPS(fps);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(program);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void getFPS(std::vector<int> fps)
{
	int low = 1000000;
	int high = 0;
	int avg = 0;

	for (int i = 0; i < fps.size(); i++)
	{
		if (low > fps[i]) low = fps[i];
		else if (high < fps[i]) high = fps[i];

		avg += fps[i];
	}

	std::cout << "\n\n-----Result-----\n";
	std::cout << "LOW  : " << low << "\n";
	std::cout << "HIGH : " << high << "\n";
	std::cout << "AVG  : " << float(avg / fps.size()) << "\n";
}

static void openGLDebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam) {

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		// OpenGL_LOG_INFO(message);
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW" << message << "\n";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM" << message << "\n";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH" << message << "\n";
		break;
	default:
		break;
	}
}

unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::stringstream ss;
		ss << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!";
		ss << message;
		std::cout << ss.str().c_str() << "\n";
		glDeleteShader(id);
		return 0;
	}

	return id;
}

