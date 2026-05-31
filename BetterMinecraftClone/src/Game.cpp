#include "Game.h"
#include <GLFW/glfw3.h>
#include "ChunkBuilder.h"
#include <iostream>
#include <array>

Game::Game() {

}

void Game::run() {
	initialize();

	running = true;

	while (running) {
		// game loop

		tick();
		render();

		if (window->shouldClose()) {
			running = false;
		}
	}

	window->terminate();
}

void Game::createWindow() {
	window = new Window(800, 600, "Better Minecraft Clone r3");
	glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1); // cap 60 fps
	
}

void Game::tick() {
	glm::vec3 offset = glm::vec3(0.0f);

	// update game state
	if (window->isKeyPressed(GLFW_KEY_W)) {
		offset.z += 0.2f;
	}
	if (window->isKeyPressed(GLFW_KEY_S)) {
		offset.z -= 0.2f;
	}
	if (window->isKeyPressed(GLFW_KEY_A)) {
		offset.x -= 0.2f;
	}
	if (window->isKeyPressed(GLFW_KEY_D)) {
		offset.x += 0.2f;
	}
	
	double x;
	double y;

	glfwGetCursorPos(window->getWindow(), &x, &y);

	camera->moveLocal(offset);
	camera->update(x, y);

	worldChunks->update(camera->getPosition());

}

void Game::render() {
	glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldChunks->render(camera);

	// end
	window->update();
	window->pollEvents();
}

void Game::initialize() {

	// init window
	createWindow();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // DEBUG: WIREFRAME

	glViewport(0, 0, window->getWidth(), window->getHeight());

	camera = new Camera((float)window->getWidth() / window->getHeight(), 0.1f, 10000.0f, 70.0f);

	createShaders();
	loadTextures();
	createWorld();
	


}

void Game::createShaders() {
	Shader* vertexShader = new Shader("shaders/terrain.vert", GL_VERTEX_SHADER);
	Shader* fragmentShader = new Shader("shaders/terrain.frag", GL_FRAGMENT_SHADER);
	terrainRendererShaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
}

void Game::createWorld() {
	worldChunks = new WorldChunks(terrainRendererShaderProgram, textureAtlas);
}

void Game::loadTextures() {
	TextureLoader* loader = new TextureLoader(16);
	CompileTimeResult result = getRuntimeBlockTypes();
	for (std::string texture : result.uniqueTextures) {
		if (texture == "") break;
		std::cout << "Loading textur e: " << texture << std::endl;
		loader->addTexture(texture.c_str());
	}
	AtlasResult atlasRes = loader->buildAtlas();
	
	textureAtlas = new TextureArray();
	for (const std::string texturePath : getRuntimeBlockTypes().uniqueTextures) {
		if (texturePath == "") break;
		textureAtlas->addTexture(textureAtlas->loadTexture(texturePath));
	}
	textureAtlas->create();

	std::cout << "All textures loaded" << std::endl;

	delete loader;
}