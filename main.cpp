#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/draw.h"
//#include "physics/RigidBody.h"

struct Game {
    GLFWwindow* window = nullptr;
    Graphics graphics;
    glm::mat4 proj{};
    int width{}, height{};

    explicit Game(GLFWwindow* window) : window(window) {
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int new_width, int new_height) {
            ((Game*) glfwGetWindowUserPointer(win))->windowSize(new_width, new_height);
        });
    }

    void windowSize(int new_width, int new_height) {
        proj = glm::ortho<float>(0.0f, new_width, 0.0f, new_height, 0.0f, 1.0f);
        width = new_width;
        height = new_height;
    }

    void run() {
        Graphics::Text text = graphics.initText("arial", 16);
        text.matrix = proj * glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 0.0f));
        text.text = "testing test 1213";
        text.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

        Graphics::Rectangle rectangle = graphics.initRectangle();
        rectangle.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        rectangle.matrix = proj * glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(300, 300, 0)), glm::vec3(100, 100, 0));

        Graphics::Image image = graphics.initImage();
        image.color = glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
        image.matrix = proj * glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(300, 300, 0)), glm::vec3(100, 100, 0));
        image.image = "res/animation/stickman.png";

        while(!glfwWindowShouldClose(window)) {
            rectangle.draw();
            text.draw();
            image.draw();

            glfwPollEvents();
            glfwSwapBuffers(window);
        }
    }
};

int main() {
    const glm::ivec2 STARTING_WINDOW_SIZE(800, 600);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(STARTING_WINDOW_SIZE.x, STARTING_WINDOW_SIZE.y, "Render Engine",
                                          nullptr, nullptr);

    glfwMakeContextCurrent(window);

    // enable VSYNC
    glfwSwapInterval(1);

    // init GL
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
    errorCheckGl("draw pre-init");

    Game game(window);
    game.windowSize(STARTING_WINDOW_SIZE.x, STARTING_WINDOW_SIZE.y);
    game.run();

    glfwTerminate();

    return 0;
}