#pragma once
#include <filesystem>

class GameObject;
class Mesh;
class Transform;
class Render;
class Framebuffer;
class Texture;
class Shader;
class Cubemap;
class Material;

class EditorGUI
{
public:
	struct SelectedObject
	{
		Render* selectedObjectRender = nullptr;
		GameObject* selectedObject = nullptr;
	};
	enum class SelectedAsset
	{
		None,
		Texture,
		Cubemap,
		Material
	};
	SelectedObject selectedObject;
	SelectedAsset selectedAsset = SelectedAsset::None;
	Texture* selectedTexture = nullptr;
	Cubemap* selectedCubemap = nullptr;
	Material* selectedMaterial = nullptr;
	Framebuffer* sceneHDR = nullptr;
	Framebuffer* viewport = nullptr;
	int sizeviewport[2] = { 800, 600 };
	float viewportBounds[2][2];
	bool viewportHovered = false;
	void BeginDockspace();
	void RenderMaterialPart(Material* material, bool& writed);
	void ShowInspectorWindow();
	void ShowHierarchyWindow();
	void RenderEditorParts();
	void RenderGizmo();
	void RenderViewport();
	void DrawDockspaceMenuBar();
	void ShowProject();
	void EDITOR_INIT();
	void ShowLightingMenu();
	void RenderOutline(Render* mesh);
private:
	std::filesystem::path projectDirectory = "Assets";
	std::filesystem::path currentDirectory = "Assets";
	std::unique_ptr<Texture> folderIcon = nullptr;
	std::unique_ptr<Texture> ModelIcon;
	std::unique_ptr<Texture> CubemapIcon;
	std::unique_ptr<Texture> MaterialIcon;
	std::unique_ptr<Shader> sceneHDRShader = nullptr;
	Transform* transform = nullptr;
	std::unique_ptr<Shader> OutlineShader = nullptr;
	bool showLightingMenu = false;
};