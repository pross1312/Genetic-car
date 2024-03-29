#include "Path.h"
#include "Config.h"
#include "Car.h"
#include <chrono>
using namespace std::chrono;

#define FONT_PATH "resources/VictorMono.ttf"
#define CONFIG_PATH "config"
#define SCROLL_SENSITIVITY 20
#define LINE_PADDING 10
#define RIGHT_PADDING 5
#define STUCK_TOLERANCE 7
#define MAX_MOVE_INCREMENT 100
#define STUCK_COLOR 0xff632eff
#define SCREEN_MOVE_FACTOR .7f
#define MAX_DISTANCE_FROM_BEST_CAR 150
#define TRAIN_MODE 1
#define COMPETE_MODE 2

Config config(CONFIG_PATH);

// genetic optimization for automatic car...
// -_- pretty dumb code.
// using tournament selection
// read configuration form config file.

const size_t fps = 60;
inline size_t tournament_size = 20;
inline bool showeye_line      = false;
inline float mutation_rate    = 0.5f;
inline size_t init_population = 100;
inline size_t stuck_count     = 0;
inline bool try_fix           = false;
inline const char* map_file   = "resources/sample_path";
inline const char* agent_file = "BestCar";
inline Car* best_car          = nullptr;
inline size_t max_move        = 200;
inline int mode               = TRAIN_MODE;
inline int seed               = time(0);
inline bool load_agent        = false;
inline sf::Font font;
void handle_training_mode(sf::RenderWindow& window, sf::Event& event, Path& path);
void handle_compete_mode(sf::RenderWindow& window, sf::Event& event, Path& path);

void usage() {
    printf("Usage: Car <Subcommand> [Options]\n");
    printf("\
Train   -seed   <int>   : random generator seed.              [default = current_time]\n\
        -init   <int>   : initial population.                 [default = 100]\n\
        -map    <path>  : map file.                           [default = 'resources/sample_path']\n\
        -agent  <path>  : file to save best performance agent.[default = 'BestCar']\n\
        -mutate <float> : file to save best performance agent.[default = 0.5]\n\
        -load   <bool>  : load previous agent or not.         [default = 0]\n\
\n\
Compete -agent <path>: agent file.                         [default = 'BestCar']\n\
        -map   <path>: map file.                           [default = 'resources/sample_path']\n");
}

const char* next_arg(const char** argv, int argc) {
    static int count = 1;
    if (count < argc) return argv[count++];
    return nullptr;
}

int main(int argc, const char** argv) {
    const char* next;
    if (!(next = next_arg(argv, argc))) {
        fprintf(stderr, "Error: no subcommand provided\n");
        usage();
        exit(EXIT_FAILURE);
    }
    if (strcmp(next, "Compete") == 0) mode = COMPETE_MODE;
    else if (strcmp(next, "Train") == 0) mode = TRAIN_MODE;
    else {
        fprintf(stderr, "Error: invalid subcommand %s.\n", next);
        usage();
        exit(EXIT_FAILURE);
    }
    try {
        next = next_arg(argv, argc);
        while (next) {
            if (mode == TRAIN_MODE) {
                if (strcmp(next, "-seed")        == 0 && (next = next_arg(argv, argc))) seed            = std::stoi(next);
                else if (strcmp(next, "-init")   == 0 && (next = next_arg(argv, argc))) init_population = std::stoi(next);
                else if (strcmp(next, "-map")    == 0 && (next = next_arg(argv, argc))) map_file        = next;
                else if (strcmp(next, "-agent")  == 0 && (next = next_arg(argv, argc))) agent_file      = next;
                else if (strcmp(next, "-mutate") == 0 && (next = next_arg(argv, argc))) mutation_rate   = std::stof(next);
                else if (strcmp(next, "-load")   == 0 && (next = next_arg(argv, argc))) load_agent      = std::stoi(next);
                else {
                    fprintf(stderr, "Error: unregconized argument %s.\n", next);
                    usage();
                    exit(EXIT_FAILURE);
                }
            }
            else if (mode == COMPETE_MODE) {
                if (strcmp(next, "-map")        == 0 && (next = next_arg(argv, argc))) map_file   = next;
                else if (strcmp(next, "-agent") == 0 && (next = next_arg(argv, argc))) agent_file = next;
                else {
                    fprintf(stderr, "Error: unregconized argument %s.\n", next);
                    usage();
                    exit(EXIT_FAILURE);
                }
            }
            else assert(false);
            next = next_arg(argv, argc);
        }
    } catch (std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        exit(EXIT_FAILURE);
    }

    Path path(config.path_width, 0x485965ff);
    try { path.load(map_file); }
    catch(const std::exception& e) {
        fprintf(stderr, "Can't read map %s.\nERROR: %s\n", map_file, e.what());
        exit(1);
    }

    sf::RenderWindow window(sf::VideoMode(config.screen_w, config.screen_h), "CarGenetic");
    window.setFramerateLimit(60);
    sf::Event event;
    window.setFramerateLimit(fps);
    font.loadFromFile(FONT_PATH);
    if (mode == TRAIN_MODE) handle_training_mode(window, event, path);
    else                    handle_compete_mode(window, event, path);
    return 0;
}

