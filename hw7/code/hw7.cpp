#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "hw.h"
#include "shader.h"
#include "camera.h"

#include <iostream>
#include <algorithm>

GLuint loadTexture(GLchar* path)
{
	// Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height, nrChannels;
	unsigned char* image = stbi_load(path, &width, &height, &nrChannels, 0);
	if (image)
	{
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// unbind and free
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(image);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	return textureID;
}

void RenderCube()
{
	static GLuint cubeVAO;
	if (cubeVAO == 0)
	{
		GLuint cubeVBO;
		GLfloat vertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
			// Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
			// Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// Render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void RenderScene(Shader &shader)
{
	static GLuint planeVAO;
	if (planeVAO == 0)
	{
		GLuint planeVBO;
		static float vertices[] = {
			// Positions     Normals   Texture Coords
			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
			-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
		};
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(planeVAO);
		//bind data
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(planeVAO);
	}
	// Floor
	glm::mat4 model(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.5));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
}

void render_hw7()
{
	//depth shader code
	static const char *depth_shader_vs = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"uniform mat4 lightSpaceMatrix;\n"
		"uniform mat4 model;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);\n"
		"}\0";

	static const char *depth_shader_fs = "#version 330 core\n"
		"void main()\n"
		"{\n"
		"   // gl_FragDepth = gl_FragCoord.z;\n"
		"}\n\0";
	//orthogonal shader code
	static const char *or_shader_vs = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 normal;\n"
		"layout (location = 2) in vec2 texCoords_vs;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"uniform mat4 lightSpaceMatrix;\n"
		"out VS_OUT {\n"
		"	vec3 FragPos;\n"
		"	vec3 Normal;\n"
		"	vec2 TexCoords;\n"
		"	vec4 FragPosLightSpace;\n"
		"} out_vs;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = projection * view * model * vec4(position, 1.0f);\n"
		"	out_vs.FragPos = vec3(model * vec4(position, 1.0f));\n"
		"   out_vs.Normal = transpose(inverse(mat3(model))) * normal;\n"
		"   out_vs.TexCoords = texCoords_vs;\n"
		"   out_vs.FragPosLightSpace = lightSpaceMatrix * vec4(out_vs.FragPos, 1.0f);\n"
		"}\0";
	static const char *or_shader_fs = "#version 330 core\n"
		"in VS_OUT {\n"
		"	vec3 FragPos;\n"
		"	vec3 Normal;\n"
		"	vec2 TexCoords;\n"
		"	vec4 FragPosLightSpace;\n"
		"} in_fs;\n"
		"struct Light {\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"	vec3 position;\n"
		"	};\n"
		"uniform sampler2D diffuseTexture;\n"
		"uniform sampler2D shadowMap;\n"
		"uniform vec3 viewPos;\n"
		"uniform Light light;\n"
		"uniform int optim;\n"
		"out vec4 FragColor;\n"
		"float ShadowCalculation(vec4 fragPosLightSpace)\n"
		"{\n"
		"	float shadow = 0.0;\n"
		"	// perform perspective divide\n"
		"	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
		"	// Transform to [0,1] range\n"
		"	projCoords = projCoords * 0.5 + 0.5;\n"
		"	// Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)\n"
		"	float closestDepth = texture(shadowMap, projCoords.xy).r; \n"
		"	// Get depth of current fragment from light's perspective \n"
		"	float currentDepth = projCoords.z; \n"
		"	if (optim == 0) \n" 
		"	{\n"
		"		shadow = currentDepth > closestDepth ? 1.0 : 0.0;\n"
		"		return shadow;\n"
		"	}\n"
		"	// Calculate bias (based on depth map resolution and slope) \n"
		"	vec3 normal = normalize(in_fs.Normal); \n"
		"	vec3 lightDir = normalize(light.position - in_fs.FragPos);\n"
		"	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);\n"
		"	// PCF \n"
		"	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n"
		"	for(int x = -1; x <= 1; ++x)\n"
		"	{\n"
		"		for(int y = -1; y <= 1; ++y)\n"
		"		{\n"
		"			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; \n"
		"			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;\n"
		"		} \n"
		"	}\n"
		"	shadow /= 9.0;\n"
		"	// Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.\n"
		"	if(projCoords.z > 1.0)\n"
		"		shadow = 0.0;\n"
		"	return shadow;\n"
		"}\n"
		"void main()\n"
		"{\n"
		"	vec3 color = texture(diffuseTexture, in_fs.TexCoords).rgb;\n"
		"	//ambient\n"
		"	vec3 ambient = light.ambient;\n"
		"	//diffuse\n"
		"	vec3 normal = normalize(in_fs.Normal);\n"
		"	vec3 lightDir = normalize(light.position - in_fs.FragPos);\n"
		"	float diff = max(dot(normal, lightDir), 0.0f);\n"
		"	vec3 diffuse = diff * light.diffuse;\n"
		"	//specular\n"
		"	vec3 viewDir = normalize(viewPos - in_fs.FragPos);\n"
		"	vec3 halfwayDir = normalize(lightDir + viewDir);\n"
		"	float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), 64.0f);\n"
		"	vec3 specular = spec * light.specular;\n"
		"	// add shadow \n"
		"	float shadow = ShadowCalculation(in_fs.FragPosLightSpace);\n"
		"	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;\n"
		"   FragColor = vec4(result, 1.0f);\n"
		"}\n\0";

	static bool optim = false;
	static float ambient = 0.2, diffuse = 0.5, specular = 1.0;
	static glm::vec3 light(1.0f, 1.0f, 1.0f);
	static Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
	static Shader simpleDepthShader(depth_shader_vs, depth_shader_fs);
	static Shader or_shader(or_shader_vs, or_shader_fs);
	static GLchar path[] = "container.jpg";
	static GLuint boxTexture = loadTexture(path);

	float time = (float)glfwGetTime();

	glm::mat4 model(1.0f), view(1.0f), projection(1.0f);
	glm::mat4 lightProjection(1.0f), lightView(1.0f), lightSpaceMatrix(1.0f);

	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
	glm::vec3 lightAmbient = ambient * light;
	glm::vec3 lightDiffuse = diffuse * light;
	glm::vec3 lightSpecular = specular * light;

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	static GLuint depthMapFBO, depthMap;
	if (depthMapFBO == 0 && depthMap == 0)
	{
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		ImGui::Begin("Shading");

		ImGui::SliderFloat("ambient", &ambient, 0, 1);
		ImGui::SliderFloat("diffuse", &diffuse, 0, 1);
		ImGui::SliderFloat("specular", &specular, 0, 1);
		ImGui::Checkbox("bonous", &optim);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// 1. depth mapping
	simpleDepthShader.use();
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderScene(simpleDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// 2. render
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	or_shader.use();
	projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	glUniform1i(glGetUniformLocation(or_shader.ID, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(or_shader.ID, "shadowMap"), 1);
	glUniformMatrix4fv(glGetUniformLocation(or_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(or_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	// Set light uniforms
	glUniform3fv(glGetUniformLocation(or_shader.ID, "light.ambient"), 1, &lightAmbient[0]);
	glUniform3fv(glGetUniformLocation(or_shader.ID, "light.diffuse"), 1, &lightDiffuse[0]);
	glUniform3fv(glGetUniformLocation(or_shader.ID, "light.specular"), 1, &lightSpecular[0]);
	glUniform3fv(glGetUniformLocation(or_shader.ID, "light.position"), 1, &lightPos[0]);
	glUniform1i(glGetUniformLocation(or_shader.ID, "optim"), int(optim));
	glUniform3fv(glGetUniformLocation(or_shader.ID, "viewPos"), 1, &camera.Position[0]);
	glUniformMatrix4fv(glGetUniformLocation(or_shader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	RenderScene(or_shader);

	// 3. visualize depth map by rendering it to plane
	/*
	debugDepthQuad.Use();
	glUniform1f(glGetUniformLocation(debugDepthQuad.Program, "near_plane"), near_plane);
	glUniform1f(glGetUniformLocation(debugDepthQuad.Program, "far_plane"), far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	RenderQuad();

	
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
	*/
}
