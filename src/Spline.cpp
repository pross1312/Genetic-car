#include "Spline.h"
#include "Helper.h"
#include <exception>
#include <iostream>

#define PTR std::make_shared
using Vec2f = sf::Vector2f;

Spline::Spline(size_t nJoints, size_t vertices_per_curve, uint32_t c):
    color              { sf::Color(c) },
    vertices_per_curve { vertices_per_curve },
    joints             { nJoints   }, // start - end - start - end - ...
    joint_ctrls        { nJoints*2 },
    vArray             { sf::LineStrip, nJoints * vertices_per_curve } {
    const size_t w         = 800;
    const size_t h         = 600;
    const float angle_step = 2.0*M_PI / nJoints;
    const float radius     = 60*nJoints;
    const float dis        = 10*nJoints;
    float angle            = M_PI;
    for (size_t i = 0; i < nJoints; i++) {
        float x = std::cos(angle)*radius + w/2.0f;
        float y = std::sin(angle)*radius + h/2.0f;
        joints[i] = Vec2f{ x, y };
        angle += angle_step;

        Vec2f rp = joints[i] - Vec2f{ w/2.0f, h/2.0f };
        rp = Helper::normalized(rp);
        rp = Helper::rotated(rp, 90.0f);
        joint_ctrls[2*i]     = joints[i] + rp*dis;
        joint_ctrls[2*i + 1] = joints[i] - rp*dis;
    }
    update();
}

void Spline::update() {
    if (vArray.getVertexCount() != joints.size()*vertices_per_curve) vArray.resize(joints.size() * vertices_per_curve);
    for (size_t i = 0; i < joints.size(); i++) {
        Vec2f start, end;
        auto ctrls = get_curve_ctrl_point(i);
        start = joints[i];
        end = i == joints.size()-1 ? joints[0] : joints[i+1];
        for (size_t vertex = 0; vertex < vertices_per_curve; vertex++) {
            size_t v_index = i*vertices_per_curve + vertex;
            vArray[v_index].position = Helper::cubic_bezier_lerp(
                    start, end, ctrls.first, ctrls.second,
                    1.0f * vertex / (vertices_per_curve - 1));
            vArray[v_index].color = color;
        }
    }
    update_cache_lengths();
}

std::pair<Vec2f, size_t> Spline::projected_point(Vec2f point) const {
    auto result = std::make_pair(Vec2f(-1, -1), -1);
    float min_dis = 1e9;
    for (size_t i = 0; i < vArray.getVertexCount(); i++) {
        auto projected_point_option = Helper::projected_point_on_line(
                point,
                vArray[i].position,
                vArray[(i+1) % vArray.getVertexCount()].position);
        auto projected_point = projected_point_option ? projected_point_option.value() : vArray[i].position;
        float temp_dis = Helper::distance(point, projected_point);
        if (temp_dis < min_dis) {
            result.first = projected_point;
            result.second = i;
            min_dis = temp_dis;
        }
    }
    return result;
}

void Spline::update_cache_lengths() {
    size_t nVertices = vArray.getVertexCount();
    if (cache_lengths.size() != nVertices+1) cache_lengths.resize(nVertices+1);
    cache_lengths[0] = 0;
    for (size_t i = 1; i < cache_lengths.size(); i++) {
        cache_lengths[i] = Helper::distance(vArray[i % nVertices].position, vArray[i-1].position) + cache_lengths[i-1];
    }
}
