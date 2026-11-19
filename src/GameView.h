#pragma once
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QKeyEvent>
#include <QTimer>
#include <memory>
#include <set>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Planet.h"
#include "Effect.h"
#include "Pickup.h"

class GameView : public QGraphicsView {
public:
    GameView(QGraphicsScene *scene, QGraphicsSimpleTextItem *scoreText);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

private:
    void tick();
    void spawnEnemies();
    void capturePlanet();
    void fire();
    void moveBullets();
    void updateEffects();
    void updatePickups();
    bool stopped() const { return over || dying > 0; }
    void enemyFire();
    void moveEnemyBullets();
    void moveEnemies();
    void updateEnemySpeed();
    void checkHits();
    void checkPlayerHit();
    void loseLife();
    void endGame(const QString &message);
    void restart();

    QGraphicsSimpleTextItem *scoreText;
    QGraphicsSimpleTextItem *livesText = nullptr;
    QGraphicsSimpleTextItem *endText = nullptr;
    QGraphicsSimpleTextItem *hintText = nullptr;

    // keys currently held down, checked every tick
    std::set<int> held;

    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<std::unique_ptr<Bullet>> enemyBullets;
    std::vector<std::unique_ptr<Planet>> planets;
    std::vector<std::unique_ptr<Effect>> effects;
    std::vector<std::unique_ptr<Pickup>> pickups;

    QTimer timer;
    int score = 0;
    bool over = false;
    // how many planets are captured so far, also which wave were on
    int captured = 0;
    // 1 = right, -1 = left. speed is pixels per tick
    int enemyDir = 1;
    qreal baseSpeed = 1;
    qreal enemySpeed = 1;
    // how many enemies the current wave started with
    int waveSize = 0;
    // enemies shot down this wave, a wave captures a planet if at least half were
    int waveKills = 0;
    // ticks until the next enemy shot
    int enemyCooldown = 90;
    // ticks until a wrench might drop
    int pickupCooldown = 600;
    // ticks until the speed bolt drops this wave, -1 = none coming
    int boltCooldown = -1;
    // only one bolt per wave, even if the wave gets replayed
    bool boltDropped = false;
    // counts down after the last life is lost so the explosions can play
    int dying = 0;
    // extra explosions still to go off during the death
    int blasts = 0;
    int blastGap = 0;
};