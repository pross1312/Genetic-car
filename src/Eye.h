#pragma once

#include "Ray.h"
#include <eigen3/Eigen/Core>
#include <vector>


class Eye {
public:
	Eye(unsigned distancePerRay, unsigned nRays);
	Eye(const Eye& base) : Eye(base._distancePerRay, base._nRays) {}
	~Eye() = default;

	Eigen::VectorXf senseDistance(const Path& path) const;
	Eigen::VectorXf senseDistance(const sf::Vector2f& A, const sf::Vector2f& B) const;
	void rotate(float angle);
	void setPosition(const sf::Vector2f& position) { *_position = position; }
	void draw(sf::RenderTarget& target, const Path& path) const;
	void draw(sf::RenderTarget& target, const sf::Vector2f& A, const sf::Vector2f& B) const;
private:
	// in degree
	unsigned _distancePerRay = 20;
	unsigned _nRays = 7;

	std::vector<Ray> _rays;
	std::shared_ptr<sf::Vector2f> _position;
};
