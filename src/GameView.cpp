#include "GameView.h"

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
    if (held.count(Qt::Key_Space) && player->ready()) {
        fire();
        player->startCooldown();
    }
    moveBullets();
    moveEnemies();
    checkHits();
    checkPlayerHit();
    // game over might have just happened, dont also count a capture
    if (over) return;
    if (enemies.empty()) capturePlanet();
}

// 2 rows of 5, called at start, on restart and for every new wave
void GameView::spawnEnemies() {
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 5; ++col) {
            enemies.push_back(std::make_unique<Enemy>(
                *scene(), 100 + col * 70, 120 + row * 90));
        }
    }
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
    spawnEnemies();
}

// new bullet each shot, starts at the middle of the ship
void GameView::fire() {
    auto b = std::make_unique<Bullet>(*scene(), 0, player->y() - 30);
    qreal cx = player->bounds().center().x();
    b->setPos(cx - b->bounds().width() / 2, player->y() - 30);
    bullets.push_back(std::move(b));
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

// fewer enemies left = faster. starts at baseSpeed, +0.15 per enemy gone
void GameView::updateEnemySpeed() {
    enemySpeed = baseSpeed + (totalEnemies - (int)enemies.size()) * 0.15;
}

// checks every bullet against every enemy, fine for 10 enemies
void GameView::checkHits() {
    for (size_t b = 0; b < bullets.size();) {
        bool hit = false;
        QRectF br = bullets[b]->hitbox();
        for (size_t e = 0; e < enemies.size(); ++e) {
            if (br.intersects(enemies[e]->hitbox())) {
                enemies.erase(enemies.begin() + e);
                bullets.erase(bullets.begin() + b);
                score += 10;
                scoreText->setText(QString("SCORE: %1").arg(score));
                updateEnemySpeed();
                hit = true;
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
    endText = scene()->addSimpleText(message);
    endText->setFont(QFont("Consolas", 48));
    endText->setBrush(QColor("#5DCAA5"));
    endText->setPos(400 - endText->boundingRect().width() / 2, 240);
    hintText = scene()->addSimpleText("press R to restart");
    hintText->setFont(QFont("Consolas", 16));
    hintText->setBrush(QColor("#5DCAA5"));
    hintText->setPos(400 - hintText->boundingRect().width() / 2, 320);
}

// wipe everything back to the starting state and start the timer again
void GameView::restart() {
    bullets.clear();
    enemies.clear();
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