#pragma once
#include "NeuralNetwork.h"
#include "Eye.h"
#include <SFML/Graphics.hpp>

enum Rotate {
	None,
	Up,
	Down,
};

class Car: public sf::Drawable {
public:
	Car();
	Car(const char* imagePath);
	Car(const Car& base);
	Car(const Car& p1, const Car& p2);
	~Car() = default;
    Car& operator=(const Car& b);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void translate(const sf::Vector2f& velocity);
	void showEyeLine(sf::RenderTarget& target, const Path& path);
	void rotate(float rotate); void setRotation(float angle);
	void think(const Path& path);
	void move();
    void control(const sf::Event& event);
	void mutate();
	void save_brain(const char* fName) const;
	void load_brain(const char* fName);
	float get_travel_distance(const Path& path) const;
	void show_line(sf::RenderTarget& target, const Path& path) const;

    // update car in for on path (like lap, check point ....)
	void update(const Path& path);
    void reset();
	inline void move_forward()                            { translate(Helper::mul(forward, velocity + accelerator)); }
	inline void setPosition(const sf::Vector2f& position) { _sprite.setPosition(position); }
	inline void setPosition(float x, float y)             { setPosition(sf::Vector2f{ x, y }); }
	inline float getRotation() const                      { return _sprite.getRotation(); }
	inline const sf::Vector2f& getForward()               { return forward; }
	inline sf::Vector2f getPosition() const               { return _sprite.getPosition(); }
    inline int getLap() const                             { return lap; }
	inline int getLastCheckPoint() const                  { return lastCheckPoint; }
    Rotate rotate_movement;

private:
    // return car position on path (projection point from car position on to _spline_array)
    // and the curve index that car is on
	std::pair<sf::Vector2f, int> FindPositionOnPath(const Path& path) const;

private:
	inline static const sf::Color maskColor{ 246, 246, 246 };
	inline static const sf::Vector2f scale{ 0.1f, 0.1f };
	inline static const float velocity = 8.0f;
	inline static const float rotateAngle = 6.0f;
    inline static const float ACCELERATE = 0.3f;

	float accelerator = 0.0f;
	size_t lastCheckPoint = 0;
	int lap = 0;
	NeuralNetwork _brain;
	sf::Vector2f _localEyePosition;
	Eye _eye;

	sf::Vector2f forward{ 1, 0 };
	sf::Texture _texture;
	sf::Sprite _sprite;
};
