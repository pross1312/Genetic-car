#include "Car.h"
#include <exception>
#define CAR_TEXTURES_PATH "resources/cars_racer.png"
#define CAR_WIDTH 225
#define CAR_HEIGHT 442
#define CAR_GAP 163
#define CAR_SCALE_TO_SCREEN {0.1f, 0.1f}
#define CAR_ROTATION_TO_POINT_RIGHT 90
#define CAR_EYE_RAYS 11
#define RAY_ANGLE 180/CAR_EYE_RAYS
#define ROTATE_UP 0
#define ROTATE_DOWN 1
#define ROTATE_NONE 2

inline bool load_car_textures() {
    for (size_t i = 0; i < Car::CAR_TYPE_COUNT; i++) {
        Car::textures[i] = new sf::Texture;
        if (!Car::textures[i]->loadFromFile(CAR_TEXTURES_PATH, sf::Rect<int>((CAR_WIDTH+CAR_GAP)*int(i), 0, CAR_WIDTH, CAR_HEIGHT)))
            return false;
    }
    return true;
}

Car::Car()
    : brain{ {CAR_EYE_RAYS, 7, 3}, relu_activate }, eye{ RAY_ANGLE, CAR_EYE_RAYS} {
    if (Car::textures[0] == nullptr) load_car_textures(); // if texture is not loaded, load it
    sprite.setTexture(*Car::textures[rand()%Car::CAR_TYPE_COUNT]);
    reset();
}

Car::Car(const Car& p1, const Car& p2)
    : brain{p1.brain, p2.brain}, eye{p1.eye}, sprite{p1.sprite} {
    sprite.setTexture(*Car::textures[rand()%CAR_TYPE_COUNT]);
    reset();
}

Car::Car(const Car& base)
    : brain{base.brain}, eye{base.eye}, sprite{base.sprite} {
}


void Car::reset() {
    sprite.setScale(CAR_SCALE_TO_SCREEN);
    auto bound = sprite.getLocalBounds();
    sprite.setOrigin(bound.width / 2, bound.height / 2);
    sprite.setRotation(CAR_ROTATION_TO_POINT_RIGHT);
    accelerator        = 0;
    forward            = { 1, 0 };
    lap                = 0;
    last_check_point   = 1;
    distance_on_path   = 0.0f;
    rotate_movement[ROTATE_UP] = rotate_movement[ROTATE_DOWN] = false;
    eye.setRotation(0);
    eye.setPosition(sprite.getPosition());
}

Car& Car::operator=(const Car& b) {
    assert(false && "Not working properly");
    brain      = b.brain;
    eye        = b.eye;
    sprite     = b.sprite;
    auto bound = sprite.getLocalBounds();
    sprite.setOrigin(bound.width / 2, bound.height / 2);
    reset();
    return *this;
}


void Car::translate(const sf::Vector2f& velocity) {
    sprite.move(velocity);
    eye.setPosition(sprite.getPosition());
}

void Car::think(const Path& path) {
    VectorXf input = eye.sense(path);
    VectorXf output = brain.forward_propagate(input);
    unsigned decision = std::max_element(output.data, output.data + output.size()) - output.data;
    switch (decision) {
        case ROTATE_UP: rotate_movement[ROTATE_UP] = true;
                break;
        case ROTATE_DOWN: rotate_movement[ROTATE_DOWN] = true;
                break;
        case ROTATE_NONE: rotate_movement[ROTATE_UP] = rotate_movement[ROTATE_DOWN] = false;
                break;
        default: assert(false && "Unknown decision");
    }
}

void Car::control(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::A) rotate_movement[ROTATE_UP] = true;
        if (event.key.code == sf::Keyboard::D) rotate_movement[ROTATE_DOWN] = true;
    }
    else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::A) rotate_movement[ROTATE_UP] = 0;
        if (event.key.code == sf::Keyboard::D) rotate_movement[ROTATE_DOWN] = 0;
    }
}

void Car::update(const Path& path) {
    assert(*eye._position == sprite.getPosition());
    auto[point, index] = path.spline.projected_point(sprite.getPosition());
    distance_on_path = Helper::distance(path.spline.vArray[index].position, sprite.getPosition()) + path.spline.cache_lengths[index];
    size_t n = path.spline.vArray.getVertexCount();
    if (last_check_point >= 1 && last_check_point <= 3 && index <= n - 2 && index >= n - 4) {
        lap--;
    }
    else if (last_check_point <= n - 2 && last_check_point >= n - 4 && index >= 1 && index <= 3) {
        lap++;
    }
    last_check_point = index;
}


void Car::move() {
    if (rotate_movement[ROTATE_UP]) {
        rotate(ROTATEANGLE);
        accelerator = 0.0f;
    }
    else if (rotate_movement[ROTATE_DOWN]) {
        rotate(-ROTATEANGLE);
        accelerator = 0.0f;
    } else {
        accelerator += ACCELERATE;
    }
    move_forward();
}

void Car::savebrain(const char* fName) const {
    std::ofstream fout{fName};
    if (!fout.is_open())
        throw std::runtime_error("Can't open file to save brain.");
    fout << brain;
    fout.close();
}

void Car::loadbrain(const char* fName) {
    std::ifstream fin{fName};
    if (!fin.is_open())
        throw std::runtime_error("Can't open file to read brain.");
    fin >> brain;
    fin.close();
}

void Car::rotate(float angle) {
    sprite.rotate(-angle);
    forward = Helper::rotated(forward, angle);
    eye.rotate(angle);
    eye.setPosition(sprite.getPosition());
}
