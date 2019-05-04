#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "hw.h"
#include "camera.h"

#include <iostream>
#include <algorithm>

Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));

unsigned int load_hw5()
{
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"out vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
		"   ourColor = aColor;\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(ourColor, 1.0f);\n"
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static bool firstMouse = true;
	static float lastX = SCR_WIDTH / 2.0f;
	static float lastY = SCR_HEIGHT / 2.0f;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void render_hw5(unsigned int shaderProgram, GLFWwindow *window)
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	static float vertices[] = {
		//上
		 2.0f,  2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		-2.0f,  2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		 2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		-2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		//下
		 2.0f,  2.0f,-2.0f, 0.0f, 1.0f, 0.0f,
		-2.0f,  2.0f,-2.0f, 0.0f, 1.0f, 0.0f,
		 2.0f, -2.0f,-2.0f, 0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f,-2.0f, 0.0f, 1.0f, 0.0f,
		//左
		-2.0f,  2.0f, 2.0f, 0.0f, 0.0f, 1.0f,
		-2.0f,  2.0f,-2.0f, 0.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, 2.0f, 0.0f, 0.0f, 1.0f,
		-2.0f, -2.0f,-2.0f, 0.0f, 0.0f, 1.0f,
		//右
		 2.0f,  2.0f, 2.0f, 0.0f, 1.0f, 1.0f,
		 2.0f,  2.0f,-2.0f, 0.0f, 1.0f, 1.0f,
		 2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 1.0f,
		 2.0f, -2.0f,-2.0f, 0.0f, 1.0f, 1.0f,
		 //前
		 2.0f,  2.0f, 2.0f, 1.0f, 0.0f, 1.0f,
		 2.0f,  2.0f,-2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f,  2.0f, 2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f,  2.0f,-2.0f, 1.0f, 0.0f, 1.0f,
		//后
		 2.0f, -2.0f, 2.0f, 1.0f, 1.0f, 0.0f,
		 2.0f, -2.0f,-2.0f, 1.0f, 1.0f, 0.0f,
		-2.0f, -2.0f, 2.0f, 1.0f, 1.0f, 0.0f,
		-2.0f, -2.0f,-2.0f, 1.0f, 1.0f, 0.0f
	};

	static unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3,

		4, 5, 6,
		5, 6, 7,

		8, 9, 10,
		9, 10,11,

		12, 13, 14,
		13, 14, 15,

		16, 17, 18,
		17, 18, 19,

		20, 21, 22,
		21, 22, 23
	};

	static int pro;
	static float left = -10, right = 10, top = 10, bottom = -10, znear = 10, zfar = -10;
	static float fovy = 45, aspect = 1, pnear = 0.1, pfar = 100;

	glm::mat4 model(1.0f);
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);
	float radius = 10.0f;
	float time = (float)glfwGetTime();

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// render loop
	// -----------
	{
		ImGui::Begin("Orthographic Transformation");

		ImGui::RadioButton("ortho", &pro, 0);
		ImGui::SameLine();
		ImGui::RadioButton("perspective", &pro, 1);
		ImGui::SameLine();
		ImGui::RadioButton("camera", &pro, 2);
		ImGui::SameLine();
		ImGui::RadioButton("bonus", &pro, 3);
		switch (pro)
		{
		case 0:
			ImGui::SliderFloat("left", &left, -10, 10);
			ImGui::SliderFloat("right", &right, -10, 10);
			ImGui::SliderFloat("top", &top, -10, 10);
			ImGui::SliderFloat("botton", &bottom, -10, 10);
			ImGui::SliderFloat("near", &znear, -10, 10);
			ImGui::SliderFloat("far", &zfar, -10, 10);
			break;
		case 1:
			ImGui::SliderFloat("fovy", &fovy, 0, 90);
			ImGui::SliderFloat("aspect", &aspect, 0, 10);
			ImGui::SliderFloat("near", &pnear, 0, 100);
			ImGui::SliderFloat("far", &pfar, 0, 100);
			break;
		case 2:
			break;
		case 3:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(window, mouse_callback);
			break;
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	float deltaTime = 1.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
	//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));

	switch (pro)
	{
	case 0:
		projection = glm::ortho(left, right, bottom, top, znear, zfar);
		break;
	case 1:
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
		projection = glm::perspective(glm::radians(fovy), aspect, pnear, pfar);
		break;
	case 2:
		view = glm::lookAt(glm::vec3(sin(time) * radius, 0.0, cos(time) * radius), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		break;
	case 3:
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);
		break;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	//EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//uniform mat4
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
