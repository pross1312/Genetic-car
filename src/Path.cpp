#include "Path.h"
#include <iostream>
#include "Helper.h"

Path::Path()
    : _spline{}, pathShape1{ 0 }, pathShape2{ 0 } {
    pathShape1.setFillColor(Config::path_color);
    pathShape2.setFillColor(Config::back_ground);
    update();
}

void Path::updatePreComputedLength() {
    const auto& vArray = _spline._splineArray;
    size_t n_Spline_Vertex = vArray.getVertexCount();
    // pre calculate distance from start to every vertex of the spline
    // since there are actually n - 2 vertex as i add 1 additional start and end vertex
    // for easy implementation, and i use the last index for the whole spline length so
    // pre computed array will be of length n - 1
    if (n_Spline_Vertex - 1 != _pre_computed_length.size())
        _pre_computed_length.resize(n_Spline_Vertex - 1);
    _pre_computed_length[0] = 0;

    // calculate
    // value of index 1 will be distance from start to vertex 1
    // but for spline array, the first is the last and last is the first... read about it in Spline.h
    // for more information
    // so ....
    for (size_t i = 1; i < _pre_computed_length.size(); i++)
        _pre_computed_length[i] = _pre_computed_length[i-1] +
                                    Helper::distance(vArray[i + 1].position, vArray[i].position);
}

void Path::readFromFile(const char* fName) {
    std::ifstream fin(fName);
    if (!fin.is_open()) {
        fprintf(stderr, "Can't open file to read");
        exit(EXIT_FAILURE);
    }

    _spline.readFromFile(fin);
    fin.close();
    update();
}



void Path::update() {
    const sf::VertexArray& vArray = _spline._splineArray;
    size_t n = vArray.getVertexCount();
    if (pathShape1.getPointCount() != n - 2) {
        pathShape1.setPointCount(n - 2);
        pathShape2.setPointCount(n - 2);
    }
    int count = 0;
    for (size_t i = 1; i < n - 1; i++) {
        sf::Vector2f normal1 = Helper::getNormal(vArray[i - 1].position, vArray[i].position);
        sf::Vector2f normal2 = Helper::getNormal(vArray[i].position, vArray[i + 1].position);
        sf::Vector2f normal = normal1 + normal2;
        Helper::normalize(normal);
        normal = Helper::mul(normal, Config::path_width);
        pathShape1.setPoint(count, normal + vArray[i].position);
        normal = Helper::mul(normal, -1);
        pathShape2.setPoint(count, normal + vArray[i].position);
        count++;
    }
    updatePreComputedLength();
}


void Path::draw(sf::RenderTarget& target, sf::RenderStates state) const {
    (void)state;
    target.draw(pathShape1);
    target.draw(pathShape2);
}

std::pair<sf::Vector2f, int> Path::getPositionOnSpline(const sf::Vector2f& position) const {
    const sf::VertexArray& vArray = _spline._splineArray;
    size_t n = vArray.getVertexCount();
    double minDistance = 10e9;
    sf::Vector2f projectionPoint{};
    int index = -1;

    // check for every single line along the spline and find projection point
    for (size_t i = 1; i < n - 1; i++) {
        std::optional<sf::Vector2f> temp = Helper::pointProjectToLine(position, vArray[i].position, vArray[i + 1].position);
        if (temp.has_value()) {
            double tempDistance = Helper::distance(position, temp.value());
            // if there are multiple projection points then i use then closest one
            if (tempDistance < minDistance) {
                projectionPoint = temp.value();
                minDistance = tempDistance;
                index = i;
            }
        }
        // check again the last past vertex.
        double toVertexDistance = Helper::distance(position, vArray[i].position);
        if (minDistance > toVertexDistance) {
            projectionPoint = vArray[i].position;
            minDistance = toVertexDistance;
            index = i;
        }
    }

    return std::make_pair(projectionPoint, index);
}

double Path::getDistanceTravel(const sf::Vector2f& position) const {
    const sf::VertexArray& vArray = _spline._splineArray;
    auto data = getPositionOnSpline(position);
    // projection point and index of last past vertex
    return Helper::distance(vArray[data.second].position, data.first) +
        _pre_computed_length[data.second - 1];   // as for why its index-1, read the implementation
}
