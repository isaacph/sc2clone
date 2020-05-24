#include <iostream>
#include "logic/Game.h"

int main() {
    const glm::ivec2 STARTING_WINDOW_SIZE(800, 600);

    glfwInit();

//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(STARTING_WINDOW_SIZE.x, STARTING_WINDOW_SIZE.y, "Render Engine",
                                          nullptr, nullptr);

    glfwMakeContextCurrent(window);

    // enable VSYNCtest
    glfwSwapInterval(1);

    // init GL
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glClearColor(0, 0, 0, 0);
    errorCheckGl("draw pre-init");

    Game game(window);

    game.communication.port = 3801;
    init_socket(game.communication.port,
                game.communication.timeout_ms,
                game.communication.socket,
                game.communication.address, true);

    pthread_t listen_pthread, response_pthread;

    pthread_create(&listen_pthread, NULL, listen_thread, &game.communication);
    pthread_create(&response_pthread, NULL, response_thread, &game.communication);

    game.windowSize(STARTING_WINDOW_SIZE.x, STARTING_WINDOW_SIZE.y);
    game.run();

    glfwTerminate();

    game.communication.exit = true;
    pthread_join(listen_pthread, NULL);
    pthread_join(response_pthread, NULL);
    close_socket(game.communication.socket);

    return 0;
}