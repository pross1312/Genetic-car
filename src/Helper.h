#pragma once
#include <optional>
#include <cmath>
#include <SFML/Graphics.hpp>



namespace Helper {
    template<typename T>
    inline sf::Vector2f getNormal(const sf::Vector2<T>& A, const sf::Vector2<T>& B) {
        return (sf::Vector2<T>) { B.y - A.y, -(B.x - A.x) };
    }
    template<typename T>
    inline void rotate(sf::Vector2<T>& v, float angle) {
        T rad = M_PI * angle / 180.0;
        T x = v.x, y = v.y;
        v = (sf::Vector2<T>){
            std::cos(rad) * x + std::sin(rad) * y,
            -std::sin(rad) * x + std::cos(rad) * y
        };
    }


    template<typename T>
    inline std::optional<sf::Vector2<T>> pointProjectToLine(const sf::Vector2<T>& point, const sf::Vector2<T>& A, const sf::Vector2<T>& B) {
        double m = -((A.x - point.x) * (B.x - A.x) + (A.y - point.y) * (B.y - A.y)) /
            ((B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y));
        if (m < 0 || m > 1)
            return {};
        return (sf::Vector2<T>) { A.x + m * (B.x - A.x), A.y + m * (B.y - A.y) };
    }

    template<typename T>
    inline bool lineIntersect(const sf::Vector2<T>& X1, const sf::Vector2<T>& X2,
        const sf::Vector2<T>& X3, const sf::Vector2<T>& X4) {
        double t = ((X1.x * 1.0 - X3.x) * (X3.y - X4.y) - (X1.y - X3.y) * (X3.x - X4.x)) /
            ((X1.x - X2.x) * (X3.y - X4.y) - (X1.y - X2.y) * (X3.x - X4.x));
        double u = ((X1.x * 1.0 - X3.x) * (X1.y - X2.y) - (X1.y - X3.y) * (X1.x - X2.x)) /
            ((X1.x - X2.x) * (X3.y - X4.y) - (X1.y - X2.y) * (X3.x - X4.x));
        return t <= 1 && t >= 0 && u <= 1 && u >= 0;
    }

    inline bool contains(const sf::ConvexShape& v, sf::Vector2f position) {
        unsigned n = v.getPointCount();
        sf::Vector2f zero{640.0f, 540.0f};
        int count = 0;
        for (int i = 0; i < n - 1; i++) {
            if (lineIntersect(v.getPoint(i), v.getPoint(i + 1), position, zero))
                count++;
        }
        if (lineIntersect(v.getPoint(0), v.getPoint(n - 1), position, zero))
            count++;
        return count == 0;
    }
    template<typename T>
    inline sf::Vector2<T> mul(const sf::Vector2<T>& v, const float& scalar) {
        sf::Vector2<T> result{ v.x* scalar, v.y* scalar };
        return result;
    }

    template<typename T>
    inline sf::Vector2<T> addVector(const sf::Vector2<T>& A, const sf::Vector2<T>& B) {
        return (sf::Vector2<T>) { A.x + B.x, A.y + B.y };
    }

    template<typename T>
    inline void normalize(sf::Vector2<T>& v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        v.x /= len;
        v.y /= len;
    }

    inline void drawLine(sf::RenderTarget& target, sf::Vector2f A, sf::Vector2f B, sf::Color c) {
        sf::Vertex v[]{ (sf::Vertex) { A, c },(sf::Vertex) { B, c } };
        target.draw(v, 2, sf::Lines);
    }

    template<typename T>
    inline float distance(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2) {
        return std::sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
    }
}
