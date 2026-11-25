#pragma once
#include "Entity.h"
#include <set>

class Player : public Entity {
public:
    // keeps a reference to the held keys that the view updates
    Player(QGraphicsScene &scene, const std::set<int> &held);

    void update() override;

    // cant fire again until cooldown hits 0
    bool ready() const { return cooldown == 0; }
    void startCooldown() { cooldown = 20; } // about a third of a second
    void reset();

    // swaps to the ship drawing for that many lives. 1 is the most damaged
    void showLives(int n);

    // the flame is drawn two ways and it flips between them while flying

    // pixels per tick, the view sets this from the game state
    void setSpeed(qreal s) { speed = s; }

private:
    void showFrame();

    const std::set<int> &held;
    int shown = 3;
    int frame = 0;
    int flicker = 0;
    int cooldown = 0;
    qreal speed = 5;
};