#pragma once
#include "Spline.h"
#include "Helper.h"
#include <optional>


class Path: public sf::Drawable {
public:
    Path();
    ~Path() = default;
    void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
    void readFromFile(const char* fName);
    void update();
    const sf::ConvexShape& getConvex1() const { return pathShape1; }
    const sf::ConvexShape& getConvex2() const { return pathShape2; }
    const sf::VertexArray& getSplineArray() const { return _spline._splineArray; }
    bool contains(const sf::Vector2f& position) {
        return Helper::contains(pathShape1, position) &&
            !Helper::contains(pathShape2, position);
    }
    sf::Vector2f getStartPosition() { return _spline._splineArray[0].position; }
    

    // return projection point of a position on to spline of path and the last past vertex...
    std::pair<sf::Vector2f, int> getPositionOnSpline(const sf::Vector2f& position) const;

    // return the travel distance along the spline for a position by project it
    // orthogonally to the spline.
    double getDistanceTravel(const sf::Vector2f& position) const;


    double getPathLength() const { return _pre_computed_length[_pre_computed_length.size()-1]; }

    // update in case their is any change to path
    // calculate value for pre computed length array
    void updatePreComputedLength();
private:

    // array to store pre calculated length of curves in path
    inline static std::vector<double> _pre_computed_length{};
    bool onEditing1 = true;
    Spline _spline;
    sf::ConvexShape pathShape1, pathShape2;

};
