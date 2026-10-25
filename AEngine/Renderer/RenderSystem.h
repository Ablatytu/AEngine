#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "../core/AEngineAPI.h"
class Scene;
class Material;
class Mesh;
class Render;
class RenderContext;
class Shader;
class Light;
class Framebuffer;
class HDRTexture;
class Cubemap;
class Texture;
class Camera;

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
	namespace GameCommands
	{
		struct RenderFramebufferWithCustomMaterial
		{
			Material* customMaterial;
			Framebuffer* source;
			Framebuffer* out;
		};
		extern RenderFramebufferWithCustomMaterial RFWCM;
	};
	struct Batch2DIn3D
	{
		std::vector<glm::mat4> models;
		std::vector<glm::vec4> colors;
		Texture* texture;
		Material* material;
		int layerZ = 0;

		bool operator<(const Batch2DIn3D& other) const
		{
			if (material != other.material) return material < other.material;
			if (layerZ != other.layerZ) return layerZ < other.layerZ;
			return texture < other.texture;
		}
	};
	struct Quad
	{
		unsigned int VAO = 0;
		unsigned int VBO = 0;
	};
	struct QuadParticleMesh
	{
		unsigned int VAO = 0;
		unsigned int VBO = 0;

		unsigned int InstanceBuffer = 0;
	};

	AENGINE_API void rs_Render(RenderContext& context, Framebuffer* sceneHDR);
	AENGINE_API void rs_Init();
	void rs_InitAfterFirstFrame();
	AENGINE_API void rs_Start(Framebuffer* sceneHDR);
	AENGINE_API void rs_End();
	void rs_CollectBatches(RenderContext& context, std::vector<Batch>& batches);
	void rs_CollectBatches2DIn3D(RenderContext& context, std::vector<Batch2DIn3D>& batches2D);
	void rs_CreateShadowMap(Light* light);
	void rs_RenderBatchOpaque(RenderContext& context, std::vector<Batch>& batches);
	void rs_RenderBatchSprites2DIn3DOpaque(RenderContext& context, std::vector<Batch2DIn3D>& batches2D);
	void rs_RenderSkyBox(RenderContext& context);
	void rs_RenderShadowMaps(RenderContext& context, std::vector<Batch>& batches);
	void rs_CopyDepthBufferFromGBuffer(RenderContext& context, Framebuffer* buffer);
	AENGINE_API void rs_CopyColorBufferToOtherBuffer(Framebuffer* frameBufferFrom, Framebuffer* frameBufferTo, int width, int height);
	void rs_RenderBatchTransparent(RenderContext& context, std::vector<Batch>& batches);
	void rs_LoadShadowsInShader(RenderContext& context, Shader* shader, unsigned int& slotTextureShadow);
	void rs_RenderSSAO(RenderContext& context);
	void rs_RenderMesh(Batch& batch, bool instance, Shader* shader, glm::mat4* modelMatrix = nullptr);
	void rs_RenderDeferredLighting(RenderContext& context, std::vector<Batch>& batches);
	void rs_LoadGBufferInShader(RenderContext& context, Shader* shader, std::vector<int> buffers);
	AENGINE_API void rs_RenderOutline(Render* render, Shader* outlineShader);
	void rs_UploadLighting(RenderContext& context);
	void rs_UploadCamera(RenderContext& context);
	AENGINE_API void rs_PostProcess(RenderContext& context, Framebuffer* sceneHDR);
	void rs_RenderEnvironmentHDR(Cubemap& cubemap, HDRTexture& hdrTexture, int width, int height);
	void rs_RenderIrradianceMap(Cubemap& envcubemap, Cubemap& irradianceCubemap, int width, int height);
	void rs_RenderPrefilterMap(Cubemap& envcubemap, Cubemap& prefilterCubemap);
	void rs_CreateBRDFTexture();
	void rs_RenderQuad();
	void rs_SetEIPMap(RenderContext& context, Shader* shader, unsigned int& slot);
	void rs_CreateCameraFramebuffers(Camera& camera);
	void rs_CreateSpriteMesh(Quad& quadMesh);
	void rs_CollectbuffersFromCamera(Camera& camera);
	void rs_RenderParticles(RenderContext& context);
	void rs_CreateParticleMesh(QuadParticleMesh& particleMesh);
	void rs_AppendPostProcess(RenderContext& context, Framebuffer* sceneHDR);
	AENGINE_API void rs_AddGameCommand(GameCommands::RenderFramebufferWithCustomMaterial newRFWCM);
	AENGINE_API void rs_ExecuteGameCommands(RenderContext& context);
	AENGINE_API void rs_Present(RenderContext& context, Framebuffer* sceneHDR, Framebuffer* targetHDR = nullptr);
	AENGINE_API void rs_ClearFramebuffer(Framebuffer* ClearFramebuffer, float width, float height, unsigned int mask);
};