void handle_compete_mode(sf::RenderWindow& window, sf::Event& event, Path& path) {
    Car agent, human;
    agent.loadbrain(agent_file);
    auto[start_position, start_rotation] = path.get_start_param();
    agent.setPosition(start_position);
    agent.rotate(start_rotation);
    human.setPosition(start_position);
    human.rotate(start_rotation);
    bool start = false;
    while (window.isOpen()) {
        auto start_time = high_resolution_clock::now();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    human.reset();
                    agent.reset();
                    agent.setPosition(start_position);
                    agent.rotate(start_rotation);
                    human.setPosition(start_position);
                    human.rotate(start_rotation);
                    start = false;
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                else {
                    start = true;
                }
            }
            human.control(event);
        }

        auto view = window.getView();
        view.setCenter(human.getPosition());
        window.setView(view);
        window.clear();

        window.draw(path);
        if (start) {
            if (path.contains(human.getPosition())) {
                human.move();
                human.update(path);
            }
            if (path.contains(agent.getPosition())) {
                agent.think(path);
                agent.move();
                agent.update(path);
            }
        }
        window.draw(agent);
        window.draw(human);
        auto interval = std::chrono::duration<float, std::chrono::seconds::period>(high_resolution_clock::now() - start_time);
        char buffer[32] {};
        sprintf(buffer, "Fps: %.2f", 1.0f/interval.count());
        sf::Text info(buffer, font);
        Vec2f info_position = window.mapPixelToCoords(sf::Vector2i(config.screen_w - info.getGlobalBounds().width - RIGHT_PADDING, 0));
        info.setPosition(info_position);
        window.draw(info);
        window.display();
    }
}

