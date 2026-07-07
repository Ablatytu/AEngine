#pragma once
class GameObject;
class Mesh;
class Transform;
class Render;

class EditorGUI
{
public:
	struct SelectedObject
	{
		Render* selectedObjectRender = nullptr;
		GameObject* selectedObject = nullptr;
	};
	SelectedObject selectedObject;
	void ShowInspectorWindow();
	void ShowHierarchyWindow();
	void RenderEditorParts();
	void RenderGizmo();
	void RenderOutline(Render* mesh);
};