#pragma once
#include "Spline.h"
#include "Helper.h"
#include <optional>
using std::pair;


class Path: public sf::Drawable {
public:
    Path();
    ~Path() = default;
	inline void draw(sf::RenderTarget& target, sf::RenderStates state) const override {
        target.draw(outer_shape, state);
        target.draw(inner_shape, state);
    }
	void save(const char* fName);
	void load(const char* fName);
    void zoom(Vec2f center, float ratio);
    void update_cache_lengths();
	void update();
    inline pair<sf::Vector2f, float> get_start_param() const { // return position and rotation needed
        const auto a = spline.vArray[0].position;
        const auto b = spline.vArray[1].position;
        return std::make_pair(a, Helper::angle_of_2_vec(b-a, Vec2f(1.0f, 0.0f)) * 180 / M_PI);
    }
    inline bool contains(const sf::Vector2f& position)         {
        return Helper::contains(outer_shape, position) && !Helper::contains(inner_shape, position);
    }
    inline float full_length() const { return cache_lengths[cache_lengths.size()-1]; }


    // array to store pre calculated length of curves in path
    inline static std::vector<float> cache_lengths;
    Spline spline;
	sf::ConvexShape outer_shape, inner_shape;
};
