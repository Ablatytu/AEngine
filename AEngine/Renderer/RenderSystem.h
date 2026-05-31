#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

class Scene;
class Material;
class Mesh;
class Render;
class RenderContext;
class Shader;
class Light;
class Framebuffer;

namespace RenderSystem
{
	extern float quadVertices[24];
	enum class RenderTime
	{
		defaultRender = 0,
		stencilRender = 1
	};
	enum class SurfaceType
	{
		opaque = 0,
		transparent = 1
	};
	enum class RenderQueue
	{
		DeferredOpaque = 0,
		ForwardOpaque = 1,
		ForwardTransparent = 2
	};
	struct Batch
	{
		Material* material;
		Mesh* mesh;
		std::vector<glm::mat4> model;
		std::vector<glm::vec3> pos;
		RenderTime rendertime = RenderTime::defaultRender;
		SurfaceType surfacetype = SurfaceType::opaque;
		RenderQueue renderqueue = RenderQueue::ForwardOpaque;
	};
	void rs_Render(RenderContext& context);
	void rs_Init();
	void rs_Start();
	void rs_End();
	void rs_CollectBatches(RenderContext& context, std::vector<Batch>& batches);
	void rs_CreateShadowMap(Light* light);
	void rs_RenderBatchOpaque(RenderContext& context, std::vector<Batch>& batches);
	void rs_RenderSkyBox(RenderContext& context);
	void rs_RenderShadowMaps(RenderContext& context, std::vector<Batch>& batches);
	void rs_CopyDepthBufferFromGBuffer(RenderContext& context, Framebuffer* buffer);
	void rs_RenderBatchTransparent(RenderContext& context, std::vector<Batch>& batches);
	void rs_RenderMesh(Batch& batch, bool instance, Shader* shader, glm::mat4* modelMatrix = nullptr);
	void rs_RenderDeferredLighting(RenderContext& context, std::vector<Batch>& batches);
	void rs_LoadGBufferInShader(RenderContext& context, Shader* shader);
	void rs_RenderOutline(Render* render, Shader* outlineShader);
	void rs_UploadLighting(RenderContext& context);
	void rs_UploadCamera(RenderContext& context);
	void rs_PostProcess(RenderContext& context);
	void rs_Present(RenderContext& context);
};