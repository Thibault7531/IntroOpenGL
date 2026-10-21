cmake_minimum_required(VERSION 4.3)
project(IntroOpenGL)

set(CMAKE_CXX_STANDARD 26)

add_compile_options(-mavx2)

# Libraries
add_subdirectory(Vendor/assimp-6.0.5)
add_subdirectory(Vendor/glfw-3.5.1)
add_subdirectory(Vendor/glm)
add_library(ImGui STATIC Vendor/imgui/imgui.cpp Vendor/imgui/imgui_demo.cpp Vendor/imgui/imgui_draw.cpp Vendor/imgui/imgui_widgets.cpp Vendor/imgui/imgui_tables.cpp Vendor/imgui/backends/imgui_impl_glfw.cpp Vendor/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(ImGui PUBLIC Vendor/imgui)

add_library(glad STATIC Vendor/glad/src/gl.c)
target_include_directories(glad PUBLIC Vendor/glad/include)

add_custom_target(OpenGLCopyImages
                  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:IntroOpenGL>/assets"
                  COMMENT "Copying assets to build directory..."
)

file(GLOB_RECURSE SOURCE_FILES Source/*.cpp)
add_executable(IntroOpenGL ${SOURCE_FILES}
               Source/Camera/KeyFrameAnimation.h
               Source/Camera/KeyFrameAnimation.cpp)
target_link_libraries(IntroOpenGL assimp ImGui glfw glad glm)
target_include_directories(IntroOpenGL PUBLIC
                           Vendor/assimp-6.0.5/include
                           Vendor/imgui
                           Vendor/imgui/backends
                           Vendor/glfw-3.5.1/includeb
                           Vendor/glad/include
                           Vendor/glm
                           Vendor/stbimage
)
add_dependencies(IntroOpenGL OpenGLCopyImages)
