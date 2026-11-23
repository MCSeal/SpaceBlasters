#pragma once

// the rules and numbers for one game, nothing on screen. GameView asks it what
// should happen and draws the result, so it can also be tested on its own
class GameState {
public:
    static constexpr int startLives = 3;
    static constexpr int maxLives = 4;
    static constexpr int planets = 3;
    enum Outcome { Playing, Won, Lost };

    // score and lives
    int score() const { return score_; }
    void addScore(int points) { score_ += points; }
    int lives() const { return lives_; }
    // true if that was the last life
    bool loseLife();
    // false if lives are already at the max
    bool gainLife();

    // waves. the wave number is also how many planets are captured
    int wave() const { return wave_; }
    int rows() const { return wave_ >= 2 ? 3 : 2; }
    int cols() const { return 5 + wave_; }
    // chances out of 100. the first wave is always full and plain
    int holeChance() const { return wave_ > 0 ? 15 : 0; }
    int toughChance() const { return wave_ > 0 ? 25 : 0; }
    // call when a wave is put on the screen, size is how many enemies
    void startWave(int size);
    int waveSize() const { return size_; }
    int kills() const { return kills_; }
    void recordKill() { ++kills_; }
    // shooting down at least half of the wave counts
    bool waveCleared() const { return kills_ * 2 >= size_; }
    // lights up the next planet. true if that was the last one, so a win
    bool capturePlanet();

    // pixels per tick, speeds up as enemies are shot down and every wave
    double enemySpeed(int enemiesLeft) const;
    // ticks between enemy shots, before the random bit is added
    int enemyFireDelay() const;

    // speed bolts drop once early in the 1st and 3rd wave
    bool boltWave() const { return wave_ == 0 || wave_ == 2; }
    bool boltPending() const { return boltWave() && !boltDropped_; }
    void boltHasDropped() { boltDropped_ = true; }
    void collectBolt() { ++bolts_; }
    double playerSpeed() const { return 5 + 2 * bolts_; }

    Outcome outcome() const { return outcome_; }
    void reset();

private:
    int score_ = 0;
    int lives_ = startLives;
    int wave_ = 0;
    int size_ = 0;
    int kills_ = 0;
    int bolts_ = 0;
    bool boltDropped_ = false;
    Outcome outcome_ = Playing;
};
