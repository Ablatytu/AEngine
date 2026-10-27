# AEngine 
## A mini 2D/3D engine built with OpenGL 3.3, Dear ImGui, GLFW, GLM, and some spaghetti code :D
### Available on both macOS and Windows
## Project Status
### The engine is currently in early development. Bugs and rough edges are to be expected

## Screenshots & Demo
### Coming soon...
[AEngine Demo Videos](https://www.youtube.com/@Ablatytu_)
## Graphics Features:
- PBR
- Normal mapping
- Parallax mapping (POM)
- Shadow mapping (Directional & Point)
- GPU Instancing
- Particle System
- Deferred shading with light volumes
- Forward rendering
## Architecture & Scripting
- Entity-Component Architecture 
- C++ scripting system: C-ABI, Hot reload, Serialization, Reflection:
  ```cpp
  void OnInspector()
  {
    AE_REGISTER_FIELD(MyValue);
    AE_SET_RANGE(MyValue, 0.0f, 100.0f);
    AE_SET_COLOR(MyValue);
  }
  ```
- Scene Management
- Resource serialization
- Custom Shader DSL:
  ```glsl
  Properties
  {
    [Range(0.0, 1.0)] metallic = (Float, 0.0)
    [Range(0.0, 1.0)] roughness = (Float, 0.5)
    [Range(0.0, 1.0)] ao = (Float, 1.0)
    objectColor = (Color, (1.0, 1.0, 1.0, 1.0))
    texture_diffuse1 = (Texture2D, white)
    coefficientNormal = (Float, 0.0)
    InvertNormalY = (Bool, False)
    normalMap = (Texture2D, normal)
    Cubemap = (TextureCube, white)
    heightMap = (Texture2D, black) 
    [Range(0.0, 0.1)] height_scale = (Float, 0.0)
  }

  RenderPath "Deferred"

  GLSLPROGRAM
  #version 330 core
  #include "Shaders/Lighting.glsl"
  #include "Shaders/Core.glsl"
  ```
- World Editor
- Custom Post-Effects:
  ```cpp
  bool YourPostProcessClass::OnRenderImage(RenderImage* source, RenderImage* destination, Camera* cam)
  {
  	yourMaterial.Set("effect", EffectIsWorking);
  	if (!EffectIsWorking)
  		GraphicsAPI::Blit(source, destination, nullptr, cam);
  	else
  		GraphicsAPI::Blit(source, destination, &matCam, cam);
  	return true;
  }
  ```
## Requirements
- C++17
- CMake (version 3.25 or higher)
## Getting Started
```bash
git clone https://github.com/Ablatytu/AEngine
cd AEngine
cmake -S . -B build
cmake --build build -j
```

## License
MIT License

 
