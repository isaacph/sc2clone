//
// Created by isaac on 9/11/2019.
//

#include "window.h"

void initGLFW() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
}

WindowProperties::WindowProperties(GLFWwindow* window_in, std::string title_in, int width_in, int height_in) :
window(window_in), title(move(title_in)), width(width_in), height(height_in) {}

std::unique_ptr<WindowProperties> createWindow(const std::string& title, int width, int height)
{
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(window);

    auto properties = new WindowProperties(window, title, width, height);
    glfwSetWindowUserPointer(window, properties);
    std::unique_ptr<WindowProperties> ptr(properties);

    properties->initCallbacks();

    // enable VSYNC
    glfwSwapInterval(1);

    return ptr;
}

void WindowProperties::initCallbacks() {
    glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int x, int y){
        auto context = (WindowProperties*) glfwGetWindowUserPointer(w);
        context->width = x;
        context->height = y;
        context->callbacks.sizeChangeEmitter.emit(glm::ivec2(x, y));
    });
    glfwSetWindowCloseCallback(window, [](GLFWwindow* w){
        auto context = (WindowProperties*) glfwGetWindowUserPointer(w);
        context->callbacks.windowCloseEmitter.emit({});
    });
    glfwSetKeyCallback(window, [](GLFWwindow* w, int k, int s, int a, int m) {
        auto context = (WindowProperties*) glfwGetWindowUserPointer(w);
        context->callbacks.keyEmitter.emit(Key{k, s, a, m});
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int b, int a, int m) {
        auto context = (WindowProperties*) glfwGetWindowUserPointer(w);
        context->callbacks.mouseButtonEmitter.emit(MouseButton{b, a, m});
    });
    glfwSetCharCallback(window, [](GLFWwindow *w, Char c) {
        auto context = (WindowProperties *) glfwGetWindowUserPointer(w);
        context->callbacks.characterEmitter.emit(c);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
        auto context = (WindowProperties*) glfwGetWindowUserPointer(w);
        context->callbacks.cursorPosEmitter.emit(glm::dvec2{x, y});
    });
    glfwSetScrollCallback(window, [](GLFWwindow* w, double sx, double sy) {
        auto context = (WindowProperties*) glfwGetWindowUserPointer(w);
        context->callbacks.scrollEmitter.emit(glm::dvec2{sx, sy});
    });
}

 /*
GLFWwindow* WindowContext::shareWindow() {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* threadWindow = glfwCreateWindow(1, 1, "Thread Window", NULL, window);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    return threadWindow;
}
 */