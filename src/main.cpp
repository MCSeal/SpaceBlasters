#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QGraphicsSimpleTextItem>
#include <QKeyEvent>
#include <QTimer>
#include <set>
#include <vector>

// loads an svg, scales it, puts it on the scene. used for everything for now
static QGraphicsSvgItem* addSprite(QGraphicsScene &scene, const QString &file,
                                   qreal x, qreal y, qreal scale) {
    auto *item = new QGraphicsSvgItem(file);
    item->setScale(scale);
    item->setPos(x, y);
    scene.addItem(item);
    return item;
}

class GameView : public QGraphicsView {
public:
    //qtimer every 16ms which translates to 60fps
    GameView(QGraphicsScene *scene, QGraphicsSvgItem *player,
             QGraphicsSimpleTextItem *scoreText)

        : QGraphicsView(scene), player(player), scoreText(scoreText) {
        // lives text top right, made here since only the view touches it
        livesText = scene->addSimpleText("LIVES: 3");
        livesText->setFont(QFont("Consolas", 16));
        livesText->setBrush(QColor("#5DCAA5"));
        livesText->setPos(660, 10);

        // 3 planets across the top, dim until captured
        for (int i = 0; i < 3; ++i) {
            auto *p = addSprite(*scene, "assets/planet-template.svg",
                                320 + i * 60, 0, 0.15);
            p->setOpacity(dimOpacity);
            planets.push_back(p);
        }

        spawnEnemies();

        // tick() runs every time the timer goes off
        connect(&timer, &QTimer::timeout, this, [this] { tick(); });
        timer.start(16);
    }

