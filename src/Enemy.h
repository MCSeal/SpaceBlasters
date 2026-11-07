#pragma once
#include "Entity.h"

// the formation is moved as a group by the view, so no update() here
class Enemy : public Entity {
public:
    Enemy(QGraphicsScene &scene, qreal x, qreal y)
        : Entity(scene, "assets/enemy-template.svg", x, y, 0.1875) {}

    // some enemies take two hits, drawn dimmer until the first one lands
    void makeTough() {
        hp = 2;
        points = 25;
        setOpacity(0.6);
    }

    // returns true once the enemy is dead
    bool hit() {
        if (--hp > 0) {
            setOpacity(1.0);
            return false;
        }
        return true;
    }

    int points = 10;

private:
    int hp = 1;
};