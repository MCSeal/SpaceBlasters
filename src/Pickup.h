#pragma once
#include "Entity.h"

// the wrench. drops from the top and is gone if it reaches the bottom
class Pickup : public Entity {
public:
    // x is the left side, it starts just above the screen
    Pickup(QGraphicsScene &scene, qreal x)
        : Entity(scene, "assets/wrench.svg", x, -80, 0.25) {}

    void update() override { moveBy(0, 2); }

    bool offScreen() const { return y() > 600; }
};
