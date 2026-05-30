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

set(GLFW_INSTALL OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glfw)

# Add source
add_executable (BetterMinecraftClone "BetterMinecraftClone.cpp" "BetterMinecraftClone.h" "Window.cpp" "Window.h" "glad/src/glad.c" "Game.cpp" "Game.h" "ChunkMesh.cpp" "ChunkMesh.h" "Shader.h" "Shader.cpp" "ShaderProgram.h" "ShaderProgram.cpp" "Camera.h" "Camera.cpp" "ChunkBuilder.h" "config.h" "ChunkBuilder.cpp" "MathUtils.h" "WorldChunks.h" "Chunk.h" "Chunk.cpp" "WorldChunks.cpp" "TerrainGenerator.h" "TerrainGenerator.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BetterMinecraftClone PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(BetterMinecraftClone PRIVATE "glad/include")
target_include_directories(BetterMinecraftClone PRIVATE "external/glm")
target_include_directories(BetterMinecraftClone PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external)

# Link
target_link_libraries(BetterMinecraftClone PRIVATE glfw)

# Copy
add_custom_command(
    TARGET BetterMinecraftClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
            "$<TARGET_FILE_DIR:BetterMinecraftClone>/shaders"
)

# TODO: Add tests and install targets if needed.


