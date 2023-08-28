#pragma once
#include "NeuralNetwork.h"
#include "Eye.h"
#include <SFML/Graphics.hpp>
const extern size_t fps;

struct Car: public sf::Drawable {
public:
	Car();
	Car(const Car& base);
	Car(const Car& p1, const Car& p2);
	~Car() = default;
    Car& operator=(const Car& b);
	void translate(const sf::Vector2f& velocity);
	void rotate(float rotate);
    void setRotation(float angle);
	void think(const Path& path);
	void move();
    void control(const sf::Event& event);
	void savebrain(const char* fName) const;
	void loadbrain(const char* fName);
	void update(const Path& path);
    void reset();
	inline void show_line(sf::RenderTarget& target, const Path& path) const {
        auto[point, index] = path.spline.projected_point(sprite.getPosition());
        Helper::draw_line(target, point, sprite.getPosition(), sf::Color::Red);
    }
	inline void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(sprite, states); }
	inline void showeye_line(sf::RenderTarget& target, const Path& path) const        { eye.draw(target, path); }
	inline float get_travel_distance(const Path& path) const              { return  distance_on_path + lap * path.spline.full_length(); }
    inline void mutate()                                                  { brain.changeRandom(); }
	inline void move_forward()                                            { translate(Helper::mul(forward, VELOCITY + accelerator)); }
	inline void setPosition(const sf::Vector2f& position)                 { sprite.setPosition(position); eye.setPosition(position); }
	inline void setPosition(float x, float y)                             { setPosition(sf::Vector2f{ x, y }); }
	inline sf::Vector2f getPosition() const                               { return sprite.getPosition(); }

	inline static const float VELOCITY    = 200.0f / fps;
	inline static const float ROTATEANGLE = 120.0f / fps;
    inline static const float ACCELERATE  = 3.0f / fps;

    enum CAR_TYPE {
        RED_CAR = 0, GREEN_CAR, YELLOW_CAR, BLUE_CAR, CAR_TYPE_COUNT
    };
	inline static sf::Texture* textures[CAR_TYPE_COUNT] {};

    bool rotate_movement[2];
    float distance_on_path = 0.0f;
	float accelerator      = 0.0f;
	size_t last_check_point  = 0;
	int lap                = 0;
	NeuralNetwork brain;
	Eye eye;

	sf::Vector2f forward{1, 0};
	sf::Sprite sprite;
};
