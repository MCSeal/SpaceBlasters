#include "GameView.h"
#include "HighScore.h"
#include <QRandomGenerator>





//qtimer every 16ms which translates to 60fps
GameView::GameView(QGraphicsScene *scene, QGraphicsSimpleTextItem *scoreText)

    : QGraphicsView(scene), scoreText(scoreText) {
    player = std::make_unique<Player>(*scene, held);

    // lives text top right, made here since only the view touches it
    livesText = scene->addSimpleText("LIVES: 3");
    livesText->setFont(QFont("Consolas", 16));
    livesText->setBrush(QColor("#5DCAA5"));
    livesText->setPos(660, 10);

    // 3 planets across the top, dim until captured
    for (int i = 0; i < 3; ++i) {
        planets.push_back(std::make_unique<Planet>(*scene, 320 + i * 60, 0));
    }

    spawnEnemies();

    // tick() runs every time the timer goes off
    connect(&timer, &QTimer::timeout, this, [this] { tick(); });
    timer.start(16);
}

//keyboard input 
// autorepeat check so holding a key doesnt spam events
void GameView::keyPressEvent(QKeyEvent *e) {
    if (e->isAutoRepeat()) return;
    // r only does anything once the game has ended
    if (e->key() == Qt::Key_R && over) {
        restart();
        return;
    }
    held.insert(e->key());
}

void GameView::keyReleaseEvent(QKeyEvent *e) {
    if (!e->isAutoRepeat()) held.erase(e->key());
}

// everything that happens each frame, in order
void GameView::tick() {
    player->update();
    updateEffects();
    if (held.count(Qt::Key_Space) && player->ready()) {
        fire();
        player->startCooldown();
    }
    moveBullets();
    moveEnemies();
    enemyFire();
    moveEnemyBullets();
    if (over) return;
    checkHits();
    checkPlayerHit();
    // game over might have just happened, dont also count a capture
    if (over) return;
    if (enemies.empty()) {
        // shooting down at least half the wave counts as a clear
        if (waveKills * 2 >= waveSize) {
            capturePlanet();
        } else {
            waveKills = 0;
            enemySpeed = baseSpeed;
            enemyDir = 1;
            enemyBullets.clear();
            spawnEnemies();
        }
    }
}

// bigger every wave: 2x5, 2x6, 3x7. later waves get random gaps and some tough enemies.
// called at start, on restart and for every new wave
void GameView::spawnEnemies() {
    int rows = captured >= 2 ? 3 : 2;
    int cols = 5 + captured;
    // centre the block, enemies are about 60px wide with 10px between
    qreal startX = (800 - ((cols - 1) * 70 + 60)) / 2.0;
    auto *rng = QRandomGenerator::global();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            // first wave is always full so it stays predictable
            if (captured > 0 && rng->bounded(100) < 15) continue;
            auto e = std::make_unique<Enemy>(
                *scene(), startX + col * 70, 120 + row * 90);
            if (captured > 0 && rng->bounded(100) < 25) e->makeTough();
            enemies.push_back(std::move(e));
        }
    }
    // never leave a wave empty
    if (enemies.empty()) {
        enemies.push_back(std::make_unique<Enemy>(*scene(), 370, 120));
    }
    waveSize = (int)enemies.size();
}

// wave cleared = next planet lights up. all 3 = win, otherwise harder wave
void GameView::capturePlanet() {
    planets[captured]->capture();
    ++captured;
    if (captured == (int)planets.size()) {
        endGame("YOU WIN");
        return;
    }
    // each wave starts a bit faster than the last
    baseSpeed += 0.3;
    enemySpeed = baseSpeed;
    enemyDir = 1;
    // leftover bullets from the last wave would clip the new enemies
    bullets.clear();
    enemyBullets.clear();
    waveKills = 0;
    spawnEnemies();
}

// new bullet each shot, starts at the middle of the ship
void GameView::fire() {
    auto b = std::make_unique<Bullet>(*scene(), 0, player->y() - 30);
    qreal cx = player->bounds().center().x();
    b->setPos(cx - b->bounds().width() / 2, player->y() - 30);
    bullets.push_back(std::move(b));
}

// plays every explosion one step, drops the ones that finished
void GameView::updateEffects() {
    for (size_t i = 0; i < effects.size();) {
        effects[i]->update();
        if (effects[i]->done()) {
            effects.erase(effects.begin() + i);
        } else {
            ++i;
        }
    }
}

void GameView::moveBullets() {
    // no ++i in the loop header because erasing shifts everything down
    for (size_t i = 0; i < bullets.size();) {
        bullets[i]->update();
        // gone off the top, erasing it also deletes the sprite
        if (bullets[i]->offScreen()) {
            bullets.erase(bullets.begin() + i);
        } else {
            ++i;
        }
    }
}

// a random enemy shoots every so often, later waves shoot more
void GameView::enemyFire() {
    if (enemies.empty()) return;
    if (--enemyCooldown > 0) return;
    enemyCooldown = qMax(30, 100 - captured * 25) + QRandomGenerator::global()->bounded(40);

    auto &shooter = enemies[QRandomGenerator::global()->bounded((int)enemies.size())];
    QRectF r = shooter->bounds();
    auto b = std::make_unique<Bullet>(*scene(), 0, r.bottom() - 10, 4);
    b->setPos(r.center().x() - b->bounds().width() / 2, r.bottom() - 10);
    enemyBullets.push_back(std::move(b));
}

