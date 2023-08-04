#include "Path.h"
#include "Car.h"
#include <list>

#define LINE_PADDING 10
#define RIGHT_PADDING 5
#define STUCK_TOLERANCE 5
#define MAX_MOVE_INCREMENT 20
#define STUCK_COLOR 0xff632eff

// genetic optimization for automatic car...
// -_- pretty dumb code.
// using tournament selection
// read configuration form config file.

inline const sf::Color background = sf::Color::Black;
inline size_t tournament_size = 0;
inline bool training = false;
inline bool show_eye_line = false;
inline float mutation_rate = 0.0f;
inline size_t init_population = 0;
inline size_t stuck_count = 0;
inline bool try_fix = false;
inline const char* map_file = NULL;
inline const char* agent_file = NULL;
inline sf::RenderWindow _window{ sf::VideoMode(Config::screen_width, Config::screen_height), "CarGenetic" };
inline sf::Event _event{};
inline Path path{};
inline sf::Clock prog_clock{};
inline sf::Font font;
inline Car* best_car_tracker = nullptr;
inline sf::Vector2f best_car_old_pos = {-1, -1};
void handle_training_mode();
void handle_compete_mode();

void usage() {
    printf("--Usage--\n");
    printf("./Car <SubCommand> [parameter]\n");
    printf("      Train <seed> <mutation rate> <initial popoulation> <map file> <out file>\n");
    printf("      Compete <agent file> <map file>\n");
    printf("Example: ./Car Compete BestCar resources/Path.txt\n");
    printf("         ./Car Train 123 0.5 20 resources/Path.txt BestCar\n");
}

int main(int argc, char** argv) {
    Config::read_config("config");

    if (argc <= 1) {
        usage();
        fprintf(stderr, "Error: no subcommand provided\n");
        exit(EXIT_FAILURE);
    }
    try {
        if (strcmp(argv[1], "Train") == 0) {
            training = true;
            srand(std::stoi(argv[2]));
            mutation_rate = std::stof(argv[3]);
            assert(std::stoi(argv[4]) >= 0);
            init_population = std::stoi(argv[4]);
            tournament_size = std::max<size_t>(size_t(init_population/5), 1ull);
            map_file = argv[5];
            agent_file = argv[6];
        }
        else if (strcmp(argv[1], "Compete") == 0) {
            agent_file = argv[2];
            map_file = argv[3];
        }
        else {
            usage();
            fprintf(stderr, "ERROR: invalid subcommand\n");
        }
    } catch (const std::exception& e) {
        printf("Error: %s\n", e.what());
        exit(EXIT_FAILURE);
    }
    _window.setFramerateLimit(24);
    font.loadFromFile("resources/VictorMono.ttf");
    path.readFromFile(map_file);
    if (training) {
        handle_training_mode();
    }
    else {
        handle_compete_mode();
    }
    return 0;
}

void handle_compete_mode() {
    Car agent {"resources/Car_agent.jpeg"};
    Car human {"resources/Car_human.jpeg"};
    agent.readBrainFromFile(agent_file);
    auto start_param = path.getStartPosition();
    printf("%f\n", start_param.second);
    agent.setPosition(start_param.first);
    agent.setRotation(start_param.second);
    human.setPosition(start_param.first);
    human.setRotation(start_param.second);
    bool start = false;
    while (_window.isOpen()) {
        while (_window.pollEvent(_event)) {
            if (_event.type == sf::Event::Closed)
                _window.close();
            else if (_event.type == sf::Event::KeyPressed) {
                if (_event.key.code == sf::Keyboard::R) {
                    human.reset();
                    agent.reset();
                    agent.setPosition(start_param.first);
                    agent.setRotation(start_param.second);
                    human.setPosition(start_param.first);
                    human.setRotation(start_param.second);
                    start = false;
                } else {
                    start = true;
                }
            }
            human.control(_event);
        }
        _window.clear();
        _window.draw(path);
        if (start) {
            if (path.contains(human.getPosition())) {
                human.move();
                float dis = human.getTravelDistance(path);
                sf::Text dis_text(std::to_string(1.0 / dis), font);
                dis_text.setPosition(Config::screen_width - 150, 0);
                _window.draw(dis_text);
            }
            if (path.contains(agent.getPosition())) {
                agent.think(path);
                agent.move();
            }
            human.update(path);
            agent.update(path);
        }
        _window.draw(agent);
        _window.draw(human);
        _window.display();
    }
}



