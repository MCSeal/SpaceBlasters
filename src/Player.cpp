#include "Player.h"
#include <QKeyEvent>

Player::Player(QGraphicsScene &scene, const std::set<int> &held)
    : Entity(scene, "assets/player-ship-3.svg", 360, 480, 0.25),
      held(held) {}

//movement for player
void Player::update() {
    qreal nx = x();
    // arrows or a/d, whichever
    if (held.count(Qt::Key_Left) || held.count(Qt::Key_A))  nx -= speed;
    if (held.count(Qt::Key_Right) || held.count(Qt::Key_D)) nx += speed;
    // keep the ship inside the window (800 wide minus ship width, roughly)
    if (nx < 0) nx = 0;
    if (nx > 720) nx = 720;
    setPos(nx, y());

    if (cooldown > 0) --cooldown;

    // flip the flame every 6 ticks
    if (++flicker >= 6) {
        flicker = 0;
        frame ^= 1;
        showFrame();
    }
}

// one svg per health, 4 only comes from a wrench
void Player::showLives(int n) {
    if (n >= 1 && n <= 4) {
        shown = n;
        showFrame();
    }
}

// the b files are the same ship with a bigger flame
void Player::showFrame() {
    setSprite(QString("assets/player-ship-%1%2.svg").arg(shown).arg(frame ? "b" : ""));
}

void Player::reset() {
    frame = 0;
    flicker = 0;
    showLives(3);
    setOpacity(1.0);
    speed = 5;
    cooldown = 0;
    setPos(360, y());
}