#pragma once
#include <optional>
#include <cmath>
#include <SFML/Graphics.hpp>


namespace Helper {
    template <typename T>
    using Vec2 = sf::Vector2<T>;
    using Vec2f = Vec2<float>;

    template<typename T>
    inline Vec2f getNormal(const Vec2<T>& A, const Vec2<T>& B) {
        return (Vec2<T>) { B.y - A.y, -(B.x - A.x) };
    }

    template<typename T>
    inline void rotate(Vec2<T>& v, float angle) {
        T rad = M_PI * angle / 180.0;
        T x = v.x, y = v.y;
        v = Vec2<T> {
            std::cos(rad) * x + std::sin(rad) * y,
            -std::sin(rad) * x + std::cos(rad) * y
        };
    }

    template<typename T>
    inline float dot(const Vec2<T>& a, const Vec2<T>& b) {
        return float(a.x*b.x + a.y*b.y);
    }

    template<typename T>
    inline float len(const Vec2<T>& a) {
        return std::sqrt(a.x*a.x + a.y*a.y);
    }


    template<typename T>
    inline float angleBetween2Vec(const Vec2<T>& a, const Vec2<T>& b) {
        return std::acos(dot(a, b) / (len(a)*len(b)));
    }

    template<typename T>
    inline std::optional<Vec2<T>> pointProjectToLine(const Vec2<T>& point, const Vec2<T>& A, const Vec2<T>& B) {
        float m = -((A.x - point.x) * (B.x - A.x) * 1.0f + (A.y - point.y) * 1.0 * (B.y - A.y)) /
            ((B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y)) * 1.0f;
        if (m < 0 || m > 1)
            return {};
        return (Vec2<T>) { A.x + m * (B.x - A.x), A.y + m * (B.y - A.y) };
    }

    template<typename T>
    inline bool lineIntersect(const Vec2<T>& X1, const Vec2<T>& X2,
        const Vec2<T>& X3, const Vec2<T>& X4) {
        double t = ((X1.x * 1.0 - X3.x) * (X3.y - X4.y) - (X1.y - X3.y) * (X3.x - X4.x)) /
            ((X1.x - X2.x) * (X3.y - X4.y) - (X1.y - X2.y) * (X3.x - X4.x));
        double u = ((X1.x * 1.0 - X3.x) * (X1.y - X2.y) - (X1.y - X3.y) * (X1.x - X2.x)) /
            ((X1.x - X2.x) * (X3.y - X4.y) - (X1.y - X2.y) * (X3.x - X4.x));
        return t <= 1 && t >= 0 && u <= 1 && u >= 0;
    }

    inline bool contains(const sf::ConvexShape& v, const Vec2f& position) {
        size_t n = v.getPointCount();
        Vec2f ref_point{position.x, -1};
        int count = 0;
        for (size_t i = 0; i < n - 1; i++) {
            if (lineIntersect(v.getPoint(i), v.getPoint(i + 1), position, ref_point))
                count++;
        }
        if (lineIntersect(v.getPoint(0), v.getPoint(n - 1), position, ref_point))
            count++;
        return count%2 == 1;
    }

    template<typename T>
    inline Vec2<T> mul(const Vec2<T>& v, const float& scalar) {
        Vec2<T> result{ v.x* scalar, v.y* scalar };
        return result;
    }

    template<typename T>
    inline void normalize(Vec2<T>& v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        v.x /= len;
        v.y /= len;
    }

    inline void drawLine(sf::RenderTarget& target, Vec2f A, Vec2f B, sf::Color c) {
        sf::Vertex v[]{ (sf::Vertex) { A, c },(sf::Vertex) { B, c } };
        target.draw(v, 2, sf::Lines);
    }

    template<typename T>
    inline float distance(const Vec2<T>& v1, const Vec2<T>& v2) {
        return std::sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
    }
}
