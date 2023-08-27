#pragma once
#include "Helper.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include <memory>
#include <SFML/Graphics.hpp>

struct Spline: public sf::Drawable {
    Spline(size_t nJoints, size_t vertices_per_curve, uint32_t c);
    ~Spline() = default;
    inline void draw(sf::RenderTarget& target, sf::RenderStates state) const override {
        target.draw(vArray, state);
    }
    void update();

    friend std::istream& operator>>(std::istream& in, Spline& spline) {
        size_t n_joints = 0;
        in.read((char*)&spline.vertices_per_curve, sizeof(spline.vertices_per_curve));
        in.read((char*)&n_joints, sizeof(n_joints));
        spline.joints.resize(n_joints);
        spline.joint_ctrls.resize(n_joints*2);
        in.read((char*)spline.joints.data(), sizeof(Vec2f) * spline.joints.size());
        in.read((char*)spline.joint_ctrls.data(), sizeof(Vec2f) * spline.joint_ctrls.size());
        in.read((char*)&spline.color, sizeof(spline.color));
        spline.update();
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, Spline& spline) {
        size_t n_joints = spline.joints.size();
        out.write((char*)&spline.vertices_per_curve, sizeof(spline.vertices_per_curve));
        out.write((char*)&n_joints, sizeof(n_joints));
        out.write((char*)spline.joints.data(), sizeof(Vec2f) * spline.joints.size());
        out.write((char*)spline.joint_ctrls.data(), sizeof(Vec2f) * spline.joint_ctrls.size());
        out.write((char*)&spline.color, sizeof(spline.color));
        return out;
    }

    void read(std::ifstream& fin);
    void write(std::ofstream& fout);
    std::pair<Vec2f, size_t> projected_point(Vec2f point) const;
    void update_cache_lengths();
    inline float full_length() const { return cache_lengths[cache_lengths.size()-1]; }
    inline std::pair<Vec2f, Vec2f> get_curve_ctrl_point(size_t i) const {
        assert(i < joints.size());
        return std::make_pair( joint_ctrls[2*i + 1], joint_ctrls[2*((i+1)%joints.size())] );
    }

    std::vector<float> cache_lengths;
    sf::Color color;
    size_t vertices_per_curve;
    std::vector<Vec2f> joints, joint_ctrls;
    sf::VertexArray vArray;
};
