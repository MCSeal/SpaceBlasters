#pragma once
#include "Entity.h"

class Bullet : public Entity {
public:
    // speed is pixels per tick, negative = up (player), positive = down (enemy)
    Bullet(QGraphicsScene &scene, qreal x, qreal y, qreal speed = -8)
        : Entity(scene, "assets/bullet-template.svg", x, y, 0.3), speed(speed) {
        hitX = 0.4;
        hitY = 0.35;
    }

    void update() override { moveBy(0, speed); }

    // gone off the top or bottom, view deletes it or they pile up forever
    bool offScreen() const { return y() < -bounds().height() || y() > 600; }

private:
    qreal speed;
};