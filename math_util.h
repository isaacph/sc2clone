//
// Created by isaac on 3/24/2020.
//

#ifndef UNTITLED2_MATH_H
#define UNTITLED2_MATH_H
#include <vector>
//#include <set>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

inline glm::vec2 operator*(const glm::vec2 &a, double b) {
    return glm::vec2(a.x * b, a.y * b);
}
inline glm::vec2 operator*(double b, const glm::vec2 &a) {
    return glm::vec2(a.x * b, a.y * b);
}
inline glm::vec2 operator/(const glm::vec2 &a, double b) {
    return glm::vec2(a.x / b, a.y / b);
}
struct SortVec {
    glm::vec2 vector;
    bool operator<(const SortVec& other) const {
        return vector.x == other.vector.x ? vector.y < other.vector.y : vector.x < other.vector.y;
    }
};
inline int sign(float num) {
    return num == 0 ? 0 : num < 0 ? -1 : 1;
}

struct Plane {
    glm::vec3 start;
    glm::vec3 norm;
    glm::vec3 axis1;
    glm::vec3 axis2;
};

struct TrianglePlane : public Plane {
    glm::vec3 points[3];
    glm::vec2 points_2d[3];
};
float proj_mag(glm::vec2 v, glm::vec2 onto);
float proj_mag(glm::vec3 v, glm::vec3 onto);

glm::vec3 proj_line_plane(glm::vec3 line_start, glm::vec3 line_dir, glm::vec3 plane_start, glm::vec3 plane_dir);
glm::vec2 proj_line_plane_2d(glm::vec3 line_start, glm::vec3 line_dir, const Plane& plane);

glm::vec2 proj_point_plane(const glm::vec3& point, const Plane& plane);
glm::vec2 perp(const glm::vec2& v);
std::pair<float, float> shadow_min_max(const std::vector<glm::vec2>& points, const glm::vec2& axis, const glm::vec2& origin);

TrianglePlane get_plane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
std::pair<float, float> shadow(const std::vector<glm::vec2>& points, const glm::vec2& axis);

// credit to https://stackoverflow.com/questions/42740765/intersection-between-line-and-triangle-in-3d
float signed_volume(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d);
bool view_line_intersects_triangle(const glm::vec3 camera_start, const glm::vec3& camera_dir,
        const glm::vec3& line_start, const glm::vec3& line_dir, const std::vector<glm::vec3>& triangle);
std::vector<glm::vec2> get_axes(std::vector<glm::vec2> points);

glm::vec3 arbitrary_perp(glm::vec3 other);

std::vector<glm::vec2> proj_frustum_plane(const glm::vec3& start, const std::vector<glm::vec3>& dirs, const Plane& plane);

// num_points must be 4 and dirs must be normalized
bool view_frustum_intersects_triangle(glm::vec3 camera_start, glm::vec3 camera_dir, glm::vec3 frustum_start, std::vector<glm::vec3> frustum, std::vector<glm::vec3> triangle);

#endif //UNTITLED2_MATH_H
