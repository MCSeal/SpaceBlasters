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

    int lives = 3;

private:
    const std::set<int> &held;
    int cooldown = 0;
};