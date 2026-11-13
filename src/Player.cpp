#include "Player.h"
#include <QKeyEvent>

Player::Player(QGraphicsScene &scene, const std::set<int> &held)
    : Entity(scene, "assets/player-ship-template.svg", 360, 480, 0.25),
      held(held) {}

//movement for player
void Player::update() {
    const qreal speed = 5;
    qreal nx = x();
    // arrows or a/d, whichever
    if (held.count(Qt::Key_Left) || held.count(Qt::Key_A))  nx -= speed;
    if (held.count(Qt::Key_Right) || held.count(Qt::Key_D)) nx += speed;
    // keep the ship inside the window (800 wide minus ship width, roughly)
    if (nx < 0) nx = 0;
    if (nx > 720) nx = 720;
    setPos(nx, y());

    if (cooldown > 0) --cooldown;
}

void Player::reset() {
    lives = 3;
    cooldown = 0;
    setPos(360, y());
}