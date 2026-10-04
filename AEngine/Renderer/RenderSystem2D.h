#pragma once
#include "RenderContext2D.h"
#include "../core/AEngineAPI.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <vector>
class Framebuffer;
class SpriteRenderer2D;
class Material;
class Texture;

namespace RenderSystem2D
{
	struct Batch2D
	{
		std::vector<glm::mat4> models;
		std::vector<glm::vec4> colors;
		Texture* texture;
		Material* material;

		bool operator<(const Batch2D& other) const
		{
			if (material != other.material) return material < other.material;
			return texture < other.texture;
		}
	};
	struct Quad
	{
		unsigned int VAO = 0;
		unsigned int VBO = 0;
	};

	Quad quad{};

	AENGINE_API void rs2D_Init();
	AENGINE_API void rs2D_Render(RenderContext2D& context);
	void rs2D_RenderSprites(RenderContext2D& context, std::vector<Batch2D>& batches2D);
	void rs2D_Collect2DBatches(RenderContext2D& context, std::vector<Batch2D>& batches2D);
	AENGINE_API void rs2D_Start(Framebuffer* sceneHDR);
	AENGINE_API void rs2D_End();
	void rs2D_CalcView(RenderContext2D& context);
}