// same as moveBullets, plus a hit on the ship costs a life
void GameView::moveEnemyBullets() {
    QRectF pr = player->hitbox();
    for (size_t i = 0; i < enemyBullets.size();) {
        enemyBullets[i]->update();
        if (enemyBullets[i]->hitbox().intersects(pr)) {
            enemyBullets.erase(enemyBullets.begin() + i);
            loseLife();
            if (over) return;
        } else if (enemyBullets[i]->offScreen()) {
            enemyBullets.erase(enemyBullets.begin() + i);
        } else {
            ++i;
        }
    }
}

// whole formation slides sideways, drops down when any enemy hits a wall
void GameView::moveEnemies() {
    qreal step = enemyDir * enemySpeed;
    bool atEdge = false;
    // look ahead first so nothing ever ends up past the wall
    for (auto &e : enemies) {
        QRectF r = e->hitbox();
        if (r.left() + step < 0 || r.right() + step > 800) atEdge = true;
    }
    if (atEdge) {
        enemyDir = -enemyDir;
        for (auto &e : enemies) e->moveBy(0, 20);
    } else {
        for (auto &e : enemies) e->moveBy(step, 0);
    }
}

// fewer enemies left = faster. starts at baseSpeed, adds up to +1.5 by the last one
void GameView::updateEnemySpeed() {
    enemySpeed = baseSpeed + (waveSize - (int)enemies.size()) * 1.5 / waveSize;
}

// checks every bullet against every enemy, fine for 10 enemies
void GameView::checkHits() {
    for (size_t b = 0; b < bullets.size();) {
        bool hit = false;
        QRectF br = bullets[b]->hitbox();
        for (size_t e = 0; e < enemies.size(); ++e) {
            if (br.intersects(enemies[e]->hitbox())) {
                bullets.erase(bullets.begin() + b);
                hit = true;
                // tough ones survive the first hit
                if (enemies[e]->hit()) {
                    QRectF r = enemies[e]->bounds();
                    effects.push_back(std::make_unique<Effect>(
                        *scene(), r.center().x(), r.center().y()));
                    score += enemies[e]->points;
                    enemies.erase(enemies.begin() + e);
                    ++waveKills;
                    scoreText->setText(QString("SCORE: %1").arg(score));
                    updateEnemySpeed();
                }
                break;
            }
        }
        if (!hit) ++b;
    }
}

// enemy touching the ship or slipping past the bottom costs a life.
// the enemy gets removed so one enemy cant drain every life at once
void GameView::checkPlayerHit() {
    QRectF pr = player->hitbox();
    for (size_t i = 0; i < enemies.size();) {
        bool touched = pr.intersects(enemies[i]->hitbox());
        bool escaped = enemies[i]->y() > 600;
        if (touched || escaped) {
            // only blow up if it hit the ship, not if it slipped past
            if (touched) {
                QRectF r = enemies[i]->bounds();
                effects.push_back(std::make_unique<Effect>(
                    *scene(), r.center().x(), r.center().y()));
            }
            enemies.erase(enemies.begin() + i);
            updateEnemySpeed();
            loseLife();
            if (over) return;
        } else {
            ++i;
        }
    }
}

void GameView::loseLife() {
    --player->lives;
    livesText->setText(QString("LIVES: %1").arg(player->lives));
    if (player->lives <= 0) endGame("GAME OVER");
}

// big text in the middle, then stop the timer so everything freezes
void GameView::endGame(const QString &message) {
    over = true;
    timer.stop();
    // an explosion frozen mid frame would sit on the end screen
    effects.clear();


    
    // grab the old best before saving so we know if this run beat it
    int oldBest = HighScore::best();
    HighScore::save(score, captured, message == "YOU WIN" ? "win" : "loss");
    int best = qMax(oldBest, score);

    endText = scene()->addSimpleText(message);
    endText->setFont(QFont("Consolas", 48));
    endText->setBrush(QColor("#58d1a9"));
    endText->setPos(400 - endText->boundingRect().width() / 2, 240);

    QString hint = QString("best: %1   press R to restart").arg(best);
    if (score > oldBest) hint = "new best!   press R to restart";
    hintText = scene()->addSimpleText(hint);
    hintText->setFont(QFont("Consolas", 16));
    hintText->setBrush(QColor("#5DCAA5"));
    hintText->setPos(400 - hintText->boundingRect().width() / 2, 320);
}

// wipe everything back to the starting state and start the timer again
void GameView::restart() {
    bullets.clear();
    enemyBullets.clear();
    enemies.clear();
    effects.clear();
    waveKills = 0;
    enemyCooldown = 90;
    // end screen text is its own items, remove those too
    scene()->removeItem(endText);
    delete endText;
    endText = nullptr;
    scene()->removeItem(hintText);
    delete hintText;
    hintText = nullptr;

    // planets go back to dim, waves back to the first one
    for (auto &p : planets) p->reset();
    captured = 0;
    baseSpeed = 1;

    score = 0;
    enemyDir = 1;
    enemySpeed = baseSpeed;
    over = false;
    scoreText->setText("SCORE: 0");
    livesText->setText("LIVES: 3");
    player->reset();
    // dont carry over keys that were held when the game ended
    held.clear();

    spawnEnemies();
    timer.start(16);
}