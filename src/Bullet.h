#pragma once
#include "Entity.h"

class Bullet : public Entity {
public:
    Bullet(QGraphicsScene &scene, qreal x, qreal y)
        : Entity(scene, "assets/bullet-template.svg", x, y, 0.3) {
        hitX = 0.4;
        hitY = 0.35;
    }

    // straight up, 8 px per tick
    void update() override { moveBy(0, -8); }

    // gone off the top, view deletes it or they pile up forever
    bool offScreen() const { return y() < -bounds().height(); }
};