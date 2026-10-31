#pragma once
#include "Entity.h"

class Planet : public Entity {
public:
    Planet(QGraphicsScene &scene, qreal x, qreal y);

    void capture();
    void reset();
    bool isCaptured() const { return captured; }

private:
    bool captured = false;
    static constexpr qreal dimOpacity = 0.25;
};