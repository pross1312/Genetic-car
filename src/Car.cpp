#include "Car.h"
#include <exception>
Car::Car(const char* imagePath)
    : _brain{ {15, 3}, relu_activate }, _eye{ 12, 15} {
    sf::Image image;
    if (!image.loadFromFile(imagePath))
        throw std::invalid_argument("Can't load car image.");
    image.createMaskFromColor(maskColor);
    if (!_texture.loadFromImage(image))
        throw std::runtime_error("Can't create texture from car image.");
    _sprite.setTexture(_texture);
    auto bound = _sprite.getLocalBounds();
    _sprite.setOrigin(bound.width / 2, bound.height / 2);
    _sprite.setScale(scale);
    reset();
}

void Car::reset() {
    accelerator         = 0;
    forward             = { 1, 0 };
    rotate_movement     = Rotate::None;
    _localEyePosition.x = 0;
    _localEyePosition.y = 0;
    lap                 = 0;
    lastCheckPoint      = 1;
    forward.x           = 1;
    forward.y           = 0;
    _eye.setPosition(_localEyePosition + _sprite.getPosition());
    _sprite.setRotation(0);
}

Car::Car(const Car& p1, const Car& p2)
    : _brain{p1._brain, p2._brain}, _eye{p1._eye}, _texture{p1._texture}, _sprite{p1._sprite} {

    auto bound = _sprite.getLocalBounds();
    _sprite.setOrigin(bound.width / 2, bound.height / 2);
    _sprite.setScale(scale);
    _sprite.setRotation(0);
    bound = _sprite.getGlobalBounds();
    _localEyePosition.x = 0;
    _localEyePosition.y = 0;

    _eye.setPosition(_localEyePosition + _sprite.getPosition());
    lap            = 0;
    lastCheckPoint = 1;
    forward.x      = 1;
    forward.y      = 0;
}

Car::Car(const Car& base)
    : _brain{base._brain}, _eye{base._eye}, _texture{base._texture}, _sprite{base._sprite} {
}

Car& Car::operator=(const Car& b) {
    assert(false && "Not working properly");
    _brain     = b._brain;
    _eye       = b._eye;
    _texture   = b._texture;
    _sprite    = b._sprite;
    auto bound = _sprite.getLocalBounds();
    _sprite.setOrigin(bound.width / 2, bound.height / 2);
    reset();
    return *this;
}

Car::Car(): Car("./resources/Car_agent.jpeg") {}

void Car::translate(const sf::Vector2f& velocity) {
    _sprite.move(velocity);
    _eye.setPosition(_sprite.getPosition() + _localEyePosition);
}

void Car::think(const Path& path) {
    Eigen::VectorXf input = _eye.sense(path);
    Eigen::VectorXf output = _brain.forward_propagate(input);
    unsigned decision = std::max_element(output.begin(), output.end()) - output.begin();
    switch (decision) {
        case 0: rotate_movement = Rotate::Up;
                break;
        case 1: rotate_movement = Rotate::Down;
                break;
        case 2: rotate_movement = Rotate::None;
                break;
        default: assert(false && "Unknown decision");
    }
}

void Car::control(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::A) {
            rotate_movement = Rotate::Up;
        }
        else if (event.key.code == sf::Keyboard::D) {
            rotate_movement = Rotate::Down;
        }
    }
    else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D) {
            rotate_movement = Rotate::None;
        }
    }
}

float Car::get_travel_distance(const Path& path) const {
    float current_lap_distance = path.project_and_get_length(_sprite.getPosition());
    return current_lap_distance + lap * path.full_length();
}

void Car::update(const Path& path) {
    auto[point, index] = path.spline.projected_point(_sprite.getPosition());
    size_t n = path.spline.vArray.getVertexCount();
    if (lastCheckPoint >= 1 && lastCheckPoint <= 3 && index <= n - 2 && index >= n - 4) {
        lap--;
    }
    else if (lastCheckPoint <= n - 2 && lastCheckPoint >= n - 4 && index >= 1 && index <= 3) {
        lap++;
    }
    lastCheckPoint = index;
}

void Car::show_line(sf::RenderTarget& target, const Path& path) const {
    auto[point, index] = path.spline.projected_point(_sprite.getPosition());
    Helper::draw_line(target, point, _sprite.getPosition(), sf::Color::Red);
}

void Car::move() {
    if (rotate_movement == Rotate::Up) {
        rotate(rotateAngle);
        accelerator = 0.0f;
    }
    else if (rotate_movement == Rotate::Down) {
        rotate(-rotateAngle);
        accelerator = 0.0f;
    } else {
        accelerator += ACCELERATE;
    }
    move_forward();
}

void Car::save_brain(const char* fName) const {
    std::ofstream fout{fName};
    if (!fout.is_open())
        throw std::runtime_error("Can't open file to save brain.");
    fout << _brain;
    fout.close();
}

void Car::load_brain(const char* fName) {
    std::ifstream fin{fName};
    if (!fin.is_open())
        throw std::runtime_error("Can't open file to read brain.");
    fin >> _brain;
    fin.close();
}

void Car::mutate() {
    _brain.changeRandom();
}


void Car::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    (void)states;
    target.draw(_sprite);
}

void Car::showEyeLine(sf::RenderTarget& target, const Path& path) {
    _eye.draw(target, path);
}

void Car::rotate(float angle) {
    _sprite.rotate(-angle);
    _localEyePosition = Helper::rotated(_localEyePosition, angle);
    _eye.setPosition(_localEyePosition + _sprite.getPosition());
    forward = Helper::rotated(forward, angle);
    _eye.rotate(angle);
}
void Car::setRotation(float angle) {
    float temp = - _sprite.getRotation();
    _sprite.setRotation(-angle);
    _localEyePosition = Helper::rotated(_localEyePosition, temp);
    _localEyePosition = Helper::rotated(_localEyePosition, angle);
    _eye.setPosition(_localEyePosition + _sprite.getPosition());
    forward = Helper::rotated(forward, temp);
    forward = Helper::rotated(forward, angle);
    _eye.setRotation(angle);
}
