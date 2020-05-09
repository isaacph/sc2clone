cmake_minimum_required(VERSION 3.14)
project(untitled2)

set(CMAKE_CXX_STANDARD 17)

find_path(GLEW_INCLUDE_DIR GL/glew.h)
find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib64)

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(SDL_image REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

macro(add_res srcDir destDir)
    file(REMOVE_RECURSE ${destDir})
    file(COPY ${srcDir} DESTINATION ${destDir})
    message("Resources updated.")
endmacro(add_res)

add_executable(client
        main.cpp
        #main_testers/dragon_curve.cpp
        graphics/draw.cpp
        graphics/draw_square.cpp
        graphics/draw_text.cpp
        graphics/draw_image.cpp
        graphics/texture_load.cpp
        graphics/shader_util.cpp
        physics/Box.cpp
        physics/RotatedBox.cpp
        test/DragonCurve.cpp
        test/DragCamera.cpp
        test/Chatbox.cpp
        graphics/load_obj.cpp
        graphics/draw_model.cpp
        graphics/draw_model_textured.cpp
        test/FlyCamera.cpp
        test/OverheadCamera.cpp
        math_util.cpp
        server/listen.cpp
        server/response.cpp
        server/packet.cpp
        server/async_console.cpp
        )

add_executable(server
        server/server_main.cpp
        server/async_console.cpp
        server/listen.cpp
        server/response.cpp
        server/packet.cpp
        )

target_link_libraries(client glfw3 GLEW::GLEW opengl32 ${FREETYPE_LIBRARIES} ${SDL_IMAGE_LIBRARIES} ws2_32)
target_link_libraries(server ws2_32)
add_res(${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR})

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)
#target_link_libraries(untitled ${OPENGL_gl_LIBRARY} ${GLFW3_LIBRARY})