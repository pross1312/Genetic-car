#pragma once
#include "Spline.h"

struct Path: public sf::Drawable {
	Path(float w, uint32_t c);
	~Path() = default;

    inline bool contains(Vec2f point) const {
        auto[pos, _] = spline.projected_point(point);
        return Helper::distance(pos, point) <= width;
    }
	void draw(sf::RenderTarget& target, sf::RenderStates state) const override {
        target.draw(vArray, state);
        target.draw(spline, state);
    }
	void save(const char* fName);
	void load(const char* fName);
    void zoom(Vec2f center, float ratio);
	void update();
    inline std::pair<sf::Vector2f, float> get_start_param() const { // return position and rotation needed
        const auto a = spline.vArray[0].position;
        const auto b = spline.vArray[1].position;
        return std::make_pair(a, Helper::angle_of_2_vec(b-a, Vec2f(1.0f, 0.0f)) * 180 / M_PI);
    }

    float width;
    uint32_t color;
	Spline spline;
    sf::VertexArray vArray;
};
