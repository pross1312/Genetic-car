#pragma once
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <fstream>
class CubicCurve: public sf::Drawable {
public:
	CubicCurve(): vArray{ sf::LineStrip, Config::nVertexs } {}
	~CubicCurve() = default;
	sf::Vector2f cubicBezierLerp(float t) const;
	void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
	void readFromFile(std::ifstream& fin);
	void update();

    // return lenght of the curve
    // calculate by add distance between 2 consecutive vertex in it...
    double length() const;
	friend class Spline;
	friend class Path;
private:
	std::shared_ptr<sf::CircleShape> start, end;
	std::shared_ptr<sf::RectangleShape> startCtrl, endCtrl;
	sf::VertexArray vArray;
	

};
