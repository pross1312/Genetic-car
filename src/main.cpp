#include "Path.h"
#include "Car.h"
#include <list>

// genetic optimization for automatic car...
// -_- pretty dumb code.
// using tournament selection
// read configuration form config file.
// need some commandline arguments like seed, mutation rate, path info.


inline const sf::Color background = sf::Color::Black;
inline const int tournament_size = 10;



int main(int argc, char** argv) {
    Config::read_config("config");

    if (argc != 5) {
        printf("--Usage--\n");
        printf("**.exe -seed- -mutation rate- -population- -map-\n");
        printf("example Car.exe 2 0.5 50 resources/Path.txt\n");
        printf("Press M to turn off mutation.\n");
        printf("Press enter to save best car info to BestCar file.\n");
        printf("Up or down to increase or decrease max moves allowed.\n");
        return EXIT_FAILURE;
    }

    srand(std::stoi(argv[1]));
    float mutationRate = std::stof(argv[2]);
    size_t GenPoolSize = std::stoi(argv[3]);

    sf::RenderWindow _window{ sf::VideoMode(Config::screen_width, Config::screen_height), "CarGenetic" };
    _window.setFramerateLimit(24);
    sf::Event _event{};
    Path path{};
    path.readFromFile(argv[4]);
    sf::Vector2f initPos{path.getStartPosition()};

    std::vector<Car> poolCar{GenPoolSize};
    std::vector<bool> onMovingCar;
    int currentAlive = GenPoolSize;
    for (int i = 0; i < GenPoolSize; i++) {
        poolCar[i].setPosition(initPos);
        onMovingCar.push_back(1);
    }

    sf::Clock clock{};

    bool onMutating = true;
    bool onSavingBestCar = false;
    int maxMove = 150;
    int movelefts = maxMove;

    sf::Font arial;
    arial.loadFromFile("C:\\Windows\\Fonts\\Arial.ttf");

    auto comp = [&path](const Car& a, const Car& b) {
        if (a.getLap() < b.getLap())
            return true;
        else if (a.getLap() == b.getLap()
            && path.getDistanceTravel(a.getPosition()) < path.getDistanceTravel(b.getPosition()))
            return true;
        return false;
    };

    while (_window.isOpen()) {
        clock.restart();
        while (_window.pollEvent(_event)) {
            if (_event.type == sf::Event::Closed)
                _window.close();
            else if (_event.type == sf::Event::KeyPressed) {
                if (_event.key.code == sf::Keyboard::M) {
                    onMutating = !onMutating;
                    if (onMutating)
                        printf("Mutation = True\n");
                    else
                        printf("Mutation = False\n");
                }
                else if (_event.key.code == sf::Keyboard::Up && maxMove > 30) {
                    maxMove += 10;
                    printf("%d\n", maxMove);
                }
                else if (_event.key.code == sf::Keyboard::Down) {
                    maxMove -= 10;
                    printf("%d\n", maxMove);
                }
                else if (_event.key.code == sf::Keyboard::Enter) {
                    onSavingBestCar = !onSavingBestCar;
                    printf("On saving best car.\n");
                }
            }
            else if (_event.type == sf::Event::KeyReleased) {

            }
        }

        // end of a generation
        if (currentAlive == 0 || movelefts <= 0) {
            auto bestCar = std::max_element(poolCar.begin(), poolCar.end(), comp);

            if (onSavingBestCar)
                bestCar->saveBrainToFile("BestCar");
            printf("%f\n", bestCar->getTravelDisance(path));
            
            // allow all car to move again
            std::fill(onMovingCar.begin(), onMovingCar.end(), true);

            // generate next generation
            std::vector<Car> new_generation;
            new_generation.push_back(*bestCar);
            new_generation[0].setPosition(path.getStartPosition());
            for (int i = 1; i < GenPoolSize; i++) {

                // select parent p1 and p2 via tournament method
                int rand_idx = rand() % (GenPoolSize - tournament_size);
                auto p1 = std::max_element(poolCar.begin() + rand_idx,
                    poolCar.begin() + rand_idx + tournament_size,
                    comp);
                rand_idx = rand() % (GenPoolSize - tournament_size);
                auto p2 = std::max_element(poolCar.begin() + rand_idx,
                    poolCar.begin() + rand_idx + tournament_size,
                    comp);
                
                // generate child
                Car temp{ *p1, *p2 };

                new_generation.push_back(temp);
                new_generation[i].setPosition(path.getStartPosition());

                // mutation
                if (rand() % 100 < mutationRate * 100) {
                    new_generation[i].mutate();
                }
            }

            poolCar = new_generation;
            // Car base = poolCar[fittestIndex];
            // base.setPosition(initPos);
            // poolCar.clear();
            // poolCar.push_back(base);
            // onMovingCar[0] = true;
            // for (int i = 1; i < GenPoolSize; i++) {
            //     onMovingCar[i] = true;
            //     poolCar.push_back(base);
            //     int chance = rand() % 100;
            //     if (onMutating && chance < mutationRate * 100) {
            //         poolCar[poolCar.size()-1].mutate();
            //     }
            // }
            currentAlive = GenPoolSize;
            movelefts = maxMove;
        }

        _window.clear(Config::back_ground);

        _window.draw(path);



        for (int i = 0; i < GenPoolSize; i++) {
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

        // display fps
        float frame_second = clock.getElapsedTime().asSeconds();
        sf::Text fps(std::to_string(1.0 / frame_second), arial);
        fps.setPosition(Config::screen_width - 100, 0);
        _window.draw(fps);

        _window.display();
    }
    return 0;
}
