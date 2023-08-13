#include "Path.h"
#include "Car.h"
#include <list>

#define SCROLL_SENSITIVITY 20
#define LINE_PADDING 10
#define RIGHT_PADDING 5
#define STUCK_TOLERANCE 5
#define MAX_MOVE_INCREMENT 20
#define STUCK_COLOR 0xff632eff

Config config("config");

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
inline sf::Font font;
inline Car* best_car_tracker = nullptr;
void handle_training_mode(sf::RenderWindow& window, sf::Event& event, Path& path);
void handle_compete_mode(sf::RenderWindow& window, sf::Event& event, Path& path);

void usage() {
    printf("--Usage--\n");
    printf("./Car <SubCommand> [parameter]\n");
    printf("      Train <seed> <mutation rate> <initial popoulation> <map file> <out file>\n");
    printf("      Compete <agent file> <map file>\n");
    printf("Example: ./Car Compete BestCar resources/Path.txt\n");
    printf("         ./Car Train 123 0.5 20 resources/Path.txt BestCar\n");
}

int main(int argc, char** argv) {
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
    sf::RenderWindow window(sf::VideoMode(config.screen_w, config.screen_h), "CarGenetic");
    sf::Event event;
    Path path;
    window.setFramerateLimit(24);
    font.loadFromFile("resources/VictorMono.ttf");
    path.load(map_file);
    if (training) {
        handle_training_mode(window, event, path);
    }
    else {
        handle_compete_mode(window, event, path);
    }
    return 0;
}

void handle_compete_mode(sf::RenderWindow& window, sf::Event& event, Path& path) {
    Car agent {"resources/Car_agent.jpeg"};
    Car human {"resources/Car_human.jpeg"};
    agent.load_brain(agent_file);
    auto[start_position, start_rotation] = path.get_start_param();
    agent.setPosition(start_position);
    agent.setRotation(start_rotation);
    human.setPosition(start_position);
    human.setRotation(start_rotation);
    bool start = false;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    human.reset();
                    agent.reset();
                    agent.setPosition(start_position);
                    agent.setRotation(start_rotation);
                    human.setPosition(start_position);
                    human.setRotation(start_rotation);
                    start = false;
                } else {
                    start = true;
                }
            }
            human.control(event);
        }
        auto view = window.getView();
        view.setCenter(agent.getPosition());
        window.setView(view);
        window.clear();
        window.draw(path);
        if (start) {
            if (path.contains(human.getPosition())) {
                human.move();
                float dis = human.get_travel_distance(path);
                sf::Text dis_text(std::to_string(1.0 / dis), font);
                dis_text.setPosition(config.screen_w - 150, 0);
                window.draw(dis_text);
                human.update(path);
            }
            if (path.contains(agent.getPosition())) {
                agent.think(path);
                agent.move();
                agent.update(path);
                agent.showEyeLine(window, path);
            }
            else {
                printf("Agent out of path\n");
            }
        }
        window.draw(agent);
        window.draw(human);
        window.display();
    }
}

void handle_training_mode(sf::RenderWindow& window, sf::Event& event, Path& path) {
    std::vector<Car> poolCar{init_population};
    std::vector<bool> onMovingCar;
    size_t current_alive = init_population;
    float best_car_performance = 0.0f;
    auto[start_position, start_rotation] = path.get_start_param();
    for (size_t i = 0; i < init_population; i++) {
        poolCar[i].setPosition(start_position);
        poolCar[i].setRotation(start_rotation);
        onMovingCar.push_back(true);
    }
    try {
        poolCar[0].load_brain(agent_file);
    } catch (const std::exception& e) {
        printf("%s\n", e.what());
    }

    size_t max_move = 100;
    size_t movelefts = max_move;
    auto comp_performance = [&path](const Car& a, const Car& b) { // return true if a < b
        if (a.getLap() < b.getLap())
            return true;
        else if (a.getLap() == b.getLap()
            && path.project_and_get_length(a.getPosition()) < path.project_and_get_length(b.getPosition()))
            return true;
        return false;
    };

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed: {
                if (event.key.code == sf::Keyboard::Up) {
                    max_move += MAX_MOVE_INCREMENT;
                }
                else if (event.key.code == sf::Keyboard::Down && max_move > 30) {
                    max_move -= MAX_MOVE_INCREMENT;
                }
                else if (event.key.code == sf::Keyboard::S) {
                    show_eye_line = !show_eye_line;
                }
                else if (event.key.code == sf::Keyboard::S) {
                    printf("Stuck: %zu\n", stuck_count);
                }
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

        if (stuck_count > STUCK_TOLERANCE && !try_fix) {
            max_move += MAX_MOVE_INCREMENT;
            try_fix = true;
        }

        // end of a generation
        if (current_alive == 0 || movelefts <= 0) {
            auto best_car = std::max_element(poolCar.begin(), poolCar.end(), comp_performance);
            best_car->save_brain(agent_file);
            float new_best_performance = best_car->get_travel_distance(path);
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
                // too stuct??? then mutate more
                if (rand() % 100 < (mutation_rate + stuck_count/100) * 100) {
                    new_generation[i].mutate();
                    if (stuck_count > STUCK_TOLERANCE) new_generation[i].mutate();
                }
            }
            poolCar.clear();
            poolCar = new_generation;
            for (auto& c : poolCar) {
                c.reset();
                c.setPosition(start_position);
                c.setRotation(start_rotation + (rand()%10) * (rand()%10 >= 5 ? -1 : 1));
            }
            current_alive = init_population;
            movelefts = max_move;
            best_car_tracker = &poolCar[0];
        }

        window.clear(config.back_ground);
        window.draw(path);
        for (size_t i = 0; i < init_population; i++) {
            if (onMovingCar[i]) {
                if (path.contains(poolCar[i].getPosition())) {
                    poolCar[i].think(path);
                    poolCar[i].move();
                    poolCar[i].update(path);
                    if (show_eye_line) poolCar[i].showEyeLine(window, path);
                }
                else {
                    onMovingCar[i] = false;
                    current_alive--;
                }
            }
            window.draw(poolCar[i]);
        }
        movelefts--;

        char buffer[124] {};
        sprintf(buffer, "\
Max move: %zu\n\
Performance: %f\n\
Alive: %zu\n", max_move, best_car_performance, current_alive);
        sf::Text info(buffer, font);
        Vec2f info_position = Helper::to_world(window, config.screen_w - info.getGlobalBounds().width - RIGHT_PADDING, 0);
        info.setPosition(info_position);
        if (stuck_count > STUCK_TOLERANCE) info.setFillColor(sf::Color(STUCK_COLOR));
        window.draw(info);
        window.display();
    }
}
