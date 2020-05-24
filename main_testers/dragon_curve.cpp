#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "../graphics/draw.h"
#include "../test/DragonCurve.h"
#include "../user_interface/DragCamera.h"

void loadObj(const std::string& path);

struct Game {
    GLFWwindow* window = nullptr;
    Graphics graphics;
    glm::mat4 proj = glm::mat4(1.0f), view = glm::mat4(1.0f);
    int width = 0, height = 0;

    DragonCurve dragonCurve;
    DragCamera camera;

    explicit Game(GLFWwindow* window) : window(window), dragonCurve(graphics), camera(window, view) {
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int new_width, int new_height) {
            ((Game*) glfwGetWindowUserPointer(win))->windowSize(new_width, new_height);
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
            ((Game*) glfwGetWindowUserPointer(win))->onMouse(button, action, mods);
        });
        glfwSetScrollCallback(window, [](GLFWwindow* win, double x, double y) {
            ((Game*) glfwGetWindowUserPointer(win))->onScroll(x, y);
        });
        glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
            ((Game*) glfwGetWindowUserPointer(win))->onKey(key, scancode, action, mods);
        });
    }

    void windowSize(int new_width, int new_height) {
        glViewport(0, 0, new_width, new_height);
        proj = glm::ortho<float>(0.0f, new_width, 0.0f, new_height, 0.0f, 1.0f);
        width = new_width;
        height = new_height;
        camera.onResize({new_width, new_width});
    }

    void onMouse(int button, int action, int mods) {
        camera.onMouse(button, action, mods);
    }

    void onScroll(double x, double y) {
        camera.onScroll(x, y);
    }

    void onKey(int key, int scancode, int action, int mods) {
        if(action == GLFW_PRESS) {
            if(key == GLFW_KEY_UP) {
                dragonCurve.add();
            } else if(key == GLFW_KEY_DOWN) {
                dragonCurve.subtract();
            }
        }
    }

    void run() {
        Graphics::Text text = graphics.initText("arial", 20);
        text.text = "Isaac Huffman";
        text.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

        //view = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 0)), glm::vec3(3, 3, 0));

        double current_time = glfwGetTime();
        double last_time = current_time;
        while(!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            current_time = glfwGetTime();
            double delta = current_time - last_time;
            last_time = current_time;

            text.matrix = proj * glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 0.0f));
            camera.update();

            text.draw();
            dragonCurve.draw(proj * view);

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
    glewInit();
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