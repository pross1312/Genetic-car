#pragma once
#include "Path.h"
#include "Helper.h"
#include <optional>
#include <SFML/Graphics.hpp>


class Ray {
public:
	Ray() = default;
	Ray(std::shared_ptr<Vec2f> position, Vec2f direction);
	~Ray() = default;


	std::optional<Vec2f> hit_line(Vec2f A, Vec2f B) const;
	std::optional<Vec2f> cast(const Path& path) const;
	// visualize ray
	void rotate(float angle);
	void setDirection(Vec2f direction) { _direction = direction; }
	void setPosition(std::shared_ptr<Vec2f> position) { _position = position; }
private:
	std::shared_ptr<Vec2f> _position;
	Vec2f _direction;
};
