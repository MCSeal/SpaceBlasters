#pragma once
#include "Entity.h"

// things that fall from the top. a wrench gives a life back, a bolt gives speed.
// gone if it reaches the bottom
class Pickup : public Entity {
public:
    enum Kind { Wrench, Bolt };

    // x is the left side, it starts just above the screen
    Pickup(QGraphicsScene &scene, qreal x, Kind kind)
        : Entity(scene, kind == Wrench ? "assets/wrench.svg" : "assets/bolt.svg",
                 x, -80, 0.25),
          kind_(kind) {}

    // the bolt falls a bit faster so its harder to grab
    void update() override { moveBy(0, kind_ == Bolt ? 3 : 2); }

    bool offScreen() const { return y() > 600; }
    Kind kind() const { return kind_; }

private:
    Kind kind_;
};
