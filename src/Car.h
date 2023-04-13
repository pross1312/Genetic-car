#pragma once
#include "NeuralNetwork.h"
#include "Eye.h"
#include <SFML/Graphics.hpp>

enum Rotate {
	None,
	Up,
	Down,
};

struct Movement {
	bool moveforward = false;
	Rotate rotate = None;
};


class Car: public sf::Drawable {
public:
	Car();
	Car(const char* imagePath);
	Car(const Car& base);
	Car(const Car& p1, const Car& p2);
	~Car() = default;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setPosition(const sf::Vector2f& position) { _sprite.setPosition(position); }
	void setPosition(float x, float y) { setPosition((sf::Vector2f) { x, y }); }
	float getRotation() const { return _sprite.getRotation(); }
	void translate(const sf::Vector2f& velocity);
	void showEyeLine(sf::RenderTarget& target, const Path& path);
	const sf::Vector2f& getForward() { return forward; }
	sf::Vector2f getPosition() const { return _sprite.getPosition(); }
	void rotate(float rotate);
	void moveForward() { translate(Helper::mul(forward, velocity + accelerator)); }
	void think(const Path& path);
	void move();
	void mutate();
	void saveBrainToFile(const char* fName) const;
	void readBrainFromFile(const char* fName);
	double getTravelDisance(const Path& path) const;
	void showLine(sf::RenderTarget& target, const Path& path) const;

    // update car in for on path (like lap, check point ....)
	void update(const Path& path);
	
    int getLap() const { return lap; }
	int getLastCheckPoint() const { return lastCheckPoint; }
	Movement movement;
private:
    // return car position on path (projection point from car position on to _spline_array)
    // and the curve index that car is on
	std::pair<sf::Vector2f, int> FindPositionOnPath(const Path& path) const;

private:
	inline static const sf::Color maskColor{ 246, 246, 246 };
	inline static const sf::Vector2f scale{ 0.05f, 0.05f };
	inline static const float velocity = 8.0f;
	inline static const float rotateAngle = 6.0f;

	float accelerator = 0.0f;
	int lastCheckPoint = 1;
	int lap = 0;
	NeuralNetwork _brain;
	sf::Vector2f _localEyePosition;
	Eye _eye;

	sf::Vector2f forward{ 1, 0 };
	sf::Texture _texture;
	sf::Sprite _sprite;

};
