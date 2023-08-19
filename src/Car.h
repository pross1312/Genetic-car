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
	void translate(const sf::Vector2f& velocity);
	void rotate(float rotate); void setRotation(float angle);
	void think(const Path& path);
	void move();
    void control(const sf::Event& event);
	void save_brain(const char* fName) const;
	void load_brain(const char* fName);
	void update(const Path& path);
    void reset();
	inline void show_line(sf::RenderTarget& target, const Path& path) const {
        auto[point, index] = path.spline.projected_point(_sprite.getPosition());
        Helper::draw_line(target, point, _sprite.getPosition(), sf::Color::Red);
    }
	inline void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(_sprite, states); }
	inline void show_eye_line(sf::RenderTarget& target, const Path& path) const        { _eye.draw(target, path); }
	inline float get_travel_distance(const Path& path) const              { return  distance_on_path + lap * path.full_length(); }
    inline void mutate()                                                  { _brain.changeRandom(); }
	inline void move_forward()                                            { translate(Helper::mul(forward, VELOCITY + accelerator)); }
	inline void setPosition(const sf::Vector2f& position)                 { _sprite.setPosition(position); }
	inline void setPosition(float x, float y)                             { setPosition(sf::Vector2f{ x, y }); }
	inline sf::Vector2f getPosition() const                               { return _sprite.getPosition(); }
    Rotate rotate_movement;

private:
	inline static const sf::Color MASKCOLOR{246, 246, 246};
	inline static const sf::Vector2f SCALE{0.1f, 0.1f};
	inline static const float VELOCITY    = 8.0f;
	inline static const float ROTATEANGLE = 6.0f;
    inline static const float ACCELERATE  = 0.3f;

    float distance_on_path = 0.0f;
	float accelerator      = 0.0f;
	size_t last_check_point  = 0;
	int lap                = 0;
	NeuralNetwork _brain;
	sf::Vector2f _localEyePosition;
	Eye _eye;

	sf::Vector2f forward{1, 0};
	sf::Texture _texture;
	sf::Sprite _sprite;
};
