#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/draw.h"
#include "observable.h"
#include "graphics/window.h"

int main() {
    initGLFW();
    std::unique_ptr<WindowProperties> windowProperties = createWindow("Window", 800, 600);
    Subscription windowSub = windowProperties->callbacks.mouseButton.subscribe([&](WindowProperties::MouseButton b) {
        std::cout << b.button << std::endl;
    });
    windowSub += windowProperties->callbacks.windowClose.subscribe([&](auto) {
        std::cout << "CLOSE" << std::endl;
    });

    while(!glfwWindowShouldClose(windowProperties->window)) {
        glfwPollEvents();
        glfwSwapBuffers(windowProperties->window);
    }

    glfwTerminate();
    return 0;
    /*
    createGLFW();

    std::unique_ptr<WindowContext> context = createWindow("Window", 800, 600);
    auto window = context->window;

    draw::init();
    draw::initFont("arial", 32);
    draw::initTexture("res/animation/stickman.png", {});

    glm::mat4 ortho;

    context->sizeChange = [&](GLFWwindow *win, int w, int h) {
        glViewport(0, 0, w, h);
        ortho = glm::ortho<double>(0, w, 0, h);
    };
    context->sizeChange(window, 800, 600);

    glm::mat4 sq = glm::translate(glm::mat4(1.0f), glm::vec3(150, 200, 0));
    sq = glm::scale(sq, glm::vec3(200, 300, 0));
    glm::mat4 man = glm::translate(glm::mat4(1.0f), glm::vec3(150, 200, 0));
    man = glm::scale(man, glm::vec3(200, 200, 0));

    while(!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);
        draw::square(ortho * sq);
        draw::text("arial", 32, "Hello world!", glm::translate(ortho, glm::vec3(300, 300, 0)));
        draw::bindTexture("res/animation/stickman.png");
        draw::image(ortho * man);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "Hello, World!" << std::endl;

    draw::destroy();
    glfwTerminate();

    return 0;
     */
}