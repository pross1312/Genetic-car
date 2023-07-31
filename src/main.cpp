#include "Path.h"
#include "Car.h"
#include <list>

// TODO: fix operator=, there is somethings wrong that make next generation best car worse than the previous one

// genetic optimization for automatic car...
// -_- pretty dumb code.
// using tournament selection
// read configuration form config file.
// need some commandline arguments like seed, mutation rate, path info.

inline const sf::Color background = sf::Color::Black;
inline const int tournament_size = 10;
inline bool training = false;
inline bool allow_downgraded = false;
inline float mutation_rate = 0.0f;
inline size_t init_population = 0;
inline const char* map_file = NULL;
inline const char* agent_file = NULL;
inline sf::RenderWindow _window{ sf::VideoMode(Config::screen_width, Config::screen_height), "CarGenetic" };
inline sf::Event _event{};
inline Path path{};
inline sf::Clock prog_clock{};
inline sf::Font font;
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
    int currentAlive = init_population;
    float best_car_performance = .0f;
    auto start_param = path.getStartPosition();
    for (size_t i = 0; i < init_population; i++) {
        poolCar[i].setPosition(start_param.first);
        poolCar[i].setRotation(start_param.second);
        onMovingCar.push_back(true);
    }
    int maxMove = 30;
    int movelefts = maxMove;
    auto comp_performance = [](const Car& a, const Car& b) {
        if (a.getLap() < b.getLap())
            return true;
        else if (a.getLap() == b.getLap()
            && path.getDistanceTravel(a.getPosition()) < path.getDistanceTravel(b.getPosition()))
            return true;
        return false;
    };

    while (_window.isOpen()) {
        prog_clock.restart();
        allow_downgraded = false;
        while (_window.pollEvent(_event)) {
            if (_event.type == sf::Event::Closed)
                _window.close();
            else if (_event.type == sf::Event::KeyPressed) {
                if (_event.key.code == sf::Keyboard::Up) {
                    maxMove += 10;
                }
                else if (_event.key.code == sf::Keyboard::Down && maxMove > 30) {
                    maxMove -= 10;
                    allow_downgraded = true;
                }
            }
            else if (_event.type == sf::Event::KeyReleased) {
            }
        }

        // end of a generation
        if (currentAlive == 0 || movelefts <= 0) {
            auto bestCar = std::max_element(poolCar.begin(), poolCar.end(), comp_performance);
            bestCar->saveBrainToFile(agent_file);
            float new_best_performance = bestCar->getTravelDistance(path);
            if (best_car_performance != .0f) {
                printf("Cur: %f, Old: %f\n", new_best_performance, best_car_performance);
                if (!allow_downgraded) {
                    assert(new_best_performance >= best_car_performance && "Best car will be passed on to next generation so performance can't get worse");
                }
            }
            best_car_performance = new_best_performance;
            std::fill(onMovingCar.begin(), onMovingCar.end(), true);
            std::vector<Car> new_generation;
            new_generation.push_back(*bestCar);
            for (size_t i = 1; i < init_population; i++) {
                int rand_idx = rand() % (init_population - tournament_size);
                auto p1 = std::max_element(poolCar.begin() + rand_idx,
                                            poolCar.begin() + rand_idx + tournament_size,
                                            comp_performance);
                rand_idx = rand() % (init_population - tournament_size);
                auto p2 = std::max_element(poolCar.begin() + rand_idx,
                                            poolCar.begin() + rand_idx + tournament_size,
                                            comp_performance);
                Car temp{ *p1, *p2 };
                new_generation.push_back(temp);
                if (rand() % 100 < mutation_rate * 100) {
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
            currentAlive = init_population;
            movelefts = maxMove;
        }
        _window.clear(Config::back_ground);
        _window.draw(path);
        for (size_t i = 0; i < init_population; i++) {
            if (onMovingCar[i]) {
                if (path.contains(poolCar[i].getPosition())) {
                    poolCar[i].think(path);
                    poolCar[i].move();
                    poolCar[i].update(path);
                }
                else {
                    onMovingCar[i] = false;
                    currentAlive--;
                }
            }
            _window.draw(poolCar[i]);
        }
        movelefts--;

        float frame_second = prog_clock.getElapsedTime().asSeconds();
        sf::Text fps(std::to_string(1.0 / frame_second), font);
        fps.setPosition(Config::screen_width - 150, 0);
        _window.draw(fps);

        sf::Text current_max_move_per_gen(std::to_string(maxMove), font);
        current_max_move_per_gen.setPosition(Config::screen_width - 150, fps.getGlobalBounds().height + 10);
        _window.draw(current_max_move_per_gen);

        sf::Text current_best_car_performance(std::to_string(best_car_performance), font);
        current_best_car_performance.setPosition(Config::screen_width - 150, fps.getGlobalBounds().height * 2 + 20);
        _window.draw(current_best_car_performance);


        _window.display();
    }
}
