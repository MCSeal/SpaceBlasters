#pragma once
#include "Entity.h"

// the formation is moved as a group by the view, so no update() here
class Enemy : public Entity {
public:
    Enemy(QGraphicsScene &scene, qreal x, qreal y)
        : Entity(scene, "assets/enemy-template.svg", x, y, 0.1875) {
        hitX = 0.18;
        hitY = 0.15;
    }
};