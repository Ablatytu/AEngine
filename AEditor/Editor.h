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
class Camera;

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
	int sizeviewport[2] = { 800, 600 };
	int sizegameport[2] = { 800, 600 };
	float gameportBounds[2][2];
	float viewportBounds[2][2];
	bool viewportHovered = false;
	void BeginDockspace();
	void RenderMaterialPart(Material* material, bool& writed);
	void ShowInspectorWindow();
	void ShowHierarchyWindow();
	void DrawHierarchyObject(GameObject* object);
	void RenderEditorParts();
	void RenderGizmo(Camera* camera);
	void RenderViewport(Camera* camera);
	void RenderGameViewport(Camera* camera);
	void DrawToolbar();
	void DrawDockspaceMenuBar();
	void ShowProject();
	void EDITOR_INIT();
	void ShowLightingMenu();
	void SetGameMode();
	void SetEditMode();
	void RenderOutline(Render* mesh);
private:
	std::filesystem::path projectDirectory = "Assets";
	std::filesystem::path currentDirectory = "Assets";
	std::unique_ptr<Texture> folderIcon = nullptr;
	std::unique_ptr<Texture> ModelIcon;
	std::unique_ptr<Texture> CubemapIcon;
	std::unique_ptr<Texture> MaterialIcon;
	std::unique_ptr<Texture> PlayIcon;
	std::unique_ptr<Texture> StopIcon;
	Transform* transform = nullptr;
	std::unique_ptr<Shader> OutlineShader = nullptr;
	bool showLightingMenu = false;
	std::string snapshotScene = "";
	std::string pastScenePathBeforePlayMode = "";
	enum class SceneMode
	{
		Edit,
		Play
	};
	SceneMode sceneMode = SceneMode::Edit;
};