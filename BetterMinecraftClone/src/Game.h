#pragma once

#include "Window.h"
#include "Shader.h"
#include "ChunkMesh.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "WorldChunks.h"
#include "TextureLoader.h"
#include "BlockHighlight.h"

class Game {
public: 
	Game();
	void run();

private:
	void initialize();
	void createWindow();

	void render();
	void tick(); 

	Window* window;

	bool running = false;

	ShaderProgram* terrainRendererShaderProgram;

	WorldChunks* worldChunks;
	BlockHighlight* blockHighlight;

	// camera
	Camera* camera;
	TextureArray* textureAtlas;

	void createShaders();
	void createWorld();
	void loadTextures();
	
};