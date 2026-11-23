#include "GameState.h"
#include <algorithm>

bool GameState::loseLife() {
    if (lives_ > 0) --lives_;
    if (lives_ == 0) outcome_ = Lost;
    return lives_ == 0;
}

bool GameState::gainLife() {
    if (lives_ >= maxLives) return false;
    ++lives_;
    return true;
}

void GameState::startWave(int size) {
    size_ = size;
    kills_ = 0;
}

bool GameState::capturePlanet() {
    ++wave_;
    // new wave, so a new bolt is allowed again
    boltDropped_ = false;
    if (wave_ >= planets) {
        outcome_ = Won;
        return true;
    }
    return false;
}

// each wave starts 0.3 faster than the last. inside a wave the speed goes up
// to 1.5 more by the time the last enemy is gone
double GameState::enemySpeed(int enemiesLeft) const {
    double base = 1 + 0.3 * wave_;
    if (size_ <= 0) return base;
    return base + (size_ - enemiesLeft) * 1.5 / size_;
}

int GameState::enemyFireDelay() const {
    return std::max(30, 100 - wave_ * 25);
}

void GameState::reset() {
    *this = GameState();
}
