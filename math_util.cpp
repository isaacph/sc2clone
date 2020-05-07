//
// Created by isaac on 5/6/2020.
//
#include <iostream>
#include "math_util.h"

float signed_volume(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
    return 1.0f / 6.0f * glm::dot(glm::cross(b-a, c-a), d-a);
}

glm::vec2 proj(glm::vec2 v, glm::vec2 onto) {
    return glm::dot(v, onto) * onto / (glm::dot(onto, onto));
}

glm::vec3 proj(glm::vec3 v, glm::vec3 onto) {
    return glm::dot(v, onto) * onto / glm::dot(onto, onto);
}

float proj_mag(glm::vec2 v, glm::vec2 onto) {
    return glm::dot(v, onto) / glm::length(onto);
}

float proj_mag(glm::vec3 v, glm::vec3 onto) {
    return glm::dot(v, onto) / glm::length(onto);
}

bool shadow_intersect(float p, float r1, float r2) {
    return r1 <= p && p <= r2 || r2 <= p && p <= r2;
}

bool shadow_intersect(float a_min, float a_max, float b_min, float b_max) {
    return (a_min <= b_min && b_min <= a_max) || (a_min <= b_max && b_max <= a_max) ||
            (b_min <= a_min && a_min <= b_max) || (b_min <= a_max && a_max <= b_max);
}

bool shadow_intersect(glm::vec2 point, glm::vec2 axis, glm::vec2 s1, glm::vec2 s2, glm::vec2 s3) {
    float p = proj_mag(point - point, axis);
    float p1 = proj_mag(s1 - point, axis);
    float p2 = proj_mag(s2 - point, axis);
    float p3 = proj_mag(s3 - point, axis);
    float min = std::min(p1, std::min(p2, p3));
    float max = std::max(p1, std::max(p2, p3));
    return shadow_intersect(p, min, max);
}

bool line_intersects_triangle(const glm::vec3& line_start, const glm::vec3& line_dir, std::vector<glm::vec3> triangle) {
    TrianglePlane plane = get_plane(triangle[0], triangle[1], triangle[2]);
    glm::vec2 int_on_plane = proj_line_plane_2d(line_start, line_dir, plane);
    std::vector<glm::vec2> axes = get_axes(std::vector<glm::vec2>(plane.points_2d, plane.points_2d + 3));
    for(auto& axis : axes) {
        if(!shadow_intersect(int_on_plane, axis, plane.points_2d[0], plane.points_2d[1], plane.points_2d[2])) {
            return false;
        }
    }

    return true;
}

glm::vec3 proj_line_plane(glm::vec3 line_start, glm::vec3 line_dir, glm::vec3 plane_start, glm::vec3 plane_dir) {
    return line_start + line_dir * glm::dot((plane_start - line_start), plane_dir) / glm::dot(line_dir, plane_dir);
}

glm::vec2 proj_line_plane_2d(glm::vec3 line_start, glm::vec3 line_dir, const Plane& plane) {
    glm::vec3 v = proj_line_plane(line_start, line_dir, plane.start, plane.norm);
    return {proj_mag(v - plane.start, plane.axis1),
            proj_mag(v - plane.start, plane.axis2)};
}
std::vector<glm::vec2> get_axes(std::vector<glm::vec2> points) {
    std::vector<glm::vec2> axes;
    for (int i = 0; i < points.size(); ++i) {
        glm::vec2 side = points[i] - points[(i + 1) % points.size()];
        if (side.x != 0 || side.y != 0) {
            axes.push_back({side.y, -side.x});
        }
    }
    return axes;
}

glm::vec2 perp(const glm::vec2& v) {
    return {v.y, -v.x};
}

TrianglePlane get_plane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 plane_start = (p1 + p2 + p3) / 3.0f;
    glm::vec3 plane_norm = glm::normalize(glm::cross(p2-p1, p3-p1));
    glm::vec3 plane_axis1 = glm::normalize(p2 - p1);
    glm::vec3 plane_axis2 = glm::normalize(glm::cross(plane_norm, plane_axis1));
    glm::vec2 plane_v0(proj_mag(p1 - plane_start, plane_axis1), proj_mag(p1 - plane_start, plane_axis2));
    glm::vec2 plane_v1(proj_mag(p2 - plane_start, plane_axis1), proj_mag(p2 - plane_start, plane_axis2));
    glm::vec2 plane_v2(proj_mag(p3 - plane_start, plane_axis1), proj_mag(p3 - plane_start, plane_axis2));
    return {plane_start, plane_norm, plane_axis1, plane_axis2, {p1, p2, p3}, {plane_v0, plane_v1, plane_v2}};
}

