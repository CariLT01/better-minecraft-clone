#include "Game.h"
#include <GLFW/glfw3.h>
#include "ChunkBuilder.h"
#include <iostream>
#include <array>
#include <memory>

Game::Game() : blockHighlight(new BlockHighlight()) {

}

Game::~Game() {

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

void Game::mouseButtonPressedCallback(GLFWwindow* window, int button, int action, int mods) {
	Game* gameInstance = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (!gameInstance->blockHighlight->getHit()) return;
	// get current look
	glm::vec3 hitPos = gameInstance->blockHighlight->getHitPos();
	
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		gameInstance->worldChunks->setBlockAt(hitPos.x, hitPos.y, hitPos.z, 0);
	}

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
		glm::vec3 placementOffset = gameInstance->blockHighlight->getHitNormal();

		
		gameInstance->worldChunks->setBlockAt(hitPos.x + placementOffset.x, hitPos.y + placementOffset.y, hitPos.z + placementOffset.z, 4);
	}

	
}

void Game::createWindow() {
	window = std::make_shared<Window>(800, 600, "Better Minecraft Clone r3");
	glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1); // cap 60 fps
	glfwSetWindowUserPointer(window->getWindow(), this);
	glfwSetMouseButtonCallback(window->getWindow(), mouseButtonPressedCallback);
	
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
	blockHighlight->update(camera, worldChunks);

}

void Game::render() {
	glClearColor(122.0f / 255.0, 196.0f / 255.0f, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldChunks->render(camera);
	blockHighlight->render(camera);

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // DEBUG: WIREFRAME

	glViewport(0, 0, window->getWidth(), window->getHeight());

	camera = std::make_shared<Camera>((float)window->getWidth() / window->getHeight(), 0.1f, 10000.0f, 70.0f);

	createShaders();
	loadTextures();
	createWorld();
	blockHighlight->initialize();
	


}

void Game::createShaders() {
	std::shared_ptr<Shader> vertexShader = std::make_shared<Shader>(static_cast<const char*>("shaders/terrain.vert"), static_cast<unsigned int>(GL_VERTEX_SHADER));
	std::shared_ptr<Shader> fragmentShader = std::make_shared<Shader>(static_cast<const char*>("shaders/terrain.frag"), static_cast<unsigned int>(GL_FRAGMENT_SHADER));

	std::vector<std::shared_ptr<Shader>> shaders = { vertexShader, fragmentShader };

	terrainRendererShaderProgram = std::make_shared<ShaderProgram>(shaders);
}

void Game::createWorld() {
	worldChunks = std::make_shared<WorldChunks>(terrainRendererShaderProgram, textureAtlas);
}

void Game::loadTextures() {
	std::unique_ptr<TextureLoader> loader = std::make_unique<TextureLoader>(16);
	CompileTimeResult result = getRuntimeBlockTypes();
	for (std::string texture : result.uniqueTextures) {
		if (texture == "") break;
		std::cout << "Loading textur e: " << texture << std::endl;
		loader->addTexture(texture.c_str());
	}
	AtlasResult atlasRes = loader->buildAtlas();
	
	textureAtlas = std::make_shared<TextureArray>();
	for (const std::string texturePath : getRuntimeBlockTypes().uniqueTextures) {
		if (texturePath == "") break;
		textureAtlas->addTexture(textureAtlas->loadTexture(texturePath));
	}
	textureAtlas->create();

	std::cout << "All textures loaded" << std::endl;
}