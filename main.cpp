#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/draw.h"
#include "test/DragonCurve.h"
#include "test/DragCamera.h"
#include "test/FlyCamera.h"
#include "test/OverheadCamera.h"
#include "math_util.h"

std::ostream& operator<<(std::ostream& os, glm::mat4 mat) {
    for(int j = 0; j < 4; j++) {
        for(int i = 0; i < 4; i++) {
            os << mat[i][j] << ", ";
        }
        os << "\n";
    }
    os << "]\n";
    return os;
}

struct Game {
    GLFWwindow* window = nullptr;
    Graphics graphics;
    glm::mat4 ortho = glm::mat4(1.0f), view = glm::mat4(1.0f), persp = glm::mat4(1.0f);
    int width = 0, height = 0;
    glm::vec2 mouse;
    glm::vec2 mouse_world;
    glm::vec3 mouse_dir;
    FlyCamera flyCamera = FlyCamera(view);
    OverheadCamera overheadCamera = OverheadCamera(view);
    bool useFlyCamera = false;
    bool useOverheadCamera = false;
    glm::vec2 drag_start;
    bool left_click = false;
    bool drag = false;

    Graphics::PlainModel workerModel = graphics.initPlainModel("res/cube.obj");

    struct Unit {
        enum Type {
            WORKER, BARRACKS, SOLDIER
        } type;
        int team;
        float radius;
        glm::vec2 position;
        float direction = 0;
        float health = 100;
        bool dead = false;
    };

    std::map<int, Unit> units;
    std::set<int> unitsSelected;

    std::vector<glm::vec3> fp;
    std::vector<glm::vec3> tp;

