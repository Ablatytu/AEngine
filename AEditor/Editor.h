#pragma once
#include <filesystem>

class GameObject;
class Mesh;
class Transform;
class Render;
class Framebuffer;
class Texture;
class Shader;

class EditorGUI
{
public:
	struct SelectedObject
	{
		Render* selectedObjectRender = nullptr;
		GameObject* selectedObject = nullptr;
	};
	SelectedObject selectedObject;
	Framebuffer* viewport = nullptr;
	Framebuffer* sceneHDR = nullptr;
	Texture* selectedTexture = nullptr;
	int sizeviewport[2] = { 800, 600 };
	float viewportBounds[2][2];
	bool viewportHovered = false;
	void BeginDockspace();
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
	Transform* transform = nullptr;
	Shader* OutlineShader = nullptr;
	bool showLightingMenu = false;
};