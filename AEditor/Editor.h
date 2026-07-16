#pragma once
class GameObject;
class Mesh;
class Transform;
class Render;
class Framebuffer;

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
	int sizeviewport[2] = { 800, 600 };
	float viewportBounds[2][2];
	bool viewportHovered = false;
	void BeginDockspace();
	void ShowInspectorWindow();
	void ShowHierarchyWindow();
	void RenderEditorParts();
	void RenderGizmo();
	void RenderViewport();
	void EDITOR_INIT();
	void RenderOutline(Render* mesh);
};