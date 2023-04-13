#pragma once
#include "Path.h"
#include "Helper.h"
#include <optional>
#include <SFML/Graphics.hpp>


class Ray {
public:
	Ray() = default;
	Ray(std::shared_ptr<sf::Vector2f> position, sf::Vector2f direction);
	~Ray() = default;


	std::optional<sf::Vector2f> checkAgainLine(sf::Vector2f A, sf::Vector2f B) const;
	std::optional<sf::Vector2f> cast(const Path& path) const;
	// visualize ray
	void rotate(float angle);
	void setDirection(sf::Vector2f direction) { _direction = direction; }
	void setPosition(std::shared_ptr<sf::Vector2f> position) { _position = position; }
private:
	std::shared_ptr<sf::Vector2f> _position;
	sf::Vector2f _direction;
};
