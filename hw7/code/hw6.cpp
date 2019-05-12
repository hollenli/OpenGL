#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "hw.h"
#include "shader.h"
#include "camera.h"

#include <iostream>
#include <algorithm>

void render_hw6()
{
	//phong shader code
	const char *phong_vs = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"layout (location = 2) in vec3 aNormal;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"out vec3 ourColor;\n"
		"out vec3 ourFragPos;\n"
		"out vec3 ourNormal;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
		"   ourColor = aColor;\n"
		"   ourFragPos = vec3(model * vec4(aPos, 1.0f));\n"
		"   ourNormal = mat3(transpose(inverse(model))) * aNormal;\n"
		"}\0";

	const char *phong_fs = "#version 330 core\n"
		"struct Material {\n"
		"	/*\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"	*/\n"
		"	float shininess;\n"
		"	};\n"
		"struct Light {\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"	vec3 position;\n"
		"	};\n"
		"in vec3 ourColor;\n"
		"in vec3 ourFragPos;\n"
		"in vec3 ourNormal;\n"
		"uniform Material material;\n"
		"uniform Light light;\n"
		"uniform vec3 viewPos;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	//ambient\n"
		"	vec3 ambient = light.ambient;\n"
		"	//diffuse\n"
		"	vec3 norm = normalize(ourNormal);\n"
		"	vec3 lightDir = normalize(light.position - ourFragPos);\n"
		"	float diff = max(dot(norm, lightDir), 0.0f);\n"
		"	vec3 diffuse = diff * light.diffuse;\n"
		"	//specular\n"
		"	vec3 viewDir = normalize(viewPos - ourFragPos);\n"
		"	vec3 reflectDir = reflect(-lightDir, norm);\n"
		"	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);\n"
		"	vec3 specular = spec * light.specular;\n"
		"	//result\n"
		"	vec3 result = (ambient + diffuse + specular) * ourColor;\n"
		"   FragColor = vec4(result, 1.0f);\n"
		"}\n\0";
	//gouraud shader code
	static const char *gouraud_vs = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"layout (location = 2) in vec3 aNormal;\n"
		"struct Material {\n"
		"	/*\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"	*/\n"
		"	float shininess;\n"
		"	};\n"
		"struct Light {\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"	vec3 position;\n"
		"	};\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"uniform Material material;\n"
		"uniform Light light;\n"
		"uniform vec3 viewPos;\n"
		"out vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
		"   vec3 ourFragPos = vec3(model * vec4(aPos, 1.0f));\n"
		"   vec3 ourNormal = mat3(transpose(inverse(model))) * aNormal;\n"
		"	//ambient\n"
		"	vec3 ambient = light.ambient;\n"
		"	//diffuse\n"
		"	vec3 norm = normalize(ourNormal);\n"
		"	vec3 lightDir = normalize(light.position - ourFragPos);\n"
		"	float diff = max(dot(norm, lightDir), 0.0f);\n"
		"	vec3 diffuse = diff * light.diffuse;\n"
		"	//specular\n"
		"	vec3 viewDir = normalize(viewPos - ourFragPos);\n"
		"	vec3 reflectDir = reflect(-lightDir, norm);\n"
		"	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);\n"
		"	vec3 specular = spec * light.specular;\n"
		"	//result\n"
		"	ourColor = (ambient + diffuse + specular) * aColor;\n"
		"}\0";

	static const char *gouraud_fs = "#version 330 core\n"
		"in vec3 ourColor;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(ourColor, 1.0f);\n"
		"}\n\0";
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	static float vertices[] = {
		//position color normal
		//前
		 0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		//后
		 0.5f,  0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,-1.0f,
		-0.5f,  0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,-1.0f,
		 0.5f, -0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,-1.0f,
		-0.5f, -0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,-1.0f,
		//左
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,-0.5f, 0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,-0.5f, 0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,
		//右
		 0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,-0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,-0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		 //上
		 0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,-0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,-0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		//下
		 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f, 0.0f,
		 0.5f, -0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f, 0.0f,
		-0.5f, -0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f, 0.0f
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

	static bool moveLight = false;
	static int choice;
	static float ambient = 0.2, diffuse = 0.5, specular = 1.0, shininess = 256;
	static glm::vec3 light(1.0f, 1.0f, 1.0f);
	static Camera camera(glm::vec3(-3.0f, 0.5f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -45.0f, -5.0f);
	static Shader phong_shader(phong_vs, phong_fs);
	static Shader gouraud_shader(gouraud_vs, gouraud_fs);

	float time = (float)glfwGetTime();
	unsigned int shaderProgram;

	glm::mat4 model(1.0f);
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);

	glm::vec3 lightPos(3.0f, 0.0f, 3.0f);
	glm::vec3 lightAmbient = ambient * light;
	glm::vec3 lightDiffuse = diffuse * light;
	glm::vec3 lightSpecular = specular * light;

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// render loop
	// -----------
	{
		ImGui::Begin("Lighting");

		ImGui::RadioButton("Phong Shading", &choice, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Gouraud Shading", &choice, 1);
		ImGui::Checkbox("Bonues", &moveLight);

		ImGui::SliderFloat("ambient", &ambient, 0, 1);
		ImGui::SliderFloat("diffuse", &diffuse, 0, 1);
		ImGui::SliderFloat("specular", &specular, 0, 1);
		ImGui::SliderFloat("shininess", &shininess, 2, 512);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	if (moveLight)
	{
		lightPos.x = 5 * cos(time / 2);
		lightPos.z = 5 * sin(time / 2);
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	switch (choice)
	{
	case 0:
		shaderProgram = phong_shader.ID;
		break;
	case 1:
		shaderProgram = gouraud_shader.ID;
		break;
	}
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
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), shininess);
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.ambient"), 1, &lightAmbient[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.diffuse"), 1, &lightDiffuse[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.specular"), 1, &lightSpecular[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camera.Position[0]);
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//draw
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
