#pragma once
#include "Helper.h"
#include <SFML/Graphics.hpp>


struct Config {
    Config(const char* file_path) { read(file_path); }
    void read(const char* file_path);
    void write(const char* file_path);
    inline std::istream& read(std::istream& in) {
        in.read((char*)this, sizeof(Config));
        return in;
    }
    inline std::ostream& write(std::ostream& out) {
        out.write((char*)this, sizeof(Config));
        return out;
    }
    void print();


    friend std::ostream& operator<<(std::ostream& out, const Config& config) {
        out << "screen_w: "           << config.screen_w           << "\n";
        out << "screen_h: "           << config.screen_h           << "\n";
        out << "path_color: "         << config.path_color         << "\n";
        out << "path_width: "         << config.path_width         << "\n";
        out << "nJoints: "            << config.nJoints            << "\n";
        out << "background: "         << config.back_ground        << "\n";
        out << "spline_color: "       << config.spline_color       << "\n";
        out << "joint_color: "        << config.joint_color        << "\n";
        out << "ctrl_point_color: "   << config.ctrl_point_color   << "\n";
        out << "line_color: "         << config.line_color         << "\n";
        out << "joint_radius: "       << config.joint_radius       << "\n";
        out << "ctrl_point_size: "    << config.ctrl_point_size    << "\n";
        out << "vertexes_per_curve: " << config.vertexes_per_curve << "\n";
        return out;
    }

    friend std::istream& operator>>(std::istream& in, Config& config) {
        in >> config.screen_w
           >> config.screen_h
           >> config.path_color
           >> config.path_width
           >> config.nJoints
           >> config.back_ground
           >> config.spline_color
           >> config.joint_color
           >> config.ctrl_point_color
           >> config.line_color
           >> config.joint_radius
           >> config.ctrl_point_size
           >> config.vertexes_per_curve;
        return in;
    }

    unsigned  screen_w           { 1920               };
    unsigned  screen_h           { 1080               };
    sf::Color path_color         { 90, 90, 90, 90     };
    float     path_width         { 5.0f               };
    size_t    nJoints            { 9                  };
    sf::Color back_ground        { sf::Color::Black   };
    sf::Color spline_color       { sf::Color::White   };
    sf::Color joint_color        { 130, 130, 130, 130 };
    sf::Color ctrl_point_color   { 130, 130, 130, 130 };
    sf::Color line_color         { 130, 130, 130, 90  };
    float     joint_radius       { 10.0f              };
    float     ctrl_point_size    { 10.0f              };
    size_t    vertexes_per_curve { 15                 };
};