void handle_training_mode() {
    std::vector<Car> poolCar{init_population};
    std::vector<bool> onMovingCar;
    int current_alive = init_population;
    float best_car_performance = 0.0f;
    auto start_param = path.getStartPosition();
    for (size_t i = 0; i < init_population; i++) {
        poolCar[i].setPosition(start_param.first);
        poolCar[i].setRotation(start_param.second);
        onMovingCar.push_back(true);
    }
    try {
        poolCar[0].readBrainFromFile(agent_file);
    } catch (const std::exception& e) {
        printf("%s\n", e.what());
    }

    int max_move = 100;
    int movelefts = max_move;
    auto comp_performance = [](const Car& a, const Car& b) { // return true if a < b
        if (a.getLap() < b.getLap())
            return true;
        else if (a.getLap() == b.getLap()
            && path.getDistanceTravel(a.getPosition()) < path.getDistanceTravel(b.getPosition()))
            return true;
        return false;
    };

    while (_window.isOpen()) {
        prog_clock.restart();
        while (_window.pollEvent(_event)) {
            if (_event.type == sf::Event::Closed)
                _window.close();
            else if (_event.type == sf::Event::KeyPressed) {
                if (_event.key.code == sf::Keyboard::Up) {
                    max_move += MAX_MOVE_INCREMENT;
                }
                else if (_event.key.code == sf::Keyboard::Down && max_move > 30) {
                    max_move -= MAX_MOVE_INCREMENT;
                }
                else if (_event.key.code == sf::Keyboard::S) {
                    show_eye_line = !show_eye_line;
                }
                else if (_event.key.code == sf::Keyboard::S) {
                    printf("Stuck: %zu\n", stuck_count);
                }
            }
            else if (_event.type == sf::Event::KeyReleased) {
            }
        }

        if (stuck_count > STUCK_TOLERANCE && !try_fix) {
            max_move += MAX_MOVE_INCREMENT;
            try_fix = true;
        }

        // end of a generation
        if (current_alive == 0 || movelefts <= 0) {
            // if (movelefts == 0 && best_car_tracker != nullptr && path.contains(best_car_tracker->getPosition())) {
            //     max_move += MAX_MOVE_INCREMENT;
            // }
            auto best_car = std::max_element(poolCar.begin(), poolCar.end(), comp_performance);
            best_car->saveBrainToFile(agent_file);
            float new_best_performance = best_car->getTravelDistance(path);
            // if (best_car_performance != 0.0f) {
            //     assert((new_best_performance >= best_car_performance ||
            //            best_car_performance - new_best_performance <= 2)
            //            && "Best car will be passed on to next generation so performance can't get too worse than before");
            // }
            if (best_car_performance < new_best_performance) {
                stuck_count = 0;
                try_fix = false;
            } else if (new_best_performance <= best_car_performance) {
                stuck_count++;
            }
            best_car_performance = new_best_performance;
            std::fill(onMovingCar.begin(), onMovingCar.end(), true);
            std::vector<Car> new_generation;
            new_generation.push_back(*best_car);
            std::sort(poolCar.begin(), poolCar.end(), comp_performance);
            for (size_t i = 1; i < init_population; i++) {
                Car* p1 = &poolCar[0];
                Car* p2 = &poolCar[0];
                for (size_t tournament = 0; tournament < tournament_size; tournament++) {
                    int rand_idx1 = rand() % init_population;
                    if (comp_performance(*p1, poolCar[rand_idx1])) {
                        p1 = &poolCar[rand_idx1];
                    }
                    int rand_idx2 = rand() % init_population;
                    if (comp_performance(*p2, poolCar[rand_idx2])) {
                        p2 = &poolCar[rand_idx2];
                    }
                }
                Car temp{ *p1, *p2 };
                new_generation.push_back(temp);
                // too stuct??? then mutate like crazy
                if (rand() % 100 < (mutation_rate + stuck_count/100) * 100) {
                    new_generation[i].mutate();
                    if (stuck_count > STUCK_TOLERANCE + 5)
                        new_generation[i].mutate();
                }
            }
            poolCar.clear();
            poolCar = new_generation;
            for (auto& c : poolCar) {
                c.reset();
                c.setPosition(start_param.first);
                c.setRotation(start_param.second);
            }
            current_alive = init_population;
            movelefts = max_move;
            best_car_tracker = &poolCar[0];
        }

        _window.clear(Config::back_ground);
        _window.draw(path);
        for (size_t i = 0; i < init_population; i++) {
            if (onMovingCar[i]) {
                if (path.contains(poolCar[i].getPosition())) {
                    poolCar[i].think(path);
                    poolCar[i].move();
                    poolCar[i].update(path);
                    if (show_eye_line) poolCar[i].showEyeLine(_window, path);
                }
                else {
                    onMovingCar[i] = false;
                    current_alive--;
                }
            }
            _window.draw(poolCar[i]);
        }
        movelefts--;

        float frame_second = prog_clock.getElapsedTime().asSeconds();
        sf::Text fps(std::string("FPS: ") + std::to_string(1.0 / frame_second), font);
        fps.setPosition(Config::screen_width - fps.getGlobalBounds().width - RIGHT_PADDING, 0);
        _window.draw(fps);

        sf::Text current_max_move_per_gen_text(std::string("Max move: ") + std::to_string(max_move), font);
        current_max_move_per_gen_text.setPosition(
                Config::screen_width - current_max_move_per_gen_text.getGlobalBounds().width - RIGHT_PADDING,
                fps.getGlobalBounds().height + LINE_PADDING);
        _window.draw(current_max_move_per_gen_text);

        sf::Text current_best_car_performance_text(std::string("Performance: ") + std::to_string(best_car_performance), font);
        if (stuck_count > STUCK_TOLERANCE) current_best_car_performance_text.setFillColor(sf::Color(STUCK_COLOR));
        current_best_car_performance_text.setPosition(
                Config::screen_width - current_best_car_performance_text.getGlobalBounds().width - RIGHT_PADDING,
                current_max_move_per_gen_text.getGlobalBounds().top + current_max_move_per_gen_text.getGlobalBounds().height + LINE_PADDING);
        _window.draw(current_best_car_performance_text);

        sf::Text current_alive_text(std::string("Alive: ") + std::to_string(current_alive), font);
        current_alive_text.setPosition(0, 0);
        _window.draw(current_alive_text);

        _window.display();
    }
}
