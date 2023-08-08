#include "Path.h"
#include "Helper.h"
#include <fstream>
extern Config config;

Path::Path():
    spline      { config.nJoints, config.vertexes_per_curve },
    outer_shape { spline.vArray.getVertexCount() },
    inner_shape { spline.vArray.getVertexCount() } {
        outer_shape.setFillColor(config.path_color);
        inner_shape.setFillColor(config.back_ground);
        update();
}

void Path::update_cache_lengths() {
    const auto& vArray = spline.vArray;
    size_t nVertices = vArray.getVertexCount();
    if (cache_lengths.size() != nVertices+1) cache_lengths.resize(nVertices+1);
    cache_lengths[0] = 0;
    for (size_t i = 1; i < cache_lengths.size(); i++) {
        cache_lengths[i] = Helper::distance(vArray[i % nVertices].position, vArray[i-1].position) + cache_lengths[i-1];
    }
}

void Path::zoom(Vec2f center, float ratio) {
    config.joint_radius    *= ratio;
    config.path_width      *= ratio;
    config.ctrl_point_size *= ratio;
    for (auto& joint : spline.joints) {
        joint = center + (joint - center) * ratio;
    }
    for (auto& joint_ctrl : spline.joint_ctrls) {
        joint_ctrl = center + (joint_ctrl - center)*ratio;
    }
    update();
}

void Path::load(const char* fName) {
    std::ifstream fin(fName, std::ios::binary | std::ios::in);
    if (!fin.is_open())
        throw std::runtime_error("Can't open file to read");
    config.read(fin);
    fin >> spline;
    fin.close();
    update();
}

void Path::save(const char* fName) {
    std::ofstream fout(fName, std::ios::binary | std::ios::out);
    if (!fout.is_open())
        throw std::runtime_error("Can't open file to write.");
    config.write(fout);
    fout << spline;
    fout.close();
}

void Path::update() {
    spline.update();
    size_t nVertices = spline.vArray.getVertexCount();
    if (outer_shape.getPointCount() != nVertices) outer_shape.setPointCount(nVertices);
    if (inner_shape.getPointCount() != nVertices) inner_shape.setPointCount(nVertices);
    for (size_t i = 0; i < nVertices; i++) {
        Vec2f left_vertex  = i == 0           ? spline.vArray[nVertices-1].position : spline.vArray[i-1].position;
        Vec2f right_vertex = i == nVertices-1 ? spline.vArray[0].position           : spline.vArray[i+1].position;
        Vec2f cur_vertex   = spline.vArray[i].position;
        Vec2f left_normal  = Helper::get_normal(left_vertex, cur_vertex);
        Vec2f right_normal = Helper::get_normal(cur_vertex, right_vertex);
        Vec2f normal       = Helper::normalized(left_normal + right_normal);
        outer_shape.setPoint(i, cur_vertex + normal*config.path_width);
        inner_shape.setPoint(i, cur_vertex - normal*config.path_width);
    }
    update_cache_lengths();
}


void Path::draw(sf::RenderTarget& target, sf::RenderStates state) const {
    target.draw(outer_shape, state);
    target.draw(inner_shape, state);
}


float Path::project_and_get_length(const sf::Vector2f& position) const {
    const sf::VertexArray& vArray = spline.vArray;
    auto[point, index] = spline.projected_point(position);
    return Helper::distance(vArray[index].position, point) +
        cache_lengths[index];
}
