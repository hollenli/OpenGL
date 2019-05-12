#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "hw.h"

#include <iostream>
#include <algorithm>

unsigned int load_hw3()
{
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
		"}\n\0";

	int success;
	char infoLog[512];
	//create and compile vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//create and compile fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

float normalize(int i)
{
	return (float)i / 200;
}

int bresenham_line(float vertices[], int v0[2], int v1[2], int size)
{
	int dx = v1[0] - v0[0], dy = v1[1] - v0[1];
	vertices[size++] = normalize(v0[0]);
	vertices[size++] = normalize(v0[1]);
	
	if (dx == 0)
	{
		for (size_t i = 1; i < abs(dy); i++)
		{
			vertices[size++] = normalize(v0[0]);
			vertices[size++] = normalize(v0[1] + ((dy) / abs(dy)) * i);
		}
	}
	else 
	{
		int len = abs(dx);
		float dtx = 1, dty = abs((float)dy / (float)dx);
		if (dty > 1)
		{
			len = abs(dy);
			dty = 1;
			dtx = abs((float)dx / (float)dy);
			float pi = 2 * dtx - dty;
			for (size_t i = 1; i < len; i++)
			{
				if (pi <= 0)
				{
					vertices[size++] = vertices[size - 2];
					pi += 2 * dtx;
				}
				else
				{
					vertices[size++] = vertices[size - 2] + normalize(dx / abs(dx));
					pi += 2 * dtx - 2 * dty;
				}
				vertices[size++] = normalize(v0[1] + (dy / abs(dy)) * i);
			}
		}
		else
		{
			float pi = 2 * dty - dtx;
			for (size_t i = 1; i < len; i++)
			{
				vertices[size++] = normalize(v0[0] + (dx / abs(dx)) * i);
				if (pi <= 0)
				{
					vertices[size++] = vertices[size - 2];
					pi += 2 * dty;
				}
				else
				{
					vertices[size++] = vertices[size - 2] + normalize(dy / abs(dy));
					pi += 2 * dty - 2 * dtx;
				}
			}
		}
	}
	return size;
}

int bresenham_circle(float vertices[], int center[2], int radius ,int size)
{
	int i = 0, r = radius, d = 3 - 2 * radius;
	for (; i <= r; i++)
	{
		vertices[size++] = normalize(center[0] + i);
		vertices[size++] = normalize(center[1] + r);

		vertices[size++] = normalize(center[0] - i);
		vertices[size++] = normalize(center[1] - r);

		vertices[size++] = normalize(center[0] + i);
		vertices[size++] = normalize(center[1] - r);

		vertices[size++] = normalize(center[0] - i);
		vertices[size++] = normalize(center[1] + r);

		vertices[size++] = normalize(center[0] + r);
		vertices[size++] = normalize(center[1] + i);

		vertices[size++] = normalize(center[0] - r);
		vertices[size++] = normalize(center[1] - i);

		vertices[size++] = normalize(center[0] + r);
		vertices[size++] = normalize(center[1] - i);

		vertices[size++] = normalize(center[0] - r);
		vertices[size++] = normalize(center[1] + i);

		if (d < 0)
		{
			d += 4 * i + 6;
		}
		else
		{
			d += 4 * (i - r) + 10;
			r--;
		}
	}
	return size;
}

int* lineEquation(int para[3], int v0[2], int v1[2], int vout[2])
{
	para[0] = v1[1] - v0[1];
	para[1] = v0[0] - v1[0];
	para[2] = v1[0] * v0[1] - v0[0] * v1[1];
	if (para[0] * vout[0] + para[1] * vout[1] + para[2] < 0)
	{
		para[0] = -para[0];
		para[1] = -para[1];
		para[2] = -para[2];
	}
	return para;
}

int rasterize_triangle(float vertices[], int v[3][2], int size)
{
	int maxX = std::max(v[0][0], std::max(v[1][0], v[2][0]));
	int minX = std::min(v[0][0], std::min(v[1][0], v[2][0]));
	int maxY = std::max(v[0][1], std::max(v[1][1], v[2][1]));
	int minY = std::min(v[0][1], std::min(v[1][1], v[2][1]));
	int equation[3][3];
	lineEquation(equation[0], v[0], v[1], v[2]);
	lineEquation(equation[1], v[1], v[2], v[0]);
	lineEquation(equation[2], v[2], v[0], v[1]);
	for (int i = minX + 1; i < maxX; i++)
	{
		for (int j = minY + 1; j < maxY; j++)
		{
			for (size_t k = 0; k < 3; k++)
			{
				if (equation[k][0] * i + equation[k][1] * j + equation[k][2] < 0)
				{
					break;
				}
				if (k == 2)
				{
					vertices[size++] = normalize(i);
					vertices[size++] = normalize(j);
				}
			}
		}
	}

	return size;
}

void render_hw3(unsigned int shaderProgram)
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	static float vertices[2 * 40000];
	static ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

	static bool isPad = false;
	static int radius = 0;
	static int center[2] = { 0 };
	static int v[3][2] = { 0 };

	int size = 0;

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// render loop
	// -----------
	{
		ImGui::Begin("setting");
		ImGui::ColorEdit3("choose color", (float*)&color, 1);

		ImGui::SliderInt2("v0:x0,y0", v[0], -99, 99);
		ImGui::SliderInt2("v1:x1,y1", v[1], -99, 99);
		ImGui::SliderInt2("v2:x2,y2", v[2], -99, 99);

		ImGui::SliderInt("radius", &radius, 0, 99);

		ImGui::Checkbox("pad", &isPad);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	size = bresenham_line(vertices, v[0], v[1], size);
	size = bresenham_line(vertices, v[1], v[2], size);
	size = bresenham_line(vertices, v[2], v[0], size);
	size = bresenham_circle(vertices, center, radius, size);
	if (isPad)
	{
		size = rasterize_triangle(vertices, v, size);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_DYNAMIC_DRAW);

	//position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, size / 2);

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
