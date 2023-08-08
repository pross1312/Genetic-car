#pragma once
#include "Config.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include <memory>
#include <SFML/Graphics.hpp>

class Spline: public sf::Drawable {
public:
    Spline(size_t nJoints, size_t vertices_per_curve);
	~Spline() = default;
	void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
	void update();

    friend std::istream& operator>>(std::istream& in, Spline& spline) {
        size_t n_joints = 0;
        in.read((char*)&n_joints, sizeof(size_t));
        spline.joints.resize(n_joints);
        spline.joint_ctrls.resize(n_joints*2);
        in.read((char*)spline.joints.data(), sizeof(Vec2f) * spline.joints.size());
        in.read((char*)spline.joint_ctrls.data(), sizeof(Vec2f) * spline.joint_ctrls.size());
        spline.update();
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, Spline& spline) {
        size_t n_joints = spline.joints.size();
        out.write((char*)&n_joints, sizeof(size_t));
        out.write((char*)spline.joints.data(), sizeof(Vec2f) * spline.joints.size());
        out.write((char*)spline.joint_ctrls.data(), sizeof(Vec2f) * spline.joint_ctrls.size());
        return out;
    }
    void read(std::ifstream& fin);
    void write(std::ofstream& fout);

    // return projected point, along with index of nearest previous vertex
    std::pair<Vec2f, size_t> projected_point(Vec2f position) const;

    inline std::pair<Vec2f, Vec2f> get_curve_ctrl_point(size_t i) const {
        assert(i < joints.size());
        return std::make_pair( joint_ctrls[2*i + 1], joint_ctrls[2*((i+1)%joints.size())] );
    }

    size_t vertices_per_curve;
    std::vector<Vec2f> joints, joint_ctrls;
    sf::VertexArray vArray;
};
