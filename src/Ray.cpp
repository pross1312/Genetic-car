#include "Ray.h"

Ray::Ray(std::shared_ptr<sf::Vector2f> position, sf::Vector2f direction)
    : _position{ position }, _direction{ direction } {}


void Ray::rotate(float angle) {
    Helper::rotate(_direction, angle);
}

std::optional<sf::Vector2f> Ray::checkAgainLine(sf::Vector2f A, sf::Vector2f B) const {
    float m = ((_position->x - A.x) * (B.y - A.y) - (_position->y - A.y) * (B.x - A.x)) /
        (-_direction.x * (B.y - A.y) + _direction.y * (B.x - A.x));
    if (m < 0)
        return {};
    float n = 0.0f;
    if (B.x != A.x)
        n = ((_position->x - A.x) / (B.x - A.x) + _direction.x * m / (B.x - A.x));
    else
        n = ((_position->y - A.y) / (B.y - A.y) + _direction.y * m / (B.y - A.y));
    if (n < 0 || n > 1)
        return {};
    return (sf::Vector2f) { _position->x + m * _direction.x, _position->y + m * _direction.y };
}

std::optional<sf::Vector2f> Ray::cast(const Path& path) const {
    const sf::ConvexShape& pathShape1 = path.getConvex1();
    const sf::ConvexShape& pathShape2 = path.getConvex2();
    unsigned nVertex = pathShape1.getPointCount();
    double minDistance = INT32_MAX;
    std::optional<sf::Vector2f> hitPoint{};
    std::optional<sf::Vector2f> p1 = checkAgainLine(pathShape1.getPoint(0), pathShape1.getPoint(nVertex-1));
    std::optional<sf::Vector2f> p2 = checkAgainLine(pathShape2.getPoint(0), pathShape2.getPoint(nVertex-1));
    if (p1.has_value()) {
        double tempDistance = Helper::distance(p1.value(), *_position);
        if (tempDistance < minDistance) {
            hitPoint = p1.value();
            minDistance = tempDistance;
        }
    }
    if (p2.has_value()) {
        double tempDistance = Helper::distance(p2.value(), *_position);
        if (tempDistance < minDistance) {
            hitPoint = p2.value();
            minDistance = tempDistance;
        }
    }
    for (int i = 0; i < nVertex - 1; i++) {
        p1 = checkAgainLine(pathShape1.getPoint(i), pathShape1.getPoint(i + 1));
        p2 = checkAgainLine(pathShape2.getPoint(i), pathShape2.getPoint(i + 1));
        if (p1.has_value()) {
            double tempDistance = Helper::distance(p1.value(), *_position);
            if (tempDistance < minDistance) {
                hitPoint = p1.value();
                minDistance = tempDistance;
            }
        }
        if (p2.has_value()) {
            double tempDistance = Helper::distance(p2.value(), *_position);
            if (tempDistance < minDistance) {
                hitPoint = p2.value();
                minDistance = tempDistance;
            }
        }
    }
    return hitPoint;
}

