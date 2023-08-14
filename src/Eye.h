#pragma once

#include "Ray.h"
#include "lin_al.h"
#include <vector>


class Eye {
public:
	Eye(unsigned distancePerRay, unsigned nRays);
	Eye(const Eye& base) : Eye(base._distancePerRay, base._nRays) {}

    // Eye& operator=(const Eye& b) = default;
    Eye& operator=(const Eye& b) {
        _distancePerRay = b._distancePerRay;
        _nRays = b._nRays;
        _rays = b._rays;
        *_position = *b._position;
        return *this;
    }
	~Eye() = default;

	VectorXf sense(const Path& path) const;
	VectorXf sense(const sf::Vector2f& A, const sf::Vector2f& B) const;
	void rotate(float angle);
	inline void setRotation(float angle) { rotate(-rotation); rotate(angle); rotation = angle; }
	void setPosition(const sf::Vector2f& position) { *_position = position; }
	void draw(sf::RenderTarget& target, const Path& path) const;
	void draw(sf::RenderTarget& target, const sf::Vector2f& A, const sf::Vector2f& B) const;

private:
	unsigned _distancePerRay = 20; // in degree
	unsigned _nRays = 7;
    float rotation = 0.0f;

	std::vector<Ray> _rays;
	std::shared_ptr<sf::Vector2f> _position;
};