std::vector<glm::vec2> proj_frustum_plane(const glm::vec3& start, const std::vector<glm::vec3>& dirs, const Plane& plane, std::vector<glm::vec2>& neg_t) {
    std::vector<glm::vec2> points_2d;
    for(auto dir : dirs) {
        float t = glm::dot((plane.start - start), plane.norm) / glm::dot(dir, plane.norm);
        auto v = start + dir * t;
        glm::vec2 v2 = {proj_mag(v - plane.start, plane.axis1),
                        proj_mag(v - plane.start, plane.axis2)};
        if(t >= 0) {
        points_2d.push_back(v2);
        } else {
            neg_t.push_back(v2);
        }
    }
    return points_2d;
}

glm::vec2 proj_point_plane(const glm::vec3& point, const Plane& plane) {
    return proj_line_plane_2d(point, plane.norm, plane);
}

std::pair<float, float> shadow_min_max(const std::vector<glm::vec2>& points, const glm::vec2& axis, const glm::vec2& origin) {
    if(points.size() == 0) {
        return {-1, -1};
    }
    float min = proj_mag(points[0] - origin, axis);
    float max = proj_mag(points[0] - origin, axis);
    for (int i = 1; i < points.size(); ++i) {
        float f = proj_mag(points[i] - origin, axis);
        min = std::min(f, min);
        max = std::max(f, max);
    }
    return {min, max};
}

bool frustum_intersects_triangle(glm::vec3 start, std::vector<glm::vec3> frustum, std::vector<glm::vec3> triangle) {
    if(frustum.size() != 4 || triangle.size() != 3) {
        return false;
    }

    TrianglePlane plane = get_plane(triangle[0], triangle[1], triangle[2]);
    std::vector<glm::vec2> frust_opp_points;
    std::vector<glm::vec2> frust_points_2d = proj_frustum_plane(start, frustum, plane, frust_opp_points);

    if(frust_points_2d.size() == 4) {
        std::vector<glm::vec2> axes = get_axes(std::vector<glm::vec2>(plane.points_2d, plane.points_2d + 3));
        std::vector<glm::vec2> axes2 = get_axes(frust_points_2d);
        axes.insert(axes.end(), axes2.begin(), axes2.end());

        for (auto &axis : axes) {
            // check if shadows projected onto axis intersect
            float a_min = proj_mag(frust_points_2d[0], axis);
            float a_max = a_min;
            for (int i = 1; i < 4; ++i) {
                float mag = proj_mag(frust_points_2d[i], axis);
                a_min = std::min(a_min, mag);
                a_max = std::max(a_max, mag);
            }

            float b_min = proj_mag(plane.points_2d[0], axis);
            float b_max = b_min;
            for (int i = 1; i < 3; ++i) {
                float mag = proj_mag(plane.points_2d[i], axis);
                b_min = std::min(b_min, mag);
                b_max = std::max(b_max, mag);
            }

            if (!shadow_intersect(a_min, a_max, b_min, b_max)) {
                return false;
            }
        }
        return true;
    } else if(frust_points_2d.size() == 2) {
        glm::vec2 origin = proj_point_plane(start, plane);
        std::vector<glm::vec2> axes = {perp(frust_points_2d[0] - origin), perp(frust_points_2d[1] - origin)};
        std::vector<float> signs = {proj_mag(frust_points_2d[1] - origin, axes[0]),
                                   proj_mag(frust_points_2d[0] - origin, axes[1])};
        for(int i = 0; i < 2; i++) {
            glm::vec2 axis = glm::normalize(axes[i] * signs[i]);
            std::pair<float, float> s = shadow_min_max(std::vector<glm::vec2>(
                    plane.points_2d, plane.points_2d + 3), axis, origin);
            if(!(s.first > 0 || s.second > 0)) {
                return false;
            }
        }
        glm::vec2 axis_start = (frust_points_2d[0] + frust_points_2d[1]) / 2;
        glm::vec2 axis = perp(frust_points_2d[1] - frust_points_2d[0]);
        axis = glm::normalize(axis * glm::dot(axis, origin - axis_start));
        std::pair<float, float> s = shadow_min_max(std::vector<glm::vec2>(
                plane.points_2d, plane.points_2d + 3), axis, axis_start);
        return !(s.first > 0 || s.second > 0);
    } else {
        return false;
    }
}