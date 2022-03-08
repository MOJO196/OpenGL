#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <sstream>

#include "GLShader.hpp"
#include "Main.h"


const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 f_color;\n"
"void main()\n"
"{\n"
"	f_color = aColor;\n"
"   gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 f_color;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(f_color, 1.0f);\n"
"}\0";

int main()
{
	//GLuint program = LoadShader("shader/vertexShaderSource.glsl", "shader/fragmentShaderSource.glsl");
	//glUseProgram(program);

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//using the new core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat vertices[] =
	{
		//-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
		// 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
		// 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right

		// 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
		//-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left
		//-0.5f,  0.5f, 1.0f, 0.0f, 0.0f  // Top-left

		-0.5f, -0.5f * float(sqrt(3)) * 1 / 3,     0.8f, 0.3f,  0.02f, // Lower left corner
		 0.5f, -0.5f * float(sqrt(3)) * 1 / 3,     0.8f, 0.3f,  0.02f, // Lower right corner
		 0.0f,  0.5f * float(sqrt(3)) * 2 / 3,     1.0f, 0.6f,  0.32f, // Upper corner
		-0.25f, 0.5f * float(sqrt(3)) * 1 / 6,     0.9f, 0.45f, 0.17f, // Inner left
		 0.25f, 0.5f * float(sqrt(3)) * 1 / 6,     0.9f, 0.45f, 0.17f, // Inner right
		 0.0f, -0.5f * float(sqrt(3)) * 1 / 3,     0.0f, 0.0f,  0.0f  // Inner down
	};

	//define window properties
	GLFWwindow* window = glfwCreateWindow(1024, 1024, "Test", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create a window!\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	glDebugMessageCallback(openGLDebugCallback, 0);

	//declare the viewport of the window
	glViewport(0, 0, 1024, 1024);


	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);

	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);


		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
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
