#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QGraphicsSimpleTextItem>
#include <QKeyEvent>
#include <QTimer>
#include <set>
#include <vector>
#include <utility>

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
             std::vector<QGraphicsSvgItem*> enemies,
             QGraphicsSimpleTextItem *scoreText)

        : QGraphicsView(scene), player(player),
          enemies(std::move(enemies)), scoreText(scoreText) {
        // lives text top right, made here since only the view touches it
        livesText = scene->addSimpleText("LIVES: 3");
        livesText->setFont(QFont("Consolas", 16));
        livesText->setBrush(QColor("#5DCAA5"));
        livesText->setPos(660, 10);

        // tick() runs every time the timer goes off
        connect(&timer, &QTimer::timeout, this, [this] { tick(); });
        timer.start(16);
    }

    //keyboard input 
protected:
    // autorepeat check so holding a key doesnt spam events
    void keyPressEvent(QKeyEvent *e) override {
        if (!e->isAutoRepeat()) held.insert(e->key());
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
        // game over might have just happened, dont also show the win
        if (over) return;
        if (enemies.empty()) endGame("YOU WIN");
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
        auto *t = scene()->addSimpleText(message);
        t->setFont(QFont("Consolas", 48));
        t->setBrush(QColor("#5DCAA5"));
        t->setPos(400 - t->boundingRect().width() / 2, 240);
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
    std::vector<QGraphicsSvgItem*> bullets;
    QTimer timer;
    // keys currently held down, checked every tick
    std::set<int> held;
    int cooldown = 0;
    int score = 0;
    int lives = 3;
    bool over = false;
    // 1 = right, -1 = left. speed is pixels per tick
    int enemyDir = 1;
    qreal enemySpeed = 1;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // scene is the game world, view is the window looking at it
    QGraphicsScene scene(0, 0, 800, 600);
    scene.setBackgroundBrush(Qt::black);

    // scales are just what looked right against the 320x320 svgs
    addSprite(scene, "assets/planet-template.svg", 650, 60, 0.34375);
    auto *player = addSprite(scene, "assets/player-ship-template.svg", 360, 480, 0.25);

    // 2 rows of 5 enemies. keeping pointers so we can hit and move them
    std::vector<QGraphicsSvgItem*> enemies;
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 5; ++col) {
            enemies.push_back(addSprite(scene, "assets/enemy-template.svg",
                                        100 + col * 70, 120 + row * 90, 0.1875));
        }
    }

    // score text in the top left
    auto *scoreText = scene.addSimpleText("SCORE: 0");
    scoreText->setFont(QFont("Consolas", 16));
    scoreText->setBrush(QColor("#5DCAA5"));
    scoreText->setPos(20, 10);

    GameView view(&scene, player, std::move(enemies), scoreText);
    view.setWindowTitle("Space Blasters");
    view.setFixedSize(820, 640);
    view.show();

    return app.exec();
}