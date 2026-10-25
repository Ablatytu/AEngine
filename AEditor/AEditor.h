#pragma once
#include <core/Application.h>
class EditorGUI;
class EditorCamera;
class Material; 
class RenderContext;
class Scene;

class AEditor : public Application
{
public:
	AEditor();
	~AEditor();
	void OnSceneLoaded() override;
private:
	void OnInit() override;
	void OnShutdown() override;
	void OnStartFrame() override;
	void OnBeforeSceneUpdate() override;
	void OnRender() override;
private:
	std::unique_ptr<EditorGUI> editorGUI;
	std::unique_ptr<EditorCamera> editorCamera;
	unsigned int frame = 0;
};