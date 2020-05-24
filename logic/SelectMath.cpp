//
// Created by isaac on 5/17/2020.
//
#include <glm/glm.hpp>
#include "../graphics/draw.h"
#include "../math_util.h"
#include "SelectMath.h"
#include "Game.h"

bool check_selection(
        const glm::vec3& cam_pos,
        const glm::vec3& cam_dir,
        const glm::vec3& mouse_dir,
        const Graphics::Model& model,
        float& dist) {
    const std::vector<Triangle> &triangles = model.getTriangles();
    bool found = false;
    float closestDist = 0;
    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto &triangle = triangles[i];
        glm::vec4 p1_4 = model.getModelMatrix() *
                         glm::vec4(triangle.p1.x, triangle.p1.y, triangle.p1.z, 1);
        glm::vec4 p2_4 = model.getModelMatrix() *
                         glm::vec4(triangle.p2.x, triangle.p2.y, triangle.p2.z, 1);
        glm::vec4 p3_4 = model.getModelMatrix() *
                         glm::vec4(triangle.p3.x, triangle.p3.y, triangle.p3.z, 1);
        glm::vec3 p1 = glm::vec3(p1_4.x / p1_4.w, p1_4.y / p1_4.w, p1_4.z / p1_4.w);
        glm::vec3 p2 = glm::vec3(p2_4.x / p2_4.w, p2_4.y / p2_4.w, p2_4.z / p2_4.w);
        glm::vec3 p3 = glm::vec3(p3_4.x / p3_4.w, p3_4.y / p3_4.w, p3_4.z / p3_4.w);
        float d = 0;
        if (view_line_intersects_triangle(cam_pos, cam_dir, cam_pos, mouse_dir, {p1, p2, p3}, d)) {
            if(!found || d < closestDist) {
                found = true;
                closestDist = dist;
            }
        }
    }
    dist = closestDist;
    return found;
}

bool check_selection(
        const glm::vec3& cam_pos,
        const glm::vec3& cam_dir,
        const std::vector<glm::vec3>& dirs,
        const Graphics::Model& model
        ) {
    const std::vector<Triangle> &triangles = model.getTriangles();
    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto &triangle = triangles[i];
        glm::vec4 p1_4 = model.getModelMatrix() *
                         glm::vec4(triangle.p1.x, triangle.p1.y, triangle.p1.z, 1);
        glm::vec4 p2_4 = model.getModelMatrix() *
                         glm::vec4(triangle.p2.x, triangle.p2.y, triangle.p2.z, 1);
        glm::vec4 p3_4 = model.getModelMatrix() *
                         glm::vec4(triangle.p3.x, triangle.p3.y, triangle.p3.z, 1);
        glm::vec3 p1 = glm::vec3(p1_4.x / p1_4.w, p1_4.y / p1_4.w, p1_4.z / p1_4.w);
        glm::vec3 p2 = glm::vec3(p2_4.x / p2_4.w, p2_4.y / p2_4.w, p2_4.z / p2_4.w);
        glm::vec3 p3 = glm::vec3(p3_4.x / p3_4.w, p3_4.y / p3_4.w, p3_4.z / p3_4.w);

        if (view_frustum_intersects_triangle(cam_pos, cam_dir, cam_pos, dirs,
                                             {p1, p2, p3})) {
            return true;
        }
    }
    return false;
}

int Game::select_units_click(bool& foundUnit, bool sameTeam) {
    int closestSelected;
    float closestDist;
    foundUnit = false;
    for (auto &p : world.units) {
        unsigned int unitID = p.first;
        auto &unit = p.second;
        if(unit.alive && (!sameTeam || unit.team == team)) {
            Graphics::Model *model = get_model_unit(unit);
            if (model != nullptr) {
                float dist = 0;
                if (check_selection(overheadCamera.camera_position,
                                    overheadCamera.camera_focus_point - overheadCamera.camera_position,
                                    mouse_dir, *model, dist)) {
                    if (!foundUnit || dist < closestDist) {
                        foundUnit = true;
                        closestSelected = unitID;
                        closestDist = dist;
                    }
                }
            }
        }
    }
    Unit& unit = world.units[closestSelected];
    return closestSelected;
}

void Game::select_units_drag() {
    std::vector<glm::vec3> dirs = {
            screen_space_to_overhead_dir(drag_start),
            screen_space_to_overhead_dir(glm::vec2(drag_start.x, mouse.y)),
            screen_space_to_overhead_dir(mouse),
            screen_space_to_overhead_dir(glm::vec2(mouse.x, drag_start.y))
    };
    for (auto &p : world.units) {
        unsigned int unitID = p.first;
        auto &unit = p.second;
        if(unit.alive && unit.team == team) {
            Graphics::Model *model = get_model_unit(unit);
            if (model != nullptr) {
                if (check_selection(
                        overheadCamera.camera_position,
                        overheadCamera.camera_focus_point - overheadCamera.camera_position,
                        dirs, *model)) {
                    world.unitsSelected.insert(unitID);
                }
            }
        }
    }
}