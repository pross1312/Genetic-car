#include "Eye.h"


Eye::Eye(unsigned distancePerRay, unsigned nRays)
    : _distancePerRay{ distancePerRay }, _nRays{ nRays }, _position{std::make_shared<sf::Vector2f>()}, _rays{} {
    float degree = -(_distancePerRay * (_nRays - 1) / 2.0f);
    for (size_t i = 0; i < nRays; i++) {
        sf::Vector2f temp{std::cos(degree * (float)M_PI / 180.0f), std::sin(degree * (float)M_PI / 180.0f)};
        Ray r(_position, temp);
        degree += _distancePerRay;
        _rays.push_back(r);
    }
}


VectorXf Eye::sense(const Path& path) const {
    VectorXf output{VectorXf(_nRays, 9999999)};
    for (unsigned i = 0; i < _nRays; i++) {
        std::optional<sf::Vector2f> temp = _rays[i].cast(path);
        if (temp.has_value()) {
            output[i] = Helper::distance(*_position, temp.value());
        }
    }
    return output;
}



VectorXf Eye::sense(const sf::Vector2f& A, const sf::Vector2f& B) const {
    VectorXf output{VectorXf(_nRays, 10000)};
    for (unsigned i = 0; i < _nRays; i++) {
        std::optional<sf::Vector2f> temp = _rays[i].hit_line(A, B);
        if (temp.has_value()) {
            output[i] = Helper::distance(*_position, temp.value());
        }
    }
    return output;
}

void Eye::rotate(float angle) {
    for (Ray& r : _rays)
        r.rotate(angle);
    rotation += angle;
}

void Eye::draw(sf::RenderTarget& target, const Path& path) const {

    for (const Ray& r : _rays) {
        std::optional<sf::Vector2f> temp = r.cast(path);
        if (temp.has_value()) {
            Helper::draw_line(target, *_position, temp.value(), sf::Color::Blue);
        }
    }

}

void Eye::draw(sf::RenderTarget& target, const sf::Vector2f& A, const sf::Vector2f& B) const {

    for (const Ray& r : _rays) {
        std::optional<sf::Vector2f> temp = r.hit_line(A, B);
        if (temp.has_value()) {
            Helper::draw_line(target, *_position, temp.value(), sf::Color::Blue);
        }
    }

}
