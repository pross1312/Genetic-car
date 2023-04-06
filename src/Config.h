#pragma once
#include <SFML/Graphics.hpp>


class Config {
public:
    static void read_config(const char* filePath);

    static void write_config(const char* filePath);

    static void print_config();

    // path config

    inline static unsigned screen_width = 1920;
    inline static unsigned screen_height = 1080;
    inline static sf::Color path_color{90, 90, 90, 90};

    inline static float path_width{ 5.0f };

    // spline config
    inline static unsigned nCurves{ 3 };
    inline static sf::Color back_ground{sf::Color::Black};
    inline static sf::Color spline_color{sf::Color::White};
    inline static sf::Color joint_color{130, 130, 130, 130};
    inline static sf::Color ctrl_point_color{130, 130, 130, 130};
    inline static sf::Color line_color{130, 130, 130, 90};
    inline static float jointRadius{ 10.0f };
    inline static float ctrlPointSize{ 10.0f };

    // curve config
    inline static sf::Color curve_color{sf::Color::White};
    inline static unsigned nVertexs{ 15 };

};