void handle_training_mode(sf::RenderWindow& window, sf::Event& event, Path& path) {
    std::vector<Car> pool_cars(init_population);
    std::vector<bool> on_moving_cars;
    size_t current_alive = init_population;
    float best_performance = 0.0f;
    auto[start_position, start_rotation] = path.get_start_param();
    for (size_t i = 0; i < pool_cars.size(); i++) {
        pool_cars[i].setPosition(start_position);
        pool_cars[i].rotate(start_rotation);
        on_moving_cars.push_back(true);
    }
    try {
        if (load_agent) pool_cars[0].loadbrain(agent_file);
    } catch (const std::exception& e) {
        printf("%s\n", e.what());
    }

    size_t movelefts = max_move;
    while (window.isOpen()) {
        auto start_time = high_resolution_clock::now();
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed: {
                if (event.key.code == sf::Keyboard::Up)
                    max_move += MAX_MOVE_INCREMENT;
                else if (event.key.code == sf::Keyboard::Down && max_move > 30)
                    max_move -= MAX_MOVE_INCREMENT;
                else if (event.key.code == sf::Keyboard::S)
                    showeye_line = !showeye_line;
                else if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            } break;
            case sf::Event::MouseWheelScrolled: {
                auto view = window.getView();
                auto center = view.getCenter();
                auto& wheel_event = event.mouseWheelScroll;
                if (wheel_event.wheel == sf::Mouse::VerticalWheel)
                    center.y -= wheel_event.delta * SCROLL_SENSITIVITY;
                else if (wheel_event.wheel == sf::Mouse::HorizontalWheel)
                    center.x -= wheel_event.delta * SCROLL_SENSITIVITY;
                else
                    assert(false);
                view.setCenter(center);
                window.setView(view);
            } break;
            default: break;
            }
        }


        // end of a generation
        if (current_alive == 0 || movelefts <= 0) {
            best_car->savebrain(agent_file);
            float new_best_performance = best_car->get_travel_distance(path);
            if (best_performance < new_best_performance) {
                stuck_count = 0;
                try_fix = false;
            } else if (new_best_performance <= best_performance) stuck_count++;
            best_performance = new_best_performance;
            if (stuck_count > STUCK_TOLERANCE && !try_fix) {
                max_move += MAX_MOVE_INCREMENT;
                try_fix = true;
            }
            std::fill(on_moving_cars.begin(), on_moving_cars.end(), true);
            std::vector<Car> new_generation;
            new_generation.push_back(*best_car);
            for (size_t i = 1; i < pool_cars.size(); i++) {
                Car* p1 = &pool_cars[rand() % pool_cars.size()];
                Car* p2 = &pool_cars[rand() % pool_cars.size()];
                for (size_t tournament = 0; tournament < tournament_size; tournament++) {
                    int rand_idx1 = rand() % pool_cars.size();
                    if (p1->get_travel_distance(path) < pool_cars[rand_idx1].get_travel_distance(path)) {
                        p1 = &pool_cars[rand_idx1];
                    }
                    int rand_idx2 = rand() % pool_cars.size();
                    if (p2->get_travel_distance(path) < pool_cars[rand_idx2].get_travel_distance(path)) {
                        p2 = &pool_cars[rand_idx2];
                    }
                }
                Car temp{ *p1, *p2 };
                new_generation.push_back(temp);
                // too stuct??? then mutate more
                if (rand() % 100 < (mutation_rate + stuck_count/100) * 100) {
                    new_generation[i].mutate();
                    if (stuck_count > STUCK_TOLERANCE) new_generation[i].mutate();
                }
            }
            pool_cars.clear();
            pool_cars = new_generation;
            for (auto& c : pool_cars) {
                c.reset();
                c.setPosition(start_position);
                // c.rotate(start_rotation + (rand()%20) * (rand()%100 >= 50 ? -1 : 1));
                c.rotate(start_rotation);
            }
            current_alive = init_population;
            movelefts = max_move;
        }

        window.clear(config.back_ground);
        window.draw(path);
        best_car = nullptr;
        for (size_t i = 0; i < pool_cars.size(); i++) {
            if (on_moving_cars[i]) {
                if (path.contains(pool_cars[i].getPosition())) {
                    pool_cars[i].think(path);
                    pool_cars[i].move();
                    pool_cars[i].update(path);
                    if (showeye_line) pool_cars[i].showeye_line(window, path);
                    if (!best_car || best_car->get_travel_distance(path) < pool_cars[i].get_travel_distance(path)) best_car = &pool_cars[i];
                    window.draw(pool_cars[i]);
                }
                else {
                    on_moving_cars[i] = false;
                    current_alive--;
                }
            }
        }
        movelefts--;

        // if there aren't any moving cars now, pick the least worst one.
        if (!best_car) best_car = &*std::max_element(pool_cars.begin() ,pool_cars.end(), [&](const auto& a, const auto& b) {return a.get_travel_distance(path) < b.get_travel_distance(path); });

        auto view = window.getView();
        auto center_to_bestcar = Helper::distance(best_car->getPosition(), view.getCenter());
        if (center_to_bestcar > MAX_DISTANCE_FROM_BEST_CAR) {
            view.move((best_car->getPosition() - view.getCenter()) * SCREEN_MOVE_FACTOR / (float)fps);
            window.setView(view);
        }


        auto interval = std::chrono::duration<float, std::chrono::seconds::period>(high_resolution_clock::now() - start_time);
        char buffer[128] {};
        sprintf(buffer, "\
Max move: %zu\n\
Performance: %.2f\n\
Alive: %zu\n\
Best: %.2f\n\
Stuck: %zu\n\
Fps: %.2f", max_move, best_car->get_travel_distance(path), current_alive, best_performance, stuck_count, 1.0f/interval.count());

        sf::Text info(buffer, font);
        Vec2f info_position = window.mapPixelToCoords(sf::Vector2i(config.screen_w - info.getGlobalBounds().width - RIGHT_PADDING, 0));
        info.setPosition(info_position);

        window.draw(info);
        window.display();
    }
}
