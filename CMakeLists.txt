cmake_minimum_required(VERSION 3.18)

# TODO: make internal ImGui dep an option, if imgui was already
# included as a dependency by the root cmake project, allow it 
# to modify kita to use that instead

# TODO: Automatically run bootstrapper by using add_custom_command

project(
	kita
	LANGUAGES CXX C
	VERSION 1.0
	DESCRIPTION "Cross-platform quick deployment implementation of user interfaces with ImGui, GLFW and OpenGL."
)

set(CMAKE_CXX_STANDARD 20)

option(KITA_LOCAL_TEST "Local test project" OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw")
find_package(OpenGL REQUIRED)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui_kita")

add_library(
	${PROJECT_NAME}

	"${CMAKE_CURRENT_SOURCE_DIR}/include/kita/kita.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/kita.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/kita/events.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/kita/helpers.hpp"
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
	glfw
	IMGUI_KITA
)

if (KITA_LOCAL_TEST)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/local_test")
endif()