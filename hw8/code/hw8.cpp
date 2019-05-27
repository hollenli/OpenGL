#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "hw.h"
#include "shader.h"

#include <iostream>
#include <algorithm>
#include <deque>

static std::deque<std::pair<float, float> > controlVec;
static bool flush = false;

int factorial(int x) {
	int ans = 1;
	for (size_t i = 1; i <= x; i++) {
		ans *= i;
	}
	return ans;
}

void draw(float *vertices, float radius, int sizeOfVec) {
	for (size_t i = 0; i < sizeOfVec; i++)
	{
		float vec[8];
		// 右上
		vec[0] = vertices[2 * i] + radius;
		vec[1] = vertices[2 * i + 1] + radius;
		// 右下
		vec[2] = vertices[2 * i] + radius;
		vec[3] = vertices[2 * i + 1] - radius;
		// 左上
		vec[4] = vertices[2 * i] - radius;
		vec[5] = vertices[2 * i + 1] + radius;
		// 左下
		vec[6] = vertices[2 * i] - radius;
		vec[7] = vertices[2 * i + 1] - radius;
		unsigned int indices[6] = {
			0, 1, 2,
			1, 2, 3
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec), vec, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		if (i != 0)
		{
			float lineVec[4];
			// 前点
			lineVec[0] = vertices[2 * (i - 1)];
			lineVec[1] = vertices[2 * (i - 1) + 1];
			// 当前点
			lineVec[2] = vertices[2 * i];
			lineVec[3] = vertices[2 * i + 1];
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVec), lineVec, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}
}

void render_hw8()
{
	//shader code
	static const char *shader_vs = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
		"}\0";

	static const char *shader_fs = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
		"}\n\0";
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	static float curveVec[2000];
	static float *sideVec;
	static Shader shader(shader_vs, shader_fs);

	float time = (float)glfwGetTime() / 5;
	float t = time - floor(time);
	float radius = 0.01;
	int sizeOfControlVec = controlVec.size();
	int curveSize = 1000;

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader.ID);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	if (flush)
	{
		for (size_t i = 0; i < curveSize; i++)
		{
			float tempt = (float)i / (float)curveSize;
			float x = 0, y = 0;
			for (size_t j = 0; j < sizeOfControlVec; j++)
			{
				int n = sizeOfControlVec - 1;
				float proportion = factorial(n) / (factorial(j)*factorial(n - j))*pow(tempt, j)*pow(1 - tempt, n - j);
				x += controlVec[j].first*proportion;
				y += controlVec[j].second*proportion;
			}
			curveVec[2 * i] = x;
			curveVec[2 * i + 1] = y;
		}
		flush = false;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(curveVec), curveVec, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_POINTS, 0, curveSize);

	for (size_t i = 0; i < sizeOfControlVec; i++)
	{
		float *temp = new float[2 * (sizeOfControlVec - i)];
		for (size_t j = 0; j < sizeOfControlVec - i; j++)
		{
			if (i == 0)
			{
				temp[2 * j] = controlVec[j].first;
				temp[2 * j + 1] = controlVec[j].second;
			}
			else
			{
				temp[2 * j] = (1 - t)*sideVec[2 * j] + t * sideVec[2 * (j + 1)];
				temp[2 * j + 1] = (1 - t)*sideVec[2 * j + 1] + t * sideVec[2 * (j + 1) + 1];
			}
		}
		draw(temp, radius, sizeOfControlVec - i);
		delete[] sideVec;
		sideVec = temp;
	}

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//delete
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
{
	// mouse button
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		controlVec.push_back(std::pair<float, float>(2 * x / (double)SCR_WIDTH - 1, 1 - 2 * y / (double)SCR_HEIGHT));
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		controlVec.pop_back();
	}
	flush = true;
}