    //keyboard input 
protected:
    // autorepeat check so holding a key doesnt spam events
    void keyPressEvent(QKeyEvent *e) override {
        if (e->isAutoRepeat()) return;
        // r only does anything once the game has ended
        if (e->key() == Qt::Key_R && over) {
            restart();
            return;
        }
        held.insert(e->key());
    }
    void keyReleaseEvent(QKeyEvent *e) override {
        if (!e->isAutoRepeat()) held.erase(e->key());
    }

private:
    // everything that happens each frame, in order
    void tick() {
        movePlayer();
        if (cooldown > 0) --cooldown;
        // cant fire again until cooldown hits 0
        if (held.count(Qt::Key_Space) && cooldown == 0) {
            fire();
            cooldown = 20; // about a third of a second
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
    void spawnEnemies() {
        for (int row = 0; row < 2; ++row) {
            for (int col = 0; col < 5; ++col) {
                enemies.push_back(addSprite(*scene(), "assets/enemy-template.svg",
                                            100 + col * 70, 120 + row * 90, 0.1875));
            }
        }
    }

    // wave cleared = next planet lights up. all 3 = win, otherwise harder wave
    void capturePlanet() {
        planets[captured]->setOpacity(1.0);
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
        for (auto *b : bullets) removeSprite(b);
        bullets.clear();
        spawnEnemies();
    }

    //movement for player
    void movePlayer() {
        const qreal speed = 5;
        qreal x = player->x();
        // arrows or a/d, whichever
        if (held.count(Qt::Key_Left) || held.count(Qt::Key_A))  x -= speed;
        if (held.count(Qt::Key_Right) || held.count(Qt::Key_D)) x += speed;
        // keep the ship inside the window (800 wide minus ship width, roughly)
        if (x < 0) x = 0;
        if (x > 720) x = 720;
        player->setX(x);
    }

    // new bullet each shot, starts at the middle of the ship
    void fire() {
        auto *b = new QGraphicsSvgItem("assets/bullet-template.svg");
        b->setScale(0.3);
        qreal cx = player->sceneBoundingRect().center().x();
        b->setPos(cx - b->sceneBoundingRect().width() / 2, player->y() - 30);
        scene()->addItem(b);
        bullets.push_back(b);
    }

    void moveBullets() {
        // no ++i in the loop header because erasing shifts everything down
        for (size_t i = 0; i < bullets.size();) {
            auto *b = bullets[i];
            b->setY(b->y() - 8);
            // gone off the top, delete it or they pile up forever
            if (b->y() < -b->sceneBoundingRect().height()) {
                removeSprite(b);
                bullets.erase(bullets.begin() + i);
            } else {
                ++i;
            }
        }
    }

    // whole formation slides sideways, drops down when any enemy hits a wall
    void moveEnemies() {
        qreal step = enemyDir * enemySpeed;
        bool atEdge = false;
        // look ahead first so nothing ever ends up past the wall
        for (auto *e : enemies) {
            QRectF r = hitbox(e, 0.18, 0.15);
            if (r.left() + step < 0 || r.right() + step > 800) atEdge = true;
        }
        if (atEdge) {
            enemyDir = -enemyDir;
            for (auto *e : enemies) e->setY(e->y() + 20);
        } else {
            for (auto *e : enemies) e->setX(e->x() + step);
        }
    }

    // fewer enemies left = faster. starts at baseSpeed, +0.15 per enemy gone
    void updateEnemySpeed() {
        enemySpeed = baseSpeed + (totalEnemies - (int)enemies.size()) * 0.15;
    }

    // checks every bullet against every enemy, fine for 10 enemies
    void checkHits() {
        for (size_t b = 0; b < bullets.size();) {
            bool hit = false;
            QRectF br = hitbox(bullets[b], 0.4, 0.35);
            for (size_t e = 0; e < enemies.size(); ++e) {
                if (br.intersects(hitbox(enemies[e], 0.18, 0.15))) {
                    removeSprite(enemies[e]);
                    enemies.erase(enemies.begin() + e);
                    removeSprite(bullets[b]);
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
    void checkPlayerHit() {
        QRectF pr = hitbox(player, 0.2, 0.35);
        for (size_t i = 0; i < enemies.size();) {
            bool touched = pr.intersects(hitbox(enemies[i], 0.18, 0.15));
            bool escaped = enemies[i]->y() > 600;
            if (touched || escaped) {
                removeSprite(enemies[i]);
                enemies.erase(enemies.begin() + i);
                updateEnemySpeed();
                loseLife();
                if (over) return;
            } else {
                ++i;
            }
        }
    }

    void loseLife() {
        --lives;
        livesText->setText(QString("LIVES: %1").arg(lives));
        if (lives <= 0) endGame("GAME OVER");
    }

    // big text in the middle, then stop the timer so everything freezes
    void endGame(const QString &message) {
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
    void restart() {
        for (auto *b : bullets) removeSprite(b);
        bullets.clear();
        for (auto *e : enemies) removeSprite(e);
        enemies.clear();
        // end screen text is its own items, remove those too
        scene()->removeItem(endText);
        delete endText;
        endText = nullptr;
        scene()->removeItem(hintText);
        delete hintText;
        hintText = nullptr;

        // planets go back to dim, waves back to the first one
        for (auto *p : planets) p->setOpacity(dimOpacity);
        captured = 0;
        baseSpeed = 1;

        score = 0;
        lives = 3;
        cooldown = 0;
        enemyDir = 1;
        enemySpeed = baseSpeed;
        over = false;
        scoreText->setText("SCORE: 0");
        livesText->setText("LIVES: 3");
        player->setX(360);
        // dont carry over keys that were held when the game ended
        held.clear();

        spawnEnemies();
        timer.start(16);
    }

    // svgs are 320x320 with empty space around the art, so shrink the box
    // to roughly the visible part. numbers are guesses, tweak if hits feel off
    static QRectF hitbox(const QGraphicsSvgItem *item, qreal fx, qreal fy) {
        QRectF r = item->sceneBoundingRect();
        return r.adjusted(r.width() * fx, r.height() * fy,
                          -r.width() * fx, -r.height() * fy);
    }

    // take it off the scene first, then delete
    void removeSprite(QGraphicsSvgItem *item) {
        scene()->removeItem(item);
        delete item;
    }

    QGraphicsSvgItem *player;
    std::vector<QGraphicsSvgItem*> enemies;
    QGraphicsSimpleTextItem *scoreText;
    QGraphicsSimpleTextItem *livesText = nullptr;
    QGraphicsSimpleTextItem *endText = nullptr;
    QGraphicsSimpleTextItem *hintText = nullptr;
    std::vector<QGraphicsSvgItem*> bullets;
    std::vector<QGraphicsSvgItem*> planets;
    QTimer timer;
    // keys currently held down, checked every tick
    std::set<int> held;
    int cooldown = 0;
    int score = 0;
    int lives = 3;
    bool over = false;
    // how many planets are captured so far, also which wave were on
    int captured = 0;
    const qreal dimOpacity = 0.25;
    // 1 = right, -1 = left. speed is pixels per tick
    int enemyDir = 1;
    qreal baseSpeed = 1;
    qreal enemySpeed = 1;
    const int totalEnemies = 10;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // scene is the game world, view is the window looking at it
    QGraphicsScene scene(0, 0, 800, 600);
    scene.setBackgroundBrush(Qt::black);

    // scales are just what looked right against the 320x320 svgs
    auto *player = addSprite(scene, "assets/player-ship-template.svg", 360, 480, 0.25);

    // score text in the top left
    auto *scoreText = scene.addSimpleText("SCORE: 0");
    scoreText->setFont(QFont("Consolas", 16));
    scoreText->setBrush(QColor("#5DCAA5"));
    scoreText->setPos(20, 10);

    GameView view(&scene, player, scoreText);
    view.setWindowTitle("Space Blasters");
    view.setFixedSize(820, 640);
    view.show();

    return app.exec();
}