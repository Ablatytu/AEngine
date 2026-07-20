#pragma once
#include <core/Application.h>
#include "../AEngine/Renderer/Material.h"
class EditorGUI;
class EditorCamera;
class Material; 
class RenderContext;

class AEditor : public Application
{
public:
	AEditor();
	~AEditor();
private:
	void OnInit() override;
	void OnShutdown() override;
	void OnBeforeSceneUpdate() override;
	void OnRender() override;
private:
	std::unique_ptr<EditorGUI> editorGUI;
	std::unique_ptr<EditorCamera> editorCamera;
	unsigned int frame = 0;
};