#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "resources/import.h"

int main() {

    glfwSetErrorCallback([](int error, auto message) {
        std::cerr << "glfw error (" << error << ") " << message << "\n";
    });
    if(!glfwInit()) {
        std::cerr << "glfwInit failed\n";
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", NULL, NULL);

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(win, GLFW_TRUE);
        }
    });

    glewExperimental = true;
    glewInit();

    glClearColor(1, 0, 0, 1);

    Dependencies dependencies = loadDependencies("res.dat");
    for(int i = 0; i < dependencies.images.size(); ++i) {
        std::cout << dependencies.images[i].name << ", ";
        std::cout << dependencies.images[i].width << ", " << dependencies.images[i].height << "\n";
    }

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    std::cout << "Hello World!" << std::endl;
    return 0;
}