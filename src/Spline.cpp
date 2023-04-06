#include "Spline.h"
#include "Helper.h"
#include <exception>
#include <iostream>
Spline::Spline(): _curves{ Config::nCurves }, _splineArray{ sf::LineStrip, 0 } {
    if (Config::nCurves < 1)
        throw std::invalid_argument("Number of vertex can't be less than 2.");
    nControlPoints = 2 * Config::nCurves;
    for (int i = 0; i < Config::nCurves; i++) {
        if (i > 0) {
            _curves[i].start = _curves[i - 1].end;
        }
        else {
            _curves[i].start = std::make_shared<sf::CircleShape>(Config::jointRadius);
            _curves[i].start->setOrigin(Config::jointRadius, Config::jointRadius);
            _curves[i].start->setFillColor(Config::joint_color);
            _curves[i].start->setPosition((i + 1) * 100, (i + 1) * 100);
        }
        if (i == Config::nCurves - 1) {
            _curves[i].end = _curves[0].start;
        }
        else {
            _curves[i].end = std::make_shared<sf::CircleShape>(Config::jointRadius);
            _curves[i].end->setOrigin(Config::jointRadius, Config::jointRadius);
            _curves[i].end->setFillColor(Config::joint_color);
            _curves[i].end->setPosition((i + 1) * 100 + 60, (i + 1) * 100 + 60);

        }

        _curves[i].startCtrl = std::make_shared<sf::RectangleShape>((sf::Vector2f) { Config::ctrlPointSize, Config::ctrlPointSize });
        _curves[i].endCtrl = std::make_shared<sf::RectangleShape>((sf::Vector2f) { Config::ctrlPointSize, Config::ctrlPointSize });


        _curves[i].startCtrl->setOrigin(Config::ctrlPointSize / 2.0f, Config::ctrlPointSize / 2.0f);
        _curves[i].startCtrl->setFillColor(Config::ctrl_point_color);
        _curves[i].startCtrl->setPosition((i + 1) * 100 + 20, (i + 1) * 100 + 20);

        _curves[i].endCtrl->setOrigin(Config::ctrlPointSize / 2.0f, Config::ctrlPointSize / 2.0f);
        _curves[i].endCtrl->setFillColor(Config::ctrl_point_color);
        _curves[i].endCtrl->setPosition((i + 1) * 100 + 40, (i + 1) * 100 + 40);
        _curves[i].update();
    }
    update();
}

std::shared_ptr<sf::RectangleShape>& Spline::getCtrlPoint(unsigned index) {
    if (index % 2 == 0)
        return _curves[index / 2].startCtrl;
    return _curves[(index - 1) / 2].endCtrl;
}

void Spline::update() {

    int n = Config::nVertexs;

    if (_splineArray.getVertexCount() != (n - 1) * _curves.size() + 2)
        _splineArray.resize((n - 1) * _curves.size() + 2);

    for (int j = 0; j < _curves.size(); j++) {
        _curves[j].update();
        const sf::VertexArray& vArray = _curves[j].vArray;
        for (int i = 0; i < n - 1; i++)
            _splineArray[(j * (n - 1)) + i + 1] = vArray[i];
    }
    _splineArray[0] = (*--_curves.end()).vArray[n - 1];
    _splineArray[_splineArray.getVertexCount() - 1] = _curves[0].vArray[0];
}



void Spline::draw(sf::RenderTarget& target, sf::RenderStates state) const {
    sf::VertexArray line{ sf::Lines, 2 };
    target.draw(_splineArray);
    for (int i = 0; i < _curves.size(); i++) {
        Helper::drawLine(target, _curves[0].start->getPosition(), _curves[0].startCtrl->getPosition(), Config::line_color);
        Helper::drawLine(target, _curves[0].end->getPosition(), _curves[0].endCtrl->getPosition(), Config::line_color);
        target.draw(*_curves[i].start);
        target.draw(*_curves[i].startCtrl);
        target.draw(*_curves[i].endCtrl);
    }
}


void Spline::readFromFile(std::ifstream& fin) {
    // change number of curves in config and regenerate spline
    fin >> Config::nCurves;
    if (_curves.size() == Config::nCurves) {
        for (int i = 0; i < Config::nCurves; i++)
            _curves[i].readFromFile(fin);
    }
    else {
        Spline temp{};
        *this = temp;
        for (int i = 0; i < Config::nCurves; i++)
            _curves[i].readFromFile(fin);
    }
    update();
}
