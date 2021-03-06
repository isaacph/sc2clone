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
        graphics/draw.cpp
        graphics/draw_square.cpp
        graphics/draw_text.cpp
        graphics/draw_image.cpp
        graphics/texture_load.cpp
        graphics/shader_util.cpp
        user_interface/Chatbox.cpp
        graphics/load_obj.cpp
        graphics/draw_model.cpp
        graphics/draw_model_textured.cpp
        user_interface/FlyCamera.cpp
        user_interface/OverheadCamera.cpp
        user_interface/FocusMode.cpp
        math_util.cpp
        server/Packet.cpp
        server/ServerNetworking.cpp
        logic/World.cpp
        logic/Game.cpp
        logic/GameCommands.cpp
        logic/SelectMath.cpp
        )

add_executable(server
        server/ServerMain.cpp
        server/Packet.cpp
        logic/World.cpp
        server/Server.cpp
        server/ServerCommands.cpp
        server/ServerNetworking.cpp
        )

add_executable(test
        test.cpp)

add_executable(test2
        test2.cpp
        server/ServerNetworking.cpp
        server/Packet.cpp)

target_link_libraries(client glfw3 GLEW::GLEW opengl32 ${FREETYPE_LIBRARIES} ${SDL_IMAGE_LIBRARIES} ws2_32)
target_link_libraries(server ws2_32)
target_link_libraries(test ws2_32)
target_link_libraries(test2 ws2_32)
add_res(${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR})

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)
#target_link_libraries(untitled ${OPENGL_gl_LIBRARY} ${GLFW3_LIBRARY})