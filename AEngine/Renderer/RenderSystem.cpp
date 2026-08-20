#include "RenderSystem.h"
#include "../scene/Scene.h"
#include "../components/Render.h"
#include "Material.h"
#include "Shader.h"
#include "Mesh.h"
#include "glad/glad.h"
#include "../scene/Skybox.h"
#include "../components/Camera.h"
#include "../APIs/ModelLoader.h"
#include "HDRTexture.h"
#include "Cubemap.h"
#include "GameObject.h"
#include "Buffer.h"
#include "../Renderer/RenderContext.h"
#include "../scene/AssetManager.h"
#include "../core/Window.h"
#include "Texture.h"
#include "../components/PostProcess.h"
#include "../components/Transform.h"
#include "../components/Light.h"
#include <AMath.h>
#include <algorithm>
#include <map>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <random>

#define MAXPOINTLIGHTS 192
#define MAXSPOTLIGHTS 128
#define MAXDIRICTIONALLIGHTS 10

namespace RenderSystem
{
	float quadVertices[24] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};
	std::unique_ptr<Shader> depthShaderDirectional = nullptr;
	std::unique_ptr<Shader> depthShaderPoint = nullptr;
	std::unique_ptr<Shader> depthScreenShader = nullptr;
	Framebuffer* exposureA = nullptr;
	Framebuffer* exposureB = nullptr;
	Framebuffer* bloom = nullptr;
	Framebuffer* bloomPing = nullptr;
	Framebuffer* bloomPong = nullptr;
	Framebuffer* GBuffer = nullptr;
	Framebuffer* SSAO = nullptr;
	Framebuffer* SSAOblur = nullptr;
	std::unique_ptr<Shader> EnviromentHDRShader = nullptr;
	std::unique_ptr<Shader> irradianceShader = nullptr;
	std::unique_ptr<Shader> LightingGBuffer = nullptr;
	std::unique_ptr<Shader> PointLightDeferred = nullptr;
	std::unique_ptr<Shader> SpotLightDeferred = nullptr;
	std::unique_ptr<Shader> prefilterShader = nullptr;
	std::unique_ptr<Shader> BRDFLUTShader = nullptr;
	std::unique_ptr<Shader> exposureShader = nullptr;
	std::unique_ptr<Shader> bloomShader = nullptr;
	std::unique_ptr<Shader> BloomBlurShader = nullptr;
	std::unique_ptr<Shader> GBufferShader = nullptr;
	std::unique_ptr<Shader> SSAOShader = nullptr;
	std::unique_ptr<Shader> SSAOBlurShader = nullptr;
	std::unique_ptr<Shader> sceneHDRShader = nullptr;
	Framebuffer* finalBloom = nullptr;
	std::vector<glm::vec3> ssaoKernel;
	std::unique_ptr<Texture> SSAONoise = nullptr;
	std::unique_ptr<Texture> BRDFLUTTexture = nullptr;
	Mesh deferredLowPolySphere;
	Mesh deferredCone;
	Mesh Cube;
	std::unique_ptr<Texture> whiteTexture = nullptr;
	unsigned int WhiteDepthCubemap = 0;

	void rs_CollectBatches(RenderContext& context, std::vector<Batch>& batches)
	{
		struct BatchKey
		{
			Material* material;
			Mesh* mesh;
			int rendertime;
			int surfaceType;
			int renderQueue;
			bool operator<(const BatchKey& batch) const
			{
				if (material != batch.material) return material < batch.material;
				if (mesh != batch.mesh) return mesh < batch.mesh;
				if (rendertime != batch.rendertime) return rendertime < batch.rendertime;
				if (renderQueue != batch.renderQueue) return renderQueue < batch.renderQueue;
				return surfaceType < batch.surfaceType;
			}
		};
		std::map <BatchKey, size_t> batches_map;
		for (auto& render : context.renders)
		{
			if (render->material && render->mesh && render->material->GetShader())
			{
				BatchKey foundBatchKey{};
				foundBatchKey.material = render->material;
				foundBatchKey.mesh = render->mesh;
				if (render == context.selectedObject)  //еще один костыль, пока не придумаю другой способ для обводки
					foundBatchKey.rendertime = static_cast<int>(RenderTime::stencilRender);
				else
					foundBatchKey.rendertime = static_cast<int>(render->renderLayer);
				foundBatchKey.surfaceType = static_cast<int>(render->material->surfaceType);
				foundBatchKey.renderQueue = static_cast<int>(render->material->GetRenderPath());
				auto foundBatch = batches_map.find(foundBatchKey);
				Batch* newBatch = nullptr;
				if (foundBatch == batches_map.end())
				{
					batches.push_back({});
					newBatch = &batches.back();
					newBatch->material = render->material;
					newBatch->mesh = render->mesh;
					batches_map[foundBatchKey] = batches.size() - 1;
					switch (render->material->surfaceType)
					{
					case Material::SurfaceType::Opaque:
						newBatch->surfacetype = SurfaceType::opaque;
						break;
					case Material::SurfaceType::Transparent:
						newBatch->surfacetype = SurfaceType::transparent;
						break;
					}
					switch (render->renderLayer)
					{
					case RenderLayer::defaultRender:
						newBatch->rendertime = RenderTime::defaultRender;
						break;
					case RenderLayer::stencilTest:
						newBatch->rendertime = RenderTime::stencilRender;
						break;
					}
					if (newBatch->surfacetype == SurfaceType::opaque)
					{
						if (newBatch->material->GetRenderPath() == Material::RenderPath::forward)
							newBatch->renderqueue = RenderQueue::ForwardOpaque;
						else
							newBatch->renderqueue = RenderQueue::DeferredOpaque;
					}
					else
						newBatch->renderqueue = RenderQueue::ForwardTransparent;
					if (render == context.selectedObject) { newBatch->rendertime = RenderTime::stencilRender; newBatch->pos.push_back(render->transform->position); newBatch->model.push_back(render->transform->GetModelMatrix());  continue; }  //Потом придумаю другой способ для selectedObject, пока что пусть будет "костыль"
						
				}
				else
				{
					newBatch = &batches[foundBatch->second];
				}
				newBatch->model.push_back(render->transform->GetModelMatrix());
				newBatch->pos.push_back(render->transform->position);
			}
		}
		std::sort(batches.begin(), batches.end(), [](const Batch& batch1, const Batch& batch2) {return static_cast<int>(batch1.renderqueue) < static_cast<int>(batch2.renderqueue); });
	}
	void rs_CreateShadowMap(Light* light)
	{
		std::cout << "Creating shadowMap - ";
		glGenTextures(1, &light->shadowMap.ID);
		if (light->typeLight == Light::Type::Point)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, light->shadowMap.ID);
			for (unsigned int i = 0; i < 6; ++i)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, light->ShadowMapResolution, light->ShadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, light->shadowMap.ID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, light->ShadowMapResolution, light->ShadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
		glGenFramebuffers(1, &light->shadowMap.IDfbo);
		glBindFramebuffer(GL_FRAMEBUFFER, light->shadowMap.IDfbo);
		if (light->typeLight == Light::Type::Directional)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->shadowMap.ID, 0);
		else if (light->typeLight == Light::Type::Point)
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light->shadowMap.ID, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Shadow framebuffer is NOT complete!" << std::endl;
		else
			std::cout << "Shadow framebuffer complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glGenVertexArrays(1, &light->shadowMap.VAO);
		//unsigned int VBO;
		//glGenBuffers(1, &VBO);

		//glBindVertexArray(light->shadowMap.VAO);
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		//glBindVertexArray(0);
		light->shadowMap.created = true;
	}
	void rs_RenderShadowMaps(RenderContext& context, std::vector<Batch>& batches)
	{
		if (!depthShaderDirectional)
			depthShaderDirectional = std::make_unique<Shader>("Shaders/DepthShaderVS.glsl", "Shaders/DepthShaderFS.glsl");
		if (!depthScreenShader)
			depthScreenShader = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/DepthmapFS.glsl");
		if (!depthShaderPoint)
			depthShaderPoint = std::make_unique<Shader>("Shaders/DepthShaderVS.glsl", "Shaders/DepthShaderPointFS.glsl", "Shaders/ShadowsPointGS.glsl");
		GLint prevFBO = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
		glm::mat4 lightMatrixCamera = glm::mat4(1.0f);
		for (auto& light : context.lights)
		{
			if (light->typeLight == Light::Type::Directional)
			{
				if (!light->shadow)
					continue;
				if (!light->shadowMap.created)
					rs_CreateShadowMap(light);
				glViewport(0, 0, light->ShadowMapResolution, light->ShadowMapResolution);
				glBindFramebuffer(GL_FRAMEBUFFER, light->shadowMap.IDfbo);
				glClear(GL_DEPTH_BUFFER_BIT);
				float near_plane = 1.0f, far_plane = 100.0f;
				glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
				glm::vec3 sceneCenter = glm::vec3(0.0f);
				glm::vec3 lightDir = glm::normalize(light->transform->forward());
				glm::vec3 lightPos = sceneCenter - lightDir * 20.0f;
				glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
				light->lightSpaceMatrix = lightProjection * lightView;
				for (auto& batch : batches)
				{
					if (batch.surfacetype == SurfaceType::transparent) continue;
					auto rendertime = batch.rendertime;
					batch.rendertime = RenderTime::defaultRender;
					depthShaderDirectional->Update();
					depthShaderDirectional->setMat4("lightSpaceMatrix", light->lightSpaceMatrix);
					if (auto t = batch.material->GetValue<bool>("GPUInstancing"))
					{
						depthShaderDirectional->setBool("GPUInstancing", *t);
						if (*t)
						{
							rs_RenderMesh(batch, *t, depthShaderDirectional.get());
						}
						else
						{
							rs_RenderMesh(batch, false, depthShaderDirectional.get());
						}
					}
					else
					{
						depthShaderDirectional->setBool("GPUInstancing", false);
						rs_RenderMesh(batch, false, depthShaderDirectional.get());
					}
					batch.rendertime = rendertime;
				}
				lightMatrixCamera = light->lightSpaceMatrix;
				//std::cout << "Start rendering FBO" << std::endl;
				//std::cout << "Updating shader FBO" << std::endl;
				//depthScreenShader->Update();
				//depthScreenShader->setFloat("near_plane", near_plane);
				//depthScreenShader->setFloat("far_plane", far_plane);
				//depthScreenShader->setInt("depthMap", 0);
				//std::cout << "Activate texture to shader" << std::endl;
				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, light->shadowMap.ID);
				//glBindVertexArray(light->shadowMap.VAO);
				//glDisable(GL_DEPTH_TEST);
				//std::cout << "Render FBO" << std::endl;
				//glDrawArrays(GL_TRIANGLES, 0, 6);
				//glEnable(GL_DEPTH_TEST);
				//glBindVertexArray(0);
			}
			else if (light->typeLight == Light::Type::Point)
			{
				if (!light->shadow)
					continue;
				if (!light->shadowMap.created)
					rs_CreateShadowMap(light);
				glViewport(0, 0, light->ShadowMapResolution, light->ShadowMapResolution);
				glBindFramebuffer(GL_FRAMEBUFFER, light->shadowMap.IDfbo);
				glClear(GL_DEPTH_BUFFER_BIT);
				float aspect = (float)light->ShadowMapResolution / (float)light->ShadowMapResolution;
				float near = 0.1f;
				float far = light->shadowMap.far_plane;
				glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
				std::vector<glm::mat4> shadowTransforms;
				shadowTransforms.push_back(shadowProj * glm::lookAt(light->transform->position, light->transform->position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				shadowTransforms.push_back(shadowProj * glm::lookAt(light->transform->position, light->transform->position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				shadowTransforms.push_back(shadowProj * glm::lookAt(light->transform->position, light->transform->position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
				shadowTransforms.push_back(shadowProj * glm::lookAt(light->transform->position, light->transform->position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
				shadowTransforms.push_back(shadowProj * glm::lookAt(light->transform->position, light->transform->position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				shadowTransforms.push_back(shadowProj * glm::lookAt(light->transform->position, light->transform->position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				for (auto& batch : batches)
				{
					if (batch.surfacetype == SurfaceType::transparent) continue;
					auto rendertime = batch.rendertime;
					batch.rendertime = RenderTime::defaultRender;
					depthShaderPoint->Update();
					depthShaderPoint->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
					for (unsigned int i = 0; i < shadowTransforms.size(); i++)
						depthShaderPoint->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
					depthShaderPoint->setFloat("far", far);
					depthShaderPoint->setVec3("lightPos", light->transform->position);
					if (auto t = batch.material->GetValue<bool>("GPUInstancing"))
					{
						depthShaderPoint->setBool("GPUInstancing", *t);
						if (*t)
						{
							rs_RenderMesh(batch, *t, depthShaderPoint.get());
						}
						else
						{
							rs_RenderMesh(batch, false, depthShaderPoint.get());
						}
					}
					else
					{
						depthShaderPoint->setBool("GPUInstancing", false);
						rs_RenderMesh(batch, false, depthShaderPoint.get());
					}
					batch.rendertime = rendertime;
				}
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
		glViewport(0, 0, context.width, context.height);
		UBOBuffer::ChangeInfoBuffer("LightMatrices", 64, &lightMatrixCamera, 0);
	}
	void rs_RenderQuad()
	{
		static unsigned int quadVAO = 0;
		static unsigned int quadVBO = 0;

		if (quadVAO == 0)
		{
			float quadVertices[24] = {
				 -1.0f,  1.0f,  0.0f, 1.0f,
				 -1.0f, -1.0f,  0.0f, 0.0f,
				  1.0f, -1.0f,  1.0f, 0.0f,

				 -1.0f,  1.0f,  0.0f, 1.0f,
				  1.0f, -1.0f,  1.0f, 0.0f,
				  1.0f,  1.0f,  1.0f, 1.0f
			};

			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);

			glBindVertexArray(quadVAO);

			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices ,GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

			glBindVertexArray(0);
		}
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
	}
	void rs_RenderOutline(Render* render, Shader* outlineShader)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		outlineShader->Update();
		outlineShader->setMat4("transform", render->transform->GetModelMatrix());
		render->mesh->Draw();
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
	}
	void rs_RenderMesh(Batch& batch, bool instance, Shader* shader, glm::mat4* modelMatrix)
	{
		if (batch.rendertime == RenderTime::stencilRender)
		{
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}
		if (instance)
		{
			batch.mesh->DrawInstance(batch.model.size(), batch.model.data());
		}
		else
		{
			if (modelMatrix)
			{
				shader->setMat4("transform", *modelMatrix);
				batch.mesh->Draw();
			}
			else
			{
				for (auto& modelmatrices : batch.model)
				{
					shader->setMat4("transform", modelmatrices);
					batch.mesh->Draw();
				}
			}
		}
		if (batch.rendertime == RenderTime::stencilRender)
		{
			glDisable(GL_STENCIL_TEST);
		}
	}
	void rs_RenderBatchOpaque(RenderContext& context, std::vector<Batch>& batches)
	{
		for (auto& batch : batches)
		{
			if (batch.surfacetype == SurfaceType::transparent) continue;
			if (batch.renderqueue != RenderQueue::ForwardOpaque) continue;
			batch.material->GetShader()->Update();
			batch.material->UpdateMaterial();
			unsigned int slots = batch.material->GetLastTextureSlot();
			rs_SetEIPMap(context, batch.material->GetShader(), slots);
			rs_LoadShadowsInShader(context, batch.material->GetShader(), slots);
			if (auto instancing = batch.material->GetValue<bool>("GPUInstancing"))
			{
				if (*instancing)
				{
					rs_RenderMesh(batch, true, batch.material->GetShader());
				}
				else
				{
					rs_RenderMesh(batch, false, batch.material->GetShader());
				}
			}
			else
			{
				rs_RenderMesh(batch, false, batch.material->GetShader());
			}
		}
	}
	void rs_CopyDepthBufferFromGBuffer(RenderContext& context, Framebuffer* buffer)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, GBuffer->GetFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer->GetFBO());

		glBlitFramebuffer(0, 0, context.width, context.height, 0, 0, context.width, context.height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void rs_RenderSkyBox(RenderContext& context)
	{
		if (context.skybox)
			context.skybox->RenderSkyBox(context.camera);
	}
	void rs_RenderBatchTransparent(RenderContext& context, std::vector<Batch>& batches)
	{
		struct TransparentBatch
		{
			int batchindex;
			float dist;
			int index;
		};
		std::vector<TransparentBatch> batchesSort;
		for (int i = 0; i < batches.size(); i++)
		{
			if (batches[i].surfacetype != SurfaceType::transparent) continue;
			for (int j = 0; j < batches[i].pos.size(); j++)
			{
				batchesSort.push_back(TransparentBatch{ i, Vector3_Distance(batches[i].pos[j], context.camera->transform->position), j });
			}
		}
		std::sort(batchesSort.begin(), batchesSort.end(), [](const TransparentBatch& batch1, const TransparentBatch& batch2) {return batch1.dist > batch2.dist; });
		glEnable(GL_BLEND);
		for (auto& batch : batchesSort)
		{
			auto& b = batches[batch.batchindex];

			b.material->GetShader()->Update();
			b.material->UpdateMaterial();
			unsigned int slots = b.material->GetLastTextureSlot();
			rs_SetEIPMap(context, b.material->GetShader(), slots);
			rs_LoadShadowsInShader(context, b.material->GetShader(), slots);
			rs_RenderMesh(b, false, b.material->GetShader(), &b.model[batch.index]);
		}
		glDisable(GL_BLEND);
	}
	void rs_RenderDeferredGeometry(RenderContext& context, std::vector<Batch>& batches)
	{
		GBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, context.width, context.height);
		for (auto& batch : batches)
		{
			if (batch.surfacetype == SurfaceType::transparent) continue;
			if (batch.renderqueue == RenderQueue::DeferredOpaque)
			{
				batch.material->GetShader()->Update();
				batch.material->UpdateMaterial();
				if (auto instancing = batch.material->GetValue<bool>("GPUInstancing"))
				{
					if (*instancing)
					{
						rs_RenderMesh(batch, true, batch.material->GetShader());
					}
					else
					{
						rs_RenderMesh(batch, false, batch.material->GetShader());
					}
				}
				else
				{
					rs_RenderMesh(batch, false, batch.material->GetShader());
				}
			}
		}
		GBuffer->UnBind();
	}
	void rs_LoadShadowsInShader(RenderContext& context, Shader* shader, unsigned int& slotTextureShadow)
	{
		bool ShadowDirectional = false;
		bool ShadowPoint = false;
		if (context.Shadows)
		{
			for (auto light : context.lights)
			{
				if (!light->shadow)
					continue;
				if (light->typeLight == Light::Type::Directional)
				{
					glActiveTexture(GL_TEXTURE0 + slotTextureShadow);
					glBindTexture(GL_TEXTURE_2D, light->shadowMap.ID);
					shader->setInt("shadowMap", slotTextureShadow);
					slotTextureShadow += 1;
					ShadowDirectional = true;
				}
				else if (light->typeLight == Light::Type::Point)
				{
					glActiveTexture(GL_TEXTURE0 + slotTextureShadow);
					glBindTexture(GL_TEXTURE_CUBE_MAP, light->shadowMap.ID);
					shader->setInt("shadowMapCube", slotTextureShadow);
					slotTextureShadow += 1;
					ShadowPoint = true;
				}
			}
		}
		if (!ShadowDirectional)
		{
			glActiveTexture(GL_TEXTURE0 + slotTextureShadow);
			glBindTexture(GL_TEXTURE_2D, whiteTexture->GetTexturePixels());
			shader->setInt("shadowMap", slotTextureShadow);
			slotTextureShadow += 1;
		}
		if (!ShadowPoint)
		{
			glActiveTexture(GL_TEXTURE0 + slotTextureShadow);
			glBindTexture(GL_TEXTURE_CUBE_MAP, WhiteDepthCubemap);
			shader->setInt("shadowMapCube", slotTextureShadow);
			slotTextureShadow += 1;
		}
		if (context.postProcess && context.postProcess->SSAO && context.postProcess->camera == context.camera)
		{
			glActiveTexture(GL_TEXTURE0 + slotTextureShadow);
			glBindTexture(GL_TEXTURE_2D, SSAOblur->GetColorBuffer(0));
			shader->setInt("SSAO", slotTextureShadow);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + slotTextureShadow);
			glBindTexture(GL_TEXTURE_2D, whiteTexture->GetTexturePixels());
			shader->setInt("SSAO", slotTextureShadow);
		}
	}
	void rs_LoadGBufferInShader(RenderContext& context, Shader* shader, std::vector<int> buffers)
	{
		for (unsigned int i = 0; i < buffers.size(); i++)
		{
			switch (i)
			{
			case 0:
				if (buffers[0] == 1)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, GBuffer->GetColorBuffer(0));
					shader->setInt("gPosition", 0);
				}
				break;
			case 1:
				if (buffers[1] == 1)
				{
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, GBuffer->GetColorBuffer(1));
					shader->setInt("gNormal", 1);
				}
				break;
			case 2:
				if (buffers[2] == 1)
				{
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, GBuffer->GetColorBuffer(2));
					shader->setInt("gAlbedo", 2);
				}
				break;
			case 3:
				if (buffers[3] == 1)
				{
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, GBuffer->GetColorBuffer(3));
					shader->setInt("gMaterial", 3);
				}
				break;
			}
		}
	}
	void rs_RenderDeferredLighting(RenderContext& context, std::vector<Batch>& batches)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		std::vector<glm::mat4> PointlightsMatrices;
		std::vector<glm::mat4> SpotlightsMatrices;

		for (Light* light : context.lights)
		{
			if (light->typeLight == Light::Type::Directional) continue;
			
			if (light->typeLight == Light::Type::Point)
			{	
				if (PointlightsMatrices.size() >= MAXPOINTLIGHTS) continue;
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, light->transform->position);
				model = glm::scale(model, glm::vec3(light->Radius));
				PointlightsMatrices.push_back(model);
			}
			else if (light->typeLight == Light::Type::Spot)
			{
				if (SpotlightsMatrices.size() >= MAXSPOTLIGHTS) continue;
				float range = light->Radius;
				float angle = acos(light->outerCutOff);
				float coneRadius = tan(angle) * range;

				glm::vec3 lightDir = glm::normalize(light->transform->forward());
				glm::quat rot = glm::rotation(glm::vec3(0, 0, -1), lightDir);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, light->transform->position);
				model *= glm::mat4_cast(rot);
				model = glm::scale(model, glm::vec3(coneRadius, coneRadius, range));

				SpotlightsMatrices.push_back(model);
			}
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		LightingGBuffer->Update();
		rs_LoadGBufferInShader(context, LightingGBuffer.get(), {1, 1, 1, 1});
		unsigned int slots = 4;
		rs_SetEIPMap(context, LightingGBuffer.get(), slots);
		rs_LoadShadowsInShader(context, LightingGBuffer.get(), slots);
		rs_RenderQuad();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		PointLightDeferred->Update();
		PointLightDeferred->setVec2("screenSize", context.width, context.height);
		rs_LoadGBufferInShader(context, PointLightDeferred.get(), {1, 1, 1, 1});
		slots = 4;
		rs_LoadShadowsInShader(context, PointLightDeferred.get(), slots);
		if(!PointlightsMatrices.empty())
			deferredLowPolySphere.DrawInstance(PointlightsMatrices.size(), PointlightsMatrices.data());
		SpotLightDeferred->Update();
		SpotLightDeferred->setVec2("screenSize", context.width, context.height);
		rs_LoadGBufferInShader(context, SpotLightDeferred.get(), {1, 1, 1, 1});
		if (!SpotlightsMatrices.empty())
			deferredCone.DrawInstance(SpotlightsMatrices.size(), SpotlightsMatrices.data());
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	}
	void rs_UploadCamera(RenderContext& context)
	{
		struct u_CameraInfo
		{
			float near;
			float far;
			float padding[2];
			glm::vec4 camPos;
		};
		struct u_Matrices
		{
			glm::mat4 view;
			glm::mat4 proj;
		};
		u_CameraInfo caminfo{};
		caminfo.near = context.camera->near;
		caminfo.far = context.camera->far;
		caminfo.camPos = glm::vec4(context.camera->transform->position, 1.0);
		if (auto t = UBOBuffer::existBuffer("u_CameraInfo"))
			UBOBuffer::ChangeInfoBuffer("u_CameraInfo", t->size, &caminfo, 0);
		u_Matrices matrices{};
		matrices.proj = context.camera->projection;
		matrices.view = context.camera->viewMatrix;
		if (auto t = UBOBuffer::existBuffer("u_Matrices"))
		{
			UBOBuffer::ChangeInfoBuffer("u_Matrices", t->size, &matrices, 0);
		}
	}
	void rs_UploadLighting(RenderContext& context)
	{
		struct DirectionalLight
		{
			float intensive;
			float pad0[3];
			glm::vec4 direction;
			glm::vec4 lightColor;

		};

		struct PointLight
		{
			float intensive;
			float pad0[3];

			glm::vec3 position;
			float pad1;

			glm::vec3 lightColor;

			float far_plane;
			float castShadow;
			float Radius;
			float pad3[2];
		};
		struct SpotLight
		{
			float intensive;
			float pad0[3];
			glm::vec4 position;
			glm::vec4  direction;
			float cutOff;
			float pad1[3];
			glm::vec4 lightColor;
			float outerCutOff;
			float pad2[3];
		};
		struct Fog
		{
			int fogEnabled;
			float pad0[3];
			glm::vec4 colorFog;
		};
		struct u_DirectionalLight
		{
			int countDirLight;
			float pad0[3];
			DirectionalLight DirectionalLights[MAXDIRICTIONALLIGHTS];
		};
		struct u_PointLight
		{
			int countPointLight;
			float pad0[3];
			PointLight PointLights[MAXPOINTLIGHTS];
		};
		struct u_SpotLight
		{
			int countSpotLight;
			float pad0[3];
			SpotLight SpotLights[MAXSPOTLIGHTS];
		};
		struct u_Fog
		{
			Fog fog;
		};
		int point = 0, dir = 0, spot = 0;

		static u_DirectionalLight directLight{};
		static u_PointLight pointLight{};
		static u_SpotLight spotLight{};
		u_Fog fogsettings{};

		directLight = {};
		pointLight = {};
		spotLight = {};

		for (Light* light : context.lights)
		{
			switch (light->typeLight)
			{
			case Light::Type::Directional:
				if (dir >= MAXDIRICTIONALLIGHTS) continue;
				directLight.DirectionalLights[dir].intensive = light->intensive;
				directLight.DirectionalLights[dir].direction = glm::vec4(light->transform->forward(), 1.0);
				directLight.DirectionalLights[dir].lightColor = glm::vec4(light->lightColor, 1.0);
				dir++;
				break;
			case Light::Type::Point:
				if (point >= MAXPOINTLIGHTS) continue;
				pointLight.PointLights[point].intensive = light->intensive;
				pointLight.PointLights[point].lightColor = light->lightColor;
				pointLight.PointLights[point].position = light->transform->position;
				pointLight.PointLights[point].far_plane = light->shadowMap.far_plane;
				pointLight.PointLights[point].castShadow = light->shadow ? 1.0f : 0.0f;
				pointLight.PointLights[point].Radius = light->Radius;
				point++;
				break;
			case Light::Type::Spot:
				if (spot >= MAXSPOTLIGHTS) continue;
				spotLight.SpotLights[spot].direction = glm::vec4(light->transform->forward(), 1.0);
				spotLight.SpotLights[spot].intensive = light->intensive;
				spotLight.SpotLights[spot].lightColor = glm::vec4(light->lightColor, 1.0);
				spotLight.SpotLights[spot].position = glm::vec4(light->transform->position, 1.0);
				spotLight.SpotLights[spot].cutOff = light->cutOff;
				spotLight.SpotLights[spot].outerCutOff = light->outerCutOff;
				spot++;
				break;
			default:
				break;
			}
		}
		spotLight.countSpotLight = spot;
		pointLight.countPointLight = point;
		directLight.countDirLight = dir;
		fogsettings.fog.colorFog = glm::vec4(context.fog.fogColor, 1.0);
		fogsettings.fog.fogEnabled = context.fog.enabled ? 1 : 0;
		if (auto t = UBOBuffer::existBuffer("u_DirectionalLight"))
			UBOBuffer::ChangeInfoBuffer("u_DirectionalLight", t->size, &directLight, 0);
		if (auto t = UBOBuffer::existBuffer("u_PointLight"))
			UBOBuffer::ChangeInfoBuffer("u_PointLight", t->size, &pointLight, 0);
		if (auto t = UBOBuffer::existBuffer("u_SpotLight"))
			UBOBuffer::ChangeInfoBuffer("u_SpotLight", t->size, &spotLight, 0);
		if (auto t = UBOBuffer::existBuffer("u_Fog"))
			UBOBuffer::ChangeInfoBuffer("u_Fog", t->size, &fogsettings, 0);
	}
	void rs_CreateCameraFramebuffers(Camera& camera)
	{
		camera.Add("defaultFramebuffer", sceneHDRShader.get(), { Framebuffer::TypeFrameBuffer::HDR }, true);
		camera.Add("ViewportFramebuffer", nullptr, { Framebuffer::TypeFrameBuffer::HDR }, true);
		camera.Add("exposureAFramebuffer", exposureShader.get(), { Framebuffer::TypeFrameBuffer::R16F }, false, 1, 1);
		camera.Add("exposureBFramebuffer", exposureShader.get(), { Framebuffer::TypeFrameBuffer::R16F }, false, 1, 1);
		camera.Add("bloomAFramebuffer", bloomShader.get(), { Framebuffer::TypeFrameBuffer::HDR }, true);
		camera.Add("bloomPingFramebuffer", BloomBlurShader.get(), { Framebuffer::TypeFrameBuffer::HDR }, true);
		camera.Add("bloomPongFramebuffer", BloomBlurShader.get(), { Framebuffer::TypeFrameBuffer::HDR }, true);
		camera.Add("GBufferFramebuffer", nullptr, { Framebuffer::TypeFrameBuffer::RGBA16F, Framebuffer::TypeFrameBuffer::RGBA16F, Framebuffer::TypeFrameBuffer::RGBA, Framebuffer::TypeFrameBuffer::RGBA }, true, -1, -1, { Framebuffer::TypeFilter::Nearest, Framebuffer::TypeFilter::Nearest, Framebuffer::TypeFilter::Nearest, Framebuffer::TypeFilter::Nearest });
		camera.Add("SSAOFramebuffer", SSAOShader.get(), { Framebuffer::TypeFrameBuffer::RED }, true, -1, -1, { Framebuffer::TypeFilter::Nearest });
		camera.Add("SSAOblurFramebuffer", SSAOBlurShader.get(), { Framebuffer::TypeFrameBuffer::RED }, true, -1, -1, { Framebuffer::TypeFilter::Nearest });
	}
	void rs_CollectbuffersFromCamera(Camera& camera)
	{
		exposureA = camera.GetFramebuffer("exposureAFramebuffer");
		exposureB = camera.GetFramebuffer("exposureBFramebuffer");
		bloom = camera.GetFramebuffer("bloomAFramebuffer");
		bloomPing = camera.GetFramebuffer("bloomPingFramebuffer");
		bloomPong = camera.GetFramebuffer("bloomPongFramebuffer");
		GBuffer = camera.GetFramebuffer("GBufferFramebuffer");
		SSAO = camera.GetFramebuffer("SSAOFramebuffer");
		SSAOblur = camera.GetFramebuffer("SSAOblurFramebuffer");
	}
	void rs_Init()
	{
		std::cout << glGetString(GL_RENDERER) << std::endl;
		sceneHDRShader = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/FramebufferFragmentShader.glsl");
		exposureShader = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/ExposureFS.glsl");
		bloomShader = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/BloomFS.glsl");
		BloomBlurShader = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/BlurFS.glsl");
		LightingGBuffer = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/DeferredLightingFS.glsl");
		PointLightDeferred = std::make_unique<Shader>("Shaders/DeferredLightingVS.glsl", "Shaders/DeferredPointLighting.glsl");
		SpotLightDeferred = std::make_unique<Shader>("Shaders/DeferredLightingVS.glsl", "Shaders/DeferredSpotLighting.glsl");
		deferredCone = AssetManager::LoadModel("Assets/Models/Cone.obj")->meshes[0];
		deferredLowPolySphere = AssetManager::LoadModel("Assets/Models/SphereLowPoly.obj")->meshes[0];
		SSAOShader = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/SSAOFS.glsl");
		SSAOBlurShader = std::make_unique<Shader>("Shaders/FramebufferVertexShader.glsl", "Shaders/SSAOblurFS.glsl");
		EnviromentHDRShader = std::make_unique<Shader>("Shaders/HDRToCubemapVS.glsl", "Shaders/HDRToCubemapFS.glsl");
		irradianceShader = std::make_unique<Shader>("Shaders/HDRToCubemapVS.glsl", "Shaders/IrradianceShaderFS.glsl");
		prefilterShader = std::make_unique<Shader>("Shaders/HDRToCubemapVS.glsl", "Shaders/PrefilterHDRMapFS.glsl");
		BRDFLUTShader = std::make_unique<Shader>("Shaders/IntegrateBRDFVS.glsl", "Shaders/IntegrateBRDFFS.glsl");
		Cube = AssetManager::LoadModel("Assets/Models/Cube.obj")->meshes[0];
		whiteTexture = std::make_unique<Texture>();
		whiteTexture->CreateColorTexture("white");
		glGenTextures(1, &WhiteDepthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, WhiteDepthCubemap);

		float depthValue = 1.0f;

		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 1, 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depthValue);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		SSAOShader->Update();
		SSAOShader->setInt("gPosition", 0);
		SSAOShader->setInt("gNormal", 1);
		SSAOShader->setInt("texNoise", 2);
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = (float)i / 64.0;
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}
		std::vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
			ssaoNoise.push_back(noise);
		}
		SSAONoise = std::make_unique<Texture>();
		SSAONoise->CreateTexture(4, 4, Texture::DataFormat::RGBA16F, Texture::DataFormat::RGB, Texture::FilterType::NEAREST, Texture::FilterType::REPEAT, ssaoNoise.data());

		BRDFLUTTexture = std::make_unique<Texture>();
		std::vector<float> emptyBRDF(512 * 512 * 2, 0.0f);
		BRDFLUTTexture->CreateTexture(512, 512, Texture::DataFormat::RG16F, Texture::DataFormat::RG, Texture::FilterType::LINEAR, Texture::FilterType::CLAMP_TO_EDGE, emptyBRDF.data());
		std::cout << "Complited" << std::endl;
	}
	void rs_InitAfterFirstFrame()
	{
		static bool firstFrame = false;
		if (!firstFrame)
		{
			rs_CreateBRDFTexture();
			firstFrame = true;
		}
	}
	void rs_Start(Framebuffer* sceneHDR)
	{
		sceneHDR->Bind();
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	void rs_End()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void rs_RenderSSAO(RenderContext& context)
	{
		if (context.postProcess && context.postProcess->SSAO && context.camera == context.postProcess->camera)
		{
			SSAO->Bind();
			glClear(GL_COLOR_BUFFER_BIT);
			SSAO->GetShader()->Update();
			rs_LoadGBufferInShader(context, SSAO->GetShader(), { 1, 1 });
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, SSAONoise->GetTexturePixels());
			for (unsigned int i = 0; i < 64; ++i)
				SSAO->GetShader()->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
			SSAO->GetShader()->setVec2("screenSize", glm::vec2(context.width, context.height));
			rs_RenderQuad();
			SSAO->UnBind();
			SSAOblur->Bind();
			SSAOblur->GetShader()->Update();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SSAO->GetColorBuffer(0));
			SSAOblur->GetShader()->setInt("SSAO", 0);
			rs_RenderQuad();
			SSAOblur->UnBind();
		}
	}
	void rs_PostProcess(RenderContext& context, Framebuffer* sceneHDR)
	{
		if (!context.postProcess) return;
		if (context.postProcess->AutoExposure && context.postProcess->camera == context.camera)
		{

			Framebuffer* oldFrameBuffer = context.camera->exposurePing ? exposureA : exposureB;
			Framebuffer* currectFrameBuffer = context.camera->exposurePing ? exposureB : exposureA;

			currectFrameBuffer->Bind();

			glViewport(0, 0, 1, 1);

			Shader* exposureShader = currectFrameBuffer->GetShader();
			exposureShader->Update();

			exposureShader->setInt("SceneTexture", 0);
			exposureShader->setInt("PreviousExposure", 1);

			float mipLevel = std::floor(std::log2(std::max(context.width, context.height)));
			exposureShader->setFloat("mipLevel", mipLevel);
			exposureShader->setFloat("targetBrightness", context.postProcess->targetBrightness);
			exposureShader->setFloat("minExposure", context.postProcess->minExposure);
			exposureShader->setFloat("maxExposure", context.postProcess->maxExposure);
			exposureShader->setFloat("adaptationSpeed", context.postProcess->adaptationSpeed);
			exposureShader->setFloat("deltaTime", Time::deltaTime); 

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sceneHDR->GetColorBuffer(0));
			glGenerateMipmap(GL_TEXTURE_2D);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, oldFrameBuffer->GetColorBuffer(0));

			rs_RenderQuad();
			currectFrameBuffer->UnBind();

			glViewport(0, 0, context.width, context.height);
			context.camera->exposurePing = !context.camera->exposurePing;
		}
		if (context.postProcess->Bloom && context.postProcess->camera == context.camera)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sceneHDR->GetColorBuffer(0));

			bloom->Bind();
			glViewport(0, 0, context.width, context.height);
			glClear(GL_COLOR_BUFFER_BIT);

			Shader* bloomShader = bloom->GetShader();
			bloomShader->Update();
			bloomShader->setInt("SceneTexture", 0);

			rs_RenderQuad();
			bloom->UnBind();

			bool horizontalBlurForBloom = true;
			bool first_iterationBlurForBloom = true;

			Shader* blurShader = bloomPing->GetShader();


			blurShader->Update();
			blurShader->setInt("image", 0);

			const unsigned int amount = 10;
			glActiveTexture(GL_TEXTURE0);
			for (unsigned int i = 0; i < amount; i++)
			{
				finalBloom = horizontalBlurForBloom ? bloomPing : bloomPong;
				finalBloom->Bind();
				blurShader->setInt("horizontal", horizontalBlurForBloom);
				glBindTexture(GL_TEXTURE_2D, first_iterationBlurForBloom ? bloom->GetColorBuffer(0) : horizontalBlurForBloom ? bloomPong->GetColorBuffer(0) : bloomPing->GetColorBuffer(0));
				rs_RenderQuad();
				horizontalBlurForBloom = !horizontalBlurForBloom;
				if (first_iterationBlurForBloom)
					first_iterationBlurForBloom = false;
			}
			finalBloom->UnBind();
		}
	}
	void rs_Present(RenderContext& context, Framebuffer* sceneHDR, Framebuffer* targetHDR)
	{
		if (targetHDR)
			targetHDR->Bind();
		sceneHDR->UseShader();
		sceneHDR->GetShader()->setBool("HDR", context.HDR);
		float exposure = 1.0f;
		bool AutoExposure = false;
		bool Bloom = false;

		if (context.postProcess && context.camera == context.postProcess->camera)
		{
			exposure = context.postProcess->exposure;
			AutoExposure = context.postProcess->AutoExposure;
		}
		if (context.postProcess && context.camera == context.postProcess->camera)
		{
			Bloom = context.postProcess->Bloom;
		}
		sceneHDR->GetShader()->setFloat("exposure", exposure);
		sceneHDR->GetShader()->setBool("autoExposure", AutoExposure);
		sceneHDR->GetShader()->setBool("Bloom", Bloom);

		sceneHDR->GetShader()->setInt("ExposureTexture", 1);
		sceneHDR->GetShader()->setInt("BloomTexture", 2);

		Framebuffer* currentExposure = context.camera->exposurePing ? exposureB : exposureA;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneHDR->GetColorBuffer(0));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, currentExposure->GetColorBuffer(0));
		if (Bloom)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, finalBloom->GetColorBuffer(0));
		}
		rs_RenderQuad();
		if (targetHDR)
			targetHDR->UnBind();
	}
	void rs_RenderEnvironmentHDR(Cubemap& cubemap, HDRTexture& hdrTexture, int width, int height)
	{
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		EnviromentHDRShader->Update();
		EnviromentHDRShader->setInt("equirectangularMap", 0);
		EnviromentHDRShader->setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture.GetTexturePixels());
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			EnviromentHDRShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap.GetTextureID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Cube.Draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}
	void rs_RenderIrradianceMap(Cubemap& envcubemap, Cubemap& irradianceCubemap, int width, int height)
	{
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		irradianceShader->Update();
		irradianceShader->setInt("environmentMap", 0);
		irradianceShader->setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envcubemap.GetTextureID());
		glViewport(0, 0, width, height);
		for (unsigned int i = 0; i < 6; ++i)
		{
			irradianceShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemap.GetTextureID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Cube.Draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}
	void rs_RenderPrefilterMap(Cubemap& envcubemap, Cubemap& prefilterCubemap)
	{
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		prefilterShader->Update();
		prefilterShader->setInt("environmentMap", 0);
		prefilterShader->setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envcubemap.GetTextureID());

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		unsigned int maxMipLevels = 5;
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader->setFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader->setMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterCubemap.GetTextureID(), mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Cube.Draw();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	void rs_CreateBRDFTexture()
	{
		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BRDFLUTTexture->GetTexturePixels(), 0);
		glViewport(0, 0, 512, 512);
		BRDFLUTShader->Update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		rs_RenderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}
	void rs_SetEIPMap(RenderContext& context, Shader* shader, unsigned int& slot)
	{
		shader->setInt("irradianceMap", slot);
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, context.irradianceMap->GetTextureID());
		slot += 1;
		shader->setInt("prefilterMap", slot);
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, context.prefilterMap->GetTextureID());
		slot += 1;
		shader->setInt("brdfLUT", slot);
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, BRDFLUTTexture->GetTexturePixels());
		slot += 1;
	}
	void rs_ClearFramebuffer(Framebuffer* ClearFramebuffer, float width, float height)
	{
		ClearFramebuffer->Bind();

		glViewport(0, 0, width, height);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ClearFramebuffer->UnBind();
	}
	void rs_Render(RenderContext& context, Framebuffer* sceneHDR)
	{
		rs_CollectbuffersFromCamera(*context.camera);
		std::vector<Batch> batches;
		rs_CollectBatches(context, batches);
		if(context.Shadows)
			rs_RenderShadowMaps(context, batches);
		rs_UploadCamera(context);
		rs_UploadLighting(context);
		UBOBuffer::UpdateAll();
		rs_RenderDeferredGeometry(context, batches);
		rs_RenderSSAO(context);
		sceneHDR->Bind();
		rs_RenderDeferredLighting(context, batches);
		rs_CopyDepthBufferFromGBuffer(context, sceneHDR);
		sceneHDR->Bind();
		rs_RenderBatchOpaque(context, batches);
		rs_RenderSkyBox(context);
		rs_RenderBatchTransparent(context, batches);
	}
}