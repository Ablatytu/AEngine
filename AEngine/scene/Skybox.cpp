#include "Skybox.h"
#include "../Renderer/Cubemap.h"
#include "AssetManager.h"
#include <glad/glad.h>
#include "../Renderer/Shader.h"
#include "SceneManager.h"
#include "Scene.h"
#include "../components/Camera.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
Skybox::Skybox(Cubemap* cubemap)
{
	skybox = cubemap;
	skyboxShader = std::make_unique<Shader>("Shaders/SkyboxVertexShader.glsl", "Shaders/SkyboxFragmentShader.glsl");
	unsigned int VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}
void Skybox::RenderSkyBox(Camera* camera)
{
	glDepthMask(false);
	glDepthFunc(GL_LEQUAL);
	glActiveTexture(GL_TEXTURE0);
	skyboxShader->Update();
	skyboxShader->setInt("cubemap", 0);
	skyboxShader->setMat4("proj", camera->projection);
	skyboxShader->setMat4("view", glm::mat4(glm::mat3(camera->viewMatrix)));
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetTextureID());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glDepthMask(true);
}
void Skybox::ChangeSkyBox(Cubemap* cubemap)
{
	skybox = cubemap;
}
Cubemap* Skybox::GetSkyboxCubemap()
{
	return skybox;
}
Skybox::~Skybox() = default; 