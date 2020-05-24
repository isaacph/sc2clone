//
// Created by isaac on 5/17/2020.
//

#ifndef UNTITLED2_SELECTMATH_H
#define UNTITLED2_SELECTMATH_H

bool check_selection(
        const glm::vec3& cam_pos,
        const glm::vec3& cam_dir,
        const glm::vec3& mouse_dir,
        const Graphics::Model& model,
        float& dist);
bool check_selection(
        const glm::vec3& cam_pos,
        const glm::vec3& cam_dir,
        const std::vector<glm::vec3>& dirs,
        const Graphics::Model& model);

#endif //UNTITLED2_SELECTMATH_H
