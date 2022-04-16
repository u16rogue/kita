cmake_minimum_required(VERSION 3.18)

if (NOT TARGET glfw)
	message(FATAL_ERROR "IMGUI_KITA can't find glfw target!")
endif()

project(
	IMGUI_KITA
	LANGUAGES CXX
	DESCRIPTION "ImGui library implementation for kita"
)

add_library(
	${PROJECT_NAME}
	STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3_loader.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_glfw.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h"
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui"
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	glfw
)