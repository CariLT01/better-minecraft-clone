# CMakeList.txt : CMake project for BetterMinecraftClone, include source and define
# project specific logic here.
#

# Add source to this project's executable.

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4 # Use the latest stable version/tag
    GIT_SHALLOW    ON
)

FetchContent_Declare(
    FastNoise2
    GIT_REPOSITORY https://github.com/Auburn/FastNoise2.git
    GIT_TAG        master # Or specify a specific release tag/commit hash
)

set(GLFW_INSTALL OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(FastNoise2)

# Add source
add_executable (BetterMinecraftClone "src/BetterMinecraftClone.cpp" "src/BetterMinecraftClone.h" "src/Window.cpp" "src/Window.h" "glad/src/glad.c" "src/Game.cpp" "src/Game.h" "src/ChunkMesh.cpp" "src/ChunkMesh.h" "src/Shader.h" "src/Shader.cpp" "src/ShaderProgram.h" "src/ShaderProgram.cpp" "src/Camera.h" "src/Camera.cpp" "src/ChunkBuilder.h" "src/config.h" "src/ChunkBuilder.cpp" "src/MathUtils.h" "src/WorldChunks.h" "src/Chunk.h" "src/Chunk.cpp" "src/WorldChunks.cpp" "src/TerrainGenerator.h" "src/TerrainGenerator.cpp" "src/TextureLoader.h" "src/TextureLoader.cpp" "src/BlockTypes.h" "src/Texture.h" "src/Texture.cpp" "src/TextureArray.cpp" "src/TextureArray.h" "src/ChunkBuilderWorkerScheduler.h"  "src/ChunkBuilderWorkerScheduler.cpp" "src/WorldGeneratorScheduler.h" "src/WorldGeneratorScheduler.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BetterMinecraftClone PROPERTY CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(BetterMinecraftClone PRIVATE "glad/include")
target_include_directories(BetterMinecraftClone PRIVATE "external/glm")
target_include_directories(BetterMinecraftClone PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external)

# Link
target_link_libraries(BetterMinecraftClone PRIVATE glfw)
target_link_libraries(BetterMinecraftClone PRIVATE FastNoise)

# Copy
add_custom_command(
    TARGET BetterMinecraftClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
            "$<TARGET_FILE_DIR:BetterMinecraftClone>/shaders"
)

add_custom_command(
    TARGET BetterMinecraftClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/textures"
            "$<TARGET_FILE_DIR:BetterMinecraftClone>/textures"
)

# TODO: Add tests and install targets if needed.


