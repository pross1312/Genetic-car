#include "Ray.h"

Ray::Ray(std::shared_ptr<Vec2f> position, Vec2f direction)
    : _position{ position }, _direction{ direction } {}


void Ray::rotate(float angle) {
    _direction = Helper::rotated(_direction, angle);
}

std::optional<Vec2f> Ray::hit_line(Vec2f A, Vec2f B) const {
    float m = ((_position->x - A.x) * (B.y - A.y) - (_position->y - A.y) * (B.x - A.x)) /
               (-_direction.x * (B.y - A.y) + _direction.y * (B.x - A.x));
    if (m < 0) return {};
    float n = 0.0f;
    if (B.x != A.x)
        n = ((_position->x - A.x) / (B.x - A.x) + _direction.x * m / (B.x - A.x));
    else
        n = ((_position->y - A.y) / (B.y - A.y) + _direction.y * m / (B.y - A.y));
    if (n < 0 || n > 1) return {};
    return (Vec2f) { _position->x + m * _direction.x, _position->y + m * _direction.y };
}

std::optional<Vec2f> Ray::cast(const Path& path) const {
    const auto& vArray = path.vArray;
    unsigned n_spline_vertex = path.spline.vArray.getVertexCount();
    float minDistance = 1e9;
    std::optional<Vec2f> hitPoint;
    for (size_t i = 1; i <= n_spline_vertex; i++) {
        auto p1 = hit_line(vArray[2*(i-1)].position, vArray[2*i].position);
        auto p2 = hit_line(vArray[2*(i-1)+1].position, vArray[2*i+1].position);
        if (p1.has_value()) {
            double tempDistance = Helper::distance(p1.value(), *_position);
            if (tempDistance < minDistance) {
                hitPoint = p1;
                minDistance = tempDistance;
            }
        }
        if (p2.has_value()) {
            double tempDistance = Helper::distance(p2.value(), *_position);
            if (tempDistance < minDistance) {
                hitPoint = p2;
                minDistance = tempDistance;
            }
        }
    }
    return hitPoint;
}
