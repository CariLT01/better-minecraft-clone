#pragma once

#include "Window.h"
#include "Shader.h"
#include "ChunkMesh.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "WorldChunks.h"
#include "TextureLoader.h"
#include "BlockHighlight.h"
#include <memory>

class Game {
public: 
	Game();
	~Game();
	void run();

private:
	void initialize();
	void createWindow();

	void render();
	void tick();

	static void mouseButtonPressedCallback(GLFWwindow* window, int button, int action, int mods);

	std::shared_ptr<Window> window;

	bool running = false;

	std::shared_ptr<ShaderProgram> terrainRendererShaderProgram;

	std::shared_ptr<WorldChunks> worldChunks;
	std::shared_ptr<BlockHighlight> blockHighlight;

	// camera
	std::shared_ptr<Camera> camera;
	std::shared_ptr<TextureArray> textureAtlas;

	void createShaders();
	void createWorld();
	void loadTextures();
	
	void setupUserInterface();
};