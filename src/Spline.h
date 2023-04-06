#pragma once
#include "CubicCurve.h"
#include "Config.h"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

class Spline: public sf::Drawable {
public:
	Spline();
	~Spline() = default;

	void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
	void readFromFile(std::ifstream& fin);
	void update();
	friend class Path;
private:
	std::shared_ptr<sf::RectangleShape>& getCtrlPoint(unsigned index);
private:
	enum State {
		Idle,
		JointEditing,
		ControlJointEditing,
	};

	State current = State::Idle;
	unsigned indexEditing = 0;
	unsigned nControlPoints = 0;
	std::vector<CubicCurve> _curves;

    // a store all the vertex along the spline of path.
    // with the first one is the last vertex second is the first and so on
    // last one is the first and the preivous one is the last
    // store it like this to make a round connected spline...
	sf::VertexArray _splineArray;
};
