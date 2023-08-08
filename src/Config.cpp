#include "Config.h"
#include <sstream>
#include <fstream>

void Config::read(const char* filePath) {
    std::ifstream fin(filePath);
    if (!fin.is_open()) {
        printf("Can't open file %s to read config.", filePath);
        exit(1);
    }
    std::stringstream ss;
    std::string s;
    while (true) {
        std::getline(fin, s);
        if (s.size() < 2)
            break;
        std::string sub = s.substr(s.find(':', 0)+1);
        ss << sub;
    }
    ss >> *this;
    fin.close();
}


void Config::print() {
    std::cout << *this << "\n";
}


void Config::write(const char* filePath) {
    std::ofstream fout(filePath);
    if (!fout.is_open()) {
        printf("Can't open file %s to write config.", filePath);
        exit(1);
    }
    fout << *this << "\n";
    fout.close();
}
