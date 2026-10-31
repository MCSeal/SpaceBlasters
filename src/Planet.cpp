#include "Planet.h"

// starts dim, lights up when captured
Planet::Planet(QGraphicsScene &scene, qreal x, qreal y)
    : Entity(scene, "assets/planet-template.svg", x, y, 0.15) {
    setOpacity(dimOpacity);
}

void Planet::capture() {
    captured = true;
    setOpacity(1.0);
}

void Planet::reset() {
    captured = false;
    setOpacity(dimOpacity);
}