    explicit Game(GLFWwindow* window) : window(window) {
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int new_width, int new_height) {
            ((Game*) glfwGetWindowUserPointer(win))->windowSize(new_width, new_height);
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
            ((Game*) glfwGetWindowUserPointer(win))->onMouse(button, action, mods);
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
            ((Game*) glfwGetWindowUserPointer(win))->onCursorPos(x, y);
        });
        glfwSetScrollCallback(window, [](GLFWwindow* win, double x, double y) {
            ((Game*) glfwGetWindowUserPointer(win))->onScroll(x, y);
        });
        glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
            ((Game*) glfwGetWindowUserPointer(win))->onKey(key, scancode, action, mods);
        });
        flyCamera.disable(window);
        overheadCamera.disable(window);
    }

    void windowSize(int new_width, int new_height) {
        glViewport(0, 0, new_width, new_height);
        ortho = glm::ortho<float>(0.0f, new_width, 0.0f, new_height, 0.0f, 1.0f);
        persp = glm::perspective(60.0f * 3.14159265359f / 180.0f, (float) new_width / (float) new_height, 0.1f, 1000.0f);
        width = new_width;
        height = new_height;
        overheadCamera.onWindowSize(window, glm::vec2(new_width, new_height));
    }

    void onMouse(int button, int action, int mods) {
        if(action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT && useOverheadCamera) {
            unsigned int closestSelected;
            bool foundUnit = false;
            for (auto &p : units) {
                unsigned int unitID = p.first;
                auto &unit = p.second;
                if (unit.type == Unit::WORKER) {
                    bool intersects = false;
                    glm::vec3 cam_pos = overheadCamera.camera_position;
                    glm::vec3 dir = mouse_dir;
                    workerModel.position = glm::vec3(unit.position.x, 1, unit.position.y);
                    workerModel.scale = glm::vec3(1);
                    const std::vector<Triangle> &triangles = workerModel.getTriangles();
                    for (size_t i = 0; i < triangles.size(); ++i) {
                        const auto &triangle = triangles[i];
                        glm::vec4 p1_4 = workerModel.getModelMatrix() *
                                         glm::vec4(triangle.p1.x, triangle.p1.y, triangle.p1.z, 1);
                        glm::vec4 p2_4 = workerModel.getModelMatrix() *
                                         glm::vec4(triangle.p2.x, triangle.p2.y, triangle.p2.z, 1);
                        glm::vec4 p3_4 = workerModel.getModelMatrix() *
                                         glm::vec4(triangle.p3.x, triangle.p3.y, triangle.p3.z, 1);
                        glm::vec3 p1 = glm::vec3(p1_4.x / p1_4.w, p1_4.y / p1_4.w, p1_4.z / p1_4.w);
                        glm::vec3 p2 = glm::vec3(p2_4.x / p2_4.w, p2_4.y / p2_4.w, p2_4.z / p2_4.w);
                        glm::vec3 p3 = glm::vec3(p3_4.x / p3_4.w, p3_4.y / p3_4.w, p3_4.z / p3_4.w);
                        if (line_intersects_triangle(cam_pos, mouse_dir, {p1, p2, p3})) {
                            intersects = true;
                        }
                    }
                    if (intersects) {
                        if(foundUnit) {
                            if(unit.position.y > units[closestSelected].position.y)
                            {
                                closestSelected = unitID;
                            }
                        } else {
                            foundUnit = true;
                            closestSelected = unitID;
                        }
                    }
                }
            }
            unitsSelected = {};
            if(foundUnit) {
                unitsSelected.insert(closestSelected);
            }
        }
        if(button == GLFW_MOUSE_BUTTON_LEFT) {
            left_click = action > 0;
            if(useOverheadCamera) {
                if (action == GLFW_PRESS) {
                    drag_start = mouse;
                }
                if (action == GLFW_RELEASE) {
                    if(drag) {
                        drag = false;
                        glm::vec3 cam_pos = overheadCamera.camera_position;
                        glm::vec2 p1d = drag_start;
                        glm::vec2 p3d = mouse;
                        glm::vec2 p2d(drag_start.x, mouse.y);
                        glm::vec2 p4d(mouse.x, drag_start.y);
                        std::vector<glm::vec3> dirs = {
                                screen_space_to_overhead_dir(p1d),
                                screen_space_to_overhead_dir(p2d),
                                screen_space_to_overhead_dir(p3d),
                                screen_space_to_overhead_dir(p4d)
                        };
                        tp = {};
                        fp = {};
                        for (auto &p : units) {
                            unsigned int unitID = p.first;
                            auto &unit = p.second;
                            if (unit.type == Unit::WORKER) {
                                bool intersects = false;
                                workerModel.position = glm::vec3(unit.position.x, 1, unit.position.y);
                                workerModel.scale = glm::vec3(1);
                                const std::vector<Triangle> &triangles = workerModel.getTriangles();
                                for (size_t i = 0; i < triangles.size(); ++i) {
                                    const auto &triangle = triangles[i];
                                    glm::vec4 p1_4 = workerModel.getModelMatrix() *
                                                     glm::vec4(triangle.p1.x, triangle.p1.y, triangle.p1.z, 1);
                                    glm::vec4 p2_4 = workerModel.getModelMatrix() *
                                                     glm::vec4(triangle.p2.x, triangle.p2.y, triangle.p2.z, 1);
                                    glm::vec4 p3_4 = workerModel.getModelMatrix() *
                                                     glm::vec4(triangle.p3.x, triangle.p3.y, triangle.p3.z, 1);
                                    glm::vec3 p1 = glm::vec3(p1_4.x / p1_4.w, p1_4.y / p1_4.w, p1_4.z / p1_4.w);
                                    glm::vec3 p2 = glm::vec3(p2_4.x / p2_4.w, p2_4.y / p2_4.w, p2_4.z / p2_4.w);
                                    glm::vec3 p3 = glm::vec3(p3_4.x / p3_4.w, p3_4.y / p3_4.w, p3_4.z / p3_4.w);
                                    auto plane = get_plane(p1, p2, p3);
                                    std::vector<glm::vec2> fp2o;
                                    std::vector<glm::vec2> frust_points_2d = proj_frustum_plane(cam_pos, dirs, plane, fp2o);
                                    if (frustum_intersects_triangle(cam_pos, dirs, {p1, p2, p3})) {
                                        intersects = true;
                                        break;
                                        /*tp.push_back(plane.start);
                                        if(frust_points_2d.size() == 2) {
                                            glm::vec2 origin = proj_point_plane(cam_pos, plane);
                                            std::vector<glm::vec2> axes = {perp(frust_points_2d[0] - origin), perp(frust_points_2d[1] - origin)};
                                            std::vector<float> signs = {proj_mag(frust_points_2d[1] - origin, axes[0]),
                                                                        proj_mag(frust_points_2d[0] - origin, axes[1])};
                                            for(int i = 0; i < 2; ++i) {
                                                glm::vec2 po = frust_points_2d[i];
                                                glm::vec2 axis = glm::normalize(axes[i] * signs[i]);
                                                std::pair<float, float> s = shadow_min_max(std::vector<glm::vec2>(
                                                        plane.points_2d, plane.points_2d + 3), axis, origin);
                                                fp.push_back(plane.start + po.x * plane.axis1 + po.y * plane.axis2);
                                                tp.push_back(plane.start + (po.x + axis.x * 0.1f) * plane.axis1 + (po.y + axis.y * 0.1f) * plane.axis2);
                                                tp.push_back(plane.start + (po.x + axis.x * s.first) * plane.axis1 + (po.y + axis.y * s.first) * plane.axis2);
                                                tp.push_back(plane.start + (po.x + axis.x * s.second) * plane.axis1 + (po.y + axis.y * s.second) * plane.axis2);
                                                if(s.first < 0 && s.second < 0) {
                                                    fp.push_back(proj_line_plane(cam_pos, plane.norm, plane.start, plane.norm) + glm::vec3(0, 0.4, 0));
                                                }
                                            }
                                            fp.push_back(proj_line_plane(cam_pos, plane.norm, plane.start, plane.norm));
                                            glm::vec2 axis_start = (frust_points_2d[0] + frust_points_2d[1]) / 2;
                                            glm::vec2 axis = perp(frust_points_2d[1] - frust_points_2d[0]);
                                            axis = glm::normalize(axis * glm::dot(axis, origin - axis_start));
                                            std::pair<float, float> s = shadow_min_max(std::vector<glm::vec2>(
                                                    plane.points_2d, plane.points_2d + 3), axis, axis_start);
                                            fp.push_back(plane.start + plane.axis1 * axis_start.x + plane.axis2 * axis_start.y);
                                            fp.push_back(plane.start + plane.axis1 * (axis_start.x + axis.x * s.first) + plane.axis2 * (axis_start.y + axis.y * s.first));
                                            fp.push_back(plane.start + plane.axis1 * (axis_start.x + axis.x * s.second) + plane.axis2 * (axis_start.y + axis.y * s.second));

                                        }*/
                                    }
                                }
                                if (intersects) {
                                    unitsSelected.insert(unitID);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void onCursorPos(double x, double y) {
        if(useOverheadCamera && left_click) {
            drag = true;
        }
    }

    void onScroll(double x, double y) {
    }

    void onKey(int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_V && action == GLFW_PRESS) {
            useFlyCamera = !useFlyCamera;
            if(useFlyCamera) {
                flyCamera.center = overheadCamera.camera_position;
                flyCamera.rotation = overheadCamera.rotation;
                flyCamera.enable(window);
                overheadCamera.disable(window);
                useOverheadCamera = false;
            } else {
                flyCamera.disable(window);
            }
        } else if(key == GLFW_KEY_G && action == GLFW_PRESS) {
            useOverheadCamera = !useOverheadCamera;
            if(useOverheadCamera) {
                flyCamera.disable(window);
                overheadCamera.enable(window);
                useFlyCamera = false;
            } else {
                overheadCamera.disable(window);
            }
        }
        flyCamera.onKey(window, key, scancode, action, mods);
    }

    glm::vec3 screen_space_to_overhead_dir(glm::vec2 screen) const {
        glm::vec4 screen_mouse((screen.x / (float) width * 2 - 1), (screen.y / (float) height * 2 - 1), 1, 1);
        screen_mouse = glm::inverse(persp * view) * screen_mouse;
        glm::vec3 m(screen_mouse.x, screen_mouse.y, screen_mouse.z);
        m *= 1.0f / screen_mouse.w;
        glm::vec3 v = glm::normalize(m);
        return glm::vec3(v.x, v.y, v.z);
    }

    void run() {
        Graphics::Text text = graphics.initText("arial", 20);
        text.text = "Isaac Huffman";
        text.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        text.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 0.0f));

        Graphics::Rectangle rect = graphics.initRectangle();
        rect.color = glm::vec4(1, 1, 1, 0.2f);

        Graphics::PlainModel model = graphics.initPlainModel("res/cube.obj");
        model.baseLightRadius = 1.0f;
        model.position = glm::vec3(0, 1, 0);
        model.rotation = glm::mat4(1.0f);
        model.color = glm::vec4(1, 0, 0, 1);
        model.scale = glm::vec3(1.0f);

        Graphics::PlainModel model2 = graphics.initPlainModel("res/cube.obj");
        model2.baseLightRadius = 1.0f;
        model2.position = glm::vec3(2, 0.5f, 2);
        model2.rotation = glm::mat4(1.0f);
        model2.color = glm::vec4(1.0f);
        model2.scale = glm::vec3(0.1f);

        Graphics::PlainTextureModel plane1 = graphics.initPlainTextureModel("res/plane.obj");
        plane1.image = "res/idk.png";
        plane1.baseLightRadius = 1.0f;
        plane1.position = glm::vec3(0, 0.01, 0);
        plane1.scale = glm::vec3(4);
        plane1.rotation = glm::mat4(1.0f);
        plane1.color = glm::vec4(1, 1, 1, 1);

        Graphics::PlainModel plane = graphics.initPlainModel("res/plane.obj");
        plane.position = glm::vec3(0, 0, 0);
        plane.scale = glm::vec3(80);
        plane.color = glm::vec4(0.5, 0.5, 0.5, 1);

        view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        Graphics::Light sun = graphics.initLight();
        sun.strength = 100001;
        sun.position = glm::vec3(80, 100, 50);
        sun.color = glm::vec3(1, 1, 1);
        graphics.ambientColor = glm::vec3(0.7f);

        workerModel.baseLightRadius = 1.0f;
        workerModel.color = glm::vec4(1, 0, 0, 1);
        workerModel.scale = glm::vec3(1.0f);

        Graphics::PlainTextureModel selectionModel = graphics.initPlainTextureModel("res/plane.obj");
        selectionModel.baseLightRadius = 1.0f;
        selectionModel.image = "res/idk.png";
        selectionModel.rotation = glm::mat4(1.0f);
        selectionModel.color = glm::vec4(1, 1, 1, 1);

        units.insert({0, {Unit::WORKER, 0, 1.4f, glm::vec2(0, 0)}});
        units.insert({1, {Unit::WORKER, 0, 1.4f, glm::vec2(3, 0)}});
        units.insert({2, {Unit::WORKER, 0, 1.4f, glm::vec2(6, 0)}});
        units.insert({3, {Unit::WORKER, 0, 1.4f, glm::vec2(6, 3)}});
        units.insert({4, {Unit::WORKER, 0, 1.4f, glm::vec2(6, 6)}});
        units.insert({5, {Unit::WORKER, 0, 1.4f, glm::vec2(9, 0)}});

        double current_time = glfwGetTime();
        double last_time = current_time;
        while(!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            current_time = glfwGetTime();
            double delta = std::min<double>(current_time - last_time, 0.1);
            last_time = current_time;

            double m[2];
            glfwGetCursorPos(window, m, m + 1);
            mouse = {m[0], height - m[1]};

            flyCamera.onMousePosition(mouse);
            flyCamera.update(delta);

            overheadCamera.onMousePosition(window, mouse);
            overheadCamera.update(delta);
            if(overheadCamera.moved) {
                drag_start = mouse;
            }
            if(useOverheadCamera) {
                glm::vec3 dir = screen_space_to_overhead_dir(mouse);
                glm::vec3 start = overheadCamera.camera_position;
                glm::vec3 plane_start = glm::vec3(0, 0, 0);
                glm::vec3 plane_norm = glm::vec3(0, 1, 0);
                glm::vec3 mouse3d = proj_line_plane(start, dir, plane_start, plane_norm);
                mouse_world = glm::vec2(mouse3d.x, mouse3d.z);// - glm::vec2(overheadCamera.camera_position.x, overheadCamera.camera_position.z);
                mouse_dir = dir;
            }


            //rot += delta;

            glEnable(GL_DEPTH_TEST);

            plane1.setup = persp * view;
            plane1.position = glm::vec3(mouse_world.x, 0.01, mouse_world.y);
            //plane1.draw();

            plane.setup = persp * view;
            plane.draw();

            selectionModel.setup = persp * view;
            for(const auto& id : unitsSelected) {
                const auto p = units.find(id);
                if(p != units.end()) {
                    const auto& unit = p->second;
                    selectionModel.position = glm::vec3(unit.position.x, 0.01, unit.position.y);
                    selectionModel.scale = glm::vec3(unit.radius * 2);
                    selectionModel.draw();
                }
            }

            workerModel.setup = persp * view;
            for(const auto& p : units) {
                const auto& unit = p.second;
                workerModel.position = glm::vec3(unit.position.x, 1, unit.position.y);
                workerModel.rotation = glm::rotate(glm::mat4(1.0f), unit.direction, glm::vec3(0, 0, 1));
                workerModel.draw();
            }

            for(auto point : fp) {
                model2.position = point;
                model2.setup = persp * view;
                model2.color = {0, 1, 0, 1};
                model2.draw();
            }

            for(auto point : tp) {
                model2.position = point;
                model2.setup = persp * view;
                model2.color = {1, 1, 1, 1};
                model2.draw();
            }

            glDisable(GL_DEPTH_TEST);

            text.setup = ortho;
            text.draw();

            if(useFlyCamera) {
                rect.matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(width / 2, height / 2, 0)),
                        glm::vec3(10, 10, 0));
                rect.setup = ortho;
                rect.draw();
            }

            if(useOverheadCamera && drag) {
                glm::vec2 center = (drag_start + mouse) * 1.0f / 2.0f;
                glm::vec2 size = mouse - drag_start;
                rect.matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0)),
                        glm::vec3(abs(size.x), abs(size.y), 0));
                rect.setup = ortho;
                rect.draw();
            }

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
    glEnable(GL_CULL_FACE);
    glClearColor(0, 0, 0, 0);
    errorCheckGl("draw pre-init");

    Game game(window);
    game.windowSize(STARTING_WINDOW_SIZE.x, STARTING_WINDOW_SIZE.y);
    game.run();

    glfwTerminate();

    return 